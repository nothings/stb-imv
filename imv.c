// Set section alignment to be nice and small
#pragma comment(linker, "/FILEALIGN:0x200")
//#pragma comment(linker, "/OPT:NOWIN98")

#define WIN32_MEAN_AND_LEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#define STB_DEFINE
#include "stb.h"          /*     http://nothings.org/stb.h         */
#include "stb_image.c"    /*     http://nothings.org/stb_image.c   */


void error(char *str) { MessageBox(NULL, str, "imv(stb) error", MB_OK); }

#define FRAME   3
#define FRAME2  (FRAME >> 1)

#define GREY  192


#define WM_APP_DECODED    WM_APP


#if WINVER < 0x0500
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#define SM_SAMEDISPLAYFORMAT    81
#endif

CHAR  szAppName[]="stb_imv";
HDC   hDC;
HWND  win;
HGLRC hRC;
HINSTANCE hInst;

char *displayName = "imv(stb)";

#define BPP 4

void platformDrawBitmap(HDC hdc, int x, int y, unsigned char *bits, int w, int h, int stride)
{
   BITMAPINFOHEADER b;
   int result;

   memset(&b, 0, sizeof(b));
   b.biSize = sizeof(b);
   b.biPlanes=1;
   b.biBitCount=BPP*8;
   b.biWidth = stride/BPP;
   b.biHeight = -h;  // tell windows the bitmap is stored top-to-bottom
   if (IsBadReadPtr(bits, w*h*BPP)) {
      assert(0);
   }
   result = SetDIBitsToDevice(hdc, x,y, w,abs(h), 0,0, 0,abs(h), bits, (BITMAPINFO *) &b, DIB_RGB_COLORS);
   if (result == 0) {
      DWORD e = GetLastError();
   }
}

void barrier(void)
{
    long dummy;
    __asm {
        xchg dummy, eax
    }
}



typedef struct
{
   int x,y,stride,frame;
   unsigned char *pixels;
} Image;

enum
{
   // owned by main thread
   LOAD_unused=0, // empty slot

   LOAD_inactive, // filename slot, not loaded
   LOAD_error_reading,
   LOAD_error_decoding,
   LOAD_available, // loaded successfully

   // owned by resizer
   LOAD_resizing,

   // owned by loader
   LOAD_reading,

   // owned by decoder
   LOAD_reading_done,
   LOAD_decoding,
};

#define MAIN_OWNS(x)   ((x)->status <= LOAD_available)

typedef struct
{
   int priority;
   char *filename;
   char *filedata;
   int len;
   Image *image;
   char *error;
   int status;
   int bail;
   int lru;
} ImageFile;

stb_semaphore cache_mutex;
stb_semaphore decode_queue;
stb_semaphore disk_command_queue;

typedef struct
{
   int num_files;
   ImageFile *files[4];
} DiskCommand;

volatile DiskCommand dc_shared;

// awake the main thread when something interesting happens
void wake(int message)
{
   PostMessage(win, message, 0,0);
}

volatile ImageFile *image_decode_pending;

void *diskload_task(void *p)
{
   for(;;) {
      int i;
      DiskCommand dc;

      // wait for a command from the main thread
OutputDebugString("READ: Waiting for disk request.\n");
      stb_sem_waitfor(disk_command_queue);
OutputDebugString("READ: Got disk request.\n");

      // grab the command; don't let the command or the cache change while we do it
      stb_sem_waitfor(cache_mutex);
      {
         dc = dc_shared;
         for (i=0; i < dc.num_files; ++i)
            dc.files[i]->status = LOAD_reading;
      }
      stb_sem_release(cache_mutex);

{
char buffer[256];
sprintf(buffer, "%d\n", dc.num_files);
OutputDebugString("READ: Iterating filelist\n");
OutputDebugString(buffer);
}
      for (i=0; i < dc.num_files; ++i) {
         int n;
         uint8 *data;
         assert(dc.files[i]->status == LOAD_reading);

         // check if the main thread changed its mind about this
         if (dc.files[i]->bail) {
OutputDebugString("READ: Bailing on disk request\n");
            dc.files[i]->status = LOAD_inactive;
         } else {
OutputDebugString("READ: Loading file\n");
            data = stb_file(dc.files[i]->filename, &n);
         
            // don't need to mutex these, because we own them via ->status
            if (data == NULL) {
               dc.files[i]->error = strdup("can't open");
               dc.files[i]->filedata = NULL;
               barrier();
               dc.files[i]->status = LOAD_error_reading;
            } else {
               dc.files[i]->error = NULL;
               dc.files[i]->filedata = data;
               dc.files[i]->len = n;
               barrier();
               dc.files[i]->status = LOAD_reading_done;
            }


OutputDebugString("READ: Putting in decode queue\n");
            // now set the image decode command for what we just loaded
            image_decode_pending = dc.files[i];
            stb_sem_release(decode_queue);

            // wake the main thread? not needed, it has nothing to do
            // wake();
         }
      }
OutputDebugString("READ: Finished command\n");
   }
}

