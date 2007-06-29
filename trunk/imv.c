/*  stb(imv)  windows image viewer
    Copyright 2007  Sean Barrett

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

// Set section alignment to be nice and small
#pragma comment(linker, "/FILEALIGN:0x200")
//#pragma comment(linker, "/OPT:NOWIN98")

#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#define STB_DEFINE
#include "stb.h"          /*     http://nothings.org/stb.h         */
#include "stb_image.c"    /*     http://nothings.org/stb_image.c   */


// all programs get the version number from the same place: version.bat
#define set   static char *
#include "version.bat"
;
#undef set

// trivial error handling
void error(char *str) { MessageBox(NULL, str, "imv(stb) error", MB_OK); }

// OutputDebugString with varargs, can be compiled out
#ifdef _DEBUG
int do_debug;
void ods(char *str, ...)
{
   if (do_debug) {
      char buffer[1024];
      va_list va;
      va_start(va,str);
      vsprintf(buffer, str, va);
      va_end(va);
      OutputDebugString(buffer);
   }
}
#define o(x) ods x
#else
#define o(x)
#endif


// size of border in pixels
#define FRAME   3

// location within frame of secondary border
#define FRAME2  (FRAME >> 1)

// color of secondary border
#define GREY  192


// internal messages (all used for waking up main thread from tasks)
enum
{
   WM_APP_DECODED = WM_APP,
   WM_APP_LOAD_ERROR,
   WM_APP_DECODE_ERROR,
};


// a few extra options for GetSystemMetrics for old compilers
#if WINVER < 0x0500
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#define SM_SAMEDISPLAYFORMAT    81
#endif

char *displayName = "imv(stb)";
CHAR  szAppName[] = "stb_imv";
HWND  win;

// number of bytes per pixel (not bits); can be 3 or 4
#define BPP 4

// lightweight SetDIBitsToDevice() wrapper
// (once upon a time this was a platform-independent, hence the name)
void platformDrawBitmap(HDC hdc, int x, int y, unsigned char *bits, int w, int h, int stride, int dim)
{
   int i;
   BITMAPINFOHEADER b;
   int result;

   memset(&b, 0, sizeof(b));
   b.biSize = sizeof(b);
   b.biPlanes=1;
   b.biBitCount=BPP*8;
   b.biWidth = stride/BPP;
   b.biHeight = -h;  // tell windows the bitmap is stored top-to-bottom
   if (dim)
      for (i=0; i < stride*h; i += 4)
         *(uint32 *)(bits+i) = (*(uint32 *)(bits+i) >> 1) & 0x7f7f7f7f;
   result = SetDIBitsToDevice(hdc, x,y, w,abs(h), 0,0, 0,abs(h), bits, (BITMAPINFO *) &b, DIB_RGB_COLORS);
   if (result == 0) {
      DWORD e = GetLastError();
   }
   if (dim)
      for (i=0; i < stride*h; i += 4)
         *(uint32 *)(bits+i) = (*(uint32 *)(bits+i) << 1);
}

// memory barrier for x86
void barrier(void)
{
    long dummy;
    __asm {
        xchg dummy, eax
    }
}

// awake the main thread when something interesting happens
void wake(int message)
{
   PostMessage(win, message, 0,0);
}

typedef struct
{
   int x,y;         // size of the image
   int stride;      // distance between rows in bytes  
   int frame;       // does this image have a frame (border)?
   uint8 *pixels;   // pointer to (0,0)th pixel
} Image;

enum
{
// owned by main thread
   LOAD_unused=0, // empty slot

   LOAD_inactive, // filename slot, not loaded

   // finished reading, needs decoding--originally decoder
   // owned this, but then we couldn't free from the cache
   LOAD_reading_done,

   // in any of the following states, the image is as done as it can be
   LOAD_error_reading,
   LOAD_error_decoding,
   LOAD_available, // loaded successfully

// owned by resizer
   LOAD_resizing,

// owned by loader
   LOAD_reading,

// owned by decoder
   LOAD_decoding,
};

// does the main thread own this? (if this is true, the main
// thread can manipulate without locking, except for LOAD_reading_done
// which requires locking)
#define MAIN_OWNS(x)   ((x)->status <= LOAD_available)

// data about a specific file
typedef struct
{
   char *filename;   // name of the file on disk, must be free()d
   char *filedata;   // data loaded from disk -- passed from reader to decoder
   int len;          // length of data loaded from disk -- as above
   Image *image;     // cached image -- passed from decoder to main
   char *error;      // error message -- from reader or decoder, must be free()d
   int status;       // current status/ownership with LOAD_* enum
   int bail;         // flag from main thread to work threads indicating to give up
   int lru;          // the larger, the higher priority--effectively a timestamp
} ImageFile;

// controls for interlocking communications
stb_mutex cache_mutex, decode_mutex;
stb_semaphore decode_queue;
stb_semaphore disk_command_queue;

// a request communicated from the main thread to the disk-loader task
typedef struct
{
   int num_files;
   ImageFile *files[4];
} DiskCommand;

// there can only be one pending command in flight
volatile DiskCommand dc_shared;

// the disk loader sits in this loop forever
void *diskload_task(void *p)
{
   for(;;) {
      int i;
      DiskCommand dc;

      // wait to be woken up by a command from the main thread
      o(("READ: Waiting for disk request.\n"));
      stb_sem_waitfor(disk_command_queue);
      // it's possible for the main thread to do:
      //   1. ... store a command in the command buffer ...
      //   2. sem_release()
      //   3. ... store a command in the command buffer ...
      //   4. sem_release()
      // and for this thread to complete a previous command and
      // reach the waitfor() right after step 3 above. If this happens,
      // this thread will pass the waitfor() (setting the semaphore to 0),
      // process the latest command, the main thread will do step 4 (setting
      // the semaphore to 1), and then this thread comes back around and
      // passes the waitfor() again with no actual pending command. This
      // case is handled below by clearing the command length to 0.

      // grab the command; don't let the command or the cache change while we do it
      stb_mutex_begin(cache_mutex);
      {
         // copy the command into a local buffer
         dc = dc_shared;
         // claim ownership over all the files in the command
         for (i=0; i < dc.num_files; ++i) {
            dc.files[i]->status = LOAD_reading;
            assert(dc.files[i]->filedata == NULL);
         }
         // clear the command so we won't re-process it
         dc_shared.num_files = 0;
      }
      stb_mutex_end(cache_mutex);

      o(("READ: Got disk request, %d items.\n", dc.num_files));
      for (i=0; i < dc.num_files; ++i) {
         int n;
         uint8 *data;
         assert(dc.files[i]->status == LOAD_reading);

         // check if the main thread changed its mind about this
         // e.g. if this is the third file in a request, and the main thread
         // has already made another command pending, then it will set this
         // flag on previous requests, and we shouldn't waste time loading
         // data that's no longer high-priority
         if (dc.files[i]->bail) {
            o(("READ: Bailing on disk request\n"));
            dc.files[i]->status = LOAD_inactive;
         } else {
            o(("READ: Loading file %s\n", dc.files[i]->filename));
            assert(dc.files[i]->filedata == NULL);

            // read the data
            data = stb_file(dc.files[i]->filename, &n);
         
            // update the results
            // don't need to mutex these, because we own them via ->status
            if (data == NULL) {
               o(("READ: error reading\n"));
               dc.files[i]->error = strdup("can't open");
               dc.files[i]->filedata = NULL;
               dc.files[i]->len = 0;
               barrier();
               dc.files[i]->status = LOAD_error_reading;
               wake(WM_APP_LOAD_ERROR); // wake main thread to react to error
            } else {
               o(("READ: Successfully read %d bytes\n", n));
               dc.files[i]->error = NULL;
               assert(dc.files[i]->filedata == NULL);
               dc.files[i]->filedata = data;
               dc.files[i]->len = n;
               barrier();
               dc.files[i]->status = LOAD_reading_done;
               stb_sem_release(decode_queue); // wake the decode task if needed
            }

         }
      }
   }
}

// given raw decoded data, make it into a proper Image (e.g. creating a
// windows-compatible bitmap with 4-byte aligned rows)
void make_image(Image *z, int image_x, int image_y, uint8 *image_data, int image_n)
{
   int i,j,k=0;
   z->pixels = image_data;
   z->x = image_x;
   z->y = image_y;
   z->stride = image_x*BPP;
   z->frame = 0;

   for (j=0; j < image_y; ++j) {
      for (i=0; i < image_x; ++i) {
         // swap RGB to BGR
         unsigned char t = image_data[k+0];
         image_data[k+0] = image_data[k+2];
         image_data[k+2] = t;

          #if BPP==4
         // if image had an alpha channel, pre-blend with background
         if (image_n == 4) {
            unsigned char *p = image_data+k;
            int a = (255-p[3]);
            if ((i ^ j) & 8) {
               p[0] += (((200 - (int) p[0])*a)>>8);
               p[1] += ((( 40 - (int) p[1])*a)>>8);
               p[2] += (((200 - (int) p[2])*a)>>8);
            } else {
               p[0] += (((150 - (int) p[0])*a)>>8);
               p[1] += ((( 30 - (int) p[1])*a)>>8);
               p[2] += (((150 - (int) p[2])*a)>>8);
            }
         }
         #endif
         k += BPP;
      }
   }
}


// Max entries in image cache. This shouldn't be TOO large, because we
// traverse it inside mutexes sometimes. Also, for large images, we'll
// hit cache-size limits fairly quickly (a 2 megapixel image requires
// 8MB, so you could only fit 50 in a 400MB cache), so no reason to be
// too large anyway
#define MAX_CACHED_IMAGES  200

// no idea if it needs to be volatile, decided not to worry about proving
// it one way or the other
volatile ImageFile cache[MAX_CACHED_IMAGES];

