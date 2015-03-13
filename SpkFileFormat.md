# Introduction #

This is the SPK file format document from daeyna.com, since
it's no longer accessible at that site and isn't on archive.org.


# Details #

```

January 17, 2008

SPK is a simple image file format that encodes an image as a delta
from another image stored separately. decoding an SPK file requires
access to this other file, which requires access to a file system.
it is not appropriate for streaming or general purpose usage. it is
primarily designed for archival purposes to reduce storage. see the
separate rationale section for more information.


file format
===========

all integers are stored as 4-byte little-endian integers.

header:

+---------+------+---------------------+----------------------------------+
|  Offset | Size | Value               | Purpose                          |
+---------|------|---------------------+----------------------------------+
|       0 |  16  | "xPIC-delta-image"  | signature                        |
|      16 |   1  |  byte VERSION=0     | version number                   |
|      17 |   4  |  integer FLEN       | length of following filename     |
|      21 | FLEN |  utf8 string NAME   | filename with terminating \0     |
| FLEN+21 |   4  |  integer WIDTH      | width of image in pixels         |
| FLEN+25 |   4  |  integer HEIGHT     | height of image in pixels        |
| FLEN+29 |   4  |  integer CHN        | number of 8-bit channels in image|
+---------|------|---------------------+----------------------------------+

note that the byte at FLEN+20 must be 0--it is the terminating nul for
the filename string, which simplifies decoding (such as if you memory map
the file).

after the header comes repeated "delta packets", until the end of the file
is reached. these are encoded as follows, assuming the start of the packet
is at location PCKT:

+------------+------+---------------------+----------------------------------+
|     Offset | Size | Value               | Purpose                          |
+------------|------|---------------------+----------------------------------+
| PCKT  +  0 |   4  | integer START       | offset in image of delta pixels  |
| PCKT  +  4 |   4  | integer LEN         | number of delta pixels in packet |
| PCKT  +  8 |  CHN | pixel               | first replacement pixel          |
| PCKT+CHN+8 |  CHN | pixel               | second replacement pixel         |
|    ...     |  ... |     ...             |  ...                             |
+------------|------|---------------------+----------------------------------+


interpretation
==============

the version number VERSION must be 0.

the NAME field contains the name of an image file stored in the same
directory as the SPK file. the name must not contain the character '/'
the character ':', or the character '\', since these are filename
separators. a decoder MUST reject SPK files using these characters, to
avoid security issues.

the NAME is interpreted as a utf8 string that names a file in the same
directory. for version 0, that file MUST be a PNG file with 8-bit channels.
the specified name should be an exact match of the name of the file in
the filesystem, including case. the named file MUST NOT be an SPK file.

after locating the file called NAME, a decoder should decode the PNG
file it contains and, if paletted, convert it to a non-palettized form
(e.g. expand the palette entries). the PNG should be decoded with an
alpha channel if transparency information is available in the file in
any form.

if the width of the decoded PNG is not equal to WIDTH, the decoder MUST
reject the SPK file. the the height of the decoded PNG is not equal to
HEIGHT, the decoder MUST reject the SPK file. if the number of channels
in the decoded PNG are not equal to CHN, the decoder MUST reject the SPK
file. the number of channels in the PNG is, for a monochrome image, 1
if no alpha channel or 3 if an alpha channel; or for a color image, 3
if no alpha channel else 4 if no alpha channel.

(if the file has not been rejected at this point, it will be accepted;
there are no further rejection cases.)

at this point the "delta packets" are applied to the image. this process
is modeled on the assumption that the PNG file is decoded into a continuous
stream of pixels, with the first row of pixels, then the second, then the
third each consecutive in memory.

each delta packet specifies a horizontal run of pixels to change. these
horizontal runs are identified by their starting pixel offset location, not
their coordinates, and the runs can cross multiple scanlines. the "pixel
offset" of the top-left pixel is 0; that of the pixel at (1,0) is 1; that
of the pixel at (0,1) is WIDTH.

each replacement pixel in the delta packet consists of CHN bytes, which
replace the corresponding pixel in the PNG image. (they are not alpha-blended;
they are directly replaced). the bytes in each pixel are stored in the
following order:

   +-----|--------------------------|
   | CHN |  byte ordering           |
   +-----|--------------------------|
   |  1  |  luminance               |
   |  2  |  lum, alpha              |
   |  3  |  red, green, blue        |
   |  4  |  red, green, blue, alpha |
   +-----|--------------------------|

each successive pixel in the delta packet occurs at an increasing pixel
offset. For example, with START=10, LEN=3, the packet modifies the 3
pixels at offsets 10, 11, and 12. If CHN is 4, the pixel data will take
up 12 bytes, and the header will take up 8 bytes, so the total packet
will require 20 bytes.

after the end of a packet, a new packet begins on the very next byte.

let END = WIDTH * HEIGHT, i.e. one more than the largest valid offset.
if START >= END, or START+COUNT-1 >= END, or START+COUNT < START, the
decoder MUST stop processing delta packets and discard the rest of the
SPK file data and produce the so-far-processed image. (in other words,
if the range specified includes any pixels outside of the image.)

if end of file is reached in the middle of a delta packet, the decoder
MAY process the available part of the packet or MAY discard it entirely.
in either case it MUST produce the so-far-processed image.

if the pixel regions specified in the delta packets overlap each other,
the decoder MUST produce results consistent with the above description
(that is, the produced pixel value comes from the last delta packet
that overlaps it).

this completes the specification.


rationale
=========

SPK files exist primarily to achieve more compressed archival of sets of
images that are very similar. we identify two important cases:

   1. the set is stored in a directory
   2. the set is stored in a compressed archive (zip, rar, 7z, etc.)

the goal of SPK is to minimize storage in both these cases, without
causing excessive overhead or complexity for image decoding.

SPK is only intended to address _lossless_ compression; if the files
are lossily compressed, SPK is not applicable.

the current standard for meeting the goal in case #1 in a lossless case
is to use PNG.

the current standard for meeting the goal in case #2 is the use of
"solid" 7-zip archives. a "solid" archive allows for compression across
multiple files, finding shared elements between them. because PNG files
are already compressed, common elements in the original images are likely
to not be common in the PNG files. to address this, the standard approach
is to compress BMP files in a solid 7z archive.

unfortunately, this means that converting between these two formats
requires both unpacking and then converting BMPs to PNGs. the intermediate
step involves having all the BMPs unpacked; since the BMPs are uncompressed,
this can take up inordinate amounts of space. (the sets that motivated
the creation of SPK were multiple gigabytes as PNGs, and tens of gigabytes
as BMPs!)

SPK is intended to allow one or a few images from a similar-set to be
encoded as PNGs, and the rest to be encoded as SPKs. the SPK files are
themselves uncompressed for simplicitly (it would be expensive to encode
each individual delta packet as a PNG, and using rectangles would be
very complicated for the encoder and might not be efficient). the SPK
file cannot be relative to another SPK file, which reduces the possible
space efficiency but keeps decoding fast and simple. although in some
cases a set is generated by applying several independent details in
combination, so many SPK files will be applying the exact same sets of
changes to a base file, we do not attempt to share this data, again for
speed and simplicity.

with a solid archive, however, the delta packets will often be identical
between multiple SPK files, and the solid archiving will further compress
them. this allows further savings in the compressed case.

some image viewers can view directly from archive files. however, viewing
a solid archive cannot be efficient, since random access is not possible.
in the non-solid case, the compressors can still reduce the size of the
SPK files (which aren't compressed), while leaving open the possibility of
random access. however, the image viewer will still need to decode two files,
so we doubt anybody would actually implement it.

we tested with two sample data sets. the first involved 204 image
files, the second involved 1,212 image files. our encoder for the
first set preserved 37 PNG files, while that for the second preserved
56 PNG files.

+-----------------------+--------------+---------------+
|  compression method   |  set 1 size  |   set 2 size  |
+-----------------------+--------------+---------------+
| BMP files             |  152,859,816 | 1,381,745,448 |
| PNG files             |   36,952,795 |   247,963,721 |
| PNG + SPK files       |    7,300,912 |    24,933,386 |
+-----------------------+--------------+---------------+
+ PNG non-solid RAR     |   36,951,803 |   247,641,338 |
| PNG solid RAR         |   30,061,920 |   194,108,416 |
| PNG solid 7z          |   26,860,657 |   145,120,893 |
| BMP solid RAR         |   16,760,954 |       [*]     |
| BMP solid 7z          |    4,081,486 |    18,612,638 |
| PNG+SPK non-solid RAR |    6,962,703 |    21,005,839 |
| PNG+SPK solid RAR     |    5,633,416 |    11,246,404 |
| PNG+SPK solid 7z      |    5,558,082 |    10,908,167 |
+-----------------------+--------------+---------------+

we omitted [*] because solid RAR is just never competitive with solid 7z.

according to these results, when stored as files, not in archives, use of .SPK
files for sets of images with many similar images can achieve a significant
storage savings, in our case, between 5x and 10x.

additionally, they are competitive with the standard way of minimizing
storage in archives. For non-solid archives, they are strictly superior
(showing a range of 6x to 12x as expected--since PNGs are already compressed,
non-solid compression offers little additional gain). for solid archives,
PNG+SPK they are always superior to PNG or BMP RAR solid archives, and they
are always superior to PNG 7z archives. Compared to the best standard
compression, BMP 7z, they are sometimes smaller and sometimes larger, in
these two cases showing a range of 1:1.5 (worse) to 2:1 (better). we do not
know how well these generalize (are they more often better or more often
worse?)

based on these results, we highly recommend using PNG+SPK in solid 7z archives
as a mechanism for distributing sets of highly-similar images. the size
result is comparable, but the decode step does not require decoding through
an enormous intermediate step. instead, the archive will decompress to the
recommended smallest-possible format for storing files. at this point, the
PNG+SPK directory can be expanded to purely PNG images to maximize
compatibility with other applications, or used directly if compatible image
viewers exist. the only drawback is the lack of an SPK => PNG decoder on
all platforms. we have provided a windows implementation and the source
code so it can be ported, but we have not attempted a Mac or Linux port
ourselves.

```