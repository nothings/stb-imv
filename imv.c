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


// Set section alignment to minimize alignment overhead

#if (_MSC_VER < 1300)
#pragma comment(linker, "/FILEALIGN:0x200")
#endif

#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <process.h>
#include <math.h>

#define STB_DEFINE
#include "stb.h"          /*     http://nothings.org/stb.h         */

// general configuration options

#ifndef USE_STBI
#define USE_STBI 1
#endif

#ifndef USE_GDIPLUS
#define USE_GDIPLUS 1
#endif

#ifndef USE_FREEIMAGE
#define USE_FREEIMAGE 1
#endif

#ifndef ALLOW_RECOLORING
#define ALLOW_RECOLORING 0
#endif

//#define MONO2

// implement USE_STBI

#if USE_STBI
#define STBI_FAILURE_USERMSG
#define STBI_NO_STDIO
#define STBI_NO_WRITE
#include "stb_image.c"    /*     http://nothings.org/stb_image.c   */
#endif 

#include "resource.h"

typedef int Bool;


// size of border in pixels
#define FRAME   3

// location within frame of secondary border
#define FRAME2  (FRAME >> 1)

// color of secondary border
#define GREY  192



Bool do_show;
float delay_time = 4;
int nearest_neighbor = 0; // internal use

// all programs get the version number from the same place: version.bat
#define set   static char *
#include "version.bat"
;
#undef set

// trivial error handling
void error(char *str) { MessageBox(NULL, str, "imv(stb) error", MB_OK); }