// choose which image to decode and claim ownership
volatile ImageFile *decoder_choose(void)
{
   int i, best_lru=0;
   volatile ImageFile *best = NULL;

   // if we get unlucky we may have to bail and start over
start:

   // iterate through the cache and find the ready-to-decode image
   // that was most in demand (the highest priority will be the most-recently
   // accessed image or, for prefetching, one right next to it; but this
   // is policy determined by the main thread, not by this thread).
   for (i=0; i < MAX_CACHED_IMAGES; ++i) {
      if (cache[i].status == LOAD_reading_done) {
         if (cache[i].lru > best_lru) {
            best = &cache[i];
            best_lru = best->lru;
         }
      }
   }
   // it's possible there is no image to decode; see the description
   // in diskload_task of how it's possible for a task to be woken
   // from the sem_release() without there being a pending command.
   if (best) {
      int retry = FALSE;
      // if there is a best one, it's possible that while iterating
      // it was flushed by the main thread. so let's make sure it's
      // still ready to decode. (Of course it ALSO could have changed
      // lru priority and other such, or not be the best anymore, but
      // it's no big deal to get that wrong since it's close.)
      stb_mutex_begin(cache_mutex);
      {
         if (best->status == LOAD_reading_done)
            best->status = LOAD_decoding;
         else
            retry = TRUE;
      }
      stb_mutex_end(cache_mutex);
      // if the status changed out from under us, try again
      if (retry)
         goto start;
   }
   return best;
}

void *decode_task(void *p)
{
   for(;;) {

      // find the best image to decode
      volatile ImageFile *f = decoder_choose();

      if (f == NULL) {
         // wait for load thread to wake us
         o(("DECODE: blocking\n"));
         stb_sem_waitfor(decode_queue);
         o(("DECODE: woken\n"));
      } else {
         int x,y,n;
         uint8 *data;
         assert(f->status == LOAD_decoding);

         // decode image
         o(("DECIDE: decoding %s\n", f->filename));
         data = stbi_load_from_memory(f->filedata, f->len, &x, &y, &n, BPP);
         o(("DECODE: decoded %s\n", f->filename));

         // free copy of data from disk, which we don't need anymore
         free(f->filedata);
         f->filedata = NULL;

         if (data == NULL) {
            // error reading file, record the reason for it
            f->error = strdup(stbi_failure_reason());
            barrier();
            f->status = LOAD_error_reading;
            // wake up the main thread in case this is the most recent image
            wake(WM_APP_DECODE_ERROR);
         } else {
            // post-process the image into the right format
            f->image = (Image *) malloc(sizeof(*f->image));
            make_image(f->image, x,y,data,n);
            barrier();
            f->status = LOAD_available;

            // wake up the main thread in case this is the most recent image
            wake(WM_APP_DECODED);
         }
      }
   }
}

// the image cache entry currently trying to be displayed (may be waiting on resizer)
ImageFile *source_c;
// the image currently being displayed--historically redundant to source_c->image
Image *source;

// allocate an image in windows-friendly format
Image *bmp_alloc(int x, int y)
{
   Image *i = malloc(sizeof(*i));
   if (!i) return NULL;
   i->x = x;
   i->y = y;
   i->stride = x*BPP;
   i->stride += (-i->stride) & 3;
   i->pixels = malloc(i->stride * i->y);
   i->frame = 0;
   if (i->pixels == NULL) { free(i); return NULL; }
   return i;
}

// toggle for whether to draw the stripe in the middle of the border
int extra_border = TRUE;

// build the border into an image--this was easier than drawing it on
// the fly, although slightly less efficient, but probably totally
// redundant now that we paint an infinite black border around the image?
// reduces flickering of the stripe, I guess.
void frame(Image *z)
{
   int i;
   z->frame = FRAME;
   memset(z->pixels, 0, FRAME*z->stride);
   memset(z->pixels + z->stride*(z->y-FRAME), 0, FRAME*z->stride);
   if (extra_border) {
      memset(z->pixels + z->stride*FRAME2 + FRAME2*BPP, GREY, (z->x-FRAME2*2)*BPP);
      memset(z->pixels + z->stride*(z->y-FRAME2-1) + FRAME2*BPP, GREY, (z->x-FRAME2*2)*BPP);
   }
   for (i=FRAME; i < z->y-FRAME; ++i) {
      memset(z->pixels + i*z->stride, 0, FRAME*BPP);
      memset(z->pixels + i*z->stride + (z->x-FRAME)*BPP, 0, FRAME*BPP);
   }
   if (extra_border) {
      for (i=2; i < z->y-2; ++i) {
         memset(z->pixels + i*z->stride+FRAME2*BPP, GREY, BPP);
         memset(z->pixels + i*z->stride + (z->x-FRAME2-1)*BPP, GREY, BPP);
      }
   }
}

void imfree(Image *x)
{
   if (x) {
      free(x->pixels);
      free(x);
   }
}

// return an Image which is a sub-region of another image
Image image_region(Image *p, int x, int y, int w, int h)
{
   Image q;
   q.stride = p->stride;
   q.x = w;
   q.y = h;
   q.pixels = p->pixels + y*p->stride + x*BPP;
   return q;
}

// the currently displayed image--may slightly lag source/source_c
// while waiting on a resize
Image *cur;

// the filename for the currently displayed image
char *cur_filename;
int show_help=0;

char helptext_center[128] =
   "imv(stb)\n"
   "Copyright 2007 Sean Barret\n"
   "http://code.google.com/p/stb-imv\n"
   "version "
;

char helptext_left[] =
   "\n\n\n\n"
   "ESC: exit\n"
   "ALT-ENTER: toggle size\n"
   "CTRL-PLUS: zoom in\n"
   "CTRL-MINUS: zoom out\n"
   "LEFT, SPACE: next image\n"
   "RIGHT, BACKSPACE: previous image\n"
   "CTRL-O: open image\n"
   "F: toggle frame\n"
   "SHIFT-F: toggle white stripe in frame\n"
   "CTRL-F: toggle both\n"
   "L: toggle filename label\n"
   "F1, H, ?: help"
;

char helptext_right[] =
   "\n\n\n\n\n"
   "right-click to exit\n"
   "left drag center to move\n"
   "left drag edges to resize\n"
   "double-click to toggle size\n"
   "mousewheel to zoom\n"
   "\n"
;

// draw the help text semi-prettily
// originally this was to try to avoid having to darken the image
// that it's rendered over, but I couldn't make that work, and with
// the darkened image there's no real need to do this, but hey, it
// looks a little nicer so why not
void draw_nice(HDC hdc, char *text, RECT *rect, uint flags)
{
#if 1
   int i,j;
   SetTextColor(hdc, RGB(80,80,80));
   //for (i=-1; i <= 1; i += 1)
   //for (j=-1; j <= 1; j += 1)
   for (i=2; i >= 1; i -= 1)
   for (j=2; j >= 1; j -= 1)
   {
      // displace the rectangle so as to displace the text
      RECT r = { rect->left+i, rect->top+j, rect->right+i, rect->bottom + j };
      if (i == 1 && j == 1)
         SetTextColor(hdc, RGB(0,0,0));
      DrawText(hdc, text, -1, &r, flags);
   }
#endif
   SetTextColor(hdc, RGB(255,255,255));
   DrawText(hdc, text, -1, rect, flags);
}

// cached error message for most recent image
char display_error[1024];

// to make an image with an error the most recent image, call this
void set_error(volatile ImageFile *z)
{
   sprintf(display_error, "File:\n%s\nError:\n%s\n", z->filename, z->error);
   InvalidateRect(win, NULL, FALSE);
   imfree(cur);
   cur = NULL;
   free(cur_filename);
   cur_filename = strdup(z->filename);
   source_c = (ImageFile *) z;
   source = NULL;
}