#define MAX_CACHED_IMAGES  200
volatile ImageFile cache[MAX_CACHED_IMAGES];
void make_image(Image *z, int image_x, int image_y, uint8 *image_data, int image_n);

void *decode_task(void *p)
{
   for(;;) {
      int i;
      volatile ImageFile *f;
      for (i=0; i < MAX_CACHED_IMAGES; ++i)
         if (cache[i].status == LOAD_reading_done)
            break;
      if (i == MAX_CACHED_IMAGES) {
OutputDebugString("DECODE: waiting for decode command\n");
         do {
            stb_sem_waitfor(decode_queue);
            f = image_decode_pending;
if (!f) OutputDebugString("DECODE: got bogus decode command\n");
         } while (f == NULL);
OutputDebugString("DECODE: got decode command\n");
         assert(f->status == LOAD_reading_done);
         // we might be signalled twice and miss an 'image_decode_pending' here,
         // but later we'll catch it above
      } else {
OutputDebugString("DECODE: found loaded, unqueued file\n");
         f = &cache[i];
         if (image_decode_pending == f) {
            image_decode_pending = NULL;
OutputDebugString("DECODE: noticed it was in the queue so cleared the queue\n");
         }
      }
      barrier();
      assert(f->status == LOAD_reading_done);
      assert(f->filedata);

      if (f->bail) {
         free(f->filedata);
         f->filedata = NULL;
         f->status = LOAD_inactive;
      } else {
         int x,y,n;
         uint8 *data;
         f->status = LOAD_decoding;
         data = stbi_load_from_memory(f->filedata, f->len, &x, &y, &n, 4);
         free(f->filedata);
         f->filedata = NULL;
         if (data == NULL) {
            f->error = strdup(stbi_failure_reason());
            barrier();
            f->status = LOAD_error_reading;
         } else {
            f->image = (Image *) malloc(sizeof(*f->image));
            make_image(f->image, x,y,data,n);
            barrier();
            f->status = LOAD_available;

            // wake up the main thread in case this new data is useful
            wake(WM_APP_DECODED);
         }
      }
   }
}

Image *source;
ImageFile *source_c;

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

void make_image(Image *z, int image_x, int image_y, uint8 *image_data, int image_n)
{
   int i;
   z->pixels = image_data;
   z->x = image_x;
   z->y = image_y;
   z->stride = image_x*BPP;
   z->frame = 0;

   // swap RGB to BGR
   for (i=0; i < image_x*image_y*BPP; i += BPP) {
      unsigned char t = image_data[i+0];
      image_data[i+0] = image_data[i+2];
      image_data[i+2] = t;
      #if BPP==4
      if (image_n == 4) {
         // apply alpha
         unsigned char *p = image_data+i;
         int a = (255-p[3]);
         p[0] += (((200 - (int) p[0])*a)>>8);
         p[1] += (((100 - (int) p[1])*a)>>8);
         p[2] += (((200 - (int) p[2])*a)>>8);
      }
      #endif
   }
}

void frame(Image *z)
{
   int i;
   z->frame = FRAME;
   memset(z->pixels, 0, FRAME*z->stride);
   memset(z->pixels + z->stride*(z->y-FRAME), 0, FRAME*z->stride);
   #ifdef GREY
      memset(z->pixels + z->stride*FRAME2 + FRAME2*BPP, GREY, (z->x-FRAME2*2)*BPP);
      memset(z->pixels + z->stride*(z->y-FRAME2-1) + FRAME2*BPP, GREY, (z->x-FRAME2*2)*BPP);
   #endif
   for (i=FRAME; i < z->y-FRAME; ++i) {
      memset(z->pixels + i*z->stride, 0, FRAME*BPP);
      memset(z->pixels + i*z->stride + (z->x-FRAME)*BPP, 0, FRAME*BPP);
   }
   #ifdef GREY
   for (i=2; i < z->y-2; ++i) {
      memset(z->pixels + i*z->stride+FRAME2*BPP, GREY, BPP);
      memset(z->pixels + i*z->stride + (z->x-FRAME2-1)*BPP, GREY, BPP);
   }
   #endif
}

