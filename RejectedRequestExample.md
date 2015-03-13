This page will be of interest to developers or potential developers.

# Introduction #

A programmer suggested using **UpdateLayeredWindow()** to support
(optionally) drawing images with alpha in them transparently on the desktop.
(I will call this behavior "desktop alpha".)

In the interests of helping developers understand why candidate features they
are considering might not be accepted to the code base, this page
describes why I rejected the above suggestion (in
the sense both of choosing not to implement it, but also making
clear that I was unlikely to accept a patch), and why it would have
been much harder to implement properly than the programmer probably imagined.

# Details #

As discussed in FeatureRequests, there are four major costs to
implementing a feature:
  1. Development time to implement the feature
  1. Increase in the size of the executable
  1. Performance/memory overhead when not using the feature
  1. Maintainence overhead for continued development of the program

A central premise to **imv(stb)** is to make significant trade-offs
for _performance_; that is, the fundamental threaded design has _huge_
costs of type #1 and #4. This was, in some sense, the premise of **imv(stb)**;
to accept that trade-off.

But because **imv(stb)** is so performance-oriented, I am strongly
opposed to any significant costs of type #3! That means additional
features must pay more #1 and #4 if needed to avoid unnecessary
overhead.

But I'm also opposed to significant costs in #4, unless the gain
to users is really significant. That can make it hard to implement
some features in any way that I find acceptable, as is the case
for this feature.

Now let's look at the actual issues of implementing this feature.

## Overview of problems ##

Just so they don't come totally out of left-field later, here are
the central issues in the existing code base and design that maDe
the proposed feature problematic at the time it was proposed:
  * compile-time switch to only store 3 bytes per pixel
  * image cache stores image already composited against background
  * no alpha resampling

## Implementation #1: proof-of-concept ##

Naively, the feature sounds pretty straightforward. There's an image
with alpha, **imv(stb)** currently loads the image and alpha blends
the image against a static background, and then it gets rendered.
So the things we need are (1) an image with alpha, (2) the ability
to draw it, and (3) some code to change the rendering path / window
type as appropriate.

How hard is a first cut, proof-of-concept implementation?

  * Disable the code that alpha-blends over a static background
  * Change the window type
  * Insert call(s) to UpdateLayeredWindow. There are also a number of calls to MoveWindow and SetWindowPos which may need changing, I don't know.

**Issues with implementation #1**

This just hard-codes the desktop alpha to be always on so we can see whether
it works or not. There are many other issues that won't work. For example,
you cannot resize an image (or use an image larger than the desktop),
because the resampler doesn't resample the alpha channel. This will
have to be addressed in a correct implementation.

## Implementation #2: real, but unsatisfactory ##

The following additional changes must be made:

  * Add a toggle for whether to make the background transparent or not
  * UpdateLayeredWindow obeys the toggle
  * If it's not transparent, you need to alpha-blend against the static background somewhere in the code (not in the current place, as will be discussed next section).
  * Modify the resampler to resample the alpha channel as well

**Issues with implementation #2**

This implementation is unsatisfactory because of the many ways it
increases the third type of cost, performance overhead for users
who don't need it.

  * The code can currently be compiled to use either 3 or 4 bytes per pixel. At the moment it uses 4 because the 32-bit resampler is faster. However, the 24-bit case could be sped up. I've chosen not to pursue that yet until we get the bicubic sampler in, which will be the more common performance case (upsampling is the slowest case). It's quite possible that the 24-bit case will be faster due to reduced cache usage; it will also have the benefit of using less memory, or being able to cache more decoded images in the same memory. The proposed alpha solution will simply not work if the code is recompiled to use 3 bytes per pixel.
  * Even if compiled for 4-bytes-per-pixel, resampling alpha correctly may incur performance overhead on the non-alpha-blended path.
  * We need to detect if images are opaque and force UpateLayeredWindow to run in opaque mode for those images, to avoid unnecessary alpha blending overhead
  * Even so, UpdateLayeredWindow is probably slower (and uses more memory) for opaque images, so really we should just use the existing path for opaque images and when desktop alpha is toggled off
  * The current implementation avoids the cost of alpha blending the image most of the time, because it's blended immediately after loading. The cached image is stored already blended. If we want to toggle desktop alpha on and off, we either need to always store the cached image non-blended--which costs an overhead when we're not doing desktop alpha (and note that the optimized bilinear resampler uses 5 integer multiplies per pixel; doing an alpha blend requires 2 integer multiplies per pixel, thus it's a potentially significant cost (40%))--or we need to cache the image maybe pre-blended and maybe not, and flush the cache when the user toggles it. Probably this should be lazy, so if a user browses through a bunch of images, then toggle one to transparent and the back, then browses back through them, they haven't been flushed. Or we need to keep two separate caches, one for each version (significantly increasing the storage overhead for images with alpha).

## Implementation #3: satisfies most requirements, but bloated ##

  * Replicate the resampler, so there's an optimized version for images with alpha, and an optimized version without
  * Switch the bytes-per-pixel from being a #define to being a per-image thing; then use 3 bytes for opaque images and 4 for ones with alpha. (Note that you can't get this easily out of stb\_image, because there's a separate issue of expanding greyscale to RGB. Either you let stb\_image return you whatever it's got, and replicate the grey->RGB conversion, or you modify stb\_image to allow querying the number of components WITHOUT decoding the image, and then use that result to decide whether to ask for 3 or 4.)
  * Make a #define to decide whether opaque images use 3 or 4 bytes.
  * Make the non-alpha resampler follow the #define.
  * Cache flushing as described above; need to record with each decoded image whether it's been pre-blended, and if it's the wrong type, flush it and redecode. This will interact messily with prefetching! There's no real good answer here. Perhaps we should cache images with alpha unblended, then the first time we need to display them blended, we blend them and update the cache with the blended version. If we need the unblended version and have the blended version, we discard the cache and reload. That fixes half the prefetch cases, but not the other half.

**Issues with implementation #3**

Now we have:

> A bunch of code squirreled everywhere through our app (all the MoveWindows,
> the support for multiple window types, switching window types on the fly,
> keeping track of the # of components in each image and whether it's been preblended, etc.)

> An inefficiency when you switch between desktop alpha and not (probably not the end of the world, since that toggling is likely rare, and you only pay the cost if you use it; but as described above, we have slightly slowed the prefetching path even when disabled, because now we don't pre-blend the alpha until display time, instead of immediately after decode).

And what do we get in return? A feature that is probably not very useful to most users. If there was no user other than the developer proposing the feature who wanted it, I have no doubt I would reject the patch, unless an efficient implementation turned out to be far simpler than I'm imagining it.