HFONT label_font;
int show_frame = TRUE;   // show border or not?
int show_label = FALSE;  // 
void display(HWND win, HDC hdc)
{
   RECT rect,r2;
   HBRUSH b = GetStockObject(BLACK_BRUSH);
   int w,h,x,y;

   // get the window size for centering
   GetClientRect(win, &rect);
   w = rect.right - rect.left;
   h = rect.bottom - rect.top;

   // set the text rendering mode for our fancy text
   SetBkMode(hdc, TRANSPARENT);

   // if the current image had an error, just display that
   if (display_error[0]) {
      FillRect(hdc, &rect, b);  // clear to black -- will flicker
      if (rect.bottom > rect.top + 100) rect.top += 50; // displace down from top; could center
      draw_nice(hdc, display_error, &rect, DT_CENTER);
      return;
   }

   // because show_frame toggles the window size, and we center the bitmap,
   // we just go ahead and render the entire bitmap with the border in it
   // regardless of the show_frame toggle. You can see that when you resize
   // a window in one dimension--the strip is still there, just off the edge
   // of the window.
   x = (w - cur->x) >> 1;
   y = (h - cur->y) >> 1;
   platformDrawBitmap(hdc, x,y,cur->pixels, cur->x, cur->y, cur->stride, show_help);

   // draw in infinite borders on all four sides
   r2 = rect;
   r2.right = x;           FillRect(hdc, &r2, b); r2=rect;
   r2.left = x + cur->x;   FillRect(hdc, &r2, b); r2 = rect;
   r2.left = x;
   r2.right = x+cur->x;
   r2.bottom = y;          FillRect(hdc, &r2, b); r2 = rect;
   r2.left = x;
   r2.right = x+cur->x;
   r2.top  = y + cur->y;   FillRect(hdc, &r2, b);

   if (show_label) {
      SIZE size;
      RECT z;
      HFONT old = NULL;
      char *name = cur_filename ? cur_filename : "(none)";
      if (label_font) old = SelectObject(hdc, label_font);

      // get rect around label so we can draw it ourselves, because
      // the DrawText() one is lame

      GetTextExtentPoint32(hdc, name, strlen(name), &size);
      z.left = rect.left+1;
      z.bottom = rect.bottom+1;
      z.top = z.bottom - size.cy - 4;
      z.right = z.left + size.cx + 10;

      FillRect(hdc, &z, b);
      z.bottom -= 2; // extra padding on bottom because it's at edge of window
      SetTextColor(hdc, RGB(255,255,255));
      DrawText(hdc, name, -1, &z, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      if (old) SelectObject(hdc, old);
   }

   if (show_help) {
      int h2;
      RECT box = rect;
      // measure height of longest text
      DrawText(hdc, helptext_left, -1, &box, DT_CALCRECT);
      h2 = box.bottom - box.top;
      // build rect of correct height
      box = rect;
      box.top = stb_max((h - h2) >> 1, 0);
      box.bottom = box.top + h2;
      // expand on left & right so following code is well behaved
      box.left -= 200; box.right += 200;

      // draw center text
      draw_nice(hdc, helptext_center, &box, DT_CENTER);

      // displace box to left and draw left column
      box.left -= 150; box.right -= 150;
      draw_nice(hdc, helptext_left, &box, DT_CENTER);

      // displace box to right and draw right column
      box.left += 300; box.right += 300;
      draw_nice(hdc, helptext_right, &box, DT_CENTER);
   }
}

typedef struct
{
   int x,y;
   int w,h;
} queued_size;

// most recent unsatisfied resize request (private to main thread)
queued_size qs;

// active resize request, mainly just used by main thread (resize
// thread writes to 'image' field.
struct
{
   queued_size size;
   Image *image;
   char *filename;
   ImageFile *image_c;
} pending_resize;

// temporary structure for communicating across stb_workq() call
typedef struct
{
   ImageFile *src;
   Image dest;
   Image *result;
} Resize;

// threaded image resizer, uses work queue AND current thread
static void image_resize(Image *dest, Image *src);

// wrapper for image_resize() to be called via work queue
void * work_resize(void *p)
{
   Resize *r = (Resize *) p;
   image_resize(&r->dest, r->src->image);
   return r->result;
}

// dedicate workqueue workers for resizing
stb_workqueue *resize_workers;

// compute the size to resize an image to given a target window (gw,gh);
// we assume the input window (sw,wh) has already been expanded by its
// frame size.
void compute_size(int gw, int gh, int sw, int sh, int *ox, int *oy)
{
   // shrink the target by the padding (the size of the frame)
   gw -= FRAME*2;
   gh -= FRAME*2;
   // shrink the source to remove the frame
   sw -= FRAME*2;
   sh -= FRAME*2;
   // compute the raw pixel resize
   if (gw*sh > gh*sw) {
      *oy = gh;
      *ox = gh * sw/sh;
   } else {
      *ox = gw;
      *oy = gw * sh/sw;
   }
}

// resize an image. if immediate=TRUE, we run it from the main thread
// and won't return until it's resized; if !immediate, we hand it to
// a workqueue and return before it's done. (note that if immediate=TRUE,
// we still use the work queue to accelerate, if possible)
void queue_resize(int w, int h, ImageFile *src_c, int immediate)
{
   static Resize res; // must be static because we expose (very briefly) to other thread
   Image *src = src_c->image;
   Image *dest;
   int w2,h2;

   if (!immediate) assert(pending_resize.size.w);
   if (src_c == NULL) return;

   // create (w2,h2) matching aspect ratio of w/h
   compute_size(w,h,src->x+FRAME*2,src->y+FRAME*2,&w2,&h2);

   // create output of the appropriate size
   dest = bmp_alloc(w2+FRAME*2,h2+FRAME*2);
   assert(dest);
   if (!dest) return;

   // encode the border around it
   frame(dest);

   // build the parameter list for image_resize
   res.src = src_c;
   res.dest = image_region(dest, FRAME, FRAME, w2, h2);
   res.result = dest;

   if (!immediate) {
      // update status to be owned by the resizer (which isn't running yet,
      // so there's no thread issues here)
      src_c->status = LOAD_resizing;
      // store data to come back for later
      pending_resize.image = NULL;
      pending_resize.image_c = src_c;
      pending_resize.filename = strdup(src_c->filename);
      // run the resizer in the background (equivalent to the call below)
      stb_workq(resize_workers, work_resize, &res, &pending_resize.image);
   } else {
      // run the resizer in the main thread
      pending_resize.image = work_resize(&res);
   }
}

// put a resize request in the "queue" (which is only one deep)
void enqueue_resize(int left, int top, int width, int height)
{
   if (cur && ((width == cur->x && height >= cur->y) || (height == cur->y && width >= cur->x))) {
      // if we have a current image, and that image can satisfy the request (they're
      // dragging one side of the image out wider), just immediately update the window
      qs.w = 0; // clear the queue
      if (!show_frame)
         left += FRAME, top += FRAME, width -= 2*FRAME, height -= 2*FRAME;
      MoveWindow(win, left, top, width, height, TRUE);
      InvalidateRect(win, NULL, FALSE);
   } else {
      // otherwise store the most recent request for processing in the main thread
      qs.x = left;
      qs.y = top;
      qs.w = width;
      qs.h = height;
   }
}

// do all operations _as if_ we had the frame
void GetAdjustedWindowRect(HWND win, RECT *rect)
{
   GetWindowRect(win, rect);
   if (!show_frame) {
      rect->left -= FRAME;
      rect->top -= FRAME;
      rect->right += FRAME;
      rect->bottom += FRAME;
   }
}

// compute the size we'd prefer this window to be at
void ideal_window_size(int w, int h, int *w_ideal, int *h_ideal, int *x, int *y);

enum
{
   DISPLAY_actual,
   DISPLAY_current,

   DISPLAY__num,
};

int display_mode;

void size_to_current(int maximize)
{
   int w2,h2;
   int w,h,x,y;
   Image *z = source;

   w2 = source->x+FRAME*2, h2 = source->y+FRAME*2;
   switch (display_mode) {
      case DISPLAY_actual: {
         int cx,cy;
         RECT rect;
         ideal_window_size(w2,h2, &w,&h, &x,&y);
         cx = GetSystemMetrics(SM_CXSCREEN);
         cy = GetSystemMetrics(SM_CYSCREEN);
         if (w <= cx && h <= cy) {
            GetAdjustedWindowRect(win, &rect);
            x = (rect.right + rect.left - w) >> 1;
            y = (rect.top + rect.bottom - h) >> 1;
            x = stb_clamp(x,0,cx-w);
            y = stb_clamp(y,0,cy-h);
         }
         break;
      }
      case DISPLAY_current:
         if (maximize) {
            x = y = -FRAME;
            w = GetSystemMetrics(SM_CXSCREEN) + FRAME*2;
            h = GetSystemMetrics(SM_CYSCREEN) + FRAME*2;
         } else {
            RECT rect;
            GetAdjustedWindowRect(win, &rect);
            x = rect.left;
            y = rect.top;
            w = rect.right - rect.left;
            h = rect.bottom - rect.top;
         }
         break;
   }

   if (w == w2 && h == h2) {
      int j;
      unsigned char *p = z->pixels;
      imfree(cur);
      free(cur_filename);
      cur = bmp_alloc(z->x + FRAME*2, z->y + FRAME*2);
      display_error[0] = 0;
      cur_filename = strdup(source_c->filename);
      frame(cur);
      {
         for (j=0; j < z->y; ++j) {
            unsigned char *q = cur->pixels + (j+FRAME)*cur->stride + FRAME*BPP;
            memcpy(q, p, z->x*BPP);
            p += z->x*BPP;
         }
      }
      if (!show_frame) x+=FRAME,y+=FRAME,w-=FRAME*2,h-=FRAME*2;
      MoveWindow(win, x,y,w,h, TRUE);
      InvalidateRect(win, NULL, FALSE);
   } else {
      qs.x = x;
      qs.y = y;
      qs.w = w;
      qs.h = h;
   }
}

void toggle_frame(void)
{
   RECT rect;
   show_frame = !show_frame;
   GetWindowRect(win, &rect);
   if (show_frame) {
      rect.left -= FRAME;
      rect.right += FRAME;
      rect.top -= FRAME;
      rect.bottom += FRAME;
   } else {
      rect.left += FRAME;
      rect.right -= FRAME;
      rect.top += FRAME;
      rect.bottom -= FRAME;
   }
   SetWindowPos(win, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOCOPYBITS|SWP_NOOWNERZORDER);
}

void update_source(ImageFile *q)
{
   Image *z = q->image;

   source = z;
   source_c = q;

   if (z)
      size_to_current(FALSE);
}

void toggle_display(void)
{
   if (source) {
      display_mode = (display_mode + 1) % DISPLAY__num;
      size_to_current(TRUE);
   }
}

char path_to_file[4096], *filename;
char **image_files;
int cur_loc = -1;

struct
{
   char *filename;
   int lru;
} *fileinfo;
stb_sdict *file_cache;

void free_fileinfo(void)
{
   int i;
   for (i=0; i < stb_arr_len(fileinfo); ++i)
      free(fileinfo[i].filename);
   stb_arr_free(fileinfo);
   fileinfo = NULL;
}

void init_filelist(void)
{
   char *s = NULL;
   int i;
   if (fileinfo) {
      filename = s = strdup(fileinfo[cur_loc].filename);
      free_fileinfo();
   }

   image_files = stb_readdir_files_mask(path_to_file, "*.jpg;*.jpeg;*.png;*.bmp");
   if (image_files == NULL) error("Error: couldn't read directory.");

   cur_loc = 0;
   stb_arr_setlen(fileinfo, stb_arr_len(image_files));
   for (i=0; i < stb_arr_len(image_files); ++i) {
      fileinfo[i].filename = image_files[i];
      fileinfo[i].lru = 0;      
      if (!stricmp(image_files[i], filename))
         cur_loc = i;
   }
   if (s) free(s);
}


int lru_stamp=1;
int max_cache_bytes = 256 * (1 << 20); // 256 MB; one 5MP image is 20MB

#define MIN_CACHE  3    // always keep 3 images cached, to allow prefetching

int ImageFilePtrCompare(const void *p, const void *q)
{
   ImageFile *a = *(ImageFile **) p;
   ImageFile *b = *(ImageFile **) q;
   return (a->lru < b->lru) ? -1 : (a->lru > b->lru);   
}

void flush_cache(int locked)
{
   int limit;
   volatile ImageFile *list[MAX_CACHED_IMAGES];
   int i, total=0, occupied_slots=0, n=0;
   for (i=0; i < MAX_CACHED_IMAGES; ++i) {
      volatile ImageFile *z = &cache[i];
      if (z->status != LOAD_unused)
         ++occupied_slots;
      if (MAIN_OWNS(z)) {
         if (z->status == LOAD_available) {
            total += z->image->stride * z->image->y;
         } else if (z->status == LOAD_reading_done) {
            total += z->len;
         }
         list[n++] = z;
      }
   }

   limit = MAX_CACHED_IMAGES - MIN_CACHE;
   if (total > max_cache_bytes || occupied_slots > limit) {
      qsort((void *) list, n, sizeof(*list), ImageFilePtrCompare);
      if (!locked) stb_mutex_begin(cache_mutex);
      for (i=0; i < n && occupied_slots > MIN_CACHE && (occupied_slots > limit || total > max_cache_bytes); ++i) {
         ImageFile p;
         /* @TODO: this is totally squirrely and probably buggy. we need to
          * rethink how we can propose things to the disk loader and then
          * later change our minds. is this mutex good enough?
          */
         {
            p.status = list[i]->status;
            if (MAIN_OWNS(&p) && p.status != LOAD_unused) {
               p = *list[i];
               list[i]->bail = 1; // force disk to bail if it gets this -- can't happen?
               list[i]->filename = NULL;
               list[i]->filedata = NULL;
               list[i]->len = 0;
               list[i]->image = NULL;
               list[i]->error = NULL;
               list[i]->status = LOAD_unused;
            }
         }
         if (MAIN_OWNS(&p) && p.status != LOAD_unused) {
            if (!locked) stb_mutex_end(cache_mutex);
o(("MAIN: freeing cache: %s\n", p.filename));
            stb_sdict_remove(file_cache, p.filename, NULL);
            --occupied_slots; // occupied slots
            if (p.status == LOAD_available)
               total -= p.image->stride * p.image->y;
            else if (p.status == LOAD_reading_done)
               total -= p.len;
            free(p.filename);
            if (p.filedata) free(p.filedata);
            if (p.image) imfree(p.image);
            if (p.error) free(p.error);
            if (!locked) stb_mutex_begin(cache_mutex);
         }
      }
      if (!locked) stb_mutex_end(cache_mutex);
o(("Reduced to %d megabytes\n", total >> 20));
   }
}

int wrap(int z)
{
   int n = stb_arr_len(image_files);
   if (z < 0) return z + n;
   while (z >= n) z = z - n;
   return z;
}

void queue_disk_command(DiskCommand *dc, int which, int make_current)
{
   volatile ImageFile *z;

   // check if we already have it cached
   filename = fileinfo[which].filename;
   z = stb_sdict_get(file_cache, filename);
   if (z) {
      // we already have a cache slot for this entry.
      z->lru = fileinfo[which].lru;
      if (!MAIN_OWNS(z)) {
         // it's being loaded/decoded
         return;
      }
      if (z->status == LOAD_reading_done)
         return;
      if (z->status == LOAD_available) {
         if (make_current)
            update_source((ImageFile *) z);
         return;
      }
      if (z->status != LOAD_inactive) {
         if (make_current) {
            set_error(z);
         }
         return;
      }
      // it's a go, use z
   } else {
      int i,tried_again=FALSE;
      // find a cache slot
      for (i=0; i < MAX_CACHED_IMAGES; ++i)
         if (cache[i].status == LOAD_unused)
            break;
      if (i == MAX_CACHED_IMAGES) {
         stb_fatal("Internal logic error: no free cache slots, but flush_cache() should free a few");
         return;
      }
      z = &cache[i];
      free(z->filename);
      assert(z->filedata == NULL);
      z->filename = strdup(filename);
      z->lru = 0;
      z->status = LOAD_inactive;
      stb_sdict_add(file_cache, filename, (void *) z);
   }
   assert(z->status == LOAD_inactive);

o(("MAIN: proposing %s\n", z->filename));
   z->status = LOAD_inactive;
   z->image = NULL;
   z->bail = 0;
   z->lru = fileinfo[which].lru;

   dc->files[dc->num_files++] = (ImageFile *) z;
}


void advance(int dir)
{
   DiskCommand dc;
   int i;
   if (image_files == NULL)
      init_filelist();

   cur_loc = wrap(cur_loc + dir);

   // set adjacent files to previous lru value, so they're 2nd-highest priority
   fileinfo[wrap(cur_loc-1)].lru = lru_stamp;
   fileinfo[wrap(cur_loc+1)].lru = lru_stamp;
   // set this file to new value
   fileinfo[cur_loc].lru = ++lru_stamp;

   // need to grab the cache
   stb_mutex_begin(cache_mutex);
   flush_cache(TRUE);
   dc.num_files = 0;
   queue_disk_command(&dc, cur_loc, 1);           // first thing to load: this file
   if (dir) {
      queue_disk_command(&dc, wrap(cur_loc+dir), 0); // second thing to load: the next file (preload)
      queue_disk_command(&dc, wrap(cur_loc-dir), 0); // last thing to load: the previous file (in case it got skipped when they went fast)
   }

   if (dc.num_files) {
      dc_shared = dc;
      for (i=0; i < dc.num_files; ++i)
         assert(dc.files[i]->filedata == NULL);
      stb_sem_release(disk_command_queue);
   }
   stb_mutex_end(cache_mutex);
   // tell loader not to bother with old data
   for (i=0; i < MAX_CACHED_IMAGES; ++i)
      if (cache[i].lru < lru_stamp-1)
         cache[i].bail = 1;
}

static char filenamebuffer[4096];
void open_file(void)
{
   OPENFILENAME o;
   memset(&o, 0, sizeof(o));
   o.lStructSize = sizeof(o);
   o.lpstrFilter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp\0";
   o.lpstrFile = filenamebuffer;
   filenamebuffer[0] = 0;
   o.nMaxFile = sizeof(filenamebuffer);
   if (!GetOpenFileName(&o))
      return;
   filename = filenamebuffer;
   stb_fixpath(filename);
   stb_splitpath(path_to_file, filename, STB_PATH);
   free_fileinfo();
   init_filelist();
   advance(0);
}


#define int(x)  ((int) (x))

void resize(int step)
{
   // first characterize the current size relative to the raw size
   int x = source->x, y = source->y;
   float s;
   int x2,y2;
   int zoom=0;

   if (cur) {
      if (cur->x > source->x + FRAME*2 || cur->y > source->y + FRAME*2) {
         for(;;) {
            s = (float) pow(2, zoom/2.0f + 0.25f);
            x2 = int(x*s);
            y2 = int(y*s);
            if (cur->x < x2 + FRAME*2 || cur->y < y2 + FRAME*2)
               break;
            ++zoom;
         }
      } else {
         for(;;) {
            s = (float) pow(2, zoom/2.0f - 0.25f);
            x2 = int(x*s);
            y2 = int(y*s);
            if (cur->x > x2 + FRAME*2 || cur->y > y2 + FRAME*2)
               break;
            --zoom;
         }
      }
      // now resize
      do {
         zoom += step;
         s = (float) pow(2, zoom/2.0);
         if (x*s < 4 || y*s < 4 || x*s > 4000 || y*s > 3000)
            return;
         x2 = int(x*s) + 2*FRAME;
         y2 = int(y*s) + 2*FRAME;
      } while (x2 == cur->x || y2 == cur->y);
   } else {
      RECT rect;
      GetAdjustedWindowRect(win, &rect);
      x2 = rect.right - rect.left;
      y2 = rect.bottom - rect.top;
      if (step > 0 && x2 <= 1200 && y2 <= 1024)
         x2 <<= 1, y2 <<= 1;
      if (step < 0 && x2 >= 64 && y2 >= 64)
         x2 >>= 1, y2 >>= 1;

   }

   {
      RECT rect;
      GetAdjustedWindowRect(win, &rect);
      x = (rect.left + rect.right)>>1;
      y = (rect.top + rect.bottom)>>1;
      x -= x2>>1;
      y -= y2>>1;
      enqueue_resize(x,y,x2,y2);
   }

   display_mode = zoom==0 ? DISPLAY_actual : DISPLAY_current;
}

enum
{
   MODE_none,
   MODE_drag,
   MODE_resize,
} dragmode;

#define setmode(x)    (dragmode = x)
#define ismode(x)     (dragmode == x)
#define anymode()     !ismode(MODE_none)

static int ex,ey;   // original mousedown location for snapping to that
static int ex2,ey2; // original mousedown location relative to bottom right
static int wx,wy;
static int rx,ry,rx2,ry2;

static void cursor_regions(int *x0, int *y0, int *x1, int *y1)
{
   RECT rect;
   int w,h,w2,h2;
   GetWindowRect(win, &rect);
   w = rect.right - rect.left;
   h = rect.bottom - rect.top;
   // compute size of handles
   w2 = w >> 4; h2 = h >> 4;
   if (w2 < 12) {
      w2 = w >> 2;
      if (w2 < 4) w2 = w >> 1;
   } else if (w2 > 100) w2 = 100;
   if (h2 < 12) {
      h2 = h >> 2;
      if (h2 < 4) h2 = h >> 1;
   } else if (h2 > 100) h2 = 100;
   if (h2 < w2) w2 = h2;
   if (w2 < h2) h2 = w2;
   *x0 = w2;
   *x1 = w - w2;
   *y0 = h2;
   *y1 = h - h2;
}

HCURSOR c_def, c_ne_sw, c_e_w, c_nw_se, c_n_s;
 
void set_cursor(int x, int y)
{
   int x0,y0,x1,y1;
   cursor_regions(&x0,&y0,&x1,&y1);
        if (x < x0 && y < y0) SetCursor(c_nw_se);
   else if (x > x1 && y > y1) SetCursor(c_nw_se);
   else if (x > x1 && y < y0) SetCursor(c_ne_sw);
   else if (x < x0 && y > y1) SetCursor(c_ne_sw);
   else if (x < x0 || x > x1) SetCursor(c_e_w);
   else if (y < y0 || y > y1) SetCursor(c_n_s);
   else SetCursor(c_def);
}

void mouse(UINT ev, int x, int y)
{
   switch (ev) {
      case WM_LBUTTONDBLCLK:
         toggle_display();
         break;
      case WM_LBUTTONDOWN:
         if (!anymode()) {
            RECT rect;
            int x0,y0,x1,y1;
            cursor_regions(&x0,&y0,&x1,&y1);
            rx = ry = 0;
            if (x < x0) rx = -1;
            if (x > x1) rx =  1;
            if (y < y0) ry = -1;
            if (y > y1) ry =  1;
            if (rx || ry)
               setmode(MODE_resize);
            else
               setmode(MODE_drag);
            SetCapture(win);
            GetAdjustedWindowRect(win, &rect);
            ex = x;
            ey = y;
            ex2 = x - (rect.right-rect.left);
            ey2 = y - (rect.bottom-rect.top);
         }
         break;
      case WM_MOUSEMOVE:
         switch(dragmode) {
            default: assert(0);
            case MODE_none:
               break;
            case MODE_drag: {
               RECT rect;
               GetWindowRect(win, &rect);
               MoveWindow(win, rect.left + x-ex, rect.top + y-ey, rect.right - rect.left, rect.bottom - rect.top, TRUE);
               set_cursor(x,y);
               break;
            }
            case MODE_resize: {
               RECT rect;
               GetAdjustedWindowRect(win, &rect);
               assert(rx || ry);
               display_mode = DISPLAY_current;

               #define LIMIT 16
               if (rx < 0) rect.left   = stb_min(rect.left+x-ex, rect.right-LIMIT);
               if (rx > 0) rect.right  = stb_max(rect.left+LIMIT, rect.left+x-ex2);
               if (ry < 0) rect.top    = stb_min(rect.top+y-ey, rect.bottom-LIMIT);
               if (ry > 0) rect.bottom = stb_max(rect.top+LIMIT, rect.top+y-ey2);
               enqueue_resize(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
               break;
            }
         }
         break;
      case WM_RBUTTONUP:
         if (!anymode())
            exit(0);
         // otherwise, disrupt a modal operation
         /* FALLTHROUGH */
      case WM_LBUTTONUP:
         ReleaseCapture();
         setmode(MODE_none);
         set_cursor(x,y);
         break;
   }
}

#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS  0xbb
#define VK_OEM_MINUS 0xbd
#endif
#ifndef VK_SLASH
#define VK_SLASH     0xbf
#endif

int best_lru = 0;

int downsample_cubic = 0;
int upsample_cubic = 1;


int WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg) {
      case WM_CREATE: {
         win = hWnd;
         break;
      }

      case WM_APP_LOAD_ERROR:
      case WM_APP_DECODE_ERROR:
      {
         int best_lru=0,i;
         volatile ImageFile *best = NULL;
         for (i=0; i < MAX_CACHED_IMAGES; ++i) {
            if (cache[i].lru > best_lru) {
               if (MAIN_OWNS(&cache[i])) {
                  if (cache[i].status >= LOAD_error_reading) {
                     best_lru = cache[i].lru;
                     best = &cache[i];
                  }
               }
            }
         }
         if (best->status == LOAD_error_reading || best->status == LOAD_error_decoding) {
            set_error(best);
         }
         break;
      }

      case WM_APP_DECODED: {
         // scan the filelist for the highest-lru, decoded image
         int i;
         ImageFile *best = NULL;
         for (i=0; i < stb_arr_len(fileinfo); ++i) {
            if (fileinfo[i].lru > best_lru) {
               ImageFile *z = stb_sdict_get(file_cache, fileinfo[i].filename);
               if (z && z->status == LOAD_available) {
                  assert(z->image != NULL);
                  best = z;
                  best_lru = fileinfo[i].lru;
               }
            }
         }
         if (best) {
            update_source(best);
         }
         flush_cache(FALSE);
         break;
      }

      case WM_MOUSEWHEEL: {
         int zdelta = (short) HIWORD(wParam);
         // ignore scaling factor and step 1 by 1
         if (zdelta > 0) resize(1);
         if (zdelta < 0) resize(-1);
         break;
      }
         
      case WM_MOUSEMOVE:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      case WM_LBUTTONDBLCLK:
         mouse(uMsg, (short) LOWORD(lParam), (short) HIWORD(lParam));
         return 0;

      case WM_SETCURSOR: {
         POINT p;
         if (GetCursorPos(&p)) {
            RECT rect;
            GetWindowRect(win, &rect);
            set_cursor(p.x - rect.left, p.y - rect.top);
            return TRUE;
         }
         return FALSE;
      }
      case WM_PAINT: {
         PAINTSTRUCT ps;
         HDC hDC = BeginPaint(hWnd, &ps);
         display(hWnd, hDC);
         EndPaint(hWnd, &ps);
         return 0;
      }

      #define MY_SHIFT (1 << 16)
      #define MY_CTRL  (1 << 17)
      #define MY_ALT   (1 << 18)

      case WM_CHAR: {
         int code = (GetKeyState(VK_SHIFT)   < 0 ? MY_SHIFT : 0)
                 | (GetKeyState(VK_CONTROL) < 0 ? MY_CTRL  : 0);
         code += wParam;
         switch (wParam) {
            case 27:
               exit(0);

            case ' ': // space
               advance(1);
               break;

            case 0x08: // backspace
               advance(-1);
               break;

            case 'l': case 'L':
               show_label = !show_label;
               InvalidateRect(win, NULL, FALSE);
               break;

            default:
               return 1;
         }
         return 0;
      }

      case WM_KEYDOWN:
      case WM_SYSKEYDOWN: {
         int code =(GetKeyState(VK_SHIFT)   < 0 ? MY_SHIFT : 0)
                 | (GetKeyState(VK_CONTROL) < 0 ? MY_CTRL  : 0)
                 | (GetKeyState(VK_MENU   ) < 0 ? MY_ALT  : 0);
         code += wParam;
         switch (code) {
            case VK_RIGHT:
            case VK_NUMPAD6:
               advance(1);
               break;
            case VK_LEFT:
            case VK_NUMPAD4:
               advance(-1);
               break;

            case VK_F1:
            case 'H':
            case 'H' | MY_SHIFT:
            case VK_SLASH:
            case VK_SLASH | MY_SHIFT:
               show_help = !show_help;
               InvalidateRect(win, NULL, FALSE);
               break;

            case 'F' | MY_SHIFT:
               extra_border = !extra_border;
               if (cur) frame(cur);
               InvalidateRect(win, NULL, FALSE);
               break;

            case 'F':
               toggle_frame();
               break;

            case 'F' | MY_CTRL:
               toggle_frame();
               extra_border = show_frame;
               if (cur) frame(cur);
               break;

            case 'C':
               upsample_cubic = !upsample_cubic;
               downsample_cubic = !downsample_cubic;
               break;

            case MY_CTRL | VK_OEM_PLUS:
            case MY_CTRL | MY_SHIFT | VK_OEM_PLUS:
               resize(1);
               break;
            case MY_CTRL | VK_OEM_MINUS:
               resize(-1);
               break;

            case MY_CTRL | 'O':
               open_file();
               break;

            case MY_ALT | '\r':
               toggle_display();
               break;
            default:
               return DefWindowProc (hWnd, uMsg, wParam, lParam);
         }
         break;
      }

      case WM_DESTROY:
         PostQuitMessage (0);
         break;

      default:
         return DefWindowProc (hWnd, uMsg, wParam, lParam);
   }
  
   return 1;
}

