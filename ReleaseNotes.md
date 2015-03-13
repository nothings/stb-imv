# Release #

> version 1.01

# Release Notes #

> Version 1.01: Release 1.01 (2012-01-17)
    * bugfix: fix crash when closing preferences
    * feature: if image is entire alpha=0, assume opaque
    * feature: full-size expands on whichever monitor image is current on
    * change: don't stretch full-size image across multiple monitors (#define ALLOW\_MULTISCREEN for old behavior)

> Version 1.0: Release 1 ( 2008-10-19 )
    * feature: open a directory
    * feature: sharpen when upscaling
    * secret feature: toggle use of full-size virtual desktop

> Version 0.99: Beta 11 ( 2008-02-07 )
    * bugfix: further attempt to support BACK/FORWARD

> Version 0.98: Beta 10 ( 2008-01-31 )
    * bugfix: attempt to support BACK/FORWARD mouse buttons
    * feature: custom .spk image-delta file format
    * secret feature: recursive slideshows with clumsy UI using 'ctrl-R' and '.'

> Version 0.97: Beta 9 ( 2007-11-27 )
    * bugfix: if starting path is unicode don't blow up trying to read the directory

> Version 0.96: Beta 8 ( 2007-10-20 )
    * feature: VC7 project files
    * feature: imv\_light.exe - uses GDI+ only, not stb\_image or FreeImage
    * feature: press 's' for a primitive slideshow of current directory
    * feature: HDR support in stb\_image
    * feature: TGA support in stb\_image
    * feature: save border choice to registry (expose in prefs)
    * secret feature: use { } [ ] to rescale dark/light images

> Version 0.95: Beta 7 ( 2007-08-15 )
    * bugfix: minor stb\_image.c fixes
    * bugfix: don't have gdi+ use threads; correct lock/unlock of global memory
    * bugfix: load non-7bit-filenames from commandline
    * feature: ctrl-i launches new viewer instance on current image
    * bugfix: fix cacheing code to allow refreshing current image after flip etc.
    * bugfix: clean up repainting when dragging top or left to avoid dragging old data
    * bugfix: fix out-of-control resize when border is off

> Version 0.94: Beta 5 (2007-07-15)
    * bugfix: changing image resize quality in preferences refreshes on OK
    * feature: sort filenames to sort numbers in human-friendly order
    * feature: show index within sorted list
    * feature: ctrl-C puts current image filename (with full path) in the clipboard
    * change: change the border control keys to 'b' instead of 'f' for next feature
    * feature: use FreeImage.dll if it's available
    * bugfix: fix bug in right or bottom cursor region due to internal cleanup

> Version 0.93: Beta 4 (2007-07-10)
    * bugfix: alter stb\_image to support jpegs with weird header blocks
    * bugfix: exit after printing directory error message
    * bugfix: change naming of frame/border variables
    * bugfix: ESC when showing help clears help, rather than exiting
    * internal: clean up registry code to halve registry ops

> Version 0.92: Beta 3 (2007-07-03)
    * internal: replace Sleep()-based thread-joining code with synchronization primitive
    * internal: change work queue internals to use stb\_mutex
    * internal: change stb\_mutex from using win32 semaphore to using CRITICAL\_SECTION
    * internal: stbi\_load\_from\_memory() only; remove stdio from stb\_image (500 bytes)

> Version 0.91: Beta 2 (2007-07-01)
    * feature: allow changing the label font size, toggle label in preferences
    * internal: various refactorings to clean up the code
    * bugfix: finish commenting code (except resizer)
    * bugfix: fix tiny leak closing the preferences dialog with the close button

> Version 0.90: Beta 1 (2007-06-30)
    * bugfix: user-friendlier error messages
    * feature: save preferences to registry
    * feature: preferences dialog

> Version 0.57 (2007-06-29)
    * feature: cubic image resampling
    * bugfix: advancing to pre-loaded image then retreated to previous image
    * bugfix: occasional error when advancing to image that was about to be decoded
    * bugfix: commented about 75% of code
    * bugfix: fix logic for fitting large images onscreen to not stop a few pixels short

> Version 0.56 (2007-06-27)
    * bugfix: stb\_image wouldn't load jpegtran output (which is invalid JFIF)

> Version 0.55 (2007-06-27)
    * feature: toggle filename label
    * feature: toggle entire border
    * feature: toggle white stripe in border
    * bugfix: display error message for files that don't load

> Version 0.54 (2007-06-26)
    * bugfix: keep current window position while switching windows in actual-size mode
    * feature: mousewheel to resize
    * feature: checkerboard border behind alpha - but image sized, not zoom-independent
    * feature: integrated help with F1/h/?
    * bugfix: resizing with ctrl- and ctrl+ correctly sets the actual-size-mode

> Version 0.53 (2007-06-25)
    * feature: double-click, alt-enter to toggle actual-size vs. fullscreen
    * feature: ctrl-O lets you open an arbitrary file
    * feature: added Open File dialog if you run without a commandline
    * bugfix: changing images doesn't change window size except in actual-size mode

> Version 0.52 (2007-06-25)
    * bugfix: hang when resizing first image