void imfree(Image *x)
{
   free(x->pixels);
   free(x);
}

Image image_region(Image *p, int x, int y, int w, int h)
{
   Image q;
   q.stride = p->stride;
   q.x = w;
   q.y = h;
   q.pixels = p->pixels + y*p->stride + x*BPP;
   return q;
}

static void image_resize(Image *dest, Image *src, ImageFile *cache);

Image *cur;

void display(HWND win, HDC hdc)
{
   RECT rect,r2;
   HBRUSH b = GetStockObject(BLACK_BRUSH);
   int w,h,x,y;
   GetClientRect(win, &rect);
   w = rect.right - rect.left;
   h = rect.bottom - rect.top;
   x = (w - cur->x) >> 1;
   y = (h - cur->y) >> 1;
   platformDrawBitmap(hdc, x,y,cur->pixels, cur->x, cur->y, cur->stride);
   // draw in the borders
   r2 = rect;
   r2.right = x;           FillRect(hdc, &r2, b); r2=rect;
   r2.left = x + cur->x;   FillRect(hdc, &r2, b); r2 = rect;
   r2.left = x;
   r2.right = x+cur->x;
   r2.bottom = y;          FillRect(hdc, &r2, b); r2 = rect;
   r2.left = x;
   r2.right = x+cur->x;
   r2.top  = y + cur->y;   FillRect(hdc, &r2, b);
}

typedef struct
{
   int x,y;
   int w,h;
} queued_size;
queued_size qs;

struct
{
   queued_size size;
   Image *image;
} pending_resize;

typedef struct
{
   ImageFile *src;
   Image dest;
   Image *result;
} Resize;

Resize res;

void * work_resize(void *p)
{
   Resize *r = (Resize *) p;
   image_resize(&r->dest, r->src->image, r->src);
   return r->result;
}

stb_workqueue *resize_workers;

void compute_size(int gw, int gh, int sw, int sh, int *ox, int *oy)
{
   gw -= FRAME*2;
   gh -= FRAME*2;
   sw -= FRAME*2;
   sh -= FRAME*2;
   if (gw*sh > gh*sw) {
      *oy = gh;
      *ox = gh * sw/sh;
   } else {
      *ox = gw;
      *oy = gw * sh/sw;
   }
}

void queue_resize(int w, int h, ImageFile *src_c, int immediate)
{
   Image *src = src_c->image;
   Image *dest;
   int w2,h2;

   if (!immediate) assert(pending_resize.size.w);

   // create (w2,h2) matching aspect ratio of w/h
   w -= FRAME*2;
   h -= FRAME*2;
   if (w*src->y > h*src->x) {
      h2 = h;
      w2 = h2 * src->x / src->y;
   } else {
      w2 = w;
      h2 = w2 * src->y / src->x;
   }
   assert(w2 >= 0 && h2 >= 0);
   dest = bmp_alloc(w2+FRAME*2,h2+FRAME*2);   assert(dest);
   frame(dest);

   res.src = src_c;
   res.dest = image_region(dest, FRAME, FRAME, w2, h2);
   res.result = dest;

   if (!immediate) {
      src_c->status = LOAD_resizing;
      pending_resize.image = NULL;
      stb_workq(resize_workers, work_resize, &res, &pending_resize.image);
   } else {
      image_resize(&res.dest, src, NULL);
      pending_resize.image = res.result;
      res.result = NULL;
   }
}

void enqueue_resize(int left, int top, int width, int height)
{
   if ((width == cur->x && height >= cur->y) || (height == cur->y && width >= cur->x)) {
      // no resize necessary, just a variant of the current shape
      MoveWindow(win, left, top, width, height, TRUE);
      InvalidateRect(win, NULL, FALSE);
   } else {
      qs.x = left;
      qs.y = top;
      qs.w = width;
      qs.h = height;
   }
}

void ideal_window_size(int w, int h, int *w_ideal, int *h_ideal, int *x, int *y);