int resize_threads;

void ideal_window_size(int w, int h, int *w_ideal, int *h_ideal, int *x, int *y)
{
   int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
   int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

   int cx2 = GetSystemMetrics(SM_CXSCREEN);
   int cy2 = GetSystemMetrics(SM_CYSCREEN);

   if (w <= cx2 && h <= cy2) {
      // if the image fits on the primary monitor, go for it
      *w_ideal = w;
      *h_ideal = h;
   } else if (w - FRAME*2 <= cx2 && h - FRAME*2 <= cy2) {
      // if the image fits on the primary monitor with border
      *w_ideal = w;
      *h_ideal = h;
   } else {
      // will we show more if we use the virtual desktop, rather than just the primary?
      int w1,h1,w2,h2;
      compute_size(cx+FRAME*2 ,cy+FRAME*2,w,h,&w1,&h1);
      compute_size(cx2+FRAME*2,cy2+FRAME*2,w,h,&w2,&h2);
      if (h1 > h2*1.25 || w1 > w2*1.25) {
         *w_ideal = stb_min(cx,w1)+FRAME*2;
         *h_ideal = stb_min(cy,h1)+FRAME*2;
      } else {
         *w_ideal = stb_min(cx2,w2)+FRAME*2;
         *h_ideal = stb_min(cy2,h2)+FRAME*2;
      }
      // compute actual size image will be
      compute_size(*w_ideal, *h_ideal, w,h, &w,&h);
      w += FRAME*2;
      h += FRAME*2;
   }

   if ((cx != cx2 || cy != cy2) && w <= cx2+FRAME*2 && h <= cy2+FRAME*2) {
      *x = (cx2 - w) >> 1;
      *y = (cy2 - h) >> 1;
   } else {
      *x = (cx - w) >> 1;
      *y = (cy - h) >> 1;
   }
}

