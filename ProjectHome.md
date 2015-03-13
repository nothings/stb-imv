Project status: 1.02 release

# Information for Users #

**imv_(stb)_** is an extremely lightweight and fast image viewer/browser for the Windows platform. The current executable is about 70KB (without executable compression).

## Features ##

Inspired by [vjpeg](http://www.stereopsis.com/vjpeg/), **imv_(stb)_** offers a very simple, no-frills image viewer with a minimal interface. Each opened image is easy to drag and resize. A minimal border around the image makes it easy to see the contents of the edge of the image, and (compared to vjpeg) prevents you from confusing a screenshot of a windows application with the actual application.

Inspired by Windows Picture and Fax Viewer, **imv_(stb)_** lets you navigate through multiple images in the same directory in a single window. Unlike Windows Picture and Fax Viewer, you can have multiple **imv(stb)** browsers open simultaneously.

Details of the latest changes can be found in the ReleaseNotes.

## Limitations ##

It is not possible to zoom in on part of an image; in other words, you can't zoom to the point where you'd need scrollbars. (Well, you can if you get sneaky, but you don't get scrollbars.)

## Issues, Bugs, Feature Requests ##

To submit a bug report, go here: [Issues](http://code.google.com/p/stb-imv/adminIssues)

To submit a suggestion or a feature request, go here: FeatureRequests

# Information for Developers #

## Image Loading ##

**imv(stb)** is built around http://nothings.org/stb_image.c, a public domain, portable JPEG/PNG/BMP/TGA reader. stb\_image.c has some limits; it doesn't handle progressive jpegs or interlaced PNGs, or 1bpp BMPs. To improve this, we dynamically load gdiplus if available (it supports many interesting file formats, and is available on most win32 machines), and dynamically load FreeImage if it's available (it supports many, many more file formats and isn't on most people's machines at all). We use stb\_image first, although it's not necessary the fastest, to provide more testing bandwidth for stb\_image.c, and because it's well-behaved and sometimes fast.

It is also possible to improve stb\_image.c or add new loaders for other types. Because Google Code does not support projects with a public domain (PD) "license", there is no Google Code project for stb\_image.c. We do not generally want to fork stb\_image.c, though, so changes to stb\_image.c in this project should all be public domain; no changes that alter the copyright status of stb\_image.c will be accepted. If you want to write new loaders that are GPL'd rather than PD'd, simply place them in separate files, and either make imv.c call them if stbi\_load fails, or use stb\_image's 'register a new loader' feature.

The current version of **imv(stb)** falls back to GDI+ and then to FreeImage if they can be dynamically loaded.

## Portability ##

**imv(stb)** itself is non-portable, offering a simple, flexible, highly-tuned user interface hard-coded to the windows platform. If you back to its earliest state it might be a reasonable starting place for a port.

The project is currently available with VC6 and VC7 project files for maximum compatibility. The automatic upgrade to VS2005 should just work.

## Performance ##

**imv(stb)** makes heavy use of threads. If you use it to browse images in a directory, it will read ahead (in either direction) by one image. Disk I/O is performed in one thread, jpeg/png decoding in a second thread, and a single image-resize operation is performed in parallel on one or more other threads (as many threads as you have processors). The main thread accepts input as fast as it can and manages the other threads (maintaining single-threaded ownership of most data but lending it as needed). For example, when you drag a corner to resize, image resize operations are "queued", but the window itself is not resized until the image resize operation completes.

The goals/ideals motivating the design are:
  1. Minimize the latency of the user experience
  1. Don't waste work
  1. Maximize the quality of the user experience
  1. If you've started doing something, finish it (corollary to 2)

These principles lead to the following threading/performance design:
  * Keep a cache of decoded images (1,2)
  * If you've loaded the image data from disk, eventually it'll get decoded (2)
  * The user can browse "ahead" of the system to a "goal image" (1)
  * Always decode the "goal image" before anything else pending; generally, decode in most-recently-browsed order (1)
  * At any given time, try disk-reading the goal image and the image before and after (1)
  * At any given time, _only_ try disk-reading the goal image and the image before and after (2)
  * If you've started decoding an image, go ahead and finish decoding it (4)
  * If you've started resizing an image, finish it and show it (4)
  * Resizer always resizes whichever decoded image is most-recently-browsed (1)
  * Don't resize the window until the resizer is done (3)
  * Draw the window without clearing it (3)
  * Don't start doing file readaheads until the first time the user browses (i.e. the first time you step to a next/prev image) (2)
  * Don't use threads (except for resizing) on opening the first image, to make a clear, optimized code path (threads couldn't reduce this latency anyway) (1,2,3)
  * Although decoded images are cached, do not cache non-decoded data, let the OS do that (2)
  * Use multiple threads for a single resize operation (1)

This leads to the following behaviors:
  * When browsing images at moderate speeds, switching images only takes the time of a single resize operation
  * When browsing images faster that they can be decoded, the user will see whichever images manage to be decoded flip by, in (sparse) browse order, but all are being decoded, so if you go back through the skipped images it's fast
  * When browsing images faster than they can be read from disk, the user will see whichever images manage to be read and decoded flip by, in (sparse) browse order. Old load requests are dropped entirely, though, so if they go back through them they'll still need loading.
  * In other words, we have to trade off between pre-decoding and wasting work. If we outrun file I/O, we choose to avoid wasting work and simply don't read and don't decode. If the file I/O keeps up, having loaded the data we go ahead and decode it, even though that's wasted work if we never go back. There isn't a _really_ good reason for handling the two cases differently, but an approximate one is that the viewer is a foreground task competing with background tasks, and it seems ok to claim lots of CPU but annoying to grind the disk (e.g. for users running peer-to-peer applications  or large file copies files in the background).

The first of these, and a general 'see some full-sized, fully-decoded images if you scan too fast' were part of the initial design requirements that motivated the specifics of the implementation.

## Todo ##
  * additional optimizations
  * refactorings to reduce size

## Possible Todo ##
  * refresh current image, refresh directory list
  * ReadDirectoryChangesW

If you want any of the features above, feel free to create an issue for it so we know there's interest.

## Done ##
  * build under VS2003
  * UNICODE (currently we support non-8bit filenames that you iterate through in a directory, but only a very few open from the commandline, and none(?) from the open file dialog)
  * Iterate through images recursively in subdirectories? (stb.h already includes a recursive readdir, so it would be trivial)
  * Slideshow mode (WM\_TIMER triggers advance(1))
  * label settings in preferences
  * comment source code (partway done)
  * better error messages in stb\_image.c
  * Preferences dialog
  * Get config info from registry (cache size, image frame preference)
  * The current bilinear resampling is a placeholder; the plan is to upsample with bicubic, and downsample with the nearest integral-sized box-filter followed by a bilinear filter
  * Display filename as label at bottom
  * Allow toggling the grey highlight in the image frame, and toggling the frame entirely
  * Display error messages for files that fail to load
  * when switching windows, leave window in current position if possible
  * mousewheel zoom
  * integrated help on F1
  * File Open dialog
  * Need an image-switching-mode 'use actual/best size' vs. 'use current size' (currently does the former). Double-click/alt-enter to switch to between actual/best size and "fullscreen" on primary, and back. Whenever you resize the window or switch to fullscreen, switch the image-switching-mode to 'use current size'; when you double-click back to best size, set the image-switching-mode to that.