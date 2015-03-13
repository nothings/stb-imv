After reading this page, you can go here to request a feature: [Issues](http://code.google.com/p/stb-imv/issues/list)

Check the Open and Closed issues to make sure it hasn't already been
requested. If not, go ahead and create a new issue. Don't worry about
the fact that it will be called a 'defect' by default.

Potential developers should also read this: RejectedRequestExample

# Information for Users #

**stb(imv)** is an image viewing program. Unless someone has an
_extremely_ compelling reason, we're not going to consider
feature requests that change that.

So, for example, we will **not** be adding features like:
  * Image filters
  * Photoshop-style Levels or Curve operations

On the other hand, the following features _are_ the sort of things
that might qualify as features appropriate for a viewing program.

  * image rotation (display only, no saving)
  * gamma correction (display only, no saving)
  * view multiple thumbnails in one window

# Information for Developers #

All the things above apply. But also:

  * Do not suggest features _because_ the implementation is easy (see notes below)
  * Do not suggest features merely by describing the implementation; describe the feature in user terms (as suggested above)

## On the Cost of Implementing Features ##

Developers especially need to be cautious about saying that features are easy. There are four major costs in implementing a feature:

  1. Development time to implement the feature
  1. Increase in the size of the executable
  1. Performance/memory overhead when not using the feature
  1. Maintainence overhead for continued development of the program

Of these, #1 is most visible, and #4 is the most important (and perhaps least-considered). So let me address the other two first.

2. Most small features will not increase the executable size much. As long as **imv(stb)** stays under 100KB, I'm not too worried, so there's plenty of room to add new features. But that doesn't mean a feature that uses 40KB is ok! A feature that requires 1KB is a no-brainer; we've got room for 50 of those. A feature that uses 10KB only leaves us room for 4 more, so it better be darn important.

3. Performance overhead for users who don't use the feature must be minimized. In some cases this is trivial, in other cases this may significantly expand the basic development cost.

That leads us to the first cost. Some possible features are non-starters due to #1; if no developer on the project is willing to devote the time necessary to implement it, it won't happen. Developer time-devotion-willingness tends to follow the developer's interests and tastes, but is influenced by user desires. Creating an issue, or starring an existing one, is a good way to influence the developers this way. Or, if you are a developer yourself, you might just devote the time yourself to implement it. However, if you implement something because you thinking it's easy, don't forget to account for the other three issues above.

As stated before, #4 is the doozy. With 2000 lines of code, a feature that can be added in 2-3 lines probably doesn't hurt much, but one which takes 100+ lines, or even just 5 lines in 5 different places, may have a non-linear effect on further development. For example, toggling the image border seems like it ought to be fairly easy, but its effect is fairly squirrely in the code base. Much of the code must reason about the image size and the window size. For simplicity, most of the code pretends the border is there while reasoning, and then at the last minute adjusts numbers as needed to compensate. But this logic doesn't apply everywhere; dragging the window just ignores the border entirely. To write any new code that pays attention to the window size, you now have to deal with the burden of worrying about this. So we have to be very careful about adding more features that introduce this kind of development overhead.

So, rule of thumb: trivial, local changes are fine by #4. If there's one place in the code where you can put an if() and do something different some of the time (plus preference setting and such), that's probably going to be fine by #4. But don't expect patches that add 200 lines of code, or change multiple places in the existing codebase, to be approved. (Unless there's high demand! Ask a developer first!)

But even a feature that is small in source and executable still has to pass the test of being useful to users as an image viewer behavior; otherwise we might get 200 features added all of which are random tangential things desired by individual developers. Even though each one by itself might be small, the effective increase in the codebase would be significant, which would hurt further development. Even if your feature is "optional", it still has significant costs. Does it require a keyboard toggle? There's only so much room to document UI on the F1 page, and even ignoring that, every added toggle _slows down users_ reading the documentation (if they don't want it). Each addition alone is trivial, but many additions add up; when there are 3x as many commands, it's a big deal.

Similarly, getting/saving preferences to the registry is trivial, but there's a significant mental difference in looking at a codebase that saves 6 prefrences rather than one that saves 100 preferences. Worse yet is the UI for setting that preference.