int cur_is_current(void)
{
   if (!cur_filename) return FALSE;
   if (!source_c || !source_c->filename) return FALSE;
   return !strcmp(cur_filename, source_c->filename);
}

#define MAX_RESIZE   4

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   char filenamebuffer[4096];
   int argc;
   char **argv = stb_tokens_quoted(lpCmdLine, " ", &argc);
   MEMORYSTATUS mem;
   MSG       msg;
   WNDCLASSEX  wndclass;
   HWND      hWnd;
   int physmem;

   int image_x, image_y;
   unsigned char *image_data;
   int image_n;

   resize_threads = stb_processor_count();

   resize_threads = 8;
   if (resize_threads > MAX_RESIZE) resize_threads = MAX_RESIZE;

   #ifdef _DEBUG
   do_debug = IsDebuggerPresent();
   #endif

   GlobalMemoryStatus(&mem);
   physmem = mem.dwTotalPhys;
   max_cache_bytes = physmem / 6;
   if (max_cache_bytes > 256 << 20) max_cache_bytes = 256 << 20;

   strcat(helptext_center, VERSION);

   /* Register the frame class */
   memset(&wndclass, 0, sizeof(wndclass));
   wndclass.cbSize        = sizeof(wndclass);
   wndclass.style         = CS_OWNDC | CS_DBLCLKS;
   wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
   wndclass.hInstance     = hInstance;
   wndclass.hIcon         = LoadIcon(hInstance, szAppName);
   wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
   wndclass.hbrBackground = GetStockObject(BLACK_BRUSH);
   wndclass.lpszMenuName  = szAppName;
   wndclass.lpszClassName = szAppName;
   wndclass.hIconSm       = LoadIcon(hInstance, szAppName);
   c_def = LoadCursor(NULL, IDC_ARROW);
   c_nw_se = LoadCursor(NULL, IDC_SIZENWSE);
   c_ne_sw = LoadCursor(NULL, IDC_SIZENESW);
   c_e_w   = LoadCursor(NULL, IDC_SIZEWE);
   c_n_s   = LoadCursor(NULL, IDC_SIZENS);

   if (!RegisterClassEx(&wndclass))
      return FALSE;

   {
      LOGFONT lf;
      memset(&lf, 0, sizeof(lf));
      lf.lfHeight       = 12;
      lf.lfOutPrecision = OUT_TT_PRECIS; // prefer truetype to raster fonts
      strcpy(lf.lfFaceName, "Times New Roman");
      label_font = CreateFontIndirect(&lf);
   }



   if (argc < 1) {
      OPENFILENAME o;
      memset(&o, 0, sizeof(o));
      o.lStructSize = sizeof(o);
      o.lpstrFilter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp\0";
      o.lpstrFile = filenamebuffer;
      filenamebuffer[0] = 0;
      o.nMaxFile = sizeof(filenamebuffer);
      if (!GetOpenFileName(&o))
         return 0;
      filename = filenamebuffer;
   } else {
      filename = argv[0];
   }
   
   resize_workers = stb_workq_new(resize_threads, resize_threads * 4);
   cache_mutex = stb_mutex_new();
   disk_command_queue = stb_sem_new(1,1);
   decode_queue = stb_sem_new(1,1);
   decode_mutex = stb_mutex_new();

   image_data = stbi_load(filename, &image_x, &image_y, &image_n, BPP);
   if (image_data == NULL) {
      char *why = stbi_failure_reason();
      char buffer[512];
      sprintf(buffer, "'%s': %s", filename, why);
      error(buffer);
      exit(0);
   }
   stb_fixpath(filename);
   stb_splitpath(path_to_file, filename, STB_PATH);
   stb_create_thread(diskload_task, NULL);
   stb_create_thread(decode_task, NULL);

   source = malloc(sizeof(*source));
   make_image(source, image_x, image_y, image_data, image_n);

   cache[0].status = LOAD_available;
   cache[0].image = source;
   cache[0].lru = lru_stamp++;
   cache[0].filename = strdup(filename);
   file_cache = stb_sdict_new(1);
   stb_sdict_add(file_cache, filename, (void *) &cache[0]);
   source_c = (ImageFile *) &cache[0];


   {
      int x,y;
      int w2 = source->x+FRAME*2, h2 = source->y+FRAME*2;
      int w,h;
      ideal_window_size(w2,h2, &w,&h, &x,&y);

      if (w == source->x+FRAME*2 && h == source->y+FRAME*2) {
         display_error[0] = 0;
         cur = bmp_alloc(image_x + FRAME*2, image_y + FRAME*2);
         frame(cur);
         {
            int j;
            unsigned char *p = image_data;
            for (j=0; j < image_y; ++j) {
               unsigned char *q = cur->pixels + (j+FRAME)*cur->stride + FRAME*BPP;
               memcpy(q, p, image_x*BPP);
               p += image_x*BPP;
            }
         }
         w=w;
      } else {
         // size is not an exact match
         queue_resize(w,h, (ImageFile *) &cache[0], TRUE);
         display_error[0] = 0;
         cur = pending_resize.image;
         pending_resize.image = NULL;
      }
      cur_filename = strdup(filename);

      wx = w;
      wy = h;
      hWnd = CreateWindow(szAppName, displayName,
                        WS_POPUP,
                        x,y, w, h,
                        NULL, NULL, hInstance, NULL);
   }

   if (!hWnd)
      return FALSE;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   InvalidateRect(hWnd, NULL, TRUE);

   for(;;) {
      // if we're not currently resizing, start a resize
      if (qs.w && pending_resize.size.w == 0) {
         if (source) {
            if (cur_is_current() && (!cur || (qs.w == cur->x && qs.h >= cur->y) || (qs.h == cur->y && qs.w >= cur->x))) {
               // no resize necessary, just a variant of the current shape
               MoveWindow(win, qs.x,qs.y,qs.w,qs.h, TRUE);
               InvalidateRect(win, NULL, FALSE);
            } else {
               o(("Enqueueing resize\n"));
               pending_resize.size = qs;
               queue_resize(qs.w, qs.h, source_c, FALSE);
            }
         }
         qs.w = 0;
      }

      if (!PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {
         // no messages, so check for pending activity
         if (pending_resize.size.w) {
            // there's a resize pending, so don't block
            if (!pending_resize.image) {
               Sleep(10);
            } else {
               HDC hdc;
               o(("Finished resize\n"));
               imfree(cur);
               pending_resize.image_c->status = LOAD_available;
               cur = pending_resize.image;
               display_error[0] = 0;
               cur_filename = pending_resize.filename;
               pending_resize.filename = NULL;
               if (!show_frame) {
                  pending_resize.size.x += FRAME;
                  pending_resize.size.y += FRAME;
                  pending_resize.size.w -= FRAME*2;
                  pending_resize.size.h -= FRAME*2;
               }
               SetWindowPos(hWnd,NULL,pending_resize.size.x, pending_resize.size.y, pending_resize.size.w, pending_resize.size.h, SWP_NOZORDER);
               barrier();
               pending_resize.size.w = 0;
               hdc = GetDC(win);
               display(hWnd, hdc);
               ReleaseDC(win, hdc);
            }
            continue;
         }
      }

      if (!GetMessage(&msg, NULL, 0, 0))
         return msg.wParam;

      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}


#define MAGIC   (1.5 * (1 << 26) * (1 << 26))
double temp;
#define FAST_FLOAT_TO_INT(x) ((q->temp = (x) + MAGIC), *(int *)&q->temp)

#define toint(x)  ((int) (x)) // FAST_FLOAT_TO_INT(x)

typedef struct {
   short i;
   unsigned char f;
} SplitPoint;

SplitPoint point_buffer[3200];

typedef struct
{
   double temp;
   Image *dest;
   Image *src;
   SplitPoint *p;
   int j0,j1;
   float dy;
   int done;
} ImageProcess;

#define CACHE_REBLOCK  64
void *image_resize_work(ImageProcess *q)
{
   int i,j,k,n=0;
   Image *dest = q->dest, *src = q->src;
   SplitPoint *p = q->p;
   float y, y0 = q->dy * q->j0;
   for (k=0; k < dest->x; k += CACHE_REBLOCK) {
      int k2 = stb_min(k + CACHE_REBLOCK, dest->x);
      y = y0;
      for (j=q->j0; j < q->j1; ++j) {
         int iy;
         int fy;
         y = q->dy * j;
         iy = (int) floor(y);
         fy = (int) floor(255.9f*(y - iy));
         if (iy >= src->y-1) {
            iy = src->y-2;
            fy = 255;
         }
         {
            unsigned char *d = &dest->pixels[j*dest->stride + k*BPP];
            unsigned char *s0 = src->pixels + src->stride*iy;
            unsigned char *s1 = s0 + src->stride;
            for (i=k; i < k2; ++i) {
               s0 += p[i].i;
               s1 += p[i].i;
               {
                  unsigned char x = p[i].f;

                  #if BPP == 4
                  uint32 c00,c01,c10,c11,rb0,rb1,rb00,rb01,rb10,rb11,rb,g;
                  c00 = *(uint32 *) s0;
                  c01 = *(uint32 *) (s0+4);
                  c10 = *(uint32 *) s1;
                  c11 = *(uint32 *) (s1+4);

                  rb00 = c00 & 0xff00ff;
                  rb01 = c01 & 0xff00ff;
                  rb0 = (rb00 + (((rb01 - rb00) * x) >> 8)) & 0xff00ff;
                  rb10 = c10 & 0xff00ff;
                  rb11 = c11 & 0xff00ff;
                  rb1 = (rb10 + (((rb11 - rb10) * x) >> 8)) & 0xff00ff;
                  rb = (rb0 + (((rb1 - rb0) * fy) >> 8)) & 0xff00ff;

                  rb00 = c00 & 0xff00;
                  rb01 = c01 & 0xff00;
                  rb0 = (rb00 + (((rb01 - rb00) * x) >> 8)) & 0xff00;
                  rb10 = c10 & 0xff00;
                  rb11 = c11 & 0xff00;
                  rb1 = (rb10 + (((rb11 - rb10) * x) >> 8)) & 0xff00;
                  g = (rb0 + (((rb1 - rb0) * fy) >> 8)) & 0xff00;

                  *(uint32 *)d = rb + g;
                  #else
                  unsigned char v00,v01,v10,v11;
                  int v0,v1;

                  v00 = s0[0]; v01 = s0[BPP+0]; v10 = s1[0]; v11 = s1[BPP+0];
                  v0 = (v00<<8) + x * (v01 - v00);
                  v1 = (v10<<8) + x * (v11 - v10);
                  v0 = (v0<<8) + fy * (v1 - v0);
                  d[0] = v0 >> 16;

                  v00 = s0[1]; v01 = s0[BPP+1]; v10 = s1[1]; v11 = s1[BPP+1];
                  v0 = (v00<<8) + x * (v01 - v00);
                  v1 = (v10<<8) + x * (v11 - v10);
                  v0 = (v0<<8) + fy * (v1 - v0);
                  d[1] = v0 >> 16;

                  v00 = s0[2]; v01 = s0[BPP+2]; v10 = s1[2]; v11 = s1[BPP+2];
                  v0 = (v00<<8) + x * (v01 - v00);
                  v1 = (v10<<8) + x * (v11 - v10);
                  v0 = (v0<<8) + fy * (v1 - v0);
                  d[2] = v0 >> 16;
                  #endif

                  d += BPP;
               }
            }
         }
         y += q->dy;
      }
   }
   q->done = TRUE;
   return NULL;
}

void image_resize_old(Image *dest, Image *src)
{
   ImageProcess proc_buffer[16], *q = stb_temp(proc_buffer, resize_threads * sizeof(*q));
   SplitPoint *p = stb_temp(point_buffer, dest->x * sizeof(*p));
   int i,j0,j1,k;
   float x,dx,dy;
   assert(src->frame == 0);
   dx = (float) (src->x - 1) / (dest->x - 1);
   dy = (float) (src->y - 1) / (dest->y - 1);
   x=0;
   for (i=0; i < dest->x; ++i) {
      p[i].i = (int) floor(x);
      p[i].f = (int) floor(255.9f*(x - p[i].i));
      if (p[i].i >= src->x-1) {
         p[i].i = src->x-2;
         p[i].f = 255;
      }
      x += dx;
      p[i].i *= BPP;
   }
   for (k=0; k < dest->x; k += CACHE_REBLOCK) {
      int k2 = stb_min(k+CACHE_REBLOCK, dest->x);
      for (i=k2-1; i > k; --i) {
         p[i].i -= p[i-1].i;
      }
   }
   j0 = 0;
   for (i=0; i < resize_threads; ++i) {
      j1 = dest->y * (i+1) / resize_threads;
      q[i].dest = dest;
      q[i].src = src;
      q[i].j0 = j0;
      q[i].j1 = j1;
      q[i].dy = dy;
      q[i].p = p;
      q[i].done = FALSE;
      j1 = j0;
   }

   if (resize_threads == 1) {
      image_resize_work(q);
   } else {
      barrier();
      for (i=1; i < resize_threads; ++i)
         stb_workq(resize_workers, image_resize_work, q+i, NULL);
      image_resize_work(q);

      for(;;) {
         for (i=1; i < resize_threads; ++i)
            if (!q[i].done)
               break;
         if (i == resize_threads) break;
         Sleep(10);
      }
   }

   stb_tempfree(point_buffer, p);
   stb_tempfree(proc_buffer , q);
}

#if BPP==4
//

#undef R
#undef G
#undef B
#undef A
#undef RGB
#undef RGBA

#define R(x) ( (x)        & 0xff)
#define G(x) (((x) >>  8) & 0xff)
#define B(x) (((x) >> 16) & 0xff)
#define A(x) (((x) >> 24) & 0xff)
#define RGBA(r,g,b,a) (((a) << 24) + ((b) << 16) + ((g) << 8) + (r))
#define RGB(r,g,b)    RGBA(r,g,b,0)

typedef uint32 Color;

// lerp() is just blend() that also "blends" alpha
// put a/256 of src over dest, including alpha
// again, cannot be used for a=256
static Color lerp(Color dest, Color src, uint8 a)
{
   int rb_src  = src  & 0xff00ff;
   int rb_dest = dest & 0xff00ff;
   int rb      = rb_dest + ((rb_src - rb_dest) * a >> 8);
   int ga_src  = (src  & 0xff00ff00) >> 8;
   int ga_dest = (dest & 0xff00ff00) >> 8;
   int ga      = (ga_dest<<8) + (ga_src - ga_dest) * a;
   return (rb & 0xff00ff) + (ga & 0xff00ff00);
}

#if 1


#define SSE __declspec(align(16))
#define MMX __declspec(align(8))

//   out = a * t^3 + b*t^2 + c*t + d
//   out = (a*t+b)*t^2 + (c*t+d)*1

MMX int16 three[4] = { 3,3,3,3 };

static void cubicRGBA(uint32 *dest, uint32 *x0, uint32 *x1, uint32 *x2, uint32 *x3, int lerp8, int step_dest, int step_src, int len)
//static uint32 cubicRGBA(uint32 x0, uint32 x1, uint32 x2, uint32 x3, int lerp8)
{
   if (len <= 0) return;
   __asm {
      // these save/restores shouldn't be necessary... but they seem to be in VC6 opt builds
      // buggy compiler, or I'm doing something wrong
      push eax
      push ebx
      push ecx
      push edx
      push esi
      push edi
      mov   edi,dest
      mov   eax,x0
      mov   ebx,x1
      mov   ecx,x2
      mov   edx,x3
      pxor  mm0,mm0
      movd  mm7,lerp8
      mov   esi,len
      punpcklbw mm7,mm7   // 0,0,0,0,0,0,lerp,lerp
      punpcklbw mm7,mm7   // 0,0,0,0,lerp,lerp,lerp,lerp
      punpcklbw mm7,mm7   // 8xlerp. (This meakes each unsigned lerp value 0..15)
      psrlw     mm7,1     // slide away from the sign bit; 1.15 lerp
   } looptop: __asm {

      movd  mm1,[eax]
      movd  mm4,[edx]
      movd  mm2,[ebx]
      movd  mm3,[ecx]
      add       eax,step_src
      add       ebx,step_src
      punpcklbw mm1,mm0   // mm1 = x0
      punpcklbw mm4,mm0   // mm4 = x3
      punpcklbw mm2,mm0   // mm2 = x1
      punpcklbw mm3,mm0   // mm3 = x2

      add       ecx,step_src
      add       edx,step_src
#if 1
      // catmull-rom cubic
      // "scheduled" to try to spread stuff out early
      // also the final shift by two has been optimized up earlier
      // (which means we really only get 6-7 good bits)
      psubw     mm4,mm1   // mm4 = x3-x0
      movq      mm5,mm2   // mm5 = x1
      movq      mm6,mm3   // mm6 = x2
      psubw     mm5,mm3   // mm5 = x1-x2
      paddw     mm3,mm1   // mm3 = x0+x2
      psubw     mm6,mm1   // mm6 = c
      psubw     mm3,mm2   // mm3 = x0+x2-d/2
      pmullw    mm5,three // mm5 = 3*(x1-x2)
      psubw     mm3,mm2   // mm3 = x0+x2-d
      pmulhw    mm6,mm7   // mm6 = c*t
      paddw     mm5,mm4   // mm5 = a
      psubw     mm3,mm5   // mm3 = b

      psllw     mm5,2     // mm5 = a(15.1)
      psllw     mm3,1     // mm3 = b
      pmulhw    mm5,mm7   // mm5 = a*t
      paddw     mm6,mm2   // mm6 = c*t+d
      paddw     mm5,mm3   // mm5 = a*t + b
      pmulhw    mm5,mm7   // mm5 = a*t^2+b*t
      pmulhw    mm5,mm7   // mm5 = a*t^3+b*t^2
      paddw     mm5,mm6
      packuswb  mm5,mm5
      movd      [edi],mm5
#else
      // unknown spline type from: http://local.wasp.uwa.edu.au/~pbourke/other/interpolation/
      psubw     mm4,mm3   // mm4 = x3-x2
      psubw     mm4,mm1   // mm4 = x3-x2-x0
      paddw     mm4,mm2   // mm4 = a0 = x3-x2-x0+x1
      psubw     mm3,mm1   // mm3 = a2 = x2-x0
      psubw     mm1,mm2   // mm1 = x0-x1
      psubw     mm1,mm4   // mm1 = a1 = x0-x1-a0
      // mm2 = a3 = y1
      psllw     mm4,3
      pmulhw    mm4,mm7
      pmulhw    mm4,mm7
      pmulhw    mm4,mm7
      psllw     mm1,2
      pmulhw    mm1,mm7
      pmulhw    mm1,mm7
      psllw     mm3,1
      pmulhw    mm3,mm7
      paddw     mm1,mm2
      paddw     mm1,mm3
      paddw     mm1,mm4
      packuswb  mm1,mm1
      movd      [edi],mm1
#endif
      add       edi,step_dest
      dec       esi
      jnz       looptop
      emms
      pop edi
      pop esi
      pop edx
      pop ecx
      pop ebx
      pop eax
   }
}

#else
static int cubic(int x0, int x1, int x2, int x3, int lerp8)
{
   int a = 3*(x1-x2) + (x3-x0);
   int d = x1+x1;
   int c = x2 - x0;
   int b = -a-d + x0+x2;

   int res = a * lerp8 + (b << 8);
   res = (res * lerp8);
   res = ((res >> 16) + c) * lerp8;
   res = ((res >> 8) + d) >> 1;
   if (res < 0) res = 0; else if (res > 255) res = 255;
   return res;
}

static void cubicRGBA(Color *dest, Color *x0, Color *x1, Color *x2, Color *x3, int lerp8, int step_dest, int step_src, int len)
{
   int i;
   for (i=0; i < len; ++i) {
      int r,g,b,a;
      r = cubic(R(*x0),R(*x1),R(*x2),R(*x3),lerp8);
      g = cubic(G(*x0),G(*x1),G(*x2),G(*x3),lerp8);
      b = cubic(B(*x0),B(*x1),B(*x2),B(*x3),lerp8);
      a = cubic(A(*x0),A(*x1),A(*x2),A(*x3),lerp8);
      *dest = RGBA(r,g,b,a);
      x0 += step_src>>2;
      x1 += step_src>>2;
      x2 += step_src>>2;
      x3 += step_src>>2;
      dest += step_dest>>2;
   }
}
#endif

#define PLUS(x,y)   ((uint32 *) ((uint8 *) (x) + (y)))

struct
{
   Image *src;
   Image *out;
   int out_len;
   int delta;
} cubic_work;

#define CUBIC_BLOCK  32
void * grCubicScaleBitmapX_work(int n)
{
   int out_w = cubic_work.out_len;
   int x,dx,i,j,k,k_start, k_end;
   Image *out = cubic_work.out;
   Image *src = cubic_work.src;
   dx = cubic_work.delta;
   k_start = out->y *   n   / resize_threads;
   k_end   = out->y * (n+1) / resize_threads;
   for (k=k_start; k < k_end; k += CUBIC_BLOCK) {
      int k2 = stb_min(k+CUBIC_BLOCK, k_end);
      x = 0;
      for (i=0; i < out_w; ++i) {
         uint32 *data = (uint32 *) (src->pixels + k*src->stride);
         uint32 *dest = (uint32 *) (out->pixels + k*out->stride) + i;
         int xp = (x >> 16);
         int xw = (x >> 8) & 255;
         if (xp == 0) {
            cubicRGBA(dest, data+xp,data+xp,data+xp+1,data+xp+2,xw,out->stride,src->stride,k2-k);
         } else if (xp >= src->x - 2) {
            if (xp == src->x-1) {
               for (j=k; j < k2; ++j) {
                  dest[0] = data[xp];
                  data = PLUS(data, src->stride);
                  dest = PLUS(dest , out->stride);
               }
            } else {
               cubicRGBA(dest, data+xp-1,data+xp,data+xp+1,data+xp+1,xw,out->stride,src->stride,k2-k);
            }
         } else {
            cubicRGBA(dest, data+xp-1,data+xp,data+xp+1,data+xp+2,xw,out->stride,src->stride,k2-k);
         }
         x += dx;
      }
   }
   barrier();
   return NULL;
}

Image *grCubicScaleBitmapX(Image *src, int out_w)
{
   int i;
   cubic_work.out = bmp_alloc(out_w, src->y);
   cubic_work.delta = (src->x-1)*65536 / (out_w-1);
   cubic_work.src = src;
   cubic_work.out_len = out_w;
   barrier();

   if (resize_threads == 1) {
      grCubicScaleBitmapX_work(0);
   } else {
      volatile void *which[MAX_RESIZE];
      for (i=0; i < resize_threads; ++i)
         which[i] = (void *) 1;
      barrier();
      for (i=1; i < resize_threads; ++i)
         stb_workq(resize_workers, (stb_thread_func) grCubicScaleBitmapX_work, (void *) i, which+i);
      grCubicScaleBitmapX_work(0);

      for(;;) {
         for (i=1; i < resize_threads; ++i)
            if (which[i])
               break;
         if (i == resize_threads) break;
         Sleep(10);
      }
   }
   return cubic_work.out;
}

Image *grCubicScaleBitmapY_work(int n)
{
   int y,dy,j,j_end;
   int out_h = cubic_work.out_len;
   Image *src = cubic_work.src;
   Image *out = cubic_work.out;
   dy = cubic_work.delta;

   j = out_h * n / resize_threads;
   j_end = out_h * (n+1) / resize_threads;
   y = j * dy;
   for (; j < j_end; ++j,y+=dy) {
      uint32 *dest  = (uint32 *) (out->pixels + j*out->stride);
      int yp = (y >> 16);
      uint8 yw = (y >> 8);
      uint32 *data1 = (uint32 *) (src->pixels + yp*src->stride);
      uint32 *data2 = PLUS(data1,src->stride);
      uint32 *data0 = (yp > 0) ? PLUS(data1, - src->stride) : data1;
      uint32 *data3 = (yp < src->y-2) ? PLUS(data2, src->stride) : data2;
      cubicRGBA(dest, data0, data1, data2, data3, yw, 4,4,out->x);
   }
   return NULL;
}

Image *grCubicScaleBitmapY(Image *src, int out_h)
{
   int i;
   cubic_work.src = src;
   cubic_work.out = bmp_alloc(src->x, out_h);
   cubic_work.delta = ((src->y-1)*65536-1) / (out_h-1);
   cubic_work.out_len = out_h;
   barrier();

   if (resize_threads == 1) {
      grCubicScaleBitmapY_work(0);
   } else {
      volatile void *which[MAX_RESIZE];
      for (i=0; i < resize_threads; ++i)
         which[i] = (void *) 1;
      barrier();
      for (i=1; i < resize_threads; ++i)
         stb_workq(resize_workers, (stb_thread_func) grCubicScaleBitmapY_work, (void *) i, which+i);
      grCubicScaleBitmapY_work(0);

      for(;;) {
         for (i=1; i < resize_threads; ++i)
            if (which[i])
               break;
         if (i == resize_threads) break;
         Sleep(10);
      }
   }
   return cubic_work.out;
}

// downsampling
Image *grScaleBitmapOneHalf(Image *src)
{
   int i,j, w,h;
   Image *res;

   w = src->x>>1;
   h = src->y>>1;

   res = bmp_alloc(w,h);
   for (j=0; j < h; j += 1) {
      Color *src0 = (uint32*)(src->pixels + 2*j * src->stride);
      Color *src1 = PLUS(src0, src->stride);
      for (i=0; i < w; i += 1) {
         Color *dest = (uint32*)(res->pixels + j * res->stride + i*BPP);
         // this will cause quantization of flat-colored regions, thus can
         // cause banding in very slow gradients
         *dest = ((src0[0] >> 2) & 0x3f3f3f3f) +
                 ((src0[1] >> 2) & 0x3f3f3f3f) +
                 ((src1[0] >> 2) & 0x3f3f3f3f) +
                 ((src1[1] >> 2) & 0x3f3f3f3f);
         src0 += 2;
         src1 += 2;
      }
   }

   return res;
}

Image *grScaleBitmapTwoThirds(Image *src)
{
   int i,j, w,h;
   Image *res;

   w = src->x/3 * 2;
   h = src->y/3 * 2;

   res = bmp_alloc(w, h);
   for (j=0; j+1 < h; j += 2) {
      Color *src0 = (uint32*)(src->pixels + 3*(j>>1) * src->stride);
      Color *src1 = PLUS(src0, src->stride);
      Color *src2 = PLUS(src1, src->stride);
      // use (2/3,1/3) and (1/3,2/3), which amounts to:
      //    A B C     W  X
      //    D E F  -> 
      //    G H I     Y  Z

      // W = A*4/9 + B * 2/9 + D * 2/9 + E * 1/9
      // for speed, approximate as A*3/8 + B*2/8 + D*2/8 + E*1/8
      for (i=0; i+1 < w; i += 2) {
         Color *dest = (uint32*)(res->pixels + j * res->stride + i*BPP);
         dest[0] = ((src0[0] >> 1) & 0x7f7f7f7f) - ((src0[0] >> 3) & 0x1f1f1f1f)
                 + ((src0[1] >> 2) & 0x3f3f3f3f) + ((src1[0] >> 2) & 0x3f3f3f3f)
                 + ((src1[1] >> 3) & 0x1f1f1f1f);
         dest[1] = ((src0[2] >> 1) & 0x7f7f7f7f) - ((src0[2] >> 3) & 0x1f1f1f1f)
                 + ((src0[1] >> 2) & 0x3f3f3f3f) + ((src1[2] >> 2) & 0x3f3f3f3f)
                 + ((src1[1] >> 3) & 0x1f1f1f1f);
         dest = PLUS(dest,res->stride);
         dest[0] = ((src2[0] >> 1) & 0x7f7f7f7f) - ((src2[0] >> 3) & 0x1f1f1f1f)
                 + ((src2[1] >> 2) & 0x3f3f3f3f) + ((src1[0] >> 2) & 0x3f3f3f3f)
                 + ((src1[1] >> 3) & 0x1f1f1f1f);
         dest[1] = ((src2[2] >> 1) & 0x7f7f7f7f) - ((src2[2] >> 3) & 0x1f1f1f1f)
                 + ((src2[1] >> 2) & 0x3f3f3f3f) + ((src1[2] >> 2) & 0x3f3f3f3f)
                 + ((src1[1] >> 3) & 0x1f1f1f1f);
         src0 += 3;
         src1 += 3;
         src2 += 3;         
      }
   }

   return res;
}

Image *grScaleBitmap(Image *src, int gx, int gy, Image *dest)
{
   Image *to_free, *res;
   int upsample=FALSE;
   to_free = NULL;

   // check if we're scaling up
   if (gx > src->x || gy > src->y)  {
      upsample = TRUE;
   } else {
      // maybe should do something smarter here, like find the
      // nearest box size, instead of repetitive powers of two
      while (gx <= (src->x >> 1) && gy <= (src->y >> 1)) {
         src = grScaleBitmapOneHalf(src);
         if (to_free) imfree(to_free);
         to_free = src;
      }

      if (gx < src->x * 0.666666f && gy < src->y * 0.666666f) {
         src = grScaleBitmapTwoThirds(src);
         if (to_free) imfree(to_free);
         to_free = src;
      }
   }

   if (gx == src->x && gy == src->y) {
      if (to_free)
         res = src;
      else {
         res = bmp_alloc(src->x, src->y);
         memcpy(res->pixels, src->pixels, res->y * res->stride);
         return res;
      }
   } else if (upsample ? upsample_cubic : downsample_cubic) {
      res = grCubicScaleBitmapY(src, gy);
      if (to_free) imfree(to_free);
      to_free = res;
      res = grCubicScaleBitmapX(res, gx);
      imfree(to_free);
    } else {
      #if 1
      image_resize_old(dest, src);
      if (to_free) imfree(to_free);
      res = NULL;
      #else
      res = grScaleBitmapX(src, gx);
      if (to_free) imfree(to_free);
      to_free = res;
      res = grScaleBitmapY(res, gy);
      imfree(to_free);
      #endif
   }
   return res;
}
#endif // BPP==4

void image_resize(Image *dest, Image *src)
{
#if BPP==3
   image_resize_old(dest, src);
#else
   int j;
   Image *temp;
   temp = grScaleBitmap(src, dest->x, dest->y, dest);
   if (temp) {
      for (j=0; j < dest->y; ++j)
         memcpy(dest->pixels + j*dest->stride, temp->pixels + j*temp->stride, BPP*dest->x);
      imfree(temp);
   }
#endif
}