// OutputDebugString with varargs, can be compiled out
#ifdef _DEBUG
int do_debug=1;
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
// if 'dim' is set, draw it darkened
void platformDrawBitmap(HDC hdc, int x, int y, unsigned char *bits, int w, int h, int stride, int dim)
{
   int i;
   BITMAPINFOHEADER b = { sizeof(b) };
   int result;

   b.biPlanes=1;
   b.biBitCount=BPP*8;
   b.biWidth = stride/BPP;
   b.biHeight = -h;  // tell windows the bitmap is stored top-to-bottom

   if (dim)
      // divide the brightness of each channel by two... (if BPP==3, this
      // does 4 pixels every 3 iterations)
      for (i=0; i < stride*h; i += 4)
         *(uint32 *)(bits+i) = (*(uint32 *)(bits+i) >> 1) & 0x7f7f7f7f;

   result = SetDIBitsToDevice(hdc, x,y, w,abs(h), 0,0, 0,abs(h), bits, (BITMAPINFO *) &b, DIB_RGB_COLORS);
   if (result == 0) {
      DWORD e = GetLastError();
   }
   // bug: we restore by shifting, so we've discarded the bottom bit;
   // thus, once you've viewed the help and come back, the display
   // is slightly wrong until you resize or switch images. so we should
   // probably save and restore it instead... slow, but we're displaying
   // the help so no big deal?
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
   int had_alpha;   // did this have alpha and we statically overwrote it?
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
stb_sync resize_merge;

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

static unsigned char alpha_background[2][3] =
{
   { 200,40,200 },
   { 150,30,150 },
};

// given raw decoded data from stbi_load, make it into a proper Image (e.g. creating a
// windows-compatible bitmap with 4-byte aligned rows and BGR color order)
#if ALLOW_RECOLORING
float lmin=0,lmax=1;
int mono;
#endif
void make_image(Image *z, int image_x, int image_y, uint8 *image_data, BOOL image_loaded_as_rgb, int image_n)
{
   #ifdef ALLOW_RECOLORING
   int ms,md, ns,nd;
   #endif
   int i,j,k,ymin=0,ymax=256*8-1;
   z->pixels = image_data;
   z->x = image_x;
   z->y = image_y;
   z->stride = image_x*BPP;
   z->frame = 0;
   z->had_alpha = (image_n==4);

   if (z->had_alpha) {
      int n=0;
      for (n=0; n < image_x * image_y; ++n)
         if (image_data[n*4+3])
            break;
      if (n == image_x * image_y) {
         // all alpha is 0, so force to 255
         for (n=0; n < image_x * image_y; ++n)
            image_data[n*4+3] = 255;
      }
   }

   #if ALLOW_RECOLORING
   if (mono) {
      k = 0;
      for (j=0; j < image_y; ++j) {
         for (i=0; i < image_x; ++i) {
            int y = image_data[k+0]*5 + image_data[k+1]*9 + image_data[k+2]*2;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
            k += BPP;
         }
      }
   }
   #endif

   #if ALLOW_RECOLORING
   if (lmin > 0)
      ms = (int) (lmin * 255), md=0;
   else
      ms = 0, md = (int)(-lmin*255);
   if (lmax < 1)
      ns = (int)(lmax * 255), nd=255;
   else
      ns = 255, nd = (int) ((2-lmax)*255);
   if (ns <= ms)
      ns = ms+1;
   if (nd < md)
      nd = md+1;
   if (ns == 256) --ns,--ms;
   if (nd == 256) --nd,--md;
   #endif

   k=0;
   for (j=0; j < image_y; ++j) {
      for (i=0; i < image_x; ++i) {
         // TODO: hoist branches outside of loops?
         if (image_loaded_as_rgb) {
            // swap RGB to BGR
            unsigned char t = image_data[k+0];
            image_data[k+0] = image_data[k+2];
            image_data[k+2] = t;
         }

         #if ALLOW_RECOLORING
         if (mono) {
#ifdef MONO2
            int p = image_data[k+2];
#else
            int y = image_data[k+0]*5 + image_data[k+1]*9 + image_data[k+2]*2;
            int p = (int) stb_linear_remap(y, ymin, ymax, 0,255);
#endif
            image_data[k+0] = p;
            image_data[k+1] = p;
            image_data[k+2] = p;
         } else if (lmin != 0 || lmax != 1) {
            int c;
            for (c=0; c < 3; ++c) {
               int z = (int) stb_linear_remap(image_data[k+c], ms,ns, md,nd);
               image_data[k+c] = stb_clamp(z, 0, 255);
            }
         }
         #endif

         #if BPP==4
         // if image had an alpha channel, pre-blend with background
         if (image_n == 4) {
            unsigned char *p = image_data+k;
            int a = (255-p[3]);
            if ((i ^ j) & 8) {
               p[0] += (((alpha_background[0][2] - (int) p[0])*a)>>8);
               p[1] += (((alpha_background[0][1] - (int) p[1])*a)>>8);
               p[2] += (((alpha_background[0][0] - (int) p[2])*a)>>8);
            } else {
               p[0] += (((alpha_background[1][2] - (int) p[0])*a)>>8);
               p[1] += (((alpha_background[1][1] - (int) p[1])*a)>>8);
               p[2] += (((alpha_background[1][0] - (int) p[2])*a)>>8);
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
      // lru priority and other such, so not be the best anymore, but
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

static uint8 *imv_decode_from_memory(uint8 *mem, int len, int *x, int *y, BOOL *loaded_as_rgb, int *n, int n_req, char *filename);
static char  *imv_failure_reason(void);

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
         int x,y,loaded_as_rgb,n;
         uint8 *data;
         assert(f->status == LOAD_decoding);

         // decode image
         o(("DECIDE: decoding %s\n", f->filename));
         data = imv_decode_from_memory(f->filedata, f->len, &x, &y, &loaded_as_rgb, &n, BPP, f->filename);
         o(("DECODE: decoded %s\n", f->filename));

         // free copy of data from disk, which we don't need anymore
         free(f->filedata);
         f->filedata = NULL;

         if (data == NULL) {
            // error reading file, record the reason for it
            f->error = strdup(imv_failure_reason());
            barrier();
            f->status = LOAD_error_reading;
            // wake up the main thread in case this is the most recent image
            wake(WM_APP_DECODE_ERROR);
         } else {
            // post-process the image into the right format
            f->image = (Image *) malloc(sizeof(*f->image));
            make_image(f->image, x, y,data, loaded_as_rgb, n);
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
   i->had_alpha = 0;
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

// free an image and its contents
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
int downsample_cubic = TRUE;
int upsample_cubic = TRUE;

int cur_loc = -1; // offset within the current list of files

// information about files we have currently loaded
struct
{
   char *filename;
   int lru;
} *fileinfo;

// declare with extra bytes so we can print the version number into it
char helptext_center[150] =
   "imv(stb)\n"
   "Copyright 2007 Sean Barrett\n"
   "http://code.google.com/p/stb-imv\n"
   "version "
;

char helptext_left[] =
   "\n\n\n\n\n\n\n"
   " ALT-ENTER: toggle size\n"
   " CTRL-PLUS: zoom in\n"
   "CTRL-MINUS: zoom out\n"
   "RIGHT, SPACE: next image\n"
   "LEFT, BACKSPACE: previous image\n"
   "      O: open image\n"
   "      B: toggle border\n"
   "SHIFT-B: toggle border but keep stripe\n"
   " CTRL-B: toggle white stripe in border\n"
   "      L: toggle filename label\n"
   "      S: slideshow in current directory\n"
   " CTRL-S: sharpen when upscaling\n"
;

char helptext_right[] =
   "\n\n\n\n\n\n\n"
   "right-click to exit\n"
   "left drag center to move\n"
   "left drag edges to resize\n"
   "double-click to toggle size\n"
   "mousewheel to zoom\n"
   "\n"
   "F1, H, ?: help\n"
   "ESC: exit\n"
   "P: change preferences\n"
   "CTRL-C: copy filename to clipboard\n"
   "CTRL-I: launch new viewer instance\n"
;

// draw the help text semi-prettily
// originally this was to try to avoid having to darken the image
// that it's rendered over, but I couldn't make that work, and with
// the darkened image there's no real need to do this, but hey, it
// looks a little nicer so why not
void draw_nice(HDC hdc, char *text, RECT *rect, uint flags)
{
   int i,j;
   SetTextColor(hdc, RGB(80,80,80));
   for (i=2; i >= 1; i -= 1)
   for (j=2; j >= 1; j -= 1)
   {
      // displace the rectangle so as to displace the text
      RECT r = { rect->left+i, rect->top+j, rect->right+i, rect->bottom + j };
      if (i == 1 && j == 1)
         SetTextColor(hdc, RGB(0,0,0));
      DrawText(hdc, text, -1, &r, flags);
   }
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
int label_font_height=12;

// build the font for the filename label
void build_label_font(void)
{
   LOGFONT lf = {0};
   lf.lfHeight       = label_font_height;
   lf.lfOutPrecision = OUT_TT_PRECIS; // prefer truetype to raster fonts
   strcpy(lf.lfFaceName, "Times New Roman");
   if (label_font) DeleteObject(label_font);
   label_font = CreateFontIndirect(&lf);
}

char path_to_file[4096];
int show_frame = TRUE;   // show border or not?
int show_label = FALSE;  // display the help text or not
int recursive = FALSE;

// WM_PAINT, etc.
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

   // should we show the name of the file?
   if (show_label) {
      SIZE size;
      RECT z;
      HFONT old = NULL;
      char buffer[1024];
      char *name = cur_filename ? cur_filename : "(none)";
      if (fileinfo) {
         if (recursive)
            sprintf(buffer, "%s ( %d / %d - %s)", name, cur_loc+1, stb_arr_len(fileinfo), path_to_file);
         else
            sprintf(buffer, "%s ( %d / %d )", name, cur_loc+1, stb_arr_len(fileinfo));
         name = buffer;
      }

      if (label_font) old = SelectObject(hdc, label_font);

      // get rect around label so we can draw it ourselves, because
      // the DrawText() one is poorly sized

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
      //box.bottom = box.top + h2;
      // expand on left & right so following code is well behaved
      // (we're centered anyway, so the exact numbers don't matter)
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
void image_resize(Image *dest, Image *src);

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
      if (!show_frame) left += FRAME, top += FRAME, width -= 2*FRAME, height -= 2*FRAME;
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

int allow_fullsize;

// compute the size we'd prefer this window to be at for 1:1-ness
void ideal_window_size(int w, int h, int *w_ideal, int *h_ideal, int *x, int *y, int cx2, int cy2)
{
   // @TODO: this probably isn't right if the virtual TL isn't (0,0)???
   int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
   int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

   if (allow_fullsize || (w <= cx2 && h <= cy2)) {
      // if the image fits on the primary monitor, go for it
      *w_ideal = w;
      *h_ideal = h;
   } else if (w - FRAME*2 <= cx2 && h - FRAME*2 <= cy2) {
      // if the image fits on the primary monitor with border...
      // this makes the test above irrelevant
      *w_ideal = w;
      *h_ideal = h;
   } else {
      // will we show more if we use the virtual desktop, rather than just the primary?
      int w1,h1,w2,h2;
      compute_size(cx+FRAME*2 ,cy+FRAME*2,w,h,&w1,&h1);
      compute_size(cx2+FRAME*2,cy2+FRAME*2,w,h,&w2,&h2);
      // require it be "significantly more" on the virtual
      #ifdef ALLOW_MULTISCREEN
      if (h1 > h2*1.25 || w1 > w2*1.25) {
         *w_ideal = stb_min(cx,w1)+FRAME*2;
         *h_ideal = stb_min(cy,h1)+FRAME*2;
      } else {
         *w_ideal = stb_min(cx2,w2)+FRAME*2;
         *h_ideal = stb_min(cy2,h2)+FRAME*2;
      }
      #else
      *w_ideal = stb_min(cx2,w2)+FRAME*2;
      *h_ideal = stb_min(cy2,h2)+FRAME*2;
      #endif
      // compute actual size image will be if fit to this window
      compute_size(*w_ideal, *h_ideal, w,h, &w,&h);
      // and add the padding in
      w += FRAME*2;
      h += FRAME*2;
   }

   // now find center point...
   #ifdef ALLOW_MULTISCREEN
   if ((cx != cx2 || cy != cy2) && w <= cx2+FRAME*2 && h <= cy2+FRAME*2) {
      // if it fits on the primary, center it on the primary
      *x = (cx2 - w) >> 1;
      *y = (cy2 - h) >> 1;
   } else {
      // otherwise center on the virtual
      *x = (cx - w) >> 1;
      *y = (cy - h) >> 1;
   }
   #else
   *x = (cx2 - w) >> 1;
   *y = (cy2 - h) >> 1;
   #endif
}


enum
{
   DISPLAY_actual,   // display the image 1:1, or fullscreen if larger than screen
   DISPLAY_current,  // display the image in the current window's size

   DISPLAY__num,
};

int display_mode;

// resize the current image to match the current window/mode, adjusting
// the window in mode DISPLAY_actual. If 'maximize' and the mode is
// DISPLAY_current, it means they've // double-clicked or alt-entered
// into 'fullscreen', so we want to maximize the window.
void size_to_current(int maximize)
{
   int w2,h2;
   int w,h,x,y,cx,cy;

   // get the monitor it's on
   HMONITOR mon = MonitorFromWindow(win, MONITOR_DEFAULTTONEAREST);
   MONITORINFO minfo = { sizeof(minfo) };
   GetMonitorInfo(mon, &minfo);

   // the 1:1 actual size WITH frame
   w2 = source->x+FRAME*2;
   h2 = source->y+FRAME*2;

   cx = minfo.rcMonitor.right - minfo.rcMonitor.left;
   cy = minfo.rcMonitor.bottom - minfo.rcMonitor.top;

   switch (display_mode) {
      case DISPLAY_actual: {
         RECT rect;
         // given the actual size, compute the ideal window size
         // (which is either 1:1 or fullscreen) and center point
         ideal_window_size(w2,h2, &w,&h, &x,&y, cx,cy);

         // if the window fits on the desktop
         if (w <= cx && h <= cy) {
            // try to use the current center point, as much as possible
            GetAdjustedWindowRect(win, &rect);
            x = (rect.right + rect.left - w) >> 1;
            y = (rect.top + rect.bottom - h) >> 1;
            x = stb_clamp(x,minfo.rcMonitor.left,minfo.rcMonitor.right-w);
            y = stb_clamp(y,minfo.rcMonitor.top,minfo.rcMonitor.bottom-h);
         } else {
            x += minfo.rcMonitor.left;
            y += minfo.rcMonitor.top;
         }
         break;
      }

      case DISPLAY_current:
         if (maximize) {
            // fullscreen, plus the frame around the edge
            x = y = -FRAME;
            w = cx + FRAME*2;
            h = cy + FRAME*2;

            x += minfo.rcMonitor.left;
            y += minfo.rcMonitor.top;
         } else {
            // just use the current window
            RECT rect;
            GetAdjustedWindowRect(win, &rect);
            x = rect.left;
            y = rect.top;
            w = rect.right - rect.left;
            h = rect.bottom - rect.top;
         }
         break;
   }

   // if the image is 1:1, we don't need to resize, so skip
   // queueing and all that and just build it
   if (w == w2 && h == h2) {
      int j;
      unsigned char *p = source->pixels;
      // free the current image
      imfree(cur);
      free(cur_filename);

      // build the new one
      cur = bmp_alloc(w2,h2);
      cur_filename = strdup(source_c->filename);
      // build a frame around the data
      frame(cur);
      // copy the raw data in
      for (j=0; j < source->y; ++j) {
         unsigned char *q = cur->pixels + (j+FRAME)*cur->stride + FRAME*BPP;
         memcpy(q, p, source->x*BPP);
         p += source->x*BPP;
      }
      // no error for this image
      display_error[0] = 0;
      // if they don't want the frame, remove it now
      if (!show_frame) x+=FRAME,y+=FRAME,w-=FRAME*2,h-=FRAME*2;

      // move/show it
      MoveWindow(win, x,y,w,h, TRUE);
      InvalidateRect(win, NULL, FALSE);
   } else {
      // not 1:1; it requires resizing, so queue a resize request
      qs.x = x;
      qs.y = y;
      qs.w = w;
      qs.h = h;
   }
}

// when the user toggles the frame on and off, toggle the flag
// and update the window size as appropriate
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

// the most recent image we've seen
int best_lru = 0;

// when we change which file is the one being viewed/resized,
// call this function to update our globals and fit to window
void update_source(ImageFile *q)
{
   source = q->image;
   source_c = q;
   o(("Making %s (%d) current\n", q->filename, q->lru));
   if (q->lru > best_lru)
      best_lru = q->lru;

   if (source)
      size_to_current(FALSE); // don't maximize
}

// toggle between the two main display modes
void toggle_display(void)
{
   if (source) {
      display_mode = (display_mode + 1) % DISPLAY__num;
      size_to_current(TRUE); // _DO_ maximize if DISPLAY_current
   }
}

// manage the list of files in the current directory
// note that this is totally detached from the image cache;
// if you switch directories, the cache will still have
// images from the old directory, and if you switch back
// before they're flushed, it will still be valid
char *filename;   // @TODO: gah, we have cur_filename AND filename. and filename is being set dumbly!

// stb_sdict is a string dictionary (strings as keys, void * as values)
// dictionary mapping filenames (key) to cached images (ImageFile *)
// We can't just replace this with an ImageFile* in the fileinfo (and
// the backpointer) because we keep ImageFile entries around for images
// not in the fileinfo list.
stb_sdict *file_cache;

// when switching/refreshing directories, free this data
void free_fileinfo(void)
{
   int i;
   for (i=0; i < stb_arr_len(fileinfo); ++i)
      free(fileinfo[i].filename); // allocated by stb_readdir
   stb_arr_free(fileinfo);
   fileinfo = NULL;
}

//derived from michael herf's code: http://www.stereopsis.com/strcmp4humans.html

// sorts like this:
//     foo.jpg
//     foo1.jpg
//     foo2.jpg
//     foo10.jpg
//     foo_1.jpg
//     food.jpg

// use upper, not lower, to get better sorting versus '_'
// no, use lower not upper, to get sorting that matches explorer
__forceinline char tupper(char b)
{
	if (b >= 'A' && b <= 'Z') return b - 'A' + 'a';
	//if (b >= 'a' && b <= 'z') return b - 'a' + 'A';
	return b;
}

__forceinline char isnum(char b)
{
	if (b >= '0' && b <= '9') return 1;
	return 0;
}

__forceinline int parsenum(char **a_p)
{
   char *a = *a_p;
	int result = *a - '0';
	++a;

	while (isnum(*a)) {
		result *= 10;
		result += *a - '0';
		++a;
	}

	*a_p = a-1;
	return result;
}

int StringCompare(char *a, char *b)
{
   char *orig_a = a, *orig_b = b;

	if (a == b) return 0;

	if (a == NULL) return -1;
	if (b == NULL) return 1;

	while (*a && *b) {

		int a0, b0;	// will contain either a number or a letter

      if (isnum(*a) && isnum(*b)) {
			a0 = parsenum(&a);
			b0 = parsenum(&b);
      } else {
         // if they are mixed number and character, use ASCII comparison
         // order between them (number before character), not herf's
         // approach (numbers after everything else). this produces the order:
         //     foo.jpg
         //     foo1.jpg
         //     food.jpg
         //     foo_.jpg
         // which I think looks better than having foo_ before food (but
         // I could be wrong, given how a blank space sorts)

			a0 = tupper(*a);
			b0 = tupper(*b);
		}

		if (a0 < b0) return -1;
		if (a0 > b0) return 1;

		++a;
		++b;
	}

	if (*a) return 1;
	if (*b) return -1;

	{
      // if strings differ only by leading 0s, use case-insensitive ASCII sort
      // (note, we should work this out more efficiently by noticing which one changes length first)
      int z = stricmp(orig_a, orig_b);
      if (z) return z;
      // if identical case-insensitive, return ASCII sort
      return strcmp(orig_a, orig_b);
   }
}

int StringCompareSort(const void *p, const void *q)
{
   return StringCompare(*(char **) p, *(char **) q);
}

char *open_filter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.tga;*.hdr;*.spk\0";

// build a filelist for the current directory
void init_filelist(void)
{
   char **image_files; // stb_arr (dynamic array type) of filenames
   char *to_free = NULL; 
   int i;
   if (fileinfo) {
      // cache the current filename so we can look for it in the list below
      // @BUG: is this leaking the old filename?
      filename = to_free = strdup(fileinfo[cur_loc].filename);
      free_fileinfo();
   }

   if (recursive)
      image_files = stb_readdir_recursive(path_to_file, open_filter + 12);
   else
      image_files = stb_readdir_files_mask(path_to_file, open_filter + 12);

   if (image_files == NULL) { error("Error: couldn't read directory."); exit(0); }
   qsort(image_files, stb_arr_len(image_files), sizeof(*image_files), StringCompareSort);

   // given the array of filenames, build an equivalent fileinfo array
   stb_arr_setlen(fileinfo, stb_arr_len(image_files));

   // while we're going through, let's look for the current file, and
   // initialize 'cur_loc' to that value. Otherwise it gets a 0.
   cur_loc = 0;
   for (i=0; i < stb_arr_len(image_files); ++i) {
      fileinfo[i].filename = image_files[i];
      fileinfo[i].lru = 0;      
      if (!stricmp(image_files[i], filename))
         cur_loc = i;
   }

   // if we made a temp copy of the filename, free it... wait, why,
   // given that we're not setting filename=NULL?!
   // @TODO: why didn't this hurt? if (to_free) free(to_free);

   // free the stb_readdir() array, but not the filenames themselves
   stb_arr_free(image_files); 
}

// current lru timestamp
int lru_stamp=1;

// maximum size of the cache
int max_cache_bytes = 256 * (1 << 20); // 256 MB; one 5MP image is 20MB

// minimum number of cache entries
#define MIN_CACHE  3    // always keep 3 images cached, to allow prefetching

// compare the lru timestamps in two cached images, with extra indirection
int ImageFilePtrCompare(const void *p, const void *q)
{
   ImageFile *a = *(ImageFile **) p;
   ImageFile *b = *(ImageFile **) q;
   return (a->lru < b->lru) ? -1 : (a->lru > b->lru);   
}

// see if we should flush any data. we should flush if
// (a) there aren't enough free slots for prefetching, and
// (b) if we're using too much memory

void flush_cache(int locked)
{
   int limit = MAX_CACHED_IMAGES - MIN_CACHE; // maximum images to cache

   volatile ImageFile *list[MAX_CACHED_IMAGES];
   int i, total=0, occupied_slots=0, n=0;

   // count number of images in use, and size they're using
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
      } // if main doesn't own, don't worry about it... so we may underestimate sometimes
   }

   if (!(total > max_cache_bytes || occupied_slots > limit))
      return;

   // sort by lru
   qsort((void *) list, n, sizeof(*list), ImageFilePtrCompare);

   // now we free earliest slots on the list... 
   
   // we could just leave the cache locked the whole time, but will be slightly smarter
   if (!locked) stb_mutex_begin(cache_mutex);

   for (i=0; i < n && occupied_slots > MIN_CACHE && (occupied_slots > limit || total > max_cache_bytes); ++i) {
      if (MAIN_OWNS(list[i]) && list[i]->status != LOAD_unused) {
         // copy the rest of the data out for later use, then clear the existing data
         ImageFile p = *list[i];
         list[i]->bail = 1; // force disk to bail if it gets this -- can't happen?
         list[i]->filename = NULL;
         list[i]->filedata = NULL;
         list[i]->len = 0;
         list[i]->image = NULL;
         list[i]->error = NULL;
         list[i]->status = LOAD_unused;

         // we're done touching this entry (but not done with the data),
         // so release the mutex
         if (!locked) stb_mutex_end(cache_mutex);

         // now do the potentially slow stuff
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

         // and now we're ready to return to the loop, so reclaim the mutex
         if (!locked) stb_mutex_begin(cache_mutex);
      }
   }
   if (!locked) stb_mutex_end(cache_mutex);
   o(("Reduced to %d megabytes\n", total >> 20));
}

// keep an index within the 'fileinfo' array
int wrap(int z)
{
   int n = stb_arr_len(fileinfo);
   if (z < 0) return z + n;
   while (z >= n) z = z - n;
   return z;
}

// consider adding a file-load command to the disk-load command
// if make_current is true, if it's already loaded, make it current
// (maybe that should be done in advance() instead?)
void queue_disk_command(DiskCommand *dc, int which, int make_current)
{
   char *filename;
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

      // it's waiting to be decoded, so doesn't need queueing
      if (z->status == LOAD_reading_done)
         return;

      // it's already loaded
      if (z->status == LOAD_available) {
         if (make_current) {
            o(("Hey look, make_currentdisk request for %s and it's ready to show!\n", z->filename));
            update_source((ImageFile *) z);
         }
         return;
      }

      // if it's not inactive and none of the above, it's an error
      if (z->status != LOAD_inactive) {
         if (make_current) {
            set_error(z);
         }
         return;
      }
      
      // z->status == LOAD_inactive
      // "fall through" to after the if, below
   } else {
      int i,tried_again=FALSE;

      // didn't already have a cache slot, so find one; we called
      // flush_cache() before calling this so a slot should be free
      for (i=0; i < MAX_CACHED_IMAGES; ++i)
         if (cache[i].status == LOAD_unused)
            break;
      if (i == MAX_CACHED_IMAGES) {
         stb_fatal("Internal logic error: no free cache slots, but flush_cache() should free a few");
         return;
      }

      // allocate this slot and fill in the info
      z = &cache[i];
      free(z->filename);
      assert(z->filedata == NULL);
      z->filename = strdup(filename);
      z->lru = 0;
      z->status = LOAD_inactive;
      stb_sdict_add(file_cache, filename, (void *) z);
   }

   // now, take the z we already had, or just allocated, prep it for loading
   assert(z->status == LOAD_inactive);

   o(("MAIN: proposing %s\n", z->filename));
   z->status = LOAD_inactive;     // we still own it for now
   z->image = NULL;
   z->bail = 0;
   z->lru = fileinfo[which].lru;  // pass lru value through

   // and now really put it on the command list
   dc->files[dc->num_files++] = (ImageFile *) z;
}


// step through the current file list
void advance(int dir)
{
   DiskCommand dc;
   int i;
   if (fileinfo == NULL)
      init_filelist();

   cur_loc = wrap(cur_loc + dir);

   // set adjacent files to previous lru value, so they're 2nd-highest priority
   fileinfo[wrap(cur_loc-1)].lru = lru_stamp;
   fileinfo[wrap(cur_loc+1)].lru = lru_stamp;
   // set this file to new value
   fileinfo[cur_loc].lru = ++lru_stamp;

   // make sure there's room for new images
   flush_cache(FALSE);

   // we're mucking with the cache like mad, so grab the mutex; it doubles
   // as a mutex on dc_shared, so don't release until we're done with dc_shared
   stb_mutex_begin(cache_mutex);
   {
      dc.num_files = 0;
      queue_disk_command(&dc, cur_loc, 1);           // first thing to load: this file
      if (dir) {
         queue_disk_command(&dc, wrap(cur_loc+dir), 0); // second thing to load: the next file (preload)
         queue_disk_command(&dc, wrap(cur_loc-dir), 0); // last thing to load: the previous file (in case it got skipped when they went fast)
      }
      filename = fileinfo[cur_loc].filename;

      if (dc.num_files) {
         dc_shared = dc;
         for (i=0; i < dc.num_files; ++i)
            assert(dc.files[i]->filedata == NULL);
         // wake up the disk thread if needed
         stb_sem_release(disk_command_queue);
      }
   }
   stb_mutex_end(cache_mutex);

   // tell disk loader not to bother with older files
   for (i=0; i < MAX_CACHED_IMAGES; ++i)
      if (cache[i].lru < lru_stamp-1)
         cache[i].bail = 1;

   if (do_show)
      SetTimer(win, 0, (int)(delay_time*1000), NULL);
}

// ctrl-O, or initial command if no filename: run
//   GetOpenFileName(), set as the active filename,
//   load the specified filelist, set cur_loc into
//   the filelist, and force it to load (and prefetch)
//   with 'advance'
static char filenamebuffer[4096];

void stb_from_utf8_multi(stb__wchar *out, char *in, int max_out)
{
   // an array of \0 strings terminated by \0\0
   for(;;) {
      int nout;
      stb_from_utf8(out, in, max_out);
      nout = wcslen(out)+1; // number of output characters consumed
      max_out -= nout;
      out += nout;
      if (in[0] == 0) return;
      in += strlen(in)+1;
   }
}

void open_file(void)
{
   stb__wchar buf1[1024], buf2[4096];
   OPENFILENAMEW o = { sizeof(o) };

   stb_from_utf8_multi(buf1, open_filter, sizeof(buf1));
   o.lpstrFilter = buf1;
   o.lpstrFile = buf2;
   buf2[0] = 0;
   o.nMaxFile = sizeof(buf2);
   o.Flags = OFN_HIDEREADONLY;
   if (!GetOpenFileNameW(&o))
      return;
   stb_to_utf8(filenamebuffer, buf2, sizeof(filenamebuffer));
   filename = filenamebuffer;
   stb_fixpath(filename);
   stb_splitpath(path_to_file, filename, STB_PATH);
   free_fileinfo();
   init_filelist();
   advance(0);
}

// cleaner casting in C--remember, C macros of the form "foo()" don't
// conflict with uses for 'foo' without a following open parenthesis,
// so this doesn't cause problems
#define int(x)  ((int) (x))


// discrete resize operation (from keyboard or mousewheel):
//   we want to resize in nice steps, but finer grained than 2x at a time.
//   so we resize at sqrt(2) at a time. to prevent rounding errors (so that
//   when you size up and back down to 1:1, so it's really 1:1), we actually
//   compute a 'zoom factor' that's log2, so zoom=0 is unzoomed, zoom=1 is
//   zoomed 2x in each dimension.
void resize(int step)
{
   int x = source->x, y = source->y;

   float s;
   int x2,y2;
   int zoom=0;  // this is log2(zoom_factor)*2; that is, a 0=1x, 2=2x, 4=4x, 6=8x

   // if we have a current image, use that
   if (cur) {
      // first characterize the current size relative to the raw size
      // we do this by linearly probing possible values for zoom
      // @TODO: refactor to combine these loops
      if (cur->x > x + FRAME*2 || cur->y > y + FRAME*2) {
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
      // if no current image (e.g. an error), just resize relative to current in power-of-two steps
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
      // compute top left to keep same center
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

// when mouse button is down, what mode are we in?
enum
{
   MODE_none,
   MODE_drag,
   MODE_resize,
} dragmode;

#define setmode(x)    (dragmode = (x))
#define ismode(x)     (dragmode == (x))
#define anymode()     !ismode(MODE_none)

static int ex,ey;   // mousedown location relative to top left
static int ex2,ey2; // mousedown location relative to bottom right
static int rx,ry;   // sides that are being resized

// compute the borders to the resize-vs-move regions (e.g. for mouse cursor)
static void cursor_regions(int *x0, int *y0, int *x1, int *y1)
{
   RECT rect;
   int w,h,z2,z;
   GetClientRect(win, &rect);
   // client dimensions
   assert(rect.left == 0 && rect.top == 0);
   w = rect.right;
   h = rect.bottom;

   // size of resize regions is identical in both axes, so
   // use smaller window size
   z = stb_min(w,h);

   // compute size of handles
   // this is a pretty ad-hoc logic that is designed to:
   //   - make them bigger with bigger windows, so easier to grab
   //     - but not too big
   //   - make them smaller with smaller windows, so there's still an ample 'move' region
   //     - but not too small
   if      (z  <   16) z2 = z >> 1;
   else if (z  <  200) z2 = z >> 2;
   else if (z  <  800) z2 = z >> 3;
   else if (z  < 1600) z2 = z >> 4;
   else                z2 = 100;

   *x0 =     z2;
   *x1 = w - z2;
   *y0 =     z2;
   *y1 = h - z2;
}

// static cursor cache of standard windows cursor for resizing
HCURSOR c_def, c_ne_sw, c_e_w, c_nw_se, c_n_s;

// given the cursor position in client coordinates, set the cursor shape
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

// all windows mouse messages that involve the cursor position route here
// (i.e. all mouse messages except mousewheel)
void mouse(UINT ev, int x, int y)
{
   switch (ev) {
      case WM_LBUTTONDBLCLK:
         toggle_display();
         break;

      case WM_LBUTTONDOWN:
         // if we're not in drag/size (and how could we be?!?)
         if (!anymode()) {
            RECT rect;
            int x0,y0,x1,y1;

            // determine which region the user clicked in; there are 9 'quadrants',
            // three in each dimension, so we measure each dimension separately
            cursor_regions(&x0,&y0,&x1,&y1);
            rx = (x < x0) ? -1 : (x > x1);
            ry = (y < y0) ? -1 : (y > y1);
            // if the middle region it's a drag; any other region is a resize
            if (rx || ry)
               setmode(MODE_resize);
            else
               setmode(MODE_drag);
            // capture the mouse until they let go
            SetCapture(win);
            // record the position of the mouse cursor relative to the window
            // sides, so we can resize properly
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

            // in drag mode, a mouse move just moves the window 
            case MODE_drag: {
               RECT rect;
               GetWindowRect(win, &rect);
               MoveWindow(win, rect.left + x-ex, rect.top + y-ey, rect.right - rect.left, rect.bottom - rect.top, TRUE);
               break;
            }

            case MODE_resize: {
               RECT rect;
               assert(rx || ry);

               GetAdjustedWindowRect(win, &rect);
               display_mode = DISPLAY_current; // resizing the window forces it out of 'actual' mode

               // "LIMIT" controls how small a window can be in each dimension
               #define LIMIT 16

               // for each direction we're resizing, compute the new position of that edge
               if (rx < 0) rect.left   = stb_min(rect.left+x-ex , rect.right  - LIMIT);
               if (rx > 0) rect.right  = stb_max(rect.left+x-ex2, rect.left   + LIMIT);
               if (ry < 0) rect.top    = stb_min(rect.top +y-ey , rect.bottom - LIMIT);
               if (ry > 0) rect.bottom = stb_max(rect.top +y-ey2, rect.top    + LIMIT);

               // then force the window to resize to the new rect
               enqueue_resize(rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top);
               break;
            }
         }
         break;

      case WM_RBUTTONUP:
         // right mouse click when not modal exits
         if (!anymode())
            exit(0);

         // otherwise, disrupt a modal operation

         /* FALLTHROUGH */

      case WM_LBUTTONUP:
         ReleaseCapture();
         setmode(MODE_none);
         set_cursor(x,y); // return cursor to normal setting
         break;
   }
}

static unsigned int physmem; // available physical memory according to GlobalMemoryStatus

char *reg_root = "Software\\SilverSpaceship\\imv";
HKEY zreg;

int reg_get(char *str, void *data, int len)
{
   unsigned int type;
   if (ERROR_SUCCESS == RegQueryValueEx(zreg, str, 0, &type, data, &len))
      if (type == REG_BINARY)
         return TRUE;
   return FALSE;
}

int reg_set(char *str, void *data, int len)
{
   return (ERROR_SUCCESS == RegSetValueEx(zreg, str, 0, REG_BINARY, data, len));
}

#if USE_STBI
int only_stbi=FALSE;
#endif

// we use very short strings for these to avoid wasting space, since
// people shouldn't be mucking with them directly anyway!
void reg_save(void)
{
   if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, reg_root, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &zreg, NULL))
   {
      int temp = max_cache_bytes >> 20;
      reg_set("ac", &alpha_background, 6);
      reg_set("up", &upsample_cubic, 4);
      reg_set("cache", &temp, 4);
      reg_set("lfs", &label_font_height, 4);
      reg_set("label", &show_label, 4);
#if USE_STBI
      reg_set("stbi", &only_stbi, 4);
#endif
      reg_set("border", &show_frame, 4);
      reg_set("stime", &delay_time, 4);
      RegCloseKey(zreg);
   }
}

void reg_load(void)
{
   int temp;
   if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, reg_root, 0, KEY_READ, &zreg))
   {
      reg_get("ac", &alpha_background, 6);
      reg_get("up", &upsample_cubic, 4);
      reg_get("lfs", &label_font_height, 4);
      reg_get("label", &show_label, 4);
      if (reg_get("cache", &temp, 4))
         max_cache_bytes = temp << 20;
#if USE_STBI
      reg_get("stbi", &only_stbi, 4);
#endif
      reg_get("border", &show_frame, 4);
      extra_border = show_frame;
      reg_get("stime", &delay_time, 4);
      RegCloseKey(zreg);
   }
}

static HWND dialog; // preferences dialog

// set an edit control's text from an integer
static void set_dialog_number(int id, int value)
{
   char buffer[16];
   sprintf(buffer, "%d", value);
   SetWindowText(GetDlgItem(dialog, id), buffer);
}

// get an edit control's text as an integer
static int get_dialog_number(int id)
{
   char buffer[32];
   int n = GetWindowText(GetDlgItem(dialog,id), buffer, sizeof(buffer)-1);
   buffer[n] = 0;
   return atoi(buffer);
}

// set an edit control's text from an integer
static void set_dialog_numberf(int id, float value)
{
   char buffer[16];
   sprintf(buffer, "%.2f", value);
   SetWindowText(GetDlgItem(dialog, id), buffer);
}

// get an edit control's text as an integer
static float get_dialog_numberf(int id)
{
   char buffer[32];
   int n = GetWindowText(GetDlgItem(dialog,id), buffer, sizeof(buffer)-1);
   buffer[n] = 0;
   return (float) atof(buffer);
}

// clamp an edit control's text into an integer range (and
// remove non-integer chacters)
static void dialog_clamp(int id, int low, int high)
{
   int x = get_dialog_number(id);
   if (x < low) x = low;
   else if (x > high) x = high;
   else return;
   set_dialog_number(id,x);
}

static void dialog_clampf(int id, float low, float high)
{
   float x = get_dialog_numberf(id);
   if (x < low) x = low;
   else if (x > high) x = high;
   else return;
   set_dialog_numberf(id,x);
}

extern unsigned char *rom_images[]; // preference images

void clear_cache(int had_alpha)
{
   int i;
   stb_mutex_begin(cache_mutex);
   for (i=0; i < MAX_CACHED_IMAGES; ++i) {
      if (cache[i].status == LOAD_available) {
         if (had_alpha ? cache[i].image->had_alpha : TRUE) {
            stb_sdict_remove(file_cache, cache[i].filename, NULL);
            free(cache[i].filename);
            imfree(cache[i].image);
            cache[i].status = LOAD_unused;
            cache[i].image = NULL;
            cache[i].filename = NULL;
         }
      }
   }
   stb_mutex_end(cache_mutex);
   free(cur_filename);
   cur_filename = NULL;
}

// preferences dialog windows procedure
BOOL CALLBACK PrefDlgProc(HWND hdlg, UINT imsg, WPARAM wparam, LPARAM lparam)
{
   static Image *pref_image;
   int i;

   dialog = hdlg; // store dialog handle to not pass it to above functions

   switch(imsg)
   {
      case WM_INITDIALOG: {
         // pick a random preference image
         int n = ((rand() >> 6) % 3);
         int x, y;
         // decode it
#if USE_STBI
         int i, j, k;
         uint8 *data = stbi_load_from_memory(rom_images[n],2000,&x,&y,NULL,1);
         pref_image = bmp_alloc(x,y);
         if (data && pref_image) {
            // convert monochrome to BGR
            for (j=0; j < y; ++j)
               for (i=0; i < x; ++i)
                  for (k=0; k < 3; ++k)
                     pref_image->pixels[pref_image->stride*j + BPP*i + k] = data[j*x+i];
         }
         if (data) free(data);
#else
         int channels;
         Bool loaded_as_rgb;
         uint8 *data = imv_decode_from_memory(rom_images[n], 2000, &x, &y, &loaded_as_rgb, &channels, BPP, "");
         assert(channels == BPP);
         pref_image = bmp_alloc(x, y);
         pref_image->pixels = data;
#endif
         // copy preferences into dialog
         SendMessage(GetDlgItem(hdlg, DIALOG_upsample), BM_SETCHECK, upsample_cubic, 0);
         SendMessage(GetDlgItem(hdlg, DIALOG_showlabel), BM_SETCHECK, show_label, 0);
#if USE_STBI
         SendMessage(GetDlgItem(hdlg, DIALOG_stbi_only), BM_SETCHECK, only_stbi, 0);
#else
         EnableWindow(GetDlgItem(hdlg, DIALOG_stbi_only), FALSE);
#endif
         SendMessage(GetDlgItem(hdlg, DIALOG_showborder), BM_SETCHECK, show_frame, 0);
         for (i=0; i < 6; ++i)
            set_dialog_number(DIALOG_r1+i, alpha_background[0][i]);
         set_dialog_number(DIALOG_cachesize, max_cache_bytes >> 20);
         set_dialog_number(DIALOG_labelheight, label_font_height);
         set_dialog_numberf(DIALOG_slideshowtime, delay_time);
         return TRUE;
      }
      case WM_PAINT: {
         if (pref_image) {
            // draw the preferences image
            RECT z;
            int x,y;
            HWND pic = GetDlgItem(hdlg, DIALOG_image);
            GetWindowRect(pic,&z);
            // not clear why these next two lines work/are required, but it's what petzold does;
            // doesn't UpdateWindow() just force a WM_PAINT? why isn't this an infinite loop?
            InvalidateRect(pic,NULL,TRUE);
            UpdateWindow(pic);
            // center it
            x = (z.right - z.left - pref_image->x) >> 1;
            y = (z.bottom - z.top - pref_image->y) >> 1;
            platformDrawBitmap(GetDC(pic), x,y,pref_image->pixels,pref_image->x,pref_image->y,pref_image->stride,0);
         }
         break;
      }
      case WM_COMMAND: {
         int k = LOWORD(wparam);
         int n = HIWORD(wparam);
         switch(k) {
            // validate the dialog entries into range and numeric only, when
            // they lose focus only
            case DIALOG_r1: case DIALOG_g1: case DIALOG_b1:
            case DIALOG_r2: case DIALOG_g2: case DIALOG_b2:
               if (n == EN_KILLFOCUS) dialog_clamp(k,0,255);
               break;
            case DIALOG_cachesize:
               if (n == EN_KILLFOCUS) dialog_clamp(k,1,(physmem>>22)*3); // 3/4 of phys mem
               break;
            case DIALOG_labelheight:
               if (n == EN_KILLFOCUS) dialog_clamp(k,1,200);
               break;
            case DIALOG_slideshowtime:
               if (n == EN_KILLFOCUS) dialog_clampf(k,0,3600 * 24 * 31); // 1 month
               break;

            case IDOK: {
               // user clicked ok... copy out current values to check for changes
               unsigned char curc[6];
               int new_border;
               int old_cubic = upsample_cubic;
               memcpy(curc, alpha_background, 6);

               // load the settings back out of the dialog box
               for (i=0; i < 6; ++i)
                  alpha_background[0][i] = get_dialog_number(DIALOG_r1+i);
               max_cache_bytes = get_dialog_number(DIALOG_cachesize) << 20;
               label_font_height = get_dialog_number(DIALOG_labelheight);
               delay_time = get_dialog_numberf(DIALOG_slideshowtime);
               upsample_cubic = BST_CHECKED == SendMessage(GetDlgItem(hdlg,DIALOG_upsample ), BM_GETCHECK,0,0);
               show_label     = BST_CHECKED == SendMessage(GetDlgItem(hdlg,DIALOG_showlabel), BM_GETCHECK,0,0);
#if USE_STBI
               only_stbi      = BST_CHECKED == SendMessage(GetDlgItem(hdlg,DIALOG_stbi_only), BM_GETCHECK,0,0);
#endif //USE_STBI
               new_border     = BST_CHECKED == SendMessage(GetDlgItem(hdlg,DIALOG_showborder),BM_GETCHECK,0,0);

               // if alpha_background changed, clear the cache of any images that used it                
               if (memcmp(alpha_background, curc, 6)) {
                  clear_cache(1);
                  // force a reload of the current image
                  advance(0);
               } else if (old_cubic != upsample_cubic) {
                  free(cur_filename);
                  cur_filename = NULL;
                  advance(0);
               }

               // if border changed, update
               if (new_border != show_frame) {
                  toggle_frame();
                  extra_border = show_frame;
                  if (cur) frame(cur);
               }

               // save the data out to the registry
               reg_save();

               // rebuild the label font
               build_label_font();

               // redraw window -- only needed if changed label state, but we can live with always
               InvalidateRect(win, NULL, FALSE);

               /* FALL THROUGH */
            }
            case IDCANCEL:
               EndDialog(hdlg,0);
               return TRUE;
         }
         break;
      }

      case WM_DESTROY:
         // we're closing the dialog, so clear the cached image
         imfree(pref_image);
         pref_image = NULL;
         break;
   }
   return FALSE;
}

#ifdef PERFTEST
void performance_test(void)
{
   int t1,t2;
   int len,i;
   uint8 *buffer = stb_file(cur_filename, &len);
   if (buffer == NULL) return;
   
   t1 = timeGetTime();

   for (i=0; i < 50; ++i) {
      int x,y,n;
      uint8 *result = imv_decode_from_memory(buffer, len, &x, &y, &n, 4, cur_filename);
      free(result);
   }

   t2 = timeGetTime();
   free(buffer);

   {
      char buffer[512];
      sprintf(buffer, "Decode time: %f ms\n", (t2-t1)/50.0);
      error(buffer);
   }
}
#endif

// missing VK definitions in old compiler
#ifndef VK_OEM_PLUS
#define VK_OEM_PLUS  0xbb
#define VK_OEM_MINUS 0xbd
#endif

#ifndef VK_SLASH
#define VK_SLASH     0xbf
#endif

#ifndef WM_APPCOMMAND
#define WM_APPCOMMAND                   0x0319
#define APPCOMMAND_BROWSER_BACKWARD       1
#define APPCOMMAND_BROWSER_FORWARD        2
#define GET_APPCOMMAND_LPARAM(lParam) ((short)(HIWORD(lParam) & ~FAPPCOMMAND_MASK))
#endif
#ifndef APPCOMMAND_OPEN
#define APPCOMMAND_OPEN                   30
#define FAPPCOMMAND_MASK  0xF000
#endif

// ok, surely windows doesn't BY DESIGN require you to store your
// HINSTANCE in a global, does it? but I couldn't find a 'GetCurrentInstance'
// or some such to tell you what instance a thread came from. But the
// HINSTANCE is needed to launch the preferences dialog. Oh well!
HINSTANCE inst;
int sharpen=0;
int dummy_window=1;

int WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (dummy_window) return DefWindowProc (hWnd, uMsg, wParam, lParam);

   switch (uMsg) {
      case WM_CREATE: {
         win = hWnd;
         break;
      }

      case WM_APP_LOAD_ERROR:
      case WM_APP_DECODE_ERROR:
      {
         // if the load/decode threads get an error, they send this message
         // to make sure the main thread gets woken up. then we have to decide
         // whether to show it or not; we do that by scanning the whole cache
         // to see what the most recently-browsed-and-displayable image is,
         // and store that in 'best'.
         int i;
         // int best_lru=0;
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
         // if the most recently-browsed and displayable image is an error, show it
         if (best && (best->status == LOAD_error_reading || best->status == LOAD_error_decoding))
            set_error(best);

         break;
      }

      case WM_APP_DECODED: {
         // if the decode thread finishes, it sends us this message. note that
         // we skip files that had an error; but we use a global variable for 'best_lru'
         // so we won't ever retreat. I'm not sure how this really interacts with
         // the above loop, though. maybe they should be combined.
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
            o(("Post-decode, found a best image, better than any before.\n"));
            update_source(best);
         }
         // since we've decoded a new image, our cache might be too big,
         // so try flushing it
         flush_cache(FALSE);
         break;
      }

      case WM_MOUSEWHEEL: {
         int zdelta = (short) HIWORD(wParam);
         // ignore wheel scaling factor and step 1 by 1
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
         // there's no GetCursorPos for the client window?
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

      case WM_TIMER: {
         advance(1);
         return 0;
      }

      case WM_APPCOMMAND: {
         switch (GET_APPCOMMAND_LPARAM(lParam)) {
            case APPCOMMAND_BROWSER_FORWARD:
               advance(1);
               break;
            case APPCOMMAND_BROWSER_BACKWARD:
               advance(-1);
               break;
            case APPCOMMAND_OPEN:
               open_file();
               break;
            default:
               return DefWindowProc (hWnd, uMsg, wParam, lParam);
         }
         break;
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
               if (!show_help)
                  exit(0);
               show_help = !show_help;
               InvalidateRect(win, NULL, FALSE);
               break;

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

            case 's': case 'S':
               do_show = !do_show;
               if (do_show)
                  SetTimer(win,0,(int)(1000*delay_time),NULL);
               else
                  KillTimer(win,0);
               break;

            case '.': {
               char buffer[512];
               strcpy(buffer, path_to_file);
               if (buffer[strlen(buffer)-1] == '/')
                  buffer[strlen(buffer)-1] = 0;
               stb_splitpath(path_to_file, buffer, STB_PATH);
               if (recursive)
                  init_filelist();
               break;
            }

            #if ALLOW_RECOLORING
            case ']': lmax = stb_clamp(lmax-1.0f/32, 0,2); clear_cache(0); advance(0); break;
            case '[': lmax = stb_clamp(lmax+1.0f/32, 0,2); clear_cache(0); advance(0); break;
            case '}': lmin = stb_clamp(lmin-1.0f/32, -1,1); clear_cache(0); advance(0); break;
            case '{': lmin = stb_clamp(lmin+1.0f/32, -1,1); clear_cache(0); advance(0); break;
            case 'Z': lmin = 0; lmax = 1.0; clear_cache(0); advance(0); break;
            case 'm': mono = !mono; clear_cache(0); advance(0); break;
            #endif

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

#ifdef MONO_THUMB
            case 'T' | MY_CTRL | MY_ALT | MY_SHIFT:
            {
               extern Image *make_mono_thumb(Image *src);
               while (pending_resize.size.w && !pending_resize.image)
                  Sleep(10);
               source = make_mono_thumb(source);
               imfree(source_c->image);
               source_c->image = source;
               size_to_current(FALSE);
               break;
            }
#endif

            case 'B' | MY_CTRL:
               extra_border = !extra_border;
               if (cur) frame(cur);
               InvalidateRect(win, NULL, FALSE);
               break;

            case 'B' | MY_SHIFT:
               toggle_frame();
               break;

            case 'B':
               toggle_frame();
               extra_border = show_frame;
               if (cur) frame(cur);
               break;

            case 'M' | MY_CTRL: {
               FILE *f = fopen("c:/imv(stb)_marked.txt", "a");
               if (f) {
                  char buffer[1024];
                  stb_fullpath(buffer, 1024, source_c->filename);
                  fprintf(f, "%s\n", buffer);
                  fclose(f);
               }
               break;
            }

            case 'C' | MY_CTRL: {
               HGLOBAL hMem;
               char buffer[1024], *t;
               stb_fullpath(buffer, 1024, source_c->filename);
               hMem = GlobalAlloc(GHND, strlen(buffer)+1);
               t = GlobalLock(hMem);
               strcpy(t, buffer);
               GlobalUnlock(hMem);
               OpenClipboard(win);
               EmptyClipboard();
               SetClipboardData(CF_TEXT, hMem);
               CloseClipboard();
               break;
            }

            case 'R' | MY_CTRL: {
               recursive = !recursive;
               init_filelist();
               break;
            }

            case 'R' | MY_ALT: {
               static int init;
               int n;
               char buffer[512], **subdir;
               recursive = TRUE;
               strcpy(buffer, path_to_file);
               if (buffer[strlen(buffer)-1] == '/')
                  buffer[strlen(buffer)-1] = 0;
               stb_splitpath(path_to_file, buffer, STB_PATH);
               subdir = stb_readdir_subdirs(path_to_file);
               if (!init) {
                  init = 1;
                  stb_srand(time(NULL));
               }
               n = stb_rand() % stb_arr_len(subdir);
               strcpy(path_to_file, subdir[n]);
               stb_readdir_free(subdir);
               init_filelist();
               advance(0);
               break;
            }

            case 'I' | MY_CTRL: {
               // not sure which of these is smaller
               #if 0
                  char buffer[MAX_PATH+1024];
                  PROCESS_INFORMATION pi={0};
                  STARTUPINFO si={0};
                  buffer[0] = '"';
                  GetModuleFileName(NULL, buffer+1, MAX_PATH);
                  strcat(buffer, "\" \"");
                  stb_fullpath(buffer+strlen(buffer), 1020, source_c->filename);
                  strcat(filename, "\"");
                  CreateProcess(NULL, buffer, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi);
               #else
                  char buffer[MAX_PATH],filename[1024] = {'\"'};
                  GetModuleFileName(NULL, buffer, sizeof(buffer));
                  stb_fullpath(filename+1, sizeof(filename)-2, source_c->filename);
                  strcat(filename, "\"");
                  _spawnl(_P_NOWAIT, buffer, buffer, filename, NULL);
               #endif
               break;
            }

#ifdef PERFTEST
            case 'D' | MY_CTRL:
               performance_test();
               break;
#endif

            case 'P':
            case 'P' | MY_CTRL:
               DialogBox(inst, MAKEINTRESOURCE(IDD_pref), hWnd, PrefDlgProc);
               break;

            case MY_CTRL | VK_OEM_PLUS:
            case MY_CTRL | MY_SHIFT | VK_OEM_PLUS:
               resize(1);
               break;
            case MY_CTRL | VK_OEM_MINUS:
               resize(-1);
               break;

            case 'S' | MY_CTRL:
               sharpen = !sharpen;
               --cur->y;
               --cur->x;
               size_to_current(0);
               break;

            case 'O':
            case MY_CTRL | 'O':
               open_file();
               break;

            case MY_CTRL | 'F':
               allow_fullsize = !allow_fullsize;
               if (allow_fullsize)
                  display_mode = DISPLAY_current;
               else
                  display_mode = !display_mode;
               toggle_display();
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

// number of threads to use in resizer
int resize_threads;

// whether 'cur' (the resized image currently displayed) actually comes from 'source'
int cur_is_current(void)
{
   if (!cur_filename) return FALSE;
   if (!source_c || !source_c->filename) return FALSE;
   return !strcmp(cur_filename, source_c->filename);
}

#if USE_GDIPLUS
typedef ULONG ULONG_PTR;

static Bool GdiplusPresent;
static ULONG_PTR GpToken;
static Bool LoadGdiplus(void);
#define ICM_SUFFIX ""
// use this definition to enable color management
//#define ICM_SUFFIX "ICM"
#endif

#if USE_FREEIMAGE
static int FreeImagePresent;
static int LoadFreeImage(void);
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   LPWSTR       cmdline = GetCommandLineW();
   int argc;
   LPWSTR       *argv = CommandLineToArgvW(cmdline, &argc);
   //int argc;
   //char **argv = stb_tokens_quoted(lpCmdLine, " ", &argc);
   char filenamebuffer[4096];

   MEMORYSTATUS mem;
   MSG          msg;
   WNDCLASSEX   wndclass = { sizeof(wndclass) };
   HWND         hWnd;


   // initial loaded image
   int image_x, image_y, image_loaded_as_rgb, image_n;
   unsigned char *image_data;

   inst = hInstance;

   // determine the number of threads to use in the resizer
   resize_threads = stb_min(stb_processor_count(), 16);

   // compute the amount of physical memory to set a guess for the cache size
   GlobalMemoryStatus(&mem);
   if (mem.dwTotalPhys == 0) --mem.dwTotalPhys;
   physmem = mem.dwTotalPhys;
   max_cache_bytes = physmem / 6;
   if (max_cache_bytes > 256 << 20) max_cache_bytes = 256 << 20;

   // load the registry preferences, if they're there (AFTER the above)
   reg_load();

   // concatenate the version number onto the help text, because
   // we can't do this statically with the current build process
   strcat(helptext_center, VERSION);

#if USE_GDIPLUS
   if (LoadGdiplus()) {
       strcat(helptext_center, "\nUsing GDI+");
       open_filter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.tga"
       #if USE_STBI
            "*.hdr;*.spk;"
       #endif
            "*.gif;*.ico;*.jng;*.tiff\0";
   }
#endif

   // now try to LoadFreeImage _after_ we've already loaded the prefs;
   // that way only_stbi can be used to suppress errors
#if USE_FREEIMAGE
   if (LoadFreeImage()) {
      strcat(helptext_center, "\nUsing FreeImage.dll: http://freeimage.sourceforge.net");
      open_filter = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.tga;"
             #if USE_STBI
                    "*.hdr;*.spk;"
             #endif
                    "*.dds;*.gif;*.ico;*.jng;*.lbm;*.pcx;*.ppm;*.psd;*.tiff\0";
   }
#endif
   assert(helptext_center[sizeof(helptext_center)-1]==0);

   // create the main window class
   wndclass.style         = CS_OWNDC | CS_DBLCLKS;
   wndclass.lpfnWndProc   = (WNDPROC)MainWndProc;
   wndclass.hInstance     = hInstance;
   wndclass.hIcon         = LoadIcon(hInstance, szAppName);
   wndclass.hCursor       = LoadCursor(NULL,IDC_ARROW);
   wndclass.hbrBackground = NULL;
   wndclass.lpszMenuName  = szAppName;
   wndclass.lpszClassName = szAppName;
   wndclass.hIconSm       = LoadIcon(hInstance, szAppName);
   if (!RegisterClassEx(&wndclass))
      return FALSE;

   // cache the cursors
   c_def   = LoadCursor(NULL, IDC_ARROW);
   c_nw_se = LoadCursor(NULL, IDC_SIZENWSE);
   c_ne_sw = LoadCursor(NULL, IDC_SIZENESW);
   c_e_w   = LoadCursor(NULL, IDC_SIZEWE);
   c_n_s   = LoadCursor(NULL, IDC_SIZENS);

   build_label_font();

   srand(time(NULL));

   if (argc < 2) {
      stb__wchar buf1[1024], buf2[4096];
      OPENFILENAMEW o = { sizeof(o) };

      stb_from_utf8_multi(buf1, open_filter, sizeof(buf1));
      o.lpstrFilter = buf1;
      o.lpstrFile = buf2;
      buf2[0] = 0;
      o.nMaxFile = sizeof(buf2);
      if (!GetOpenFileNameW(&o))
         return 0;
      stb_to_utf8(filenamebuffer, buf2, sizeof(filenamebuffer));
      filename = filenamebuffer;
   } else {
      DWORD p = GetFileAttributesW(argv[1]);
      if (p != 0xffffffff && (p & FILE_ATTRIBUTE_DIRECTORY)) {
         filename = "";
         recursive = 1;
         stb_to_utf8(path_to_file, argv[1], sizeof(path_to_file));
         init_filelist();
         if (stb_arr_len(fileinfo))
            filename = fileinfo[0].filename;
         else {
            error("No image files in folder.");
         }
      } else {
         // else grab the first one... what about additional names? should
         // we launch more windows? or initialize the filelist to them, I guess?
         stb_to_utf8(filenamebuffer, argv[1], sizeof(filenamebuffer));
         filename = filenamebuffer;
      }
   }
   
   // allocate worker threads
   resize_workers = stb_workq_new(resize_threads, resize_threads * 4);

   // load initial image
   {
      char *why=NULL;
      int len;
      uint8 *data = stb_file(filename, &len);
      if (!data)
         why = "Couldn't open file";
      else {
         image_data = imv_decode_from_memory(data, len, &image_x, &image_y, &image_loaded_as_rgb, &image_n, BPP, filename);
         if (image_data == NULL)
            why = imv_failure_reason();
      }

      if (why) {
         // we treat errors on initial image differently: message box and exit...
         // now that we handle errors nicely, this is kind of dumb... but what
         // size should the initial window be?
         char buffer[512];
         sprintf(buffer, "'%s': %s", filename, why);
         error(buffer);
         exit(0);
      }
   }

   // fix the filename & path for consistency with readdir()
   stb_fixpath(filename);
   // extract just the path
   stb_splitpath(path_to_file, filename, STB_PATH);

   // allocate semaphores / mutexes
   cache_mutex  = stb_mutex_new();
   decode_mutex = stb_mutex_new();
   decode_queue       = stb_sem_new(1);
   disk_command_queue = stb_sem_new(1);
   resize_merge = stb_sync_new();

   // go ahead and start the other tasks
   stb_create_thread(diskload_task, NULL);
   stb_create_thread(decode_task, NULL);

   // create the source image by converting the image data to BGR,
   // pre-blending alpha
   source = malloc(sizeof(*source));
   make_image(source, image_x, image_y, image_data, image_loaded_as_rgb, image_n);

   // create a cache entry in case they start browsing later
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
      int cx,cy;
      HMONITOR mon;
      MONITORINFO minfo = { sizeof(minfo) };

      // we have to create a window first to find out the correct monitor
      // because i can't find any other way to discover this
      hWnd = CreateWindow(szAppName, displayName,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,CW_USEDEFAULT,1,1,
                        //x,y, w, h,
                        NULL, NULL, hInstance, NULL);

      if (!hWnd)
         return FALSE;

      // get the monitor it's on

      mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
      GetMonitorInfo(mon, &minfo);

      DestroyWindow(hWnd);

      cx = minfo.rcMonitor.right - minfo.rcMonitor.left;
      cy = minfo.rcMonitor.bottom - minfo.rcMonitor.top;

      //cx = GetSystemMetrics(SM_CXSCREEN);
      //cy = GetSystemMetrics(SM_CYSCREEN);

      // determine an initial window size, and resize 
      ideal_window_size(w2,h2, &w,&h, &x,&y, cx,cy);

      // if the size exactly matches, don't resize, just copy
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
      if (!show_frame) {
         x += FRAME;
         y += FRAME;
         w -= FRAME*2;
         h -= FRAME*2;
      }

      x += minfo.rcMonitor.left;
      y += minfo.rcMonitor.top;

      // now create the real window
      dummy_window = 0;
      hWnd = CreateWindow(szAppName, displayName,
                        WS_POPUP,
                        x,y, w, h,
                        NULL, NULL, hInstance, NULL);
   } // open brace for defining some temporary variables

   // display the window
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   InvalidateRect(hWnd, NULL, TRUE);

   for(;;) {
      // if we're not currently resizing, and there's a resize request
      if (qs.w && pending_resize.size.w == 0) {
         if (source) {
            // is the image we're showing the image to resize, and does the size match?
            if (cur_is_current() && (!cur || (qs.w == cur->x && qs.h >= cur->y) || (qs.h == cur->y && qs.w >= cur->x))) {
               // no resize necessary, just a variant of the current shape
               if (!show_frame) qs.x += FRAME, qs.y += FRAME, qs.w -= 2*FRAME, qs.h -= 2*FRAME;
               MoveWindow(win, qs.x,qs.y,qs.w,qs.h, TRUE);
               InvalidateRect(win, NULL, FALSE);
            } else {
               o(("Enqueueing resize\n"));
               pending_resize.size = qs;
               queue_resize(qs.w, qs.h, source_c, FALSE);
            }
            //flush = FALSE;
         }
         qs.w = 0;
      }

      // this is currently done in a stupid way--we should have the resizer
      // post us a message to wake us up, but I wrote this before I had
      // that infrastructure worked out

      if (!PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE)) {
         // no messages, so check for a resize completing
         if (pending_resize.size.w) {
            // there's a resize pending, so don't block
            if (!pending_resize.image) {
               // @TODO: use a message instead!
               // resize isn't done, so sleep for a bit and try again
               Sleep(10);
               continue;
            } else {
               // resize is done
               HDC hdc;
               o(("Finished resize\n"));

               // reclaim ownership of the image from the resizer
               pending_resize.image_c->status = LOAD_available;

               // free the current image we're about to write over
               imfree(cur);
               cur = pending_resize.image;
               // pending_resize.filename was strdup()ed, so just take ownership of it
               cur_filename = pending_resize.filename;
               pending_resize.filename = NULL;

               // clear error messages
               display_error[0] = 0;

               if (!show_frame) {
                  pending_resize.size.x += FRAME;
                  pending_resize.size.y += FRAME;
                  pending_resize.size.w -= FRAME*2;
                  pending_resize.size.h -= FRAME*2;
               }

               // resize the window
               SetWindowPos(hWnd,NULL,pending_resize.size.x, pending_resize.size.y, pending_resize.size.w, pending_resize.size.h, SWP_NOZORDER|SWP_NOCOPYBITS);
               //MoveWindow(hWnd,pending_resize.size.x, pending_resize.size.y, pending_resize.size.w, pending_resize.size.h, FALSE);

               // clear the resize request info
               barrier();
               pending_resize.size.w = 0;

               // paint the window              
               hdc = GetDC(win);
               display(hWnd, hdc);
               ReleaseDC(win, hdc);

               // restart from the top
               continue;
            }
         }
      }

      // we can get rid of this with peek-with-remove, surely?
      if (!GetMessage(&msg, NULL, 0, 0))
         return msg.wParam;

      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

/////////////////////////////////////////////////////////////////////////////
//
//    Everything from here on down just does image resizing
//

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
         if (nearest_neighbor) fy = 0; else
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
                  if (nearest_neighbor) x = 0;
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

                  if (nearest_neighbor) x = 0;
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
   return NULL;
}

void image_resize_bilinear(Image *dest, Image *src)
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
      j1 = j0;
   }

   if (resize_threads == 1) {
      image_resize_work(q);
   } else {
      stb_sync_set_target(resize_merge, resize_threads);
      for (i=1; i < resize_threads; ++i)
         stb_workq_reach(resize_workers, image_resize_work, q+i, NULL, resize_merge);
      image_resize_work(q);
      stb_sync_reach_and_wait(resize_merge);
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
;// again, cannot be used for a=256
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
MMX int16 round[4] = { 128,128,128,128 };

static void cubic_interpolate_span(uint32 *dest,
                                   uint32 *x0, uint32 *x1, uint32 *x2, uint32 *x3,
                                   int lerp8, int step_dest, int step_src, int len)
{
   if (len <= 0) return;
   __asm {
      // these save/restores shouldn't be necessary... but they seem to be needed
      // in VC6 opt builds; either a buggy compiler, or I'm doing something wrong
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
      // clearer way to thinkg of this: mm7 contains t/2
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

      // extra precision
      psllw     mm1,2
      psllw     mm2,2
      psllw     mm3,2
      psllw     mm4,2

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
      psubw     mm6,mm1   // mm6 = x2-x0 = c
      psubw     mm3,mm2   // mm3 = x0+x2-d/2
      pmullw    mm5,three // mm5 = 3*(x1-x2)
      psubw     mm3,mm2   // mm3 = x0+x2-d
      pmulhw    mm6,mm7   // mm6 = c*t/2
      paddw     mm5,mm4   // mm5 = a
      psubw     mm3,mm5   // mm3 = b

      psllw     mm5,2     // mm5 = a*4
      psllw     mm3,1     // mm3 = b*2
      pmulhw    mm5,mm7   // mm5 = a*t*2
      paddw     mm6,mm2   // mm6 = (c*t+d)/2
      paddw     mm5,mm3   // mm5 = (a*t + b)*2
      pmulhw    mm5,mm7   // mm5 = a*t^2+b*t
      pmulhw    mm5,mm7   // mm5 = (a*t^3+b*t^2)/2
      paddw     mm5,mm6
      psraw     mm5,2
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

static void cubic_interpolate_span(Color *dest, Color *x0, Color *x1, Color *x2, Color *x3, int lerp8, int step_dest, int step_src, int len)
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
void * cubic_interp_1d_x_work(int n)
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
            cubic_interpolate_span(dest, data+xp,data+xp,data+xp+1,data+xp+2,xw,out->stride,src->stride,k2-k);
         } else if (xp >= src->x - 2) {
            if (xp == src->x-1) {
               for (j=k; j < k2; ++j) {
                  dest[0] = data[xp];
                  data = PLUS(data, src->stride);
                  dest = PLUS(dest , out->stride);
               }
            } else {
               cubic_interpolate_span(dest, data+xp-1,data+xp,data+xp+1,data+xp+1,xw,out->stride,src->stride,k2-k);
            }
         } else {
            cubic_interpolate_span(dest, data+xp-1,data+xp,data+xp+1,data+xp+2,xw,out->stride,src->stride,k2-k);
         }
         x += dx;
      }
   }
   return NULL;
}

Image *cubic_interp_1d_x(Image *src, int out_w)
{
   int i;
   cubic_work.out = bmp_alloc(out_w, src->y);
   cubic_work.delta = (src->x-1)*65536 / (out_w-1);
   cubic_work.src = src;
   cubic_work.out_len = out_w;
   barrier();

   if (resize_threads == 1) {
      cubic_interp_1d_x_work(0);
   } else {
      stb_sync_set_target(resize_merge, resize_threads);
      for (i=1; i < resize_threads; ++i)
         stb_workq_reach(resize_workers, (stb_thread_func) cubic_interp_1d_x_work, (void *) i, NULL, resize_merge);
      cubic_interp_1d_x_work(0);
      stb_sync_reach_and_wait(resize_merge);
   }
   return cubic_work.out;
}

Image *cubic_interp_1d_y_work(int n)
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
      cubic_interpolate_span(dest, data0, data1, data2, data3, yw, 4,4,out->x);
   }
   return NULL;
}

Image *cubic_interp_1d_y(Image *src, int out_h)
{
   int i;
   cubic_work.src = src;
   cubic_work.out = bmp_alloc(src->x, out_h);
   cubic_work.delta = ((src->y-1)*65536-1) / (out_h-1);
   cubic_work.out_len = out_h;

   if (resize_threads == 1) {
      cubic_interp_1d_y_work(0);
   } else {
      barrier();
      stb_sync_set_target(resize_merge, resize_threads);
      for (i=1; i < resize_threads; ++i)
         stb_workq_reach(resize_workers, (stb_thread_func) cubic_interp_1d_y_work, (void *) i, NULL, resize_merge);
      cubic_interp_1d_y_work(0);
      stb_sync_reach_and_wait(resize_merge);
   }
   return cubic_work.out;
}

// downsampling
Image *downsample_half(Image *src)
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

Image *downsample_two_thirds(Image *src)
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

void do_sharpen(uint8 *data, int stride, int x, int y)
{
   int i,j,k;
   uint8 prev[3200*4];
   if (x > 3200) return;
   memcpy(prev, data, x*BPP);
   for (j=1; j < y-1; ++j) {
      uint8 *next = data + stride*(j+1);
      unsigned char left[4];
      memcpy(left, data+stride*j+BPP, BPP);
      for (i=1; i < x-1; ++i) {
         unsigned char temp[4];
         memcpy(temp, data+stride*j+i*BPP,BPP);
         for (k=0; k < BPP; ++k) {
            int v = data[stride*j+i*BPP+k] * 16;
            v -= (prev[i*BPP+k-4] + prev[i*BPP+k] + prev[i*BPP+k+4]);
            v -= (next[i*BPP+k-4] + next[i*BPP+k] + next[i*BPP+k+4]);
            v -= (left[k] + data[stride*j+i*BPP+4+k]);
            v >>= 3;
            if (v < 0) v = 0; else if (v > 255) v = 255;
            data[stride*j+i*BPP+k] = v;
         }
         // now temp becomes the new left
         // but first:
         memcpy(prev+(i-1)*BPP, left, BPP);
         memcpy(left, temp, BPP);
      }
   }
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
      // current biggest problem perf-wise is on scaling down, we don't
      // use threads

      // maybe should do something smarter here, like find the
      // nearest box size, instead of repetitive powers of two
      while (gx <= (src->x >> 1) && gy <= (src->y >> 1)) {
         src = downsample_half(src);
         if (to_free) imfree(to_free);
         to_free = src;
      }

      if (gx < src->x * 0.666666f && gy < src->y * 0.666666f) {
         src = downsample_two_thirds(src);
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
      res = cubic_interp_1d_y(src, gy);
      if (to_free) imfree(to_free);
      to_free = res;
      res = cubic_interp_1d_x(res, gx);
      imfree(to_free);
    } else {
      #if 1
      image_resize_bilinear(dest, src);
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
   if (upsample && sharpen)
      do_sharpen(res->pixels, res->stride, res->x, res->y);
   return res;
}
#endif // BPP==4

void image_resize(Image *dest, Image *src)
{
#if BPP==3
   image_resize_bilinear(dest, src);
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

char imv_failure_buffer[1024];
char *imv_failure_string;

static char  *imv_failure_reason(void)
{
   return imv_failure_string;
}

#if USE_GDIPLUS


#pragma pack(push,8)

// from GdiplusTypes.h

typedef enum
{
   GpOk = 0,
   GpGenericError = 1,
   GpInvalidParameter = 2,
   GpOutOfMemory = 3,
   GpObjectBusy = 4,
   GpInsufficientBuffer = 5,
   GpNotImplemented = 6,
   GpWin32Error = 7,
   GpWrongState = 8,
   GpAborted = 9,
   GpFileNotFound = 10,
   GpValueOverflow = 11,
   GpAccessDenied = 12,
   GpUnknownImageFormat = 13,
   GpFontFamilyNotFound = 14,
   GpFontStyleNotFound = 15,
   GpNotTrueTypeFont = 16,
   GpUnsupportedGdiplusVersion = 17,
   GpGdiplusNotInitialized = 18,
   GpPropertyNotFound = 19,
   GpPropertyNotSupported = 20
} GpStatus;

// from GdiplusInit.h

typedef enum
{
   GpDebugEventLevelFatal,
   GpDebugEventLevelWarning
} GpDebugEventLevel;

typedef VOID (WINAPI *GpDebugEventProc)(GpDebugEventLevel level, CHAR *message);

typedef GpStatus (WINAPI *GpNotificationHookProc)(ULONG_PTR *token);
typedef VOID (WINAPI *GpNotificationUnhookProc)(ULONG_PTR token);

typedef struct {
   UINT32 GdiplusVersion;
   GpDebugEventProc DebugEventCallback;
   BOOL SuppressBackgroundThread;
   BOOL SuppressExternalCodecs;
} GdiplusStartupInput;

typedef struct {
   GpNotificationHookProc NoticationHook;
   GpNotificationUnhookProc NotificatoinUnhook;
} GdiplusStartupOutput;

static HINSTANCE GdiplusDLL;
static GdiplusStartupOutput gpStartupOutput;

// from GdiplusHeaders.h
typedef void GpImage; // opaque type

typedef struct { 
   GpImage* nativeImage;
   GpStatus lastResult;
   GpStatus loadStatus;
} GpBitmap;

// from GdiplusTypes.h
typedef struct {
   INT X;
   INT Y;
   INT Width;
   INT Height;
} GpRect;

// from GdiplusPixelFormat.h
typedef INT GpPixelFormat;
// In-memory pixel data formats:
// bits 0-7 = format index
// bits 8-15 = pixel size (in bits)
// bits 16-23 = flags
// bits 24-31 = reserved
#define GpPixelFormatGDI       0x00020000 // Is a GDI-supported format
#define GpPixelFormatAlpha     0x00040000 // Has an alpha component
#define GpPixelFormatCanonical 0x00200000 
#define GpPixelFormat24bppRGB  (8  | (24 << 8) | GpPixelFormatGDI)
#define GpPixelFormat32bppARGB (10 | (32 << 8) | GpPixelFormatAlpha | GpPixelFormatGDI | GpPixelFormatCanonical)

// from GdiplusImaging.h
typedef struct {
   UINT Width;
   UINT Height;
   INT Stride;
   GpPixelFormat PixelFormat;
   VOID* Scan0;
   UINT_PTR Reserved;
} GpBitmapData;

typedef enum
{
   GpImageLockModeRead        = 0x0001,
   GpImageLockModeWrite       = 0x0002,
   GpImageLockModeUserInputBuf= 0x0004
} GpImageLockMode;

typedef __declspec(dllimport) GpStatus (WINAPI *GdiplusStartupProc)(ULONG_PTR *token, const GdiplusStartupInput* input, GdiplusStartupOutput* output);
static GdiplusStartupProc GdiplusStartup;

typedef __declspec(dllimport) GpStatus (WINAPI *GdiplusShutdownProc)(ULONG_PTR token);
static GdiplusShutdownProc GdiplusShutdown;

typedef __declspec(dllimport) GpStatus (WINAPI *GdipCreateBitmapFromStreamProc)(IStream* stream, GpBitmap **bitmap);
static GdipCreateBitmapFromStreamProc GdipCreateBitmapFromStream;

typedef __declspec(dllimport) GpStatus (WINAPI *GdipDisposeImageProc)(GpImage *image);
static GdipDisposeImageProc GdipDisposeImage;

typedef __declspec(dllimport) GpStatus (WINAPI *GdipBitmapLockBitsProc)(GpBitmap* bitmap, const GpRect* rect, GpImageLockMode flags, GpPixelFormat format, GpBitmapData* lockedBitmapData);
static GdipBitmapLockBitsProc GdipBitmapLockBits;

typedef __declspec(dllimport) GpStatus (WINAPI *GdipBitmapUnlockBitsProc)(GpBitmap* bitmap, GpBitmapData* lockedBitmapData);
static GdipBitmapUnlockBitsProc GdipBitmapUnlockBits;

#pragma pack(pop)

FARPROC GpFunc(char *str)
{
   FARPROC p = GetProcAddress(GdiplusDLL, str);
   if (p == NULL)
      GdiplusPresent = FALSE; // if something doesn't load, bail!
   return p;
}

static Bool LoadGdiplus(void)
{
   static Bool InitializationAttempted = FALSE;
   if (InitializationAttempted)
      return GdiplusPresent;

   InitializationAttempted = TRUE;
   GdiplusPresent = FALSE;
   GdiplusDLL = LoadLibrary("gdiplus.dll");
   if (!GdiplusDLL)
      return GdiplusPresent;

   GdiplusPresent = TRUE;
   GdiplusStartup = (GdiplusStartupProc)GpFunc("GdiplusStartup");
   //GdiplusShutdown = (GdiplusShutdownProc)GpFunc("GdiplusShutdown");
   GdipCreateBitmapFromStream = (GdipCreateBitmapFromStreamProc)GpFunc("GdipCreateBitmapFromStream" ICM_SUFFIX);
   GdipDisposeImage = (GdipDisposeImageProc)GpFunc("GdipDisposeImage");
   GdipBitmapLockBits = (GdipBitmapLockBitsProc)GpFunc("GdipBitmapLockBits");
   GdipBitmapUnlockBits = (GdipBitmapUnlockBitsProc)GpFunc("GdipBitmapUnlockBits");
   if (!GdiplusPresent) {
#if USE_STBI
      if (!only_stbi)
#endif
         error("Invalid GdiPlus.dll; disabling GDI+ support.");
   } else {
      // no need to use GDI+ backup thread, or to call the hook methods in gpStartupOutput
      GdiplusStartupInput gpStartupInput = { 1, NULL, TRUE, FALSE };
      if (GdiplusStartup(&GpToken, &gpStartupInput, &gpStartupOutput) != GpOk) {
         GdiplusPresent = FALSE;
#if USE_STBI
         if (!only_stbi)
#endif
            error("Failed to initialize GdiPlus.dll; disabling GDI+ support.");
      }
   }

   return GdiplusPresent;
}

static uint8 *LoadImageWithGdiplus(uint8 *mem, int len, int *x, int *y, int *n, int n_req) {
   HGLOBAL hMem = NULL;
   IStream* stream = NULL;
   GpBitmap* bitmap = NULL;
   GpBitmapData data;
   GpPixelFormat pixelFormat;
   size_t i, image_sz = 0;
   uint8 *buf = NULL, *ret = NULL;
   image_sz = 0;
   *x = 0;
   *y = 0;
   *n = n_req;
   data.Scan0 = NULL;

   hMem = GlobalAlloc(GMEM_MOVEABLE, len);
   if (!hMem)
      goto liwgExit;

   buf = GlobalLock(hMem);
   if (!buf) goto liwgExit;

   memcpy(buf, mem, len);
   if (GlobalUnlock(hMem))
      goto liwgExit;

   if (CreateStreamOnHGlobal(buf, FALSE, &stream) != S_OK)
      goto liwgExit;

   if (GdipCreateBitmapFromStream(stream, &bitmap) != GpOk)
      goto liwgExit;

#if BPP == 3
   assert(n_req == 3);
   pixelFormat = GpPixelFormat24bppRGB;
#else
   assert(n_req == 4);
   pixelFormat = GpPixelFormat32bppARGB;
#endif

   if (GdipBitmapLockBits(bitmap, NULL, GpImageLockModeRead, pixelFormat, &data) != GpOk)
      goto liwgExit;

   *x = data.Width;
   *y = data.Height;
   *n = n_req;
   image_sz = data.Width * data.Height * n_req;
   ret = (uint8*)malloc(image_sz);
   for (i=0; i<data.Height; ++i)
      memcpy(&ret[i*data.Width*n_req], &((uint8*)data.Scan0)[i*data.Stride], data.Width*n_req);
    
liwgExit:
   if (data.Scan0) GdipBitmapUnlockBits(bitmap, &data);
   if (bitmap)     GdipDisposeImage(bitmap);
   if (stream)     stream->lpVtbl->Release(stream);
   if (hMem)       GlobalFree(hMem);

   return ret;
}

#endif

#if USE_FREEIMAGE

// FreeImage types

typedef int FREE_IMAGE_FORMAT;
typedef struct FIBITMAP FIBITMAP;
typedef struct FIMEMORY FIMEMORY;

typedef void (*FreeImage_OutputMessageFunction)(FREE_IMAGE_FORMAT fif, const char *msg);

#define fitype(x)    typedef __declspec(dllimport) x __stdcall


// FreeImage functions that we're going to import;
fitype(void) freeimage_setoutputmessage(FreeImage_OutputMessageFunction omf);
static freeimage_setoutputmessage *FreeImage_SetOutputMessage;

fitype(FIMEMORY *) freeimage_openmemory(uint8 *data, unsigned size);
static freeimage_openmemory *FreeImage_OpenMemory;

fitype(void) freeimage_closememory(FIMEMORY *stream);
static freeimage_closememory *FreeImage_CloseMemory;

fitype(int) freeimage_seekmemory(FIMEMORY *stream, long offset, int origin);
static freeimage_seekmemory *FreeImage_SeekMemory;

fitype(FIBITMAP *) freeimage_loadfrommemory(FREE_IMAGE_FORMAT fif, FIMEMORY *fi, int flags);
static freeimage_loadfrommemory *FreeImage_LoadFromMemory;

fitype(void) freeimage_converttorawbits(BYTE *bits, FIBITMAP *dib, int pitch, unsigned bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask, BOOL topdown);
static freeimage_converttorawbits *FreeImage_ConvertToRawBits;

typedef __declspec(dllimport) void __stdcall freeimage_unload(FIBITMAP *dib);
static freeimage_unload *FreeImage_Unload;

typedef __declspec(dllimport) unsigned __stdcall freeimage_getwidth(FIBITMAP *dib);
static freeimage_getwidth *FreeImage_GetWidth;

typedef __declspec(dllimport) unsigned __stdcall freeimage_getheight(FIBITMAP *dib);
static freeimage_getheight *FreeImage_GetHeight;

typedef __declspec(dllimport) FREE_IMAGE_FORMAT __stdcall freeimage_getfiletypefrommemory(FIMEMORY *fi, int size);
static freeimage_getfiletypefrommemory *FreeImage_GetFileTypeFromMemory;

typedef __declspec(dllimport) FREE_IMAGE_FORMAT __stdcall freeimage_getfiffromfilename(const char *filename);
static freeimage_getfiffromfilename *FreeImage_GetFIFFromFilename;

typedef __declspec(dllimport) BYTE *__stdcall freeimage_getbits(FIBITMAP *dib);
static freeimage_getbits *FreeImage_GetBits;

typedef __declspec(dllimport) int __stdcall freeimage_istransparent(FIBITMAP *dib);
static freeimage_istransparent *FreeImage_IsTransparent;

static void
FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
   strcpy(imv_failure_buffer, message);
   imv_failure_string = imv_failure_buffer;
}

static HINSTANCE FreeImageDLL;

FARPROC fifunc(char *str)
{
   FARPROC p = GetProcAddress(FreeImageDLL, str);
   if (p == NULL)
      FreeImagePresent = FALSE; // if something doesn't load, bail!
   return p;
}

static int LoadFreeImage(void)
{
   static int InitializationAttempted;
   if (InitializationAttempted) return FreeImagePresent;

   InitializationAttempted = TRUE;
   FreeImagePresent = FALSE;
   FreeImageDLL = LoadLibrary("FreeImage.dll");
   if (!FreeImageDLL) return FreeImagePresent;

   FreeImagePresent = TRUE;
   FreeImage_ConvertToRawBits = (freeimage_converttorawbits *)fifunc("_FreeImage_ConvertToRawBits@32");
   FreeImage_GetBits = (freeimage_getbits *)fifunc("_FreeImage_GetBits@4");
   FreeImage_GetFIFFromFilename = (freeimage_getfiffromfilename *)fifunc("_FreeImage_GetFIFFromFilename@4");
   FreeImage_GetFileTypeFromMemory = (freeimage_getfiletypefrommemory *)fifunc("_FreeImage_GetFileTypeFromMemory@8");
   FreeImage_GetHeight = (freeimage_getheight *)fifunc("_FreeImage_GetHeight@4");
   FreeImage_GetWidth = (freeimage_getwidth *)fifunc("_FreeImage_GetWidth@4");
   FreeImage_LoadFromMemory = (freeimage_loadfrommemory *)fifunc("_FreeImage_LoadFromMemory@12");
   FreeImage_SetOutputMessage = (freeimage_setoutputmessage *)fifunc("_FreeImage_SetOutputMessage@4");
   FreeImage_Unload = (freeimage_unload *)fifunc("_FreeImage_Unload@4");
   FreeImage_OpenMemory = (freeimage_openmemory *)fifunc("_FreeImage_OpenMemory@8");
   FreeImage_CloseMemory = (freeimage_closememory *)fifunc("_FreeImage_CloseMemory@4");
   FreeImage_SeekMemory = (freeimage_seekmemory *)fifunc("_FreeImage_SeekMemory@12");
   FreeImage_IsTransparent = (freeimage_istransparent *)fifunc("_FreeImage_IsTransparent@4");

   if (!FreeImagePresent) {
#if USE_STBI
      if (!only_stbi)
#endif
         error("Invalid FreeImage.dll; disabling FreeImage support.");
   } else
      FreeImage_SetOutputMessage(FreeImageErrorHandler);

   return FreeImagePresent;
}

uint8 *LoadImageWithFreeImage(FIMEMORY *fi, int *x, int *y, int *n, int n_req)
{
   uint8 *Result = 0;
   FREE_IMAGE_FORMAT FileFormat = FreeImage_GetFileTypeFromMemory(fi, 0);
   FIBITMAP *Bitmap;
   if(FileFormat == -1) {
      // @TODO: propogate the filename to here?
      // bail!
      return NULL;
      // FileFormat = FreeImage_GetFIFFromFilename(FromFilename);
   }

   FreeImage_SeekMemory(fi, 0, SEEK_SET);

   Bitmap = FreeImage_LoadFromMemory(FileFormat, fi, 0);
   if(Bitmap) {
      int32 Width = FreeImage_GetWidth(Bitmap);
      int32 Height = FreeImage_GetHeight(Bitmap);

      Result = (uint8 *) malloc(Width * Height * BPP);
      if(Result) {
         FreeImage_ConvertToRawBits(Result, Bitmap, BPP*Width, BPP*8, 0xff0000,0x00ff00,0xff, FALSE);
         *x = Width;
         *y = Height;
         *n = FreeImage_IsTransparent(Bitmap) ? 4 : 3;
      }
      FreeImage_Unload(Bitmap);
   }
   return Result;
} 
#endif

static uint8 *imv_decode_from_memory(uint8 *mem, int len, int *x, int *y, Bool* loaded_as_rgb, int *n, int n_req, char *filename)
{
   uint8 *res = NULL;
   imv_failure_string = NULL;

   // prefer STBI over everything else

   *loaded_as_rgb = FALSE;
#if USE_STBI
   res = stbi_load_from_memory(mem, len, x, y, n, n_req);
   if (res) {
       *loaded_as_rgb = TRUE;
       return res;
   }
   imv_failure_string = stbi_failure_reason();

   if ((mem[0] == 's' || mem[0] == 'x') && memcmp(mem+1, "PIC-delta-image", 16) == 0) {
      char full_filename[1024];
      int len2;
      uint8 *mem2;
      FILE *f;
      stb_splitpath(full_filename, filename, STB_PATH);
      strcat(full_filename, mem+17+4);
      f = fopen(full_filename, "rb");
      if (f && (len2 = stb_filelen(f), mem2 = malloc(len2)) != NULL) {
         fread(mem2, 1, len2, f);
         fclose(f); f = NULL;
         res = stbi_load_from_memory(mem2, len2, x, y, n, n_req);
         if (res) {
            int i,offset,c;
            *loaded_as_rgb = TRUE;
            offset = 17;
            offset += 4 + *(int *) (mem+offset);
            if (  *x != *(int *) (mem+offset  )
               || *y != *(int *) (mem+offset+4) )
            {
               free(res);
               free(mem2);
               return NULL;
            }
            offset += 8; // skip x,y
            c = *(int *) (mem+offset);
            assert(c >= 1 && c <= 4);
            offset += 4;
            while (offset < len) {
               int start = *(int *) (mem+offset);
               int count = *(int *) (mem+offset+4);
               assert(start < *x* *y);
               assert(count <= *x * *y - start);
               if (start < 0 || start >= *x * *y)
                  break;
               if (start+count < start || start+count > *x * *y)
                  break;
               offset += 8;
               for (i=0; i < count; ++i) {
                  memcpy(res + start * n_req, mem+offset, c);
                  ++start;
                  offset += c;
               }
            }
            free(mem2);
            return res;
         }
         free(mem2);
      }
      if (f) fclose(f);
   }

   if (only_stbi)
      return res;
#endif

   // prefer GDI+ over FreeImage

#if USE_GDIPLUS
   if (GdiplusPresent) {
       res = LoadImageWithGdiplus(mem, len, x, y, n, n_req);
       if (res)
           return res;
   }
#endif

   // FreeImage is the final fallback

#if USE_FREEIMAGE
   if (FreeImagePresent) {
      FIMEMORY *fi = FreeImage_OpenMemory(mem,len);
      res = LoadImageWithFreeImage(fi, x, y, n, n_req);
      FreeImage_CloseMemory(fi);
      // if no error message is generated, because it's not a known type,
      // we'll get the unknown-type message from stbi_failure_reason()
   }
#endif

   return res;
}