void update_source(ImageFile *q)
{
   Image *z = q->image;
   int w2,h2;
   int w,h,x,y;

   source = z;
   source_c = q;

   w2 = source->x+FRAME*2, h2 = source->y+FRAME*2;
   ideal_window_size(w2,h2, &w,&h, &x,&y);

   if (w == source->x+FRAME*2 && h == source->y+FRAME*2) {
      int j;
      unsigned char *p = z->pixels;
      free(cur);
      cur = bmp_alloc(z->x + FRAME*2, z->y + FRAME*2);
      frame(cur);
      {
         for (j=0; j < z->y; ++j) {
            unsigned char *q = cur->pixels + (j+FRAME)*cur->stride + FRAME*BPP;
            memcpy(q, p, z->x*BPP);
            p += z->x*BPP;
         }
      }
      MoveWindow(win, x,y,w,h, TRUE);
      InvalidateRect(win, NULL, FALSE);
   } else {
      #if 1
      qs.x = x;
      qs.y = y;
      qs.w = w;
      qs.h = h;
      #else
      pending_resize.size.x = x;
      pending_resize.size.y = y;
      pending_resize.size.w = w;
      pending_resize.size.h = h;
      queue_resize(w,h, q, FALSE);
      #endif
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

void init_filelist(void)
{
   char *s = NULL;
   int i;
   if (fileinfo) {
      filename = s = strdup(fileinfo[cur_loc].filename);
      for (i=0; i < stb_arr_len(fileinfo); ++i) {
         free(fileinfo[i].filename);
      }
      stb_arr_free(fileinfo);
   }

   image_files = stb_readdir_files_mask(path_to_file, "*.jpg;*.jpeg;*.png;*.bmp");
   if (image_files == NULL) error("Error: couldn't read directory.");

   stb_fixpath(filename);
   stb_create_thread(diskload_task, NULL);
   stb_create_thread(decode_task, NULL);
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

void flush_cache(void)
{
   // @TODO
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
      if (z->status == LOAD_available) {
         if (make_current)
            update_source((ImageFile *) z);
         return;
      }
      if (z->status != LOAD_inactive) {
         // there was an error loading it... @todo, display the error
         return;
      }
      // it's a go, use z
   } else {
      int i;
      // find a cache slot
      for (i=0; i < MAX_CACHED_IMAGES; ++i)
         if (cache[i].status == LOAD_unused)
            break;
      if (i == MAX_CACHED_IMAGES) {
         // @TODO: find lru
         return;
      }
      z = &cache[i];
      free(z->filename);
      z->filename = strdup(filename);
      z->lru = 0;
      z->status = LOAD_inactive;
      stb_sdict_add(file_cache, filename, (void *) z);
   }
   assert(z->status == LOAD_inactive);

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
   stb_sem_waitfor(cache_mutex);
   dc.num_files = 0;
   queue_disk_command(&dc, cur_loc, 1);           // first thing to load: this file
   queue_disk_command(&dc, wrap(cur_loc+dir), 0); // second thing to load: the next file (preload)
   queue_disk_command(&dc, wrap(cur_loc-dir), 0); // last thing to load: the previous file (in case it got skipped when they went fast)

   if (dc.num_files) {
      dc_shared = dc;
      stb_sem_release(disk_command_queue);
   }
   stb_sem_release(cache_mutex);
   // tell loader not to bother with old data
   for (i=0; i < MAX_CACHED_IMAGES; ++i)
      if (cache[i].lru < lru_stamp-1)
         cache[i].bail = 1;
}


#define int(x)  ((int) (x))

void resize(int step)
{
   // first characterize the current size relative to the raw size
   int x = source->x, y = source->y;
   float s;
   int x2,y2;
   int zoom=0;
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

   {
      RECT rect;
      GetWindowRect(win, &rect);
      x = (rect.left + rect.right)>>1;
      y = (rect.top + rect.bottom)>>1;
      x -= x2>>1;
      y -= y2>>1;
      enqueue_resize(x,y,x2,y2);
   }
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
   }
   if (h2 < 12) {
      h2 = h >> 2;
      if (h2 < 4) h2 = h >> 1;
   }
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
            GetWindowRect(win, &rect);
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
               GetWindowRect(win, &rect);
               assert(rx || ry);
               #define LIMIT 16
               if (rx < 0) rect.left   = stb_min(rect.left+x-ex, rect.right+-LIMIT);
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

int best_lru = 0;

int WINAPI MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg) {
      case WM_CREATE: {
         win = hWnd;
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
         flush_cache();
         break;
      }

      case WM_MOUSEMOVE:
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
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
         hDC = BeginPaint(hWnd, &ps);
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

            case MY_CTRL | VK_OEM_PLUS:
            case MY_CTRL | MY_SHIFT | VK_OEM_PLUS:
               resize(1);
               break;
            case MY_CTRL | VK_OEM_MINUS:
               resize(-1);
               break;

            case MY_ALT | '\r':
               // alt-enter
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
      *w_ideal = w;
      *h_ideal = h;
   } else if (w - FRAME*2 <= cx2 && h - FRAME*2 <= cy2) {
      *w_ideal = w;
      *h_ideal = h;
   } else {
      // will we show more if we use the full desktop?
      int w1,h1,w2,h2;
      compute_size(cx ,cy ,w,h,&w1,&h1);
      compute_size(cx2,cy2,w,h,&w2,&h2);
      if (h1 > h2*1.25 || w1 > w2*1.25) {
         *w_ideal = stb_min(cx,w1);
         *h_ideal = stb_min(cy,h1);
      } else {
         *w_ideal = stb_min(cx2,w2);
         *h_ideal = stb_min(cy2,h2);
      }
      // compute actual size image will be
      compute_size(*w_ideal, *h_ideal, w,h, &w,&h);
   }

   if ((cx != cx2 || cy != cy2) && w <= cx2+FRAME*2 && h <= cy2+FRAME*2) {
      *x = (cx2 - w) >> 1;
      *y = (cy2 - h) >> 1;
   } else {
      *x = (cx - w) >> 1;
      *y = (cy - h) >> 1;
   }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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

   hInst = hInstance;
   GlobalMemoryStatus(&mem);
   physmem = mem.dwTotalPhys;

   /* Register the frame class */
   memset(&wndclass, 0, sizeof(wndclass));
   wndclass.cbSize        = sizeof(wndclass);
   wndclass.style         = CS_OWNDC;
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

   if (argc < 1) { MessageBox(NULL, "Specify an image file to view", "imv(stb)", MB_OK); exit(0); }
   
   resize_workers = stb_workq_new(resize_threads, resize_threads * 4);
   cache_mutex = stb_sem_new(1,0);
   disk_command_queue = stb_sem_new(1,1);
   decode_queue = stb_sem_new(1,1);

   image_data = stbi_load(argv[0], &image_x, &image_y, &image_n, BPP);
   if (image_data == NULL) {
      char *why = stbi_failure_reason();
      char buffer[512];
      sprintf(buffer, "'%s': %s", lpCmdLine, why);
      error(buffer);
      exit(0);
   }
   stb_fixpath(argv[0]);
   stb_splitpath(path_to_file, argv[0], STB_PATH);
   filename = argv[0];

   source = malloc(sizeof(*source));
   make_image(source, image_x, image_y, image_data, image_n);

   cache[0].status = LOAD_available;
   cache[0].image = source;
   cache[0].lru = lru_stamp++;
   cache[0].filename = strdup(filename);
   file_cache = stb_sdict_new(1);
   stb_sdict_add(file_cache, filename, (void *) &cache[0]);


   {
      int x,y;
      int w2 = source->x+FRAME*2, h2 = source->y+FRAME*2;
      int w,h;
      ideal_window_size(w2,h2, &w,&h, &x,&y);

      if (w == source->x+FRAME*2 && h == source->y+FRAME*2) {
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
         cur = pending_resize.image;
         pending_resize.image = NULL;
      }

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
         if ((qs.w == cur->x && qs.h >= cur->y) || (qs.h == cur->y && qs.w >= cur->x)) {
            // no resize necessary, just a variant of the current shape
            MoveWindow(win, qs.x,qs.y,qs.w,qs.h, TRUE);
            InvalidateRect(win, NULL, FALSE);
         } else {
OutputDebugString("Enqueueing resize\n");
            pending_resize.size = qs;
            queue_resize(qs.w, qs.h, source_c, FALSE);
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
OutputDebugString("Finished resize\n");
               imfree(cur);
               cur = pending_resize.image;
               SetWindowPos(hWnd,NULL,pending_resize.size.x, pending_resize.size.y, pending_resize.size.w, pending_resize.size.h, SWP_NOZORDER);
               barrier();
               pending_resize.size.w = 0;
               display(hWnd, hDC);
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
   ImageFile *src_c;
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

volatile reentry;
void image_resize(Image *dest, Image *src, ImageFile *src_c)
{
   ImageProcess proc_buffer[16], *q = stb_temp(proc_buffer, resize_threads * sizeof(*q));
   SplitPoint *p = stb_temp(point_buffer, dest->x * sizeof(*p));
   int i,j0,j1,k;
   float x,dx,dy;
   assert(reentry == 0);
   assert(src->frame == 0);
   ++reentry;
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
      q[i].src_c = src_c;
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

   if(src_c) src_c->status = LOAD_available;
   stb_tempfree(point_buffer, p);
   stb_tempfree(proc_buffer , q);
   --reentry;
}
