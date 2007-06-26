/* stb-1.89 -- Sean's Tool Box -- public domain -- http://nothings.org/stb.h
          no warranty is offered or implied; use this code at your own risk

This is a single header file with a bunch of useful utilities
for getting stuff done in C/C++.

Bug reports, feature requests, etc. can be mailed to 'sean' at the above site.

 ============================================================================
                                                                             
   You MUST                                                                  
                                                                             
      #define STB_DEFINE                                                     
                                                                             
   in EXACTLY _one_ C or C++ file that includes this header, BEFORE the
   include, like this:                                                                
                                                                             
      #define STB_DEFINE                                                     
      #include "stb.h"
      
   All other files should just #include "stb.h" without the #define.


 ============================================================================



1. Features overview         - a quick sketch of what you'll find
2. Manifest                  - other files of note
3. History                   - upgrading? here's what's changed
4. Documentation             - one-line docs for each function



                      --------------------------------
 
  
1. Features overview:



   STRINGS
                             stb_strncpy, stb_strtok, etc.
                             stb_tolower, stb_skipwhite, stb_dupreplace
                             stb_tokens_*

   FILES
                             stb_fgets, stb_fgets_malloc
                             stb_fput_varlen, stb_fput_ranged
                             stb_readdir_*
                             stb_splitpath
                             stb_fopen, stb_fclose
                             stb_file, stb_stringfile, stb_filelen
                             stb_miniml_*
 
   LOGGING, ERRORS           
                             stb(), stb_fatal()

   VECTOR<> TYPE IN C 
                             stb_arr_*

   HASH VALUES
                             stb_hash
                             stb_crc32, stb_adler32, stb_sha1

   DATA STRUCTURES
                             stb_sdict, stb_extra
                             stb_ps

   MATH & BIT OPERATIONS
                             stb_rand, stb_shuffle
                             stb_bitcount, stb_bitreverse, stb_is_pow2
                             stb_lerp, stb_unlerp, stb_linear_remap

   SEARCHING
                             stb_regex_*, stb_matcher_*, stb_wildcard_*

   MEMORY MANAGEMENT
                             stb_malloc
                             STB_MALLOC_WRAPPER
                             stb_wrapper




   Functions in stb.h fall into two categories:

       hack-oriented
       app-oriented

   A hack-oriented function is probably not very robust. It's designed
   to streamline the process of creating things like personal tools that
   don't need robustness anyway. Hack-oriented functions may be designed
   to be extremely convenient to use but not very efficient, or be
   designed to be extremely efficient (since some tools need good
   efficiency).

   An app-oriented function is robust and reasonably efficient.

   I haven't documented which is which; my point is that if you're
   doing the latter, some care is required. For example, routines
   which stu__accept an output buffer but no length for that buffer
   are obviously not robust.


                      --------------------------------


2. Manifest

   stb.h  (this file)    -- the entire library: the only file you really need
   stb.html (TODO)       -- more complete documentation
   stb.c                 -- unit tests for most of the library


                      --------------------------------


3. Version History

   1.89   support ';' in constant-string wildcards; stb_mutex wrapper (can implement
          with EnterCriticalRegion eventually)
   1.88   portable threading API (only for win32 so far); worker thread queueing
   1.87   fix wildcard handling in stb_readdir_recursive
   1.86   support ';' in wildcards
   1.85   make stb_regex work with non-constant strings;
               beginnings of stb_introspect()
   1.84   (forgot to make notes)
   1.83   whoops, stb_keep_if_different wasn't deleting the temp file
   1.82   bring back stb_compress from stb_file.h for cmirror
   1.81   various bugfixes, STB_FASTMALLOC_INIT inits FASTMALLOC in release
   1.80   stb_readdir returns utf8; write own utf8-utf16 because lib was wrong
   1.79   stb_write
   1.78   calloc() support for malloc wrapper, STB_FASTMALLOC
   1.77   STB_FASTMALLOC
   1.76   STB_STUA - Lua-like language; (stb_image, stb_csample, stb_bilinear)
   1.75   alloc/free array of blocks; stb_hheap bug; a few stb_ps_ funcs;
          hash*getkey, hash*copy; stb_bitset; stb_strnicmp; bugfix stb_bst
   1.74   stb_replaceinplace; use stdlib C function to convert utf8 to UTF-16
   1.73   fix performance bug & leak in stb_ischar (C++ port lost a 'static')
   1.72   remove stb_block, stb_block_manager, stb_decompress (to stb_file.h)
   1.71   stb_trimwhite, stb_tokens_nested, etc.
   1.70   back out 1.69 because it might problemize mixed builds; stb_filec()
   1.69   (stb_file returns 'char *' in C++)
   1.68   add a special 'tree root' data type for stb_bst; stb_arr_end
   1.67   full C++ port. (stb_block_manager)
   1.66   stb_newell_normal
   1.65   stb_lex_item_wild -- allow wildcard items which MUST match entirely
   1.64   stb_data
   1.63   stb_log_name
   1.62   stb_define_sort; C++ cleanup
   1.61   stb_hash_fast -- Paul Hsieh's hash function (beats Bob Jenkins'?)
   1.60   stb_delete_directory_recursive
   1.59   stb_readdir_recursive
   1.58   stb_bst variant with parent pointer for O(1) iteration, not O(log N)
   1.57   replace LCG random with Mersenne Twister (found a public domain one)
   1.56   stb_perfect_hash, stb_ischar, stb_regex
   1.55   new stb_bst API allows multiple BSTs per node (e.g. secondary keys)
   1.54   bugfix: stb_define_hash, stb_wildmatch, regexp
   1.53   stb_define_hash; recoded stb_extra, stb_sdict use it
   1.52   stb_rand_define, stb_bst, stb_reverse
   1.51   fix 'stb_arr_setlen(NULL, 0)'
   1.50   stb_wordwrap
   1.49   minor improvements to enable the scripting language
   1.48   better approach for stb_arr using stb_malloc; more invasive, clearer
   1.47   stb_lex (lexes stb.h at 1.5ML/s on 3Ghz P4; 60/70% of optimal/flex)
   1.46   stb_wrapper_*, STB_MALLOC_WRAPPER
   1.45   lightly tested DFA acceleration of regexp searching
   1.44   wildcard matching & searching; regexp matching & searching
   1.43   stb_temp
   1.42   allow stb_arr to use stb_malloc/realloc; note this is global
   1.41   make it compile in C++; (disable stb_arr in C++)
   1.40   stb_dupe tweak; stb_swap; stb_substr
   1.39   stb_dupe; improve stb_file_max to be less stupid
   1.38   stb_sha1_file: generate sha1 for file, even > 4GB
   1.37   stb_file_max; partial support for utf8 filenames in Windows
   1.36   remove STB__NO_PREFIX - poor interaction with IDE, not worth it
          streamline stb_arr to make it separately publishable
   1.35   bugfixes for stb_sdict, stb_malloc(0), stristr
   1.34   (streaming interfaces for stb_compress)
   1.33   stb_alloc; bug in stb_getopt; remove stb_overflow
   1.32   (stb_compress returns, smaller&faster; encode window & 64-bit len)
   1.31   stb_prefix_count
   1.30   (STB__NO_PREFIX - remove stb_ prefixes for personal projects)
   1.29   stb_fput_varlen64, etc.
   1.28   stb_sha1
   1.27   ?
   1.26   stb_extra
   1.25   ?
   1.24   stb_copyfile
   1.23   stb_readdir
   1.22   ?
   1.21   ?
   1.20   ?
   1.19   ?
   1.18   ?
   1.17   ?
   1.16   ?
   1.15   stb_fixpath, stb_splitpath, stb_strchr2
   1.14   stb_arr
   1.13   ?stb, stb_log, stb_fatal
   1.12   ?stb_hash2
   1.11   miniML
   1.10   stb_crc32, stb_adler32
   1.09   stb_sdict
   1.08   stb_bitreverse, stb_ispow2, stb_big32
          stb_fopen, stb_fput_varlen, stb_fput_ranged
          stb_fcmp, stb_feq
   1.07   (stb_encompress)
   1.06   stb_compress
   1.05   stb_tokens, (stb_hheap)
   1.04   stb_rand
   1.03   ?(s-strings)
   1.02   ?stb_filelen, stb_tokens
   1.01   stb_tolower
   1.00   stb_hash, stb_intcmp
          stb_file, stb_stringfile, stb_fgets
          stb_prefix, stb_strlower, stb_strtok
          stb_image
          (stb_array), (stb_arena)

Parenthesized items have since been removed.

                      --------------------------------


4. Documentation


Functions which take an output pointer parameter (e.g. for multiple return
values) always stu__accept NULL for that parameter unless noted otherwise.


 LEGEND / KEY

   Characters prefixed before a function description have particular meanings:

     -      call free() on only the return value to clean up _all_ memory
            (even for e.g. arrays of arrays, which are allocated all at once)

     %      function is a macro that writes to its first argument, so that
            must be an lvalue, and you should avoid side effects

     .      function is a macro that uses its arguments multiple times, so
            be careful about side effects

     @      data is cached for a passed-in string based on its pointer value,
            so the string must be constant; passing a NULL constant string
            clears all cached data

    retv    in descriptions, shorthand for 'value returned by the function'


non-"stb_" prefixed items
  int8 , uint8             -- 8-bit integer types
  int16, uint16            -- 16-bit integer types
  int32, uint32            -- 32-bit integer types
  uchar, ushort,           -- unsigned integer types
  uint, ulong
  M_PI                     -- PI (3.141592...)
  deg2rad(a)               -- convert degrees to radians
  rad2deg(a)               -- convert radians to degrees
. min(a,b)                 -- minimum; not defined if compiling C++
. max(a,b)                 -- maximum; not defined if compiling C++
. swap(TYPE,a,b)
. stb_clamp(x,a,b)         -- constrain x to a<=x<=b

    #define STB_ONLY to suppress the definition of things in the above section

memory checking
  STB_MALLOC_WRAPPER           -- track malloc()/free() __FILE__ & __LINE__
  stb_wrapper_listall(func)    -- call func() with all outstanding allocations
  stb_wrapper_dump(filename)   -- dump outstanding allocations to file
  stb_wrapper_malloc(p,sz,f,l) -- track allocation @ file:f line:l
  stb_wrapper_free(p,f,l)      -- track deallocation
  stb_wrapper_realloc(old,new,newsz,f,l) -- track realloc
  stb_wrapper_calloc(n,sz,f,l) -- track calloc (or just use wrapper_malloc!)

     If you _globally_ define STB_MALLOC_WRAPPER, then any file that includes
     stb.h will have malloc/free/realloc/strdup tracked by stb.h using
     __FILE__ and __LINE__. Don't define this if you don't want the bloat!

     Note that the tracker does not store data directly in the allocations;
     instead it uses a separate data structure (whose malloc/frees are NOT
     tracked). This allows the wrappers above to be used on custom allocators
     without any invasiveness (and avoids corruption issues).

errors and logging
  void    stb_fatal(char *format, ...) -- print error and exit
  void    stb_(char *format, ...)      -- log to "stb.log"
  void    stb_log(int active)          -- turn on/off stb_(); default on
  void    stb_log_fileline(int active) -- on/off logging file/line in DEBUG
  void    stb_log_name(char *name)     -- use 'name' instead of "stb.log"

memory
  void    stb_swap(p,q,size)         -- swap non-overlapping blocks of memory
  void *  stb_copy(p,size)           -- strdup() for memory
  void    stb_reverse(p,num,size)    -- reverse array p

C-strings
  int     stb_prefix_count(x,y)      -- rval = length of matching prefix
. int     stb_prefix(x,y)            -- 0==strncmp that y is a prefix of x
. int     stb_prefixi(x,y)           -- 0==strnicmp that y is a prefix of x
  int     stb_suffix(x,y)            -- check if y is a suffix of x
  int     stb_suffixi(x,y)           -- check if y is a suffix of y
  int     stb_strncpy(x,y,n)         -- strncpy with guaranteed termination
  char *  stb_substr(x,n)            -- malloc() an n-char prefix of x
- char *  stb_duplower(x)            -- malloc() and lowercase a string
  void    stb_tolower(x)             -- in-place lowercase a string
  char *  stb_strtok(x,y,del)        -- copy y->x to any del, return new y
- char *  stb_dupreplace(x,old,new)  -- replace "old" with "new" in x
  char *  stb_strichr(x,y)           -- strchr(x,y) case-insensitive
  char *  stb_stristr(x,y)           -- strstr(x,y) case-insensitive
  char *  stb_stricmp(x,y)           -- portable stricmp
@ int     stb_ischar(c,s)            -- is c in the constant string s? (fast)

C-strings (stb-1.05)
  char *  stb_skipwhite(str)         -- return ptr into str past whitespace
- char ** stb_tokens(str,del,int *n) -- make array of tokens (*n = count)
- char ** stb_tokens_allowempty(...) -- as above, can have 0-length tokens
- char ** stb_tokens_stripwhite(...) -- tokens lack leading&trailing space
- char ** stb_tokens_quoted(...)     -- "quote" delimiters and l&t space

C-strings (stb-1.15)
  void    stb_fixpath(path)          -- turn all \ into /
  char *  stb_strchr2(str,x,y)       -- return ptr to first x or y
  char *  stb_strrchr2(str,x,y)      -- return ptr to last x or y
  void    stb_replacedir(buf,f,path) -- replace existing dir in file with path
  void    stb_splitpath(buf,path,f)  -- copy component of path into buf:
                                         f = STB_PATH -- include path
                                         f = STB_FILE -- include filename
                                         f = STB_EXT  -- include extension
                                         f = STB_PATH_FILE
                                         f = STB_FILE_EXT

bit operations (stb-1.08)
  int     stb_bitcount(uint)         -- number of 1 bits
  uint32  stb_bitreverse(uint)       -- bitwise reverse
  uint    stb_bitreverse8(uchar)     -- bit reverse of 8 bits
  uint    stb_big32(uchar *)         -- decode 32-bit big-endian int
  uint    stb_big16(uchar *)         -- decode 16-bit big-endian int
  uint    stb_little32(uchar *)      -- decode 32-bit little-endian int
  uint    stb_little16(uchar *)      -- decode 16-bit little-endian int
  int     stb_is_pow2(uint n)        -- is n a power of two? is_pow2(0)=1
  int     stb_log2_ceil(uint n)      -- _ceil (5)=_ceil (8)=3; _ceil (0)=-1
  int     stb_log2_floor(uint n)     -- _floor(4)=_floor(7)=2; _floor(0)=-1
  int     stb_lowbit8(uint n)        -- index of smallest 1 bit, or -1
  int     stb_highbit8(uint n)       -- index of highest 1 bit, or -1

math
.       stb_lerp(x, a,b)             -- lerp from a to b based on x=[0,1]
.       stb_unlerp(y, a,b)           -- compute x s.t. y=stb_lerp(x,a,b)
.       stb_linear_remap(y, a,b,c,d) -- return z = [c..d] as y = [a..b]

  int     stb_float_eq(x,y,thresh,ulps) -- true if x=y within thresh or ulps
  
32-bit "checksums" (stb-1.10; sha-1 1.27)
  uint32  stb_crc32(buf,len)         -- CRC-32 checksum of buffer
  uint32  stb_crc32_block(crc,buf,n) -- update crc32; init STB_CRC32_SEED
  uint32  stb_adler32(ad32,buf,n)    -- update adler32; STB_ADLER32_SEED
  void    stb_sha1(out,buf,len)      -- output 20 byte digest for len-byte buf
  int     stb_sha1_file(out,filename)-- retval=1 if success, 0=can't open

binary search helper
  int   stb_search_binary(s,min,max,flag) -- binary search, store in s, pass
                                        min&max indices, retval = guess index
  int   stb_search_open(s,min,flag)  -- as above, but open-ended on right
  int   stb_probe(s,c,&result)       -- c<0,=0,>0 <=> goal<guess,=guess,>guess
                                        result=next guess, retval=keep going?
            if flag is true, search finds smallest index equal to goal
            if flag is false, search finds largest index equal to goal

  sample usage:
      stb_search s;
      int r = stb_search_binary(&s,0,100,1);
      while (stb_probe(&s,strcmp(goal,str[r]),&r));
      if (!strcmp(goal,str[r])) ...

hashing
  uint    stb_hash_fast(char *, uint)-- hash known-length, quality Hsieh hash
  uint    stb_hash(char *)           -- hash value for string
  uint    stb_hashptr(void *)        -- hash value of pointer itself
  uint    stb_rehash(uint)           -- secondary hash value from raw hash
  uint    stb_rehash_improved(uint)  -- better rehash
  uint    stb_hash2(char *,uint *)   -- compute two separate 32-bit hashes

  stb_define_hash(TYPE,PREFIX,KEY,EMPTY,DEL,HASH,VALUE)
     declares a hash table named TYPE, with functions named PREFIXwhatever,
     with a key of type KEY, reserved key values EMPTY and DEL, hashing
     code 'HASH' which given KEY k computes a hash value and 'return's it,
     and the hash table producing values of type VALUE, where NULL is the
     result for get that has no value. See code for more details.

  int stb_perfect_create(stb_perfect *,uint*,n) -- compute n-item perfect hash
                                                   retval=output table size
  void stb_perfect_destroy(stb_perfect *)       -- free perfect hash
  int stb_perfect_hash(stb_perfect *, uint x)   -- hash x; retval=-1 if not


string dictionary     [ hash table from strings to pointers ]

  stb_sdict*stb_sdict_new(use_arena) -- dictionary map strings to void*
  void      stb_sdict_delete(dict,func)-- free sdict; func(v) for all values
  void *  stb_sdict_get(dict,str)    -- return value for string
  int     stb_sdict_add(dict,str,p)  -- add <str,p> if !str present else 0
  void *  stb_sdict_remove(dict,str) -- del <str,?> if str present else 0
  void *  stb_sdict_change(dict,str,p)-- replace old val with new, retv=old
  char *  stb_sdict_iter(dict,prev,&v)-- retv=next item; prev=NULL to start

. stb_sdict_for(dict,int_var,str_var,p_var) {...}
                                     -- iter over <str_var,p_var> in dict

extra-data dictiontary  [ hash table from pointers to pointers ]

   stb_extra* stb_extra_new(void)      -- dictionary from void* to void*
   void       stb_extra_delete(extra,func)-- free dict; func(v) for all values
   void *     stb_extra_get(dict,ptr)  -- get value for pointer (NULL if none)
   void       stb_extra_add(dict,p,v)  -- bind key p1 to key p2 in dict
   void       stb_extra_set(dict,p,v)  -- set key p1 to key p2, add if needed
   void *     stb_extra_remove(dict,p1)-- remove key p1, retval=value for p1

portable 32-bit random numbers via Mersenne Twister or LCG (BCPL generator)
  void    stb_srand(uint32 n)        -- set seed
  uint32  stb_rand()                 -- generate random number
  double  stb_frand()                -- generate random number [0..1)
.         stb_lerp(stb_frand(),x,y)  -- generate random number [x..y)
.         stb_rand_define(name,seed) -- defines random generator function NAME
  void    stb_shuffle(void*,n,sz,r)  -- shuffle array length n, itemsize=sz
                                        use 'r' as seed; don't disturb
                                        random stream. Hint: use stb_rand()
                                        or rand() as seed.
  uint32  stb_srandLCG(uint32 n)     -- set seed, returning old one
  uint32  stb_randLCG()              -- generate random number
  double  stb_frandLCG()             -- generate random number [0..1)

generic qsort routines
  int     stb_intcmp(a,b)            -- qsort integer compare func
  int     stb_floatcmp(a,b)          -- qsort float compare func
  int     stb_doublecmp(a,b)         -- qsort double compare func
  int     stb_strcmp(a,b)            -- qsort string compare func
  void    stb_cmpoffset(int n)       -- set field offset within struct

  e.g.  stb_cmpoffset(offsetof(ptype, float_field));
        qsort(p, n, sizeof(*p), stb_floatcmp);
        stb_cmpoffset(0); // don't hose other code that doesn't set it

templated quicksort
.         stb_define_sort(FUNCNAME, TYPE, COMPARE_CODE) -- define a sort func

  This will define a sort function which has the name "FUNCNAME", and takes
  two parameters: an array of TYPE, and an integer length for that array.
  This allows inlining the comparison, which can perform better than
  qsort(). For a simple test on a large array of ints, performance was
  2x qsort() in a VC6 release build, and roughly identical in a debug build.

  You need to define three things:
      FUNCNAME      -- the name of your sort function
      TYPE          -- the type of data it will sort; this must be copyable
      COMPARE_CODE  -- an expression that compares two TYPE * named a and b.
                       It should return TRUE if a and b are in sorted order
                       but NOT equal, and FALSE otherwise.

  If you need to do more complex comparisons, start your compare code with
  "0;", and then compute your comparison into the variable 'c', e.g.
       0;  c = *a < *b;

  To clarify, here are some rough equivalents:

  stb_declare_sort(FUNCNAME, TYPE):
      void FUNCNAME(TYPE *p, int n);

  stb_define_sort(FUNCNAME, TYPE, COMPARE_CODE):
      int compare_func(const void *p, const void *q)
      {
         TYPE *a = p;
         TYPE *b = q;
         if (COMPARE_CODE)
            return -1;
         else
            return 0 or 1;
      }

      void FUNCNAME(TYPE *p, int n)
      {
         qsort(p, n, sizeof(TYPE), compare_func);
      }

  stb_define_sort_static is identical to stb_define_sort, but FUNCNAME
  gets a storage specifier 'static'.


stb_dupe   -- find duplicates in very large sets in O(N log log N) time
  stb_dupe *stb_dupe_create(hash,eq,count,ineq) -- create dupe finder
                  hash  = hash function on void* item
                  eq    = equality comparison for two void* items
                  count = estimated number of items, or 0
                  ineq  = inequality comparison for two items, or NULL
  void  stb_dupe_free(dupe*)            -- free dupe finder
  void  stb_dupe_add(dupe*, void *item) -- add item to dupe set
  void  stb_dupe_finish(dupe *)         -- find dupes
  int   stb_dupe_numsets(dupe *)     -- number of duplicate sets
  void **stb_dupe_set(dupe*,index)   -- list of dupes in set 'index',0..num-1
  int   stb_dupe_set_count(dupe*,index) -- number of dupes in list

options parsing
  char ** stb_getopt(&argc,argv)      -- retval=options; argc/argv modded
  char ** stb_getopt_param(&c,v,plist)-- options in plist take one parameter
  void    stb_getopt_free(options)       (note options is NULL terminated)

directory reading (stb-1.23)
  char ** stb_readdir_files(dir)     -- return stb_arr of files in dir
  char ** stb_readdir_subdirs(dir)   -- return stb_arr of subdirs in dir
  void    stb_readdir_free(char**)   -- free above

file handling 3 (stb-1.24)
  int     stb_copyfile(char *src, char *dest) -- copy file src to dest
  int     stb_rename(char *src, char*dest) -- utf8 rename()

file handling
  char *  stb_fgets(buf,len,FILE*)   -- fgets with no trailing \n
- char *  stb_fgets_malloc(FILE*)    -- fgets arbitrarily long
  size_t  stb_filelen(FILE *)        -- length of open file
- void *  stb_file(name, int*n)      -- read n-byte file into memory
- char ** stb_stringfile(name, *n)   -- read n lines as strings
- char ** stb_stringfile_trimmed(name, int *n, comment_char)
                                     -- strips leading whitespace; skips
                         empty lines and lines starting with comment char,

file handling 2        [all functions work for both int and uint]

  void    stb_fput_varlen(file,int)  -- fwrite 1..5 bytes, small #s smaller
  int     stb_fget_varlen(f)         -- fread 1..5 bytes as above
  int     stb_size_varlen(val)       -- number of bytes required for value
  void    stb_fput_varlenu(file,uint)-- fwrite 1..5 bytes, small #s smaller
  uint    stb_fget_varlenu(f)        -- fread 1..5 bytes as above
  int     stb_size_varlenu(val)      -- number of bytes required for value
  void    stb_fput_ranged(f,val,b,n) -- write val, b <= val < b+n
  int     stb_fget_ranged(f,b,n)     -- read val, b <= val < b+n
  int     stb_size_ranged(b,n)       -- number of bytes required for range
  void    stb_fput_varlen64(file,i64)-- fwrite 1..9 bytes, small #s smaller
  int64   stb_fget_varlen64(f)       -- fread 1..9 bytes as above
  int     stb_size_varlen64(val64)   -- number of bytes required for value

full file processing
  int     stb_fcmp(char *x, char *y) -- compare files x <=> y, 0 if eq
  int     stb_feq(char *x, char *y)  -- 'stb_fcmp()==0', faster

no-overwrite file I/O             [use tempfile and move on close]

  int     stb_fullpath(abs,sz,fname) -- make fname not cwd-relative
  char *  stb_mktemp(char *template) -- if(Windows)_mktemp();else mkstemp()
  FILE *  stb_fopen(filename,mode)   -- "w"/"wb": overwrite only on success
  int     stb_fclose(FILE *,keep)    -- if keep & no error, save to file
                                        keep == 0 == stb_keep_no => discard
                                        keep == 1 == stb_keep_yes => keep
                                        keep == 2 == stb_keep_if_different

sliding-dict compression         [English 2:1, stb.h 3:1, EXE 3:2]
  int    stb_decompress_length(cbuf,n) -- length of decompressed cbuf
  int    stb_decompress(dbuf,cbuf,n)   -- decompress cbuf[0..n) to dbuf
  int    stb_compress(cbuf,dbuf,n)     -- compress dbuf[0..n) to cbuf
  void   stb_compress_window(n)        -- maximum distance to loook back
  void   stb_compress_hashsize(n)      -- bytes to use for hash table
  int   stb_compress_tofile(name,dbuf,n)  -- compress dbuf[0..n) > name
- char *stb_decompress_fromfile(name,&n)  -- decompress from file 'name'

stb_arr -- growable array          [a la STL vector<>]

  Note that the pointer 'r' itself gets realloced/moved around on any
  call marked with '%', so don't try to keep multiple pointers to the array.
  If you pass one into a function, pass it by reference or return the
  changed value out the other side. (Obviously could avoid this by wrapping
  it inside another structure, but then it would be much more painful to
  access an entry from C, which is the whole point of this particular API.)

  TYPE *r=NULL;           -- declare 0-length array in C
  stb_arr<TYPE> r=NULL    -- declare 0-length array in C++
  stb_arr(TYPE) r         -- declare 0-length array in either (e.g. func decl)
  r[i]                    -- access i'th element of array

  void * stb_arr_free(r)      -- free (returns NULL) e.g. a=stb_arr_free(a)
. int    stb_arr_len(r)       -- number of elements in array
. int    stb_arr_empty(r)     -- stb_arr_len(r)==0
. int    stb_arr_valid(r,i)   -- test if i is valid index in array [0..len)
% TYPE   stb_arr_push(r,v)    -- append copyable item 'TYPE v' at end
. TYPE   stb_arr_pop(r)       -- remove item from end and return it
. TYPE   stb_arr_last(r)      -- LVALUE of last item, e.g. one just pushed
% TYPE * stb_arr_add(r)       -- a gets 1 uninitialized item @end, ret=last
% TYPE * stb_arr_addn(r,n)    -- a gets n uninitialized items @end, ret=1st
% TYPE * stb_arr_atleast(r,n) -- makes sure a has n "unallocated" elements
% TYPE * stb_arr_setlen(r,n)  -- makes 'stb_arr_len(a) == n' true
% TYPE * stb_arr_makevalid(r,n)--makes 'stb_arr_valid(r,n)' true
% TYPE * stb_arr_setsize(r,n) -- [rare!] sets number of internal elements
  void * stb_arr_copy(r)      -- make a duplicate of the array 
  int    stb_arr_storage(r)   -- count #bytes to store array

stb_ps -- pointer set            -- a time/space efficient set of pointers
  int     stb_ps_find(ps,p)      -- true if p is in ps
  stb_ps *stb_ps_add(ps,p)       -- put p (!= NULL) into ps
  stb_ps *stb_ps_remove(ps,p)    -- remove p from ps if present
  stb_ps *stb_ps_remove_any(ps,&p) -- remove some element of ps and put in p
  void    stb_ps_delete(ps)      -- discard the pointer set
  int     stb_ps_count(ps)       -- # of pointers in pointer set
  void ** stb_ps_getlist(ps,&c)  -- malloc array of pointers from set
  int     stb_ps_writelist(ps,list,sz)--write as many as sz pointers to list
  int     stb_ps_enum(ps,cdata,f(p,cdata))--traverse all pointers in set
  void ** stb_ps_fastlist(ps,&c) -- retval=list, c=length; list[i] is valid
                                    iff stb_ps_fastlist_valid(list[i]) 


XML-style parser, MiniML         [XML subset for data description]
  void    stb_mml_free(stb_mml *z)   -- free an MML tree
  stb_mml *stb_mml_parse(char *s)    -- parse string into MML tree
  stb_mml *stb_mml_file(char *file)  -- parse file into MML tree

    MiniML spec:            stb_mml *m;
   nested XML tags          m->tag          : string w. tag name
   no options/attribs       m->leaf_data    : string w. tag contents IFF leaf
   no <foo/> style          m->num_children : number of child stb_mml nodes
   all content in leaves    m->child[i]     : i'th child -- [0,num_children)
   </foo> optional for leaf

stb_alloc -- hierarchical memory manager (stb_malloc(1) to get an arena)

   If you allocate a block q assigned to block p, then freeing p frees q.
   Assigning to block NULL means it's global, it never auto-frees.

   void   stb_free(p)              -- free a malloc, _leaf; also assignees
   void * stb_malloc(p,size)       -- alloc a block q assigned to p
   void * stb_malloc_nofree(p,sz)  -- alloc q assigned to p; can't stb_free(q)
   void * stb_malloc_leaf  (p,sz)  -- alloc q assigned to p; can't assign to q
   void * stb_malloc_raw   (p,sz)  -- alloc q assigned to p; nofree & leaf
   void * stb_malloc_global(sz)    -- alloc q unassigned; stb_malloc(0,sz)
   void * stb_realloc  (  q,sz)    -- realloc a malloc, _global, _leaf
   void * stb_realloc_c(p,q,sz)    -- malloc/free/realloc based on q=NULL,sz=0
   void   stb_reassign(p,q)        -- reassign q to p, remove old assignment
                                      also only for malloc, _global, _leaf

   int stb_alloc_chunk_size;       -- chunk size for _leaf, _raw, _string
   int stb_alloc_alignment;        -- alignment to force allocations to 

stb_match

  int    stb_wildmatch(s,t)    -- does wildcard s match t
  int    stb_wildmatchi(s,t)   -- does wildcard s match t case insensitively
  int    stb_wildfind(s,t)     -- is wildcard s anywhere in t
  int    stb_wildfindi(s,t)    -- ditto, case insensitive

  stb_matcher *stb_regex_matcher(char *r)     -- make regexp matching object
  void         stb_matcher_free(match *)      -- free it
  int          stb_matcher_match(match *,str) -- does str match regexp?
  int          stb_matcher_find(match *,str)  -- is regexp anywhere in str?

  Regexps:     x    x?     x+       x*     (x)   foo|bar
               .  [a-z]  []a-z]  [^a-z]    ^foo    foo$
                    \?     \+       \*   \(   \)   \[  \]
               no {}

@ int stb_regex(char *regex, char *str)  -- regex search on _constant_ regex
      (uses the regex pointer to cache a compiled regexp behind the scenes)

stb_lex
  stb_matcher * stb_lex_matcher()  -- create a lexing parser
  void stb_lex_item(matcher,re,val)-- define lexing regexp re to return val
  int  stb_lex(matcher,input,&len) -- find longest item (len=length), retv=val

word wrapping
   int stb_wordwrap(pairs, pair_max, count, *str) -- wrap str to width count
   int *stb_wordwrapalloc(int count, char *str) -- as above, but alloc results

stb_temp -- temporary storage on stack or not:

% void  * stb_temp(buffer, needed_size)  -- reval = buffer or malloc if big
  void    stb_tempfree(buffer, p)        -- p = retval of stb_temp()

balanced binary search tree (uses AA tree, a 2-3 that's simpler than red-black)

.          stb_bst_fields(NAME)      -- add to structure to be searched,
                                        prefixed with 'struct ThisStructType'

           You can represent a BST index of this structure by using a pointer
           to the structure (the tree pointer IS the structure pointer). But
           then update functions will return new pointers, so wrap it if you
           want.

           Note that if you have multiple fields in the structure you want
           to index separately, you can do so by using different NAMEs.

.          stb_bst(TYPE,NAME,keyname,KEYTYPE,compare)
                 TYPE = structure name
                 NAME = name prefixed to BST functions
                 keyname = name of field in structure to index
                 KEYTYPE = type of field in structure to index
                 compare = _expression_ to compare two KEYTYPES a and b, then
                            compute <0,0,>0 for a<b,a=b,a>b

.          stb_bst_general(TYPE,NAME,compare) -- allow multi-field comparisons;
                 TYPE = structure name              does not define NAME##find
                 NAME = name prefixed to BST functions
                 compare = code to take two structure pointers p and q, then
                            invoke "return" with <0,0,>0 for p<q,p=q,p>q

.          stb_bst_find(NAME,tree,fcomp) -- define find function for _general
                TYPE *myfind(TYPE *mytree, myvariousfields)
                    stb_bst_find(NAME,mytree, ...fcomp-code...)
                fcomp-code should compute <0, 0, >0 comparing myvariousfields
                   to mytree (e.g. < 0 iff myvariousfields < mytree) and store
                   the result in the variable 'c'.

   TYPE *NAME##insert(tree,item)      -- retval=new tree
   TYPE *NAME##remove(tree,item)      -- retval=new tree
   TYPE *NAME##first(tree)            -- retval=first item in tree
   TYPE *NAME##last(tree)             -- retval=last item in tree
   TYPE *NAME##next(tree,item)        -- retval=next item after item
   TYPE *NAME##prev(tree,item)        -- retval=prev item after item
   TYPE *NAME##find(tree,key)         -- retval=item with key if exists
                
image -- generic 32-bit image structure, fast alpha blend operations

  pixels are 'unsigned int' (or uint32, to be explicit)

. stb_rgba(r,g,b,a) [each uint8]         -- make a 32-bit pixel
. stb_rgb (r,g,b)                        -- 32-bit pixel, opaque
. stb_r(p), stb_g(p), stb_b(p), stb_a(p) -- get 8-bit channel values
. stb_image_opaque (p)                   -- is 32-bit pixel opaque?
. stb_image_transparent(p)               -- is 32-bit pixel totally clear?
. stb_image_over   (p,q)                 -- blend p over q (result opaque)
. stb_image_blend  (p,q,uint8 a)         -- blend p over q using alpha=a
. stb_image_blend_half(p,q)              -- blend p over q using alpha=0.5

  stb_image i;
  
  stb_image_define  (&i,width,height,data) -- create, with pointer to data
  stb_image_subimage(&i,&j,x,y,w,h)        -- shares (x,y,w,h) block of j
. stb_image_pixel   (&i,x,y)               -- lvalue of (x,y)'th pixel


*****************************************************************************/

//@ 
//@ This documentation is for stb.h version @@VERSION which consists of a
//@ single header file which should be located <a href="stb.h">here</a>.
//@ Generally the most recent version should be available on the internet at
//@ <a href="http://nothings.org/stb.h">http://nothings.org/stb.h</a>.
//@ <p>Some infrequently used components have been removeed and are
//@ available separately: <a href="http://nothings.org/stb_file.h">compression
//@ and other file ops</a> and <a href="http://nothings.org/stb_image.h">image
//@ and sampling</a>.

#ifndef STB__INCLUDE_STB_H
#define STB__INCLUDE_STB_H

#include <stdlib.h>     // stdlib could have min/max
#include <stdio.h>      // need FILE

#ifdef STB_PERSONAL
   typedef int Bool;
   #define False 0
   #define True 1
#endif

#ifdef STB_MALLOC_WRAPPER_PAGED
   #define STB_MALLOC_WRAPPER_DEBUG
#endif
#ifdef STB_MALLOC_WRAPPER_DEBUG
   #define STB_MALLOC_WRAPPER
#endif
#ifdef STB_MALLOC_WRAPPER_FASTMALLOC
   #define STB_FASTMALLOC
   #define STB_MALLOC_WRAPPER
#endif

#ifdef STB_FASTMALLOC
   #ifndef _WIN32
      #undef STB_FASTMALLOC
   #endif
#endif

#ifdef STB_DEFINE
   #include <assert.h>
   #include <string.h>
   #include <stdarg.h>
   #include <stddef.h>
   #include <ctype.h>
   #include <math.h>
   #ifndef _WIN32
   #include <unistd.h>
   #else
   #include <io.h>      // _mktemp
   #include <direct.h>  // _rmdir
   #endif
   #include <sys/types.h> // stat()/_stat()
   #include <sys/stat.h>  // stat()/_stat()

   #ifdef _WIN32   
      #define STB__INLINE static __forceinline
   #elif defined(__cplusplus)
      #define STB__INLINE static inline
   #else
      #define STB__INLINE static
   #endif
#endif

#define stb_min(a,b)   ((a) < (b) ? (a) : (b))
#define stb_max(a,b)   ((a) > (b) ? (a) : (b))

#ifndef STB_ONLY
   #if !defined(__cplusplus) && !defined(min) && !defined(max)
     #define min(x,y) stb_min(x,y)
     #define max(x,y) stb_max(x,y)
   #endif

   #ifndef M_PI
     #define M_PI  3.14159265358979323846f
   #endif
   
   #ifndef TRUE
     #define TRUE  1
     #define FALSE 0
   #endif
   
   #ifndef deg2rad
   #define deg2rad(a)  ((a)*M_PI/180)
   #endif
   #ifndef rad2deg
   #define rad2deg(a)  ((a)/M_PI*180)
   #endif
   
   #ifndef swap
   #ifndef __cplusplus
   #define swap(TYPE,a,b)  \
               do { TYPE stb__t; stb__t = (a); (a) = (b); (b) = stb__t; } while (0)
   #endif              
   #endif
    
   typedef unsigned char  uint8 ;
   typedef   signed char   int8 ;
   typedef unsigned short uint16;
   typedef   signed short  int16;
  #if defined(STB_USE_LONG_FOR_32_BIT_INT) || defined(STB_LONG32)
   typedef unsigned long  uint32;
   typedef   signed long   int32;
  #else
   typedef unsigned int   uint32;
   typedef   signed int    int32;
  #endif

   typedef unsigned char  uchar ;
   typedef unsigned short ushort;
   typedef unsigned int   uint  ;
   typedef unsigned long  ulong ;

   // produce compile errors if the sizes aren't right
   typedef char stb__testsize16[sizeof(int16)==2];
   typedef char stb__testsize32[sizeof(int32)==4];
#endif

// if we're STB_ONLY, can't rely on uint32 or even uint, so all the
// variables we'll use herein need typenames prefixed with 'stb':
typedef unsigned char stb_uchar;
typedef unsigned char stb_uint8;
typedef unsigned int  stb_uint;
typedef unsigned short stb_uint16;
typedef          short stb_int16;
typedef   signed char  stb_int8;
#if defined(STB_USE_LONG_FOR_32_BIT_INT) || defined(STB_LONG32)
  typedef unsigned long  stb_uint32;
  typedef          long  stb_int32;
#else
  typedef unsigned int   stb_uint32;
  typedef          int   stb_int32;
#endif
typedef char stb__testsize2_16[sizeof(stb_uint16)==2];
typedef char stb__testsize2_32[sizeof(stb_uint32)==4];

#ifdef __cplusplus
   #define STB_EXTERN   extern "C"
#else
   #define STB_EXTERN   extern
#endif

// check for well-known debug defines
#if defined(DEBUG) || defined(_DEBUG) || defined(DBG)
   #ifndef NDEBUG
      #define STB_DEBUG
   #endif
#endif

#ifdef STB_DEBUG
   #include <assert.h>
#endif

//@ <h2>stb_wrapper_*</h2>
//@
//@ record file/line information for tracking leaks<p>
//$ newp=pointer to a memory block that was just allocated
//$ oldp=pointer to a memory block that was previously allocated
//$ sz=size of the memory block just allocated
//$ file=name of the source file where the allocation operation occurred
//$ line=line number of the source file where the allocation operation occurred
//$ record an allocation made by a malloc()-like routine
STB_EXTERN void stb_wrapper_malloc(void *newp, int sz, char *file, int line);
//$ record a deallocation made by a free()-like routine
STB_EXTERN void stb_wrapper_free(void *oldp, char *file, int line);
//$ record a reallocation made by a realloc()-like routine
STB_EXTERN void stb_wrapper_realloc(void *oldp, void *newp, int sz, char *file, int line);
STB_EXTERN void stb_wrapper_calloc(size_t num, size_t sz, char *file, int line);
//$ enumerate all the currently outstanding allocations that haven't been freed
//$ (callback function provided by the client which reports each allocated block and the file/line pair from the malloc/realloc that created it)
STB_EXTERN void stb_wrapper_listall(void (*func)(void *ptr, int sz, char *file, int line));
//$ write a text file with a listing of all currently outstanding allocations
//$ (name of the text file to create)
STB_EXTERN void stb_wrapper_dump(char *filename);
//$ Return the allocation size of an allocated block, or 0 if not allocated.
STB_EXTERN int stb_wrapper_allocsize(void *oldp);
//$ Check to make sure a block is really allocated
STB_EXTERN void stb_wrapper_check(void *oldp);

#ifdef STB_DEFINE
// this is a special function used inside malloc wrapper
// to do allocations that aren't tracked (to avoid
// reentrancy). Of course if someone _else_ wraps realloc,
// this breaks, but if they're doing that AND the malloc
// wrapper they need to explicitly check for reentrancy.
//
// only define realloc_raw() and we do realloc(NULL,sz)
// for malloc() and realloc(p,0) for free().
static void * stb__realloc_raw(void *p, int sz)
{
   if (p == NULL) return malloc(sz);
   if (sz == 0)   { free(p); return NULL; }
   return realloc(p,sz);
}
#endif

#ifdef STB_FASTMALLOC
STB_EXTERN void * stb_smalloc(size_t sz);
STB_EXTERN void   stb_sfree(void *p);
STB_EXTERN void * stb_srealloc(void *p, size_t sz);
STB_EXTERN void * stb_scalloc(size_t n, size_t sz);
STB_EXTERN char * stb_sstrdup(char *s);

#define malloc  stb_smalloc
#define free    stb_sfree
#define realloc stb_srealloc
#define strdup  stb_sstrdup
#define calloc  stb_scalloc
#endif

#ifndef STB_MALLOC_ALLCHECK
   #define stb__check(p)  1
#else
   #ifndef STB_MALLOC_WRAPPER
      #error STB_MALLOC_ALLCHECK requires STB_MALLOC_WRAPPER
   #else
      #define stb__check(p) stb_mcheck(p)
   #endif
#endif

#ifdef STB_MALLOC_WRAPPER
   STB_EXTERN void * stb__malloc(int, char *, int);
   STB_EXTERN void * stb__realloc(void *, int, char *, int);
   STB_EXTERN void * stb__calloc(size_t n, size_t s, char *, int);
   STB_EXTERN void   stb__free(void *, char *file, int);
   STB_EXTERN char * stb__strdup(char *s, char *file, int);
   STB_EXTERN void   stb_malloc_checkall(void);
   STB_EXTERN void   stb_malloc_check_counter(int init_delay, int rep_delay);
   #ifndef STB_MALLOC_WRAPPER_DEBUG
      #define stb_mcheck(p) 1
   #else
      STB_EXTERN int   stb_mcheck(void *);
   #endif


   #ifdef STB_DEFINE

   #ifdef STB_MALLOC_WRAPPER_DEBUG
      #define STB__PAD   32
      #define STB__BIAS  16
      #define STB__SIG   0x51b01234
      #define STB__FIXSIZE(sz)  (((sz+3) & ~3) + STB__PAD)
      #define STB__ptr(x,y)   ((char *) (x) + (y))
   #else
      #define STB__ptr(x,y)   (x)
      #define STB__FIXSIZE(sz)  (sz)
   #endif

   #ifdef STB_MALLOC_WRAPPER_DEBUG
   int stb_mcheck(void *p)
   {
      unsigned int sz;
      if (p == NULL) return 1;
      p = ((char *) p) - STB__BIAS;
      sz = * (unsigned int *) p;
      assert(* (unsigned int *) STB__ptr(p,4) == STB__SIG);
      assert(* (unsigned int *) STB__ptr(p,8) == STB__SIG);
      assert(* (unsigned int *) STB__ptr(p,12) == STB__SIG);
      assert(* (unsigned int *) STB__ptr(p,sz-4) == STB__SIG+1);
      assert(* (unsigned int *) STB__ptr(p,sz-8) == STB__SIG+1);
      assert(* (unsigned int *) STB__ptr(p,sz-12) == STB__SIG+1);
      assert(* (unsigned int *) STB__ptr(p,sz-16) == STB__SIG+1);
      stb_wrapper_check(STB__ptr(p, STB__BIAS));
      return 1;
   }

   static void stb__check2(void *p, int sz, char *file, int line)
   {
      stb_mcheck(p);
   }

   void stb_malloc_checkall(void)
   {
      stb_wrapper_listall(stb__check2);
   }
   #else
   void stb_malloc_checkall(void) { }
   #endif

   static int stb__malloc_wait=(1 << 30), stb__malloc_next_wait = (1 << 30), stb__malloc_iter;
   void stb_malloc_check_counter(int init_delay, int rep_delay)
   {
      stb__malloc_wait = init_delay;
      stb__malloc_next_wait = rep_delay;
   }

   void stb_mcheck_all(void)
   {
      #ifdef STB_MALLOC_WRAPPER_DEBUG
      ++stb__malloc_iter;
      if (--stb__malloc_wait <= 0) {
         stb_malloc_checkall();
         stb__malloc_wait = stb__malloc_next_wait;
      }
      #endif
   }

   #ifdef STB_MALLOC_WRAPPER_PAGED
   #define STB__WINDOWS_PAGE (1 << 12)
   #ifndef _WINDOWS_
   STB_EXTERN __declspec(dllimport) void * __stdcall VirtualAlloc(void *p, unsigned long size, unsigned long type, unsigned long protect);
   STB_EXTERN __declspec(dllimport) int   __stdcall VirtualFree(void *p, unsigned long size, unsigned long freetype);
   #endif
   #endif

   static void *stb__malloc_final(int sz)
   {
      #ifdef STB_MALLOC_WRAPPER_PAGED
      int aligned = (sz + STB__WINDOWS_PAGE - 1) & ~(STB__WINDOWS_PAGE-1);
      char *p = VirtualAlloc(NULL, aligned + STB__WINDOWS_PAGE, 0x2000, 0x04); // RESERVE, READWRITE
      if (p == NULL) return p;
      VirtualAlloc(p, aligned,   0x1000, 0x04); // COMMIT, READWRITE
      return p;
      #else
      return malloc(sz);
      #endif
   }

   static void stb__free_final(void *p)
   {
      #ifdef STB_MALLOC_WRAPPER_PAGED
      VirtualFree(p, 0, 0x8000); // RELEASE
      #else
      free(p);
      #endif
   }

   int stb__malloc_failure;
   static void *stb__realloc_final(void *p, int sz, int old_sz)
   {
      #ifdef STB_MALLOC_WRAPPER_PAGED
      void *q = stb__malloc_final(sz);
      if (q == NULL)
          return ++stb__malloc_failure, q;
      // @TODO: deal with p being smaller!
      memcpy(q, p, sz < old_sz ? sz : old_sz);
      stb__free_final(p);
      return q;
      #else
      return realloc(p,sz);
      #endif
   }

   void stb__free(void *p, char *file, int line)
   {
      stb_mcheck_all();
      if (!p) return;
      #ifdef STB_MALLOC_WRAPPER_DEBUG
      stb_mcheck(p);
      #endif
      stb_wrapper_free(p,file,line);
      #ifdef STB_MALLOC_WRAPPER_DEBUG
         p = STB__ptr(p,-STB__BIAS);
         * (unsigned int *) STB__ptr(p,0) = 0xdeadbeef;
         * (unsigned int *) STB__ptr(p,4) = 0xdeadbeef;
         * (unsigned int *) STB__ptr(p,8) = 0xdeadbeef;
         * (unsigned int *) STB__ptr(p,12) = 0xdeadbeef;
      #endif
      stb__free_final(p);
   }

   void * stb__malloc(int sz, char *file, int line)
   {
      void *p;
      stb_mcheck_all();
      if (sz == 0) return NULL;
      if (stb__malloc_iter == 12741)
         sz = sz;
      p = stb__malloc_final(STB__FIXSIZE(sz));
      if (p == NULL) p = stb__malloc_final(STB__FIXSIZE(sz));
      if (p == NULL) p = stb__malloc_final(STB__FIXSIZE(sz));
      if (p == NULL) {
         ++stb__malloc_failure;
         #ifdef STB_MALLOC_WRAPPER_DEBUG
         stb_malloc_checkall();
         #endif
         return p;
      }
      #ifdef STB_MALLOC_WRAPPER_DEBUG
      * (int *) STB__ptr(p,0) = STB__FIXSIZE(sz);
      * (unsigned int *) STB__ptr(p,4) = STB__SIG;
      * (unsigned int *) STB__ptr(p,8) = STB__SIG;
      * (unsigned int *) STB__ptr(p,12) = STB__SIG;
      * (unsigned int *) STB__ptr(p,STB__FIXSIZE(sz)-4) = STB__SIG+1;
      * (unsigned int *) STB__ptr(p,STB__FIXSIZE(sz)-8) = STB__SIG+1;
      * (unsigned int *) STB__ptr(p,STB__FIXSIZE(sz)-12) = STB__SIG+1;
      * (unsigned int *) STB__ptr(p,STB__FIXSIZE(sz)-16) = STB__SIG+1;
      p = STB__ptr(p, STB__BIAS);
      #endif
      stb_wrapper_malloc(p,sz,file,line);
      return p;
   }

   void * stb__realloc(void *p, int sz, char *file, int line)
   {
      void *q;

      stb_mcheck_all();
      if (p == NULL) return stb__malloc(sz,file,line);
      if (sz == 0  ) { stb__free(p,file,line); return NULL; }

      #ifdef STB_MALLOC_WRAPPER_DEBUG
         stb_mcheck(p);
         p = STB__ptr(p,-STB__BIAS);
      #endif
      #ifdef STB_MALLOC_WRAPPER_PAGED
      {
         int n = stb_wrapper_allocsize(STB__ptr(p,STB__BIAS));
         if (!n)
            stb_wrapper_check(STB__ptr(p,STB__BIAS));
         q = stb__realloc_final(p, STB__FIXSIZE(sz), STB__FIXSIZE(n));
      }
      #else
      q = realloc(p, STB__FIXSIZE(sz));
      #endif
      if (q == NULL)
         return ++stb__malloc_failure, q;
      #ifdef STB_MALLOC_WRAPPER_DEBUG
      * (int *) STB__ptr(q,0) = STB__FIXSIZE(sz);
      * (unsigned int *) STB__ptr(q,4) = STB__SIG;
      * (unsigned int *) STB__ptr(q,8) = STB__SIG;
      * (unsigned int *) STB__ptr(q,12) = STB__SIG;
      * (unsigned int *) STB__ptr(q,STB__FIXSIZE(sz)-4) = STB__SIG+1;
      * (unsigned int *) STB__ptr(q,STB__FIXSIZE(sz)-8) = STB__SIG+1;
      * (unsigned int *) STB__ptr(q,STB__FIXSIZE(sz)-12) = STB__SIG+1;
      * (unsigned int *) STB__ptr(q,STB__FIXSIZE(sz)-16) = STB__SIG+1;

      q = STB__ptr(q, STB__BIAS);
      p = STB__ptr(p, STB__BIAS);
      #endif
      stb_wrapper_realloc(p,q,sz,file,line);
      return q;
   }

   STB_EXTERN int stb_log2_ceil(unsigned int);
   static void *stb__calloc(size_t n, size_t sz, char *file, int line)
   {
      void *q;
      stb_mcheck_all();
      if (n == 0 || sz == 0) return NULL;
      if (stb_log2_ceil(n) + stb_log2_ceil(sz) >= 32) return NULL;
      q = stb__malloc(n*sz, file, line);
      if (q) memset(q, 0, n*sz);
      return q;
   }

   char * stb__strdup(char *s, char *file, int line)
   {
      char *p;
      stb_mcheck_all();
      p = stb__malloc(strlen(s)+1, file, line);
      if (!p) return p;
      strcpy(p, s);
      return p;
   }
   #endif

   #ifdef STB_FASTMALLOC
   #undef malloc
   #undef realloc
   #undef free
   #undef strdup
   #undef calloc
   #endif

   #define malloc(s)      stb__malloc (  s, __FILE__, __LINE__)
   #define realloc(p,s)   stb__realloc(p,s, __FILE__, __LINE__)
   #define calloc(n,s)    stb__calloc (n,s, __FILE__, __LINE__)
   #define free(p)        stb__free   (p,   __FILE__, __LINE__)
   #define strdup(p)      stb__strdup (p,   __FILE__, __LINE__)

#endif

//////////////////////////////////////////////////////////////////////////////
//
//                         Windows UTF8 filename handling
//
// Windows stupidly treats 8-bit filenames as some dopey code page,
// rather than utf-8. If we want to use utf8 filenames, we have to
// convert them to WCHAR explicitly and call WCHAR versions of the
// file functions. So, ok, we do.


#ifdef _WIN32
   #define stb__fopen(x,y)    _wfopen(stb__from_utf8(x), stb__from_utf8_alt(y))
   #define stb__windows(x,y)  x
#else
   #define stb__fopen(x,y)    fopen(x,y)
   #define stb__windows(x,y)  y
#endif


typedef unsigned short stb__wchar;

STB_EXTERN stb__wchar * stb_from_utf8(stb__wchar *buffer, char *str, int n);
STB_EXTERN char       * stb_to_utf8  (char *buffer, stb__wchar *str, int n);

STB_EXTERN stb__wchar *stb__from_utf8(char *str);
STB_EXTERN stb__wchar *stb__from_utf8_alt(char *str);
STB_EXTERN char *stb__to_utf8(stb__wchar *str);


#ifdef STB_DEFINE
stb__wchar * stb_from_utf8(stb__wchar *buffer, char *str, int n)
{
   uint32 c;
   int i=0;
   --n;
   while (*str) {
      if (i >= n)
         return NULL;
      if (!(*str & 0x80))
         buffer[i++] = *str++;
      else if ((*str & 0xe0) == 0xc0) {
         c = (*str++ & 0x1f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         buffer[i++] = c + (*str++ & 0x3f);
      } else if ((*str & 0xf0) == 0xe0) {
         c = (*str++ & 0x0f) << 12;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         buffer[i++] = c + (*str++ & 0x3f);
      } else if ((*str & 0xf8) == 0xf0) {
         c = (*str++ & 0x07) << 18;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 12;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f);
         // surrogate pair values are invalid
         if ((c & 0xFFFFF800) == 0xD800) return NULL;
         if (c >= 0x10000) {
            c -= 0x10000;
            if (i + 2 > n) return NULL;
            buffer[i++] = 0xD800 | (0x3ff & (c >> 10));
            buffer[i++] = 0xDC00 | (0x3ff & (c      ));
         }
      } else
         return NULL;
   }
   buffer[i] = 0;
   return buffer;
}

char * stb_to_utf8(char *buffer, stb__wchar *str, int n)
{
   int i=0;
   --n;
   while (*str) {
      if (*str < 0x80) {
         if (i+1 > n) return NULL;
         buffer[i++] = (char) *str++;
      } else if (*str < 0x800) {
         if (i+2 > n) return NULL;
         buffer[i++] = 0xc0 + (*str >> 6);
         buffer[i++] = 0x80 + (*str & 0x3f);
         str += 1;
      } else if (*str >= 0xd800 && *str < 0xdc00) {
         uint32 c;
         if (i+4 > n) return NULL;
         c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
         buffer[i++] = 0xf0 + (c >> 18);
         buffer[i++] = 0x80 + ((c >> 12) & 0x3f);
         buffer[i++] = 0x80 + ((c >>  6) & 0x3f);
         buffer[i++] = 0x80 + ((c      ) & 0x3f);
         str += 2;
      } else if (*str >= 0xdc00 && *str < 0xe000) {
         return NULL;
      } else {
         if (i+3 > n) return NULL;
         buffer[i++] = 0xe0 + (*str >> 12);
         buffer[i++] = 0x80 + ((*str >> 6) & 0x3f);
         buffer[i++] = 0x80 + ((*str     ) & 0x3f);
         str += 1;
      }
   }
   buffer[i] = 0;
   return buffer;
}

stb__wchar *stb__from_utf8(char *str)
{
   static stb__wchar buffer[4096];
   return stb_from_utf8(buffer, str, 4096);
}

stb__wchar *stb__from_utf8_alt(char *str)
{
   static stb__wchar buffer[64];
   return stb_from_utf8(buffer, str, 64);
}

char *stb__to_utf8(stb__wchar *str)
{
   static char buffer[4096];
   return stb_to_utf8(buffer, str, 4096);
}

#endif

//////////////////////////////////////////////////////////////////////////////
//
//                         Miscellany
//


//@ <hr><h2>Logging</h2>
//$ Fatal error. Print an error message, trap into the debugger, and exit.
//$ fmt=string to print; supports `printf`-style formatting of additional parameters
STB_EXTERN void stb_fatal(char *fmt, ...);
//$ Print a message to the logfile if it is enabled (defaults to enabled)
STB_EXTERN void stb_(char *fmt, ...);
//$ Enable/disable the log file
//$ (if non-zero, logging will be enabled; if zero, logging will be disabled)
STB_EXTERN void stb_log(int active);
//$ Enable/disable recording file &amp; line information to the log file.
//$ This is only available in debug builds, or if STB_DEBUG is otherwise set.
//$ (if non-zero, file/line recording will be enabled; if zero, file/line recording will be disabled)
STB_EXTERN void stb_log_fileline(int active);
//$ Set the filename to use logging with stb_()<p>Default is "stb.log"
//$ (Name of the file where future calls to stb_() will print.)
STB_EXTERN void stb_log_name(char *filename);

//@ <hr><h2>Miscellany</h2>
//$ Swap two blocks of memory
//$ (pointer to first block of memory to swap|pointer to second block of memory to swap,size of both blocks in bytes)
STB_EXTERN void stb_swap(void *p, void *q, size_t sz);
//$ malloc() a copy of a block of memory (like strdup, but for memory)
//$ returns NULL if malloc() returns NULL
//$ (pointer to the block of memory to copy|size of the the block of memory in bytes)
STB_EXTERN void *stb_copy(void *p, size_t sz);
//$ Free all the pointers in an array
//$ (pointer to pointers to free|number of pointers to free)
STB_EXTERN void stb_pointer_array_free(void **p, int len);
//$ Allocate an array of pointer to memory blocks
//$ (number of pointers top-level array|length of each block in bytes)
STB_EXTERN void **stb_array_block_alloc(int count, int blocksize);

#define stb_arrcount(x)   (sizeof(x)/sizeof((x)[0]))


STB_EXTERN int  stb__record_fileline(char *f, int n);

#ifdef STB_DEFINE

static char *stb__file;
static int   stb__line;

int  stb__record_fileline(char *f, int n)
{
   stb__file = f;
   stb__line = n;
   return 0;
}

void stb_fatal(char *s, ...)
{
   va_list a;
   if (stb__file)
      fprintf(stderr, "[%s:%d] ", stb__file, stb__line);
   va_start(a,s);
   fputs("Fatal error: ", stderr);
   vfprintf(stderr, s, a);
   va_end(a);
   fputs("\n", stderr);
   #ifdef _WIN32
   #ifdef STB_DEBUG
   __asm int 3;   // trap to debugger!
   #endif
   #endif
   exit(1);
}

static int stb__log_active=1, stb__log_fileline=1;

void stb_log(int active)
{
   stb__log_active = active;
}

void stb_log_fileline(int active)
{
   stb__log_fileline = active;
}

char *stb__log_filename = "stb.log";

void stb_log_name(char *s)
{
   stb__log_filename = s;
}

void stb_(char *s, ...)
{
   if (stb__log_active) {
      FILE *f = fopen(stb__log_filename, "a");
      if (f) {
         va_list a;
         if (stb__log_fileline && stb__file)
            fprintf(f, "[%s:%4d] ", stb__file, stb__line);
         va_start(a,s);
         vfprintf(f, s, a);
         va_end(a);
         fputs("\n", f);
         fclose(f);
      }
   }
}


typedef struct { char d[4]; } stb__4;
typedef struct { char d[8]; } stb__8;

// optimize the small cases, though you shouldn't be calling this for those!
void stb_swap(void *p, void *q, size_t sz)
{
   char buffer[256];
   if (p == q) return;
   if (sz == 4) {
      stb__4 temp    = * ( stb__4 *) p;
      * (stb__4 *) p = * ( stb__4 *) q;
      * (stb__4 *) q = temp;
      return;
   } else if (sz == 8) {
      stb__8 temp    = * ( stb__8 *) p;
      * (stb__8 *) p = * ( stb__8 *) q;
      * (stb__8 *) q = temp;
      return;
   }

   while (sz > sizeof(buffer)) {
      stb_swap(p, q, sizeof(buffer));
      p = (char *) p + sizeof(buffer);
      q = (char *) q + sizeof(buffer);
      sz -= sizeof(buffer);
   }

   memcpy(buffer, p     , sz);
   memcpy(p     , q     , sz);
   memcpy(q     , buffer, sz);
}

void *stb_copy(void *p, size_t sz)
{
   void *q = malloc(sz);
   memcpy(q, p, sz);
   return q;
}

void stb_pointer_array_free(void **p, int len)
{
   int i;
   for (i=0; i < len; ++i)
      free(p[i]);
}

void **stb_array_block_alloc(int count, int blocksize)
{
   int i;
   char *p = (char *) malloc(sizeof(void *) * count + count * blocksize);
   void **q;
   if (p == NULL) return NULL;
   q = (void **) p;
   p += sizeof(void *) * count;
   for (i=0; i < count; ++i)
      q[i] = p + i * blocksize;
   return q;
}
#endif

#ifdef STB_DEBUG
   // tricky hack to allow recording FILE,LINE even in varargs functions
   #define STB__RECORD_FILE(x)  (stb__record_fileline(__FILE__, __LINE__),(x))
   #define stb_log              STB__RECORD_FILE(stb_log)
   #define stb_                 STB__RECORD_FILE(stb_)
   #ifndef STB_FATAL_CLEAN
   #define stb_fatal            STB__RECORD_FILE(stb_fatal)
   #endif
   #define STB__DEBUG(x)        x
#else
   #define STB__DEBUG(x)
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                         stb_temp
//

//$ Allocate a block of memory either on the stack or from malloc. 
//$ block=temporary variable on the stack (e.g. an array)
//$ (=; this is returned by stb_temp if it is large enough, otherwise memory is obtained with malloc()|the size of memory requested)
#define stb_temp(block, sz)     stb__temp(block, sizeof(block), (sz))

STB_EXTERN void * stb__temp(void *b, int b_sz, int want_sz);
//$ Deallocate the memory allocated by stb_temp()
//$ (= that was passed to stb_temp()|the pointer returned by stb_temp())
STB_EXTERN void   stb_tempfree(void *block, void *ptr);

#ifdef STB_DEFINE

void * stb__temp(void *b, int b_sz, int want_sz)
{
   if (b_sz >= want_sz)
      return b;
   else
      return malloc(want_sz);
}

void   stb_tempfree(void *b, void *p)
{
   if (p != b)
      free(p);
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
//                      math/sampling operations
//


#define stb_lerp(t,a,b)               ( (a) + (t) * (float) ((b)-(a)) )
#define stb_unlerp(t,a,b)             ( ((t) - (a)) / (float) ((b) - (a)) )

//$ Clamp a number so it lies within a specified range.
//$ Note this is a macro that evaluates arguments more than once.
//$ (value to clamp|minimum value of result|maximum value of result)
#define stb_clamp(x,xmin,xmax)  ((x) < (xmin) ? (xmin) : (x) > (xmax) ? (xmax) : (x))

STB_EXTERN void stb_newell_normal(float *normal, int num_vert, float **vert, int normalize);
STB_EXTERN void stb_linear_controller(float *curpos, float target_pos, float acc, float deacc, float dt);

STB_EXTERN int stb_float_eq(float x, float y, float delta, int max_ulps);
STB_EXTERN int stb_is_prime(unsigned int m);
STB_EXTERN unsigned int stb_power_of_two_nearest_prime(int n);

STB_EXTERN double stb_linear_remap(double x, double a, double b,
                                             double c, double d);

#ifdef STB_DEFINE
void stb_newell_normal(float *normal, int num_vert, float **vert, int normalize)
{
   int i,j;
   float p;
   normal[0] = normal[1] = normal[2] = 0;
   for (i=num_vert-1,j=0; j < num_vert; i=j++) {
      float *u = vert[i];
      float *v = vert[j];
      normal[0] = (u[1] - v[1]) * (u[2] + v[2]);
      normal[1] = (u[2] - v[2]) * (u[0] + v[0]);
      normal[2] = (u[0] - v[0]) * (u[1] + v[1]);
   }
   if (normalize) {
      p = normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2];
      p = (float) (1.0 / sqrt(p));
      normal[0] *= p;
      normal[1] *= p;
      normal[2] *= p;
   }
}

void stb_linear_controller(float *curpos, float target_pos, float acc, float deacc, float dt)
{
   float sign = 1, p, cp = *curpos;
   if (cp == target_pos) return;
   if (target_pos < cp) {
      target_pos = -target_pos;
      cp = -cp;
      sign = -1;
   }
   // first decelerate
   if (cp < 0) {
      p = cp + deacc * dt;
      if (p > 0) {
         p = 0;
         dt = dt - cp / deacc;
         if (dt < 0) dt = 0;
      } else {
         dt = 0;
      }
      cp = p;
   }
   // now accelerate
   p = cp + acc*dt;
   if (p > target_pos) p = target_pos;
   *curpos = p * sign;
   // @TODO: testing
}

float stb_quadratic_controller(float target_pos, float curpos, float maxvel, float maxacc, float dt, float *curvel)
{
   return 0; // @TODO
}

int stb_float_eq(float x, float y, float delta, int max_ulps)
{
   if (fabs(x-y) <= delta) return 1;
   if (abs(*(int *)&x - *(int *)&y) <= max_ulps) return 1;
   return 0;
}

int stb_is_prime(unsigned int m)
{
   unsigned int i,j;
   if (m < 2) return 0;
   if (m == 2) return 1;
   if (!(m & 1)) return 0;
   if (m % 3 == 0) return (m == 3);
   for (i=5; (j=i*i), j <= m && j > i; i += 6) {
      if (m %   i   == 0) return 0;
      if (m % (i+2) == 0) return 0;
   }
   return 1;
}

unsigned int stb_power_of_two_nearest_prime(int n)
{
   static unsigned int tab[32] = { 0,0,0,0,1,0,-1,0,1,-1,-1,3,-1,0,-1,2,1,
                                   0,2,0,-1,-4,-1,5,-1,18,-2,15,2,-1,2,0 };
   if (!tab[0]) {
      int i;
      for (i=0; i < 32; ++i)
         tab[i] = (1 << i) + 2*tab[i] - 1;
      tab[1] = 2;
      tab[0] = 1;
   }
   if (n >= 32) return -5; // assumes 32-bit!
   return tab[n];
}

double stb_linear_remap(double x, double x_min, double x_max,
                                  double out_min, double out_max)
{
   return stb_lerp(stb_unlerp(x,x_min,x_max),out_min,out_max);
}
#endif

// create a macro so it's faster, but you can get at the function pointer
#define stb_linear_remap(t,a,b,c,d)   stb_lerp(stb_unlerp(t,a,b),c,d)


//////////////////////////////////////////////////////////////////////////////
//
//                         bit operations
//

#define stb_big32(c)    (((c)[0]<<24) + (c)[1]*65536 + (c)[2]*256 + (c)[3])
#define stb_little32(c) (((c)[3]<<24) + (c)[2]*65536 + (c)[1]*256 + (c)[0])
#define stb_big16(c)    ((c)[0]*256 + (c)[1])
#define stb_little16(c) ((c)[1]*256 + (c)[0])

STB_EXTERN          int stb_bitcount(unsigned int a);
STB_EXTERN unsigned int stb_bitreverse8(unsigned char n);
STB_EXTERN unsigned int stb_bitreverse(unsigned int n);

STB_EXTERN          int stb_is_pow2(unsigned int n);
STB_EXTERN          int stb_log2_ceil(unsigned int n);
STB_EXTERN          int stb_log2_floor(unsigned int n);

STB_EXTERN          int stb_lowbit8(unsigned int n);
STB_EXTERN          int stb_highbit8(unsigned int n);

#ifdef STB_DEFINE
int stb_bitcount(unsigned int a)
{
   a = (a & 0x55555555) + ((a >>  1) & 0x55555555); // max 2
   a = (a & 0x33333333) + ((a >>  2) & 0x33333333); // max 4
   a = (a + (a >> 4)) & 0x0f0f0f0f; // max 8 per 4, now 8 bits
   a = (a + (a >> 8)); // max 16 per 8 bits
   a = (a + (a >> 16)); // max 32 per 8 bits
   return a & 0xff;
}

unsigned int stb_bitreverse8(unsigned char n)
{
   n = ((n & 0xAA) >> 1) + ((n & 0x55) << 1);
   n = ((n & 0xCC) >> 2) + ((n & 0x33) << 2);
   return (unsigned char) ((n >> 4) + (n << 4));
}

unsigned int stb_bitreverse(unsigned int n)
{
  n = ((n & 0xAAAAAAAA) >>  1) | ((n & 0x55555555) << 1);
  n = ((n & 0xCCCCCCCC) >>  2) | ((n & 0x33333333) << 2);
  n = ((n & 0xF0F0F0F0) >>  4) | ((n & 0x0F0F0F0F) << 4);
  n = ((n & 0xFF00FF00) >>  8) | ((n & 0x00FF00FF) << 8);
  return (n >> 16) | (n << 16);
}

int stb_is_pow2(unsigned int n)
{
   return (n & (n-1)) == 0;
}

// tricky use of 4-bit table to identify 5 bit positions (note the '-1')
// 3-bit table would require another tree level; 5-bit table wouldn't save one
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4035)  // disable warning about no return value
int stb_log2_floor(unsigned int n)
{
   __asm {
      bsr eax,n
      jnz done
      mov eax,-1
   }
   done:;
}
#pragma warning(pop)
#else
int stb_log2_floor(unsigned int n)
{
   static signed char log2_4[16] = { -1,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3 };

   // 2 compares if n < 16, 3 compares otherwise
   if (n < (1U << 14))
        if (n < (1U <<  4))        return     0 + log2_4[n      ];
        else if (n < (1U <<  9))      return  5 + log2_4[n >>  5];
             else                     return 10 + log2_4[n >> 10];
   else if (n < (1U << 24))
             if (n < (1U << 19))      return 15 + log2_4[n >> 15];
             else                     return 20 + log2_4[n >> 20];
        else if (n < (1U << 29))      return 25 + log2_4[n >> 25];
             else                     return 30 + log2_4[n >> 30];
}
#endif

// define ceil from floor
int stb_log2_ceil(unsigned int n)
{
   if (stb_is_pow2(n))  return     stb_log2_floor(n);
   else                 return 1 + stb_log2_floor(n);
}

int stb_highbit8(unsigned int n)
{
   return stb_log2_ceil(n&255);
}

int stb_lowbit8(unsigned int n)
{
   static signed char lowbit4[16] = { -1,0,1,0, 2,0,1,0, 3,0,1,0, 2,0,1,0 };
   int k = lowbit4[n & 15];
   if (k >= 0) return k;
   k = lowbit4[(n >> 4) & 15];
   if (k >= 0) return k+4;
   return k;
}
#endif



//////////////////////////////////////////////////////////////////////////////
//
//                            qsort Compare Routines
//

#ifdef _WIN32
   //$ case-insensitive comparison (portable equivalent to stricmp or strcasecmp)
   #define stb_stricmp(a,b) stricmp(a,b)
   //$ case-insensitive comparison (portable equivalent to strnicmp or strncasecmp)
   #define stb_strnicmp(a,b,n) strnicmp(a,b,n)
#else
   //$ SKIP!
   #define stb_stricmp(a,b) strcasecmp(a,b)
   //$ SKIP!
   #define stb_strnicmp(a,b,n) strncasecmp(a,b,n)
#endif


STB_EXTERN int stb_intcmp(const void *a, const void *b);
STB_EXTERN int stb_qsort_strcmp(const void *a, const void *b);
STB_EXTERN int stb_qsort_stricmp(const void *a, const void *b);
STB_EXTERN int stb_floatcmp(const void *a, const void *b);
STB_EXTERN int stb_doublecmp(const void *a, const void *b);
STB_EXTERN void stb_cmpoffset(int off);

#ifdef STB_DEFINE
static int stb__cmpoffset=0;

int stb_intcmp(const void *a, const void *b)
{
   const int p = *(const int *) ((const char *) a + stb__cmpoffset);
   const int q = *(const int *) ((const char *) b + stb__cmpoffset);
   return p < q ? -1 : p > q;
}

int stb_floatcmp(const void *a, const void *b)
{
   const float p = *(const float *) ((const char *) a + stb__cmpoffset);
   const float q = *(const float *) ((const char *) b + stb__cmpoffset);
   return p < q ? -1 : p > q;
}

int stb_doublecmp(const void *a, const void *b)
{
   const double p = *(const double *) ((const char *) a + stb__cmpoffset);
   const double q = *(const double *) ((const char *) b + stb__cmpoffset);
   return p < q ? -1 : p > q;
}

int stb_qsort_strcmp(const void *a, const void *b)
{
   const char *p = *(const char **) ((const char *) a + stb__cmpoffset);
   const char *q = *(const char **) ((const char *) b + stb__cmpoffset);
   return strcmp(p,q);
}

int stb_qsort_stricmp(const void *a, const void *b)
{
   const char *p = *(const char **) ((const char *) a + stb__cmpoffset);
   const char *q = *(const char **) ((const char *) b + stb__cmpoffset);
   return stb_stricmp(p,q);
}

void stb_cmpoffset(int off)
{
   stb__cmpoffset = off;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                           Binary Search Toolkit
//

typedef struct
{
   int minval, maxval, guess;
   int mode, step;
} stb_search;

STB_EXTERN int stb_search_binary(stb_search *s, int minv, int maxv, int find_smallest);
STB_EXTERN int stb_search_open(stb_search *s, int minv, int find_smallest);
STB_EXTERN int stb_probe(stb_search *s, int compare, int *result); // return 0 when done

#ifdef STB_DEFINE
enum
{
   STB_probe_binary_smallest,
   STB_probe_binary_largest,
   STB_probe_open_smallest,
   STB_probe_open_largest,
};

static int stb_probe_guess(stb_search *s, int *result)
{
   switch(s->mode) {
      case STB_probe_binary_largest:
         if (s->minval == s->maxval) {
            *result = s->minval;
            return 0;
         }
         assert(s->minval < s->maxval);
         // if a < b, then a < p <= b
         s->guess = s->minval + (((unsigned) s->maxval - s->minval + 1) >> 1);
         break;

      case STB_probe_binary_smallest:
         if (s->minval == s->maxval) {
            *result = s->minval;
            return 0;
         }
         assert(s->minval < s->maxval);
         // if a < b, then a <= p < b
         s->guess = s->minval + (((unsigned) s->maxval - s->minval) >> 1);
         break;
      case STB_probe_open_smallest:
      case STB_probe_open_largest:
         s->guess = s->maxval;  // guess the current maxval
         break;
   }
   *result = s->guess;
   return 1;
}

int stb_probe(stb_search *s, int compare, int *result)
{
   switch(s->mode) {
      case STB_probe_open_smallest:
      case STB_probe_open_largest: {
         if (compare <= 0) {
            // then it lies within minval & maxval
            if (s->mode == STB_probe_open_smallest)
               s->mode = STB_probe_binary_smallest;
            else
               s->mode = STB_probe_binary_largest;
         } else {
            // otherwise, we need to probe larger
            s->minval  = s->maxval + 1;
            s->maxval  = s->minval + s->step;
            s->step   += s->step;
         }
         break;
      }
      case STB_probe_binary_smallest: {
         // if compare < 0, then s->minval <= a <  p
         // if compare = 0, then s->minval <= a <= p
         // if compare > 0, then         p <  a <= s->maxval
         if (compare <= 0)
            s->maxval = s->guess;
         else
            s->minval = s->guess+1;
         break;
      }
      case STB_probe_binary_largest: {
         // if compare < 0, then s->minval <= a < p
         // if compare = 0, then         p <= a <= s->maxval
         // if compare > 0, then         p <  a <= s->maxval
         if (compare < 0)
            s->maxval = s->guess-1;
         else
            s->minval = s->guess;
         break;
      }
   }
   return stb_probe_guess(s, result);
}

int stb_search_binary(stb_search *s, int minv, int maxv, int find_smallest)
{
   int r;
   if (maxv < minv) return minv-1;
   s->minval = minv;
   s->maxval = maxv;
   s->mode = find_smallest ? STB_probe_binary_smallest : STB_probe_binary_largest;
   stb_probe_guess(s, &r);
   return r;
}

int stb_search_open(stb_search *s, int minv, int find_smallest)
{
   int r;
   s->step   = 4;
   s->minval = minv;
   s->maxval = minv+s->step;
   s->mode = find_smallest ? STB_probe_open_smallest : STB_probe_open_largest;
   stb_probe_guess(s, &r);
   return r;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                           String Processing
//

//$ returns true if the second string is a prefix of the first string, that
//$ is if the first string begins with the second string.
//$ (string to test|expected prefix)
#define stb_prefixi(s,t)  (0==stb_strnicmp((s),(t),strlen(t)))

enum stb_splitpath_flag
{
   STB_PATH = 1,
   STB_FILE = 2,
   STB_EXT  = 4,
   STB_PATH_FILE = STB_PATH + STB_FILE,
   STB_FILE_EXT  = STB_FILE + STB_EXT,
   STB_EXT_NO_PERIOD = 8,
};

STB_EXTERN char * stb_skipwhite(char *s);
STB_EXTERN char * stb_trimwhite(char *s);
STB_EXTERN char * stb_skipnewline(char *s);
STB_EXTERN char * stb_strncpy(char *s, char *t, int n);
STB_EXTERN char * stb_substr(char *t, int n);
STB_EXTERN char * stb_duplower(char *s);
STB_EXTERN void   stb_tolower (char *s);
STB_EXTERN char * stb_strchr2 (char *s, char p1, char p2);
STB_EXTERN char * stb_strrchr2(char *s, char p1, char p2);
STB_EXTERN char * stb_strtok(char *output, char *src, char *delimit);
STB_EXTERN char * stb_strtok_keep(char *output, char *src, char *delimit);
STB_EXTERN char * stb_strtok_invert(char *output, char *src, char *allowed);
STB_EXTERN char * stb_dupreplace(char *s, char *find, char *replace);
STB_EXTERN void   stb_replaceinplace(char *s, char *find, char *replace);
STB_EXTERN char * stb_splitpath(char *output, char *src, int flag);
STB_EXTERN char * stb_splitpathdup(char *src, int flag);
STB_EXTERN char * stb_replacedir(char *output, char *src, char *dir);
STB_EXTERN char * stb_replaceext(char *output, char *src, char *ext);
STB_EXTERN void   stb_fixpath(char *path);
STB_EXTERN int    stb_suffix (char *s, char *t);
STB_EXTERN int    stb_suffixi(char *s, char *t);
STB_EXTERN int    stb_prefix (char *s, char *t);
STB_EXTERN char * stb_strichr(char *s, char t);
STB_EXTERN char * stb_stristr(char *s, char *t);
STB_EXTERN int    stb_prefix_count(char *s, char *t);
STB_EXTERN char * stb_plural(int n);  // "s" or ""

STB_EXTERN char **stb_tokens(char *src, char *delimit, int *count);
STB_EXTERN char **stb_tokens_nested(char *src, char *delimit, int *count, char *nest_in, char *nest_out);
STB_EXTERN char **stb_tokens_nested_empty(char *src, char *delimit, int *count, char *nest_in, char *nest_out);
STB_EXTERN char **stb_tokens_allowempty(char *src, char *delimit, int *count);
STB_EXTERN char **stb_tokens_stripwhite(char *src, char *delimit, int *count);
STB_EXTERN char **stb_tokens_withdelim(char *src, char *delimit, int *count);
STB_EXTERN char **stb_tokens_quoted(char *src, char *delimit, int *count);
// with 'quoted', allow delimiters to appear inside quotation marks, and don't
// strip whitespace inside them (and we delete the quotation marks unless they
// appear back to back, in which case they're considered escaped)

#ifdef STB_DEFINE

char *stb_plural(int n)
{
   return n == 1 ? "" : "s";
}

int stb_prefix(char *s, char *t)
{
   while (*t)
      if (*s++ != *t++)
         return FALSE;
   return TRUE;
}

int stb_prefix_count(char *s, char *t)
{
   int c=0;
   while (*t) {
      if (*s++ != *t++)
         break;
      ++c;
   }
   return c;
}

int stb_suffix(char *s, char *t)
{
   size_t n = strlen(s);
   size_t m = strlen(t);
   if (m <= n)
      return 0 == strcmp(s+n-m, t);
   else
      return 0;
}

int stb_suffixi(char *s, char *t)
{
   size_t n = strlen(s);
   size_t m = strlen(t);
   if (m <= n)
      return 0 == stb_stricmp(s+n-m, t);
   else
      return 0;
}

// originally I was using this table so that I could create known sentinel
// values--e.g. change whitetable[0] to be true if I was scanning for whitespace,
// and false if I was scanning for nonwhite. I don't appear to be using that
// functionality anymore (I do for tokentable, though), so just replace it
// with isspace()
char *stb_skipwhite(char *s)
{
   while (isspace((unsigned char) *s)) ++s;
   return s;
}

char *stb_skipnewline(char *s)
{
   if (s[0] == '\r' || s[0] == '\n') {
      if (s[0]+s[1] == '\r' + '\n') ++s;
      ++s;
   }
   return s;
}

char *stb_trimwhite(char *s)
{
   int i,n;
   s = stb_skipwhite(s);
   n = (int) strlen(s);
   for (i=n-1; i >= 0; ++i)
      if (!isspace(s[i]))
         break;
   s[i+1] = 0;
   return s;
}

char *stb_strncpy(char *s, char *t, int n)
{
   strncpy(s,t,n);
   s[n-1] = 0;
   return s;
}

char *stb_substr(char *t, int n)
{
   char *a;
   int z = (int) strlen(t);
   if (z < n) n = z;
   a = (char *) malloc(n+1);
   strncpy(a,t,n);
   a[n] = 0;
   return a;
}

char *stb_duplower(char *s)
{
   char *p = strdup(s), *q = p;
   while (*q) {
      *q = tolower(*q);
      ++q;
   }
   return p;
}

void stb_tolower(char *s)
{
   while (*s) {
      *s = tolower(*s);
      ++s;
   }
}

char *stb_strchr2(char *s, char x, char y)
{
   for(; *s; ++s)
      if (*s == x || *s == y)
         return s;
   return NULL;
}

char *stb_strrchr2(char *s, char x, char y)
{
   char *r = NULL;
   for(; *s; ++s)
      if (*s == x || *s == y)
         r = s;
   return r;
}

char *stb_strichr(char *s, char t)
{
   if (tolower(t) == toupper(t))
      return strchr(s,t);
   return stb_strchr2(s, (char) tolower(t), (char) toupper(t));
}

char *stb_stristr(char *s, char *t)
{
   size_t n = strlen(t);
   char *z;
   if (n==0) return s;
   while ((z = stb_strichr(s, *t)) != NULL) {
      if (0==stb_strnicmp(z, t, n))
         return z;
      s = z+1;
   }
   return NULL;
}

static char *stb_strtok_raw(char *output, char *src, char *delimit, int keep, int invert)
{
   if (invert) {
      while (*src && strchr(delimit, *src) != NULL) {
         *output++ = *src++;
      }
   } else {
      while (*src && strchr(delimit, *src) == NULL) {
         *output++ = *src++;
      }
   }
   *output = 0;
   if (keep)
      return src;
   else
      return *src ? src+1 : src;
}

char *stb_strtok(char *output, char *src, char *delimit)
{
   return stb_strtok_raw(output, src, delimit, 0, 0);
}

char *stb_strtok_keep(char *output, char *src, char *delimit)
{
   return stb_strtok_raw(output, src, delimit, 1, 0);
}

char *stb_strtok_invert(char *output, char *src, char *delimit)
{
   return stb_strtok_raw(output, src, delimit, 1,1);
}

static char **stb_tokens_raw(char *src_, char *delimit, int *count,
                             int stripwhite, int allow_empty, char *start, char *end)
{
   int nested = 0;
   unsigned char *src = (unsigned char *) src_;
   static char stb_tokentable[256]; // rely on static initializion to 0
   static char stable[256],etable[256];
   char *out;
   char **result;
   int num=0;
   unsigned char *s;

   s = (unsigned char *) delimit; while (*s) stb_tokentable[*s++] = 1;
   if (start) {
      s = (unsigned char *) start;         while (*s) stable[*s++] = 1;
      s = (unsigned char *) end;   if (s)  while (*s) stable[*s++] = 1;
      s = (unsigned char *) end;   if (s)  while (*s) etable[*s++] = 1;
   }
   stable[0] = 1;

   // two passes through: the first time, counting how many
   s = (unsigned char *) src;
   while (*s) {
      // state: just found delimiter
      // skip further delimiters
      if (!allow_empty) {
         stb_tokentable[0] = 0;
         while (stb_tokentable[*s])
            ++s;
         if (!*s) break;
      }
      ++num;
      // skip further non-delimiters
      stb_tokentable[0] = 1;
      if (stripwhite == 2) { // quoted strings
         while (!stb_tokentable[*s]) {
            if (*s != '"')
               ++s;
            else {
               ++s;
               if (*s == '"')
                  ++s;   // "" -> ", not start a string
               else {
                  // begin a string
                  while (*s) {
                     if (s[0] == '"') {
                        if (s[1] == '"') s += 2; // "" -> "
                        else { ++s; break; } // terminating "
                     } else
                        ++s;
                  }
               }
            }
         }
      } else 
         while (nested || !stb_tokentable[*s]) {
            if (stable[*s]) {
               if (!*s) break;
               if (end ? etable[*s] : nested)
                  --nested;
               else
                  ++nested;
            }
            ++s;
         }
      if (allow_empty) {
         if (*s) ++s;
      }
   }
   // now num has the actual count... malloc our output structure
   // need space for all the strings: strings won't be any longer than
   // original input, since for every '\0' there's at least one delimiter
   result = (char **) malloc(sizeof(*result) * (num+1) + (s-src+1));
   if (result == NULL) return result;
   out = (char *) (result + (num+1));
   // second pass: copy out the data
   s = (unsigned char *) src;
   num = 0;
   nested = 0;
   while (*s) {
      char *last_nonwhite;
      // state: just found delimiter
      // skip further delimiters
      if (!allow_empty) {
         stb_tokentable[0] = 0;
         if (stripwhite)
            while (stb_tokentable[*s] || isspace(*s))
               ++s;
         else
            while (stb_tokentable[*s])
               ++s;
      } else if (stripwhite) {
         while (isspace(*s)) ++s;
      }
      if (!*s) break;
      // we're past any leading delimiters and whitespace
      result[num] = out;
      ++num;
      // copy non-delimiters
      stb_tokentable[0] = 1;
      last_nonwhite = out-1;
      if (stripwhite == 2) {
         while (!stb_tokentable[*s]) {
            if (*s != '"') {
               if (!isspace(*s)) last_nonwhite = out;
               *out++ = *s++;
            } else {
               ++s;
               if (*s == '"') {
                  if (!isspace(*s)) last_nonwhite = out;
                  *out++ = *s++; // "" -> ", not start string
               } else {
                  // begin a quoted string
                  while (*s) {
                     if (s[0] == '"') {
                        if (s[1] == '"') { *out++ = *s; s += 2; }
                        else { ++s; break; } // terminating "
                     } else
                        *out++ = *s++;
                  }
                  last_nonwhite = out-1; // all in quotes counts as non-white
               }
            }
         }
      } else {
         while (nested || !stb_tokentable[*s]) {
            if (!isspace(*s)) last_nonwhite = out;
            if (stable[*s]) {
               if (!*s) break;
               if (end ? etable[*s] : nested)
                  --nested;
               else
                  ++nested;
            }
            *out++ = *s++;
         }
      }

      if (stripwhite) // rewind to last non-whitespace char
         out = last_nonwhite+1;
      *out++ = '\0';

      if (*s) ++s; // skip delimiter
   }
   s = (unsigned char *) delimit; while (*s) stb_tokentable[*s++] = 0;
   if (start) {
      s = (unsigned char *) start;         while (*s) stable[*s++] = 1;
      s = (unsigned char *) end;   if (s)  while (*s) stable[*s++] = 1;
      s = (unsigned char *) end;   if (s)  while (*s) etable[*s++] = 1;
   }
   if (count != NULL) *count = num;
   result[num] = 0;
   return result;
}

char **stb_tokens(char *src, char *delimit, int *count)
{
   return stb_tokens_raw(src,delimit,count,0,0,0,0);
}

char **stb_tokens_nested(char *src, char *delimit, int *count, char *nest_in, char *nest_out)
{
   return stb_tokens_raw(src,delimit,count,0,0,nest_in,nest_out);
}

char **stb_tokens_nested_empty(char *src, char *delimit, int *count, char *nest_in, char *nest_out)
{
   return stb_tokens_raw(src,delimit,count,0,1,nest_in,nest_out);
}

char **stb_tokens_allowempty(char *src, char *delimit, int *count)
{
   return stb_tokens_raw(src,delimit,count,0,1,0,0);
}

char **stb_tokens_stripwhite(char *src, char *delimit, int *count)
{
   return stb_tokens_raw(src,delimit,count,1,1,0,0);
}

char **stb_tokens_quoted(char *src, char *delimit, int *count)
{
   return stb_tokens_raw(src,delimit,count,2,1,0,0);
}

char *stb_dupreplace(char *src, char *find, char *replace)
{
   size_t len_find = strlen(find);
   size_t len_replace = strlen(replace);
   int count = 0;

   char *s,*p,*q;

   s = strstr(src, find);
   if (s == NULL) return strdup(src);
   do {
      ++count;
      s = strstr(s + len_find, find);
   } while (s != NULL);

   p = (char *)  malloc(strlen(src) + count * (len_replace - len_find) + 1);
   if (p == NULL) return p;
   q = p;
   s = src;
   for (;;) {
      char *t = strstr(s, find);
      if (t == NULL) {
         strcpy(q,s);
         assert(strlen(p) == strlen(src) + count*(len_replace-len_find));
         return p;
      }
      memcpy(q, s, t-s);
      q += t-s;
      memcpy(q, replace, len_replace);
      q += len_replace;
      s = t + len_find;
   }
}

void stb_replaceinplace(char *src, char *find, char *replace)
{
   size_t len_find = strlen(find);
   size_t len_replace = strlen(replace);
   int count = 0, delta;

   char *s,*p,*q;

   delta = len_replace - len_find;
   assert(delta <= 0);
   if (delta > 0) return;

   p = strstr(src, find);
   if (p == NULL) return;

   s = q = p;
   while (*s) {
      memcpy(q, replace, len_replace);
      p += len_find;
      q += len_replace;
      s = strstr(p, find);
      if (s == NULL) s = p + strlen(p);
      memmove(q, p, s-p);
      q += s-p;
      p = s;
   }
   *q = 0;
}

void stb_fixpath(char *path)
{
   for(; *path; ++path)
      if (*path == '\\')
         *path = '/';
}

static char *stb__splitpath_raw(char *buffer, char *path, int flag)
{
   int len=0,x,y, n = (int) strlen(path), f1,f2;
   char *s = stb_strrchr2(path, '/', '\\');
   char *t = strrchr(path, '.');
   if (s && t && t < s) t = NULL;
   if (s) ++s;

   if (flag == STB_EXT_NO_PERIOD)
      flag |= STB_EXT;

   if (!(flag & (STB_PATH | STB_FILE | STB_EXT))) return NULL;

   f1 = s == NULL ? 0 : s-path; // start of filename
   f2 = t == NULL ? n : t-path; // just past end of filename

   if (flag & STB_PATH) {
      x = 0; if (f1 == 0 && flag == STB_PATH) len=2;
   } else if (flag & STB_FILE) {
      x = f1;
   } else {
      x = f2;
      if (flag & STB_EXT_NO_PERIOD)
         if (buffer[x] == '.')
            ++x;
   }

   if (flag & STB_EXT)
      y = n;
   else if (flag & STB_FILE)
      y = f2;
   else
      y = f1;

   if (buffer == NULL) {
      buffer = (char *) malloc(y-x + len + 1);
      if (!buffer) return NULL;
   }

   if (len) { strcpy(buffer, "./"); return buffer; }
   strncpy(buffer, path+x, y-x);
   buffer[y-x] = 0;
   return buffer;
}

char *stb_splitpath(char *output, char *src, int flag)
{
   return stb__splitpath_raw(output, src, flag);
}

char *stb_splitpathdup(char *src, int flag)
{
   return stb__splitpath_raw(NULL, src, flag);
}

char *stb_replacedir(char *output, char *src, char *dir)
{
   char buffer[4096];
   stb_splitpath(buffer, src, STB_FILE | STB_EXT);
   if (dir)
      sprintf(output, "%s/%s", dir, buffer);
   else
      strcpy(output, buffer);
   return output;
}

char *stb_replaceext(char *output, char *src, char *ext)
{
   char buffer[4096];
   stb_splitpath(buffer, src, STB_PATH | STB_FILE);
   if (ext)
      sprintf(output, "%s.%s", buffer, ext[0] == '.' ? ext+1 : ext);
   else
      strcpy(output, buffer);
   return output;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                   stb_alloc - hierarchical allocator
//
//                                     inspired by http://swapped.cc/halloc
//
//
// When you alloc a given block through stb_alloc, you have these choices:
//
//       1. does it have a parent?
//       2. can it have children?
//       3. can it be freed directly?
//       4. is it transferrable?
//       5. what is its alignment?
//
// Here are interesting combinations of those:
//
//                              children   free    transfer     alignment
//  arena                          Y         Y         N           n/a
//  no-overhead, chunked           N         N         N         normal
//  string pool alloc              N         N         N            1
//  parent-ptr, chunked            Y         N         N         normal
//  low-overhead, unchunked        N         Y         Y         normal
//  general purpose alloc          Y         Y         Y         normal
//
// Unchunked allocations will probably return 16-aligned pointers. If
// we 16-align the results, we have room for 4 pointers. For smaller
// allocations that allow finer alignment, we can reduce the pointers.
//
// The strategy is that given a pointer, assuming it has a header (only
// the no-overhead allocations have no header), we can determine the
// type of the header fields, and the number of them, by stepping backwards
// through memory and looking at the tags in the bottom bits.
//
// Implementation strategy:
//     chunked allocations come from the middle of chunks, and can't
//     be freed. thefore they do not need to be on a sibling chain.
//     they may need child pointers if they have children.
//
// chunked, with-children
//     void *parent;
//
// unchunked, no-children -- reduced storage
//     void *next_sibling;
//     void *prev_sibling_nextp;
//
// unchunked, general
//     void *first_child;
//     void *next_sibling;
//     void *prev_sibling_nextp;
//     void *chunks;
//
// so, if we code each of these fields with different bit patterns
// (actually same one for next/prev/child), then we can identify which
// each one is from the last field.

STB_EXTERN void  stb_free(void *p);
STB_EXTERN void *stb_malloc_global(size_t size);
STB_EXTERN void *stb_malloc(void *context, size_t size);
STB_EXTERN void *stb_malloc_nofree(void *context, size_t size);
STB_EXTERN void *stb_malloc_leaf(void *context, size_t size);
STB_EXTERN void *stb_malloc_raw(void *context, size_t size);
STB_EXTERN void *stb_realloc(void *ptr, size_t newsize);

STB_EXTERN void stb_reassign(void *new_context, void *ptr);
STB_EXTERN void stb_malloc_validate(void *p, void *parent);

extern int stb_alloc_chunk_size ;
extern int stb_alloc_count_free ;
extern int stb_alloc_count_alloc;
extern int stb_alloc_alignment  ;

#ifdef STB_DEFINE

int stb_alloc_chunk_size  = 65536;
int stb_alloc_count_free  = 0;
int stb_alloc_count_alloc = 0;
int stb_alloc_alignment   = -16;

typedef struct stb__chunk
{
   struct stb__chunk *next;
   int                data_left;
   int                alloc;
} stb__chunk;

typedef struct
{
   void *  next;
   void ** prevn;
} stb__nochildren;

typedef struct
{
   void ** prevn;
   void *  child;
   void *  next;
   stb__chunk *chunks;
} stb__alloc;

typedef struct
{
   stb__alloc *parent;
} stb__chunked;

#define STB__PARENT          1
#define STB__CHUNKS          2

typedef enum
{
   STB__alloc      = STB__CHUNKS,
   STB__chunked    = STB__PARENT,
   STB__nochildren = 0,

   STB__chunk_raw  = 4,
} stb__alloc_type;

#define STB__DECODE(x,v)  ((void *) ((char *) (x) - (v)))
#define STB__ENCODE(x,v)  ((void *) ((char *) (x) + (v)))

#define stb__parent(z)       (stb__alloc *) STB__DECODE((z)->parent, STB__PARENT)
#define stb__chunks(z)       (stb__chunk *) STB__DECODE((z)->chunks, STB__CHUNKS)

#define stb__setparent(z,p)  (z)->parent = (stb__alloc *) STB__ENCODE((p), STB__PARENT)
#define stb__setchunks(z,c)  (z)->chunks = (stb__chunk *) STB__ENCODE((c), STB__CHUNKS)

static stb__alloc stb__alloc_global =
{
   NULL,
   NULL,
   NULL,
   (stb__chunk *) STB__ENCODE(NULL, STB__CHUNKS)
};

static stb__alloc_type stb__identify(void *p)
{
   void **q = (void **) p;
   assert(sizeof(int) == sizeof(*q));
   // @TODO: deal with sizeof(pointer) != sizeof(int)
   return (stb__alloc_type) (((int *) q)[-1] & 3);
}

static void *** stb__prevn(void *p)
{
   if (stb__identify(p) == STB__alloc) {
      stb__alloc      *s = (stb__alloc *) p - 1;
      return &s->prevn;
   } else {
      stb__nochildren *s = (stb__nochildren *) p - 1;
      return &s->prevn;
   }
}

void stb_free(void *p)
{
   if (p == NULL) return;

   // count frees so that unit tests can see what's happening
   ++stb_alloc_count_free;

   switch(stb__identify(p)) {
      case STB__chunked:
         // freeing a chunked-block with children does nothing;
         // they only get freed when the parent does
         // surely this is wrong, and it should free them immediately?
         // otherwise how are they getting put on the right chain?
         return;
      case STB__nochildren: {
         stb__nochildren *s = (stb__nochildren *) p - 1;
         // unlink from sibling chain
         *(s->prevn) = s->next;
         if (s->next)
            *stb__prevn(s->next) = s->prevn;
         free(s);
         return;
      }
      case STB__alloc: {
         stb__alloc *s = (stb__alloc *) p - 1;
         stb__chunk *c, *n;
         void *q;

         // unlink from sibling chain, if any
         *(s->prevn) = s->next;
         if (s->next)
            *stb__prevn(s->next) = s->prevn;

         // first free chunks
         c = (stb__chunk *) stb__chunks(s);
         while (c != NULL) {
            n = c->next;
            stb_alloc_count_free += c->alloc;
            free(c);
            c = n;
         }

         // validating
         stb__setchunks(s,NULL);
         s->prevn = NULL;
         s->next = NULL;

         // now free children
         while ((q = s->child) != NULL) {
            stb_free(q);
         }

         // now free self
         free(s);
         return;
      }
      default:
         assert(0); /* NOTREACHED */
   }
}

void stb_malloc_validate(void *p, void *parent)
{
   if (p == NULL) return;

   switch(stb__identify(p)) {
      case STB__chunked:
         // freeing a chunked-block with children does nothing;
         // they only get freed when the parent does
         return;
      case STB__nochildren: {
         stb__nochildren *n = (stb__nochildren *) p - 1;
         if (n->prevn)
            assert(*n->prevn == p);
         if (n->next) {
            assert(*stb__prevn(n->next) == &n->next);
            stb_malloc_validate(n, parent);
         }
         return;
      }
      case STB__alloc: {
         stb__alloc *s = (stb__alloc *) p - 1;

         if (s->prevn)
            assert(*s->prevn == p);

         if (s->child) {
            assert(*stb__prevn(s->child) == &s->child);
            stb_malloc_validate(s->child, p);
         }

         if (s->next) {
            assert(*stb__prevn(s->next) == &s->next);
            stb_malloc_validate(s->next, parent);
         }
         return;
      }
      default:
         assert(0); /* NOTREACHED */
   }
}

static void * stb__try_chunk(stb__chunk *c, int size, int align, int pre_align)
{
   char *memblock = (char *) (c+1), *q;
   int  iq, start_offset;

   // we going to allocate at the end of the chunk, not the start. confusing,
   // but it means we don't need both a 'limit' and a 'cur', just a 'cur'.
   // the block ends at: p + c->data_left
   //   then we move back by size
   start_offset = c->data_left - size;

   // now we need to check the alignment of that
   q = memblock + start_offset;
   iq = (int) q;
   assert(sizeof(q) == sizeof(iq));

   // suppose align = 2
   // then we need to retreat iq far enough that (iq & (2-1)) == 0
   // to get (iq & (align-1)) = 0 requires subtracting (iq & (align-1))

   start_offset -= iq & (align-1);
   assert(((int) (memblock+start_offset) & (align-1)) == 0);

   // now, if that + pre_align works, go for it!
   start_offset -= pre_align;

   if (start_offset >= 0) {
      c->data_left = start_offset;
      return memblock + start_offset;
   }

   return NULL;
}

static void stb__sort_chunks(stb__alloc *src)
{
   stb__chunk *c = stb__chunks(src), *d;
   if (c == NULL) return;
   d = c->next;
   if (d == NULL) return;
   if (c->data_left > d->data_left) return;

   c->next = d->next;
   d->next = c;
   stb__setchunks(src, d);
}

static void * stb__alloc_chunk(stb__alloc *src, int size, int align, int pre_align)
{
   void *p;
   stb__chunk *c = stb__chunks(src);

   if (c && size <= stb_alloc_chunk_size) {

      p = stb__try_chunk(c, size, align, pre_align);
      if (p) { ++c->alloc; return p; }

      // try a second chunk to reduce wastage
      if (c->next) {
         p = stb__try_chunk(c->next, size, align, pre_align);
         if (p) { ++c->alloc; return p; }
   
         // put the bigger chunk first, since the second will get buried
         // the upshot of this is that, until it gets allocated from, chunk #2
         // is always the largest remaining chunk. (could formalize
         // this with a heap!)
         stb__sort_chunks(src);
         c = stb__chunks(src);
      }
   }

   // allocate a new chunk
   {
      stb__chunk *n;

      int chunk_size = stb_alloc_chunk_size;
      // we're going to allocate a new chunk to put this in
      if (size > chunk_size)
         chunk_size = size;

      assert(sizeof(*n) + pre_align <= 16);

      // loop trying to allocate a large enough chunk
      // the loop is because the alignment may cause problems if it's big
      while (1) {
         n = (stb__chunk *) malloc(16 + chunk_size);
         if (n == NULL) return NULL;

         n->data_left = chunk_size - sizeof(*n);

         p = stb__try_chunk(n, size, align, pre_align);
         if (p != NULL) {
            n->next = c;
            stb__setchunks(src, n);

            // if we just used up the whole block immediately,
            // move the following chunk up
            n->alloc = 1;
            if (size == chunk_size)
               stb__sort_chunks(src);

            return p;
         }

         free(n);
         chunk_size += 16+align;
      }
   }
}

static stb__alloc * stb__get_context(void *context)
{
   if (context == NULL) {
      return &stb__alloc_global;
   } else {
      int u = stb__identify(context);
      // if context is chunked, grab parent
      if (u == STB__chunked) {
         stb__chunked *s = (stb__chunked *) context - 1;
         return stb__parent(s);
      } else {
         return (stb__alloc *) context - 1;
      }
   }
}

static void stb__insert_alloc(stb__alloc *src, stb__alloc *s)
{
   s->prevn = &src->child;
   s->next  = src->child;
   src->child = s+1;
   if (s->next)
      *stb__prevn(s->next) = &s->next;
}

static void stb__insert_nochild(stb__alloc *src, stb__nochildren *s)
{
   s->prevn = &src->child;
   s->next  = src->child;
   src->child = s+1;
   if (s->next)
      *stb__prevn(s->next) = &s->next;
}

static void * malloc_base(void *context, size_t size, stb__alloc_type t, int align)
{
   void *p;

   stb__alloc *src = stb__get_context(context);

   if (align <= 0) {
      // compute worst-case C packed alignment
      // e.g. a 24-byte struct is 8-aligned
      int align_proposed = 1 << stb_lowbit8(size);

      if (align_proposed < 0)
         align_proposed = 4;

      if (align_proposed == 0) {
         if (size == 0)
            align_proposed = 1;
         else
            align_proposed = 256;
      }

      // a negative alignment means 'don't align any larger
      // than this'; so -16 means we align 1,2,4,8, or 16

      if (align < 0) {
         if (align_proposed > -align)
            align_proposed = -align;
      }

      align = align_proposed;
   }

   assert(stb_is_pow2(align));

   // don't cause misalignment when allocating nochildren
   if (t == STB__nochildren && align > 8)
      t = STB__alloc;

   switch (t) {
      case STB__alloc: {
         stb__alloc *s = (stb__alloc *) malloc(size + sizeof(*s));
         if (s == NULL) return NULL;
         p = s+1;
         s->child = NULL;
         stb__insert_alloc(src, s);

         stb__setchunks(s,NULL);
         break;
      }

      case STB__nochildren: {
         stb__nochildren *s = (stb__nochildren *) malloc(size + sizeof(*s));
         if (s == NULL) return NULL;
         p = s+1;
         stb__insert_nochild(src, s);
         break;
      }

      case STB__chunk_raw: {
         p = stb__alloc_chunk(src, size, align, 0);
         if (p == NULL) return NULL;
         break;
      }

      case STB__chunked: {
         stb__chunked *s;
         if (align < 4) align = 4;
         s = (stb__chunked *) stb__alloc_chunk(src, size, align, sizeof(*s));
         if (s == NULL) return NULL;
         stb__setparent(s, src);
         p = s+1;
         break;
      }

      default: assert(0); /* NOTREACHED */
   }

   ++stb_alloc_count_alloc;
   return p;
}

void *stb_malloc_global(size_t size)
{
   return malloc_base(NULL, size, STB__alloc, stb_alloc_alignment);
}

void *stb_malloc(void *context, size_t size)
{
   return malloc_base(context, size, STB__alloc, stb_alloc_alignment);
}

void *stb_malloc_nofree(void *context, size_t size)
{
   return malloc_base(context, size, STB__chunked, stb_alloc_alignment);
}

void *stb_malloc_leaf(void *context, size_t size)
{
   return malloc_base(context, size, STB__nochildren, stb_alloc_alignment);
}

void *stb_malloc_raw(void *context, size_t size)
{
   return malloc_base(context, size, STB__chunk_raw, stb_alloc_alignment);
}

char *stb_malloc_string(void *context, size_t size)
{
   return (char *) malloc_base(context, size, STB__chunk_raw, 1);
}

void *stb_realloc(void *ptr, size_t newsize)
{
   stb__alloc_type t;

   if (ptr == NULL) return stb_malloc(NULL, newsize);
   if (newsize == 0) { stb_free(ptr); return NULL; }
   
   t = stb__identify(ptr);
   assert(t == STB__alloc || t == STB__nochildren);

   if (t == STB__alloc) {
      stb__alloc *s = (stb__alloc *) ptr - 1;

      s = (stb__alloc *) realloc(s, newsize + sizeof(*s));
      if (s == NULL) return NULL;

      ptr = s+1;

      // update pointers
      (*s->prevn) = ptr;
      if (s->next)
         *stb__prevn(s->next) = &s->next;

      if (s->child)
         *stb__prevn(s->child) = &s->child;

      return ptr;
   } else {
      stb__nochildren *s = (stb__nochildren *) ptr - 1;

      s = (stb__nochildren *) realloc(ptr, newsize + sizeof(s));
      if (s == NULL) return NULL;

      // update pointers
      (*s->prevn) = s+1;
      if (s->next)
         *stb__prevn(s->next) = &s->next;

      return s+1;
   }
}

void *stb_realloc_c(void *context, void *ptr, size_t newsize)
{
   if (ptr == NULL) return stb_malloc(context, newsize);
   if (newsize == 0) { stb_free(ptr); return NULL; }
   // @TODO: verify you haven't changed contexts
   return stb_realloc(ptr, newsize);
}

void stb_reassign(void *new_context, void *ptr)
{
   stb__alloc *src = stb__get_context(new_context);

   stb__alloc_type t = stb__identify(ptr);
   assert(t == STB__alloc || t == STB__nochildren);

   if (t == STB__alloc) {
      stb__alloc *s = (stb__alloc *) ptr - 1;

      // unlink from old
      *(s->prevn) = s->next;
      if (s->next)
         *stb__prevn(s->next) = s->prevn;

      stb__insert_alloc(src, s);
   } else {
      stb__nochildren *s = (stb__nochildren *) ptr - 1;

      // unlink from old
      *(s->prevn) = s->next;
      if (s->next)
         *stb__prevn(s->next) = s->prevn;

      stb__insert_nochild(src, s);
   }
}

#endif


//////////////////////////////////////////////////////////////////////////////
//
//                                stb_arr
//
//  An stb_arr is directly useable as a pointer (use the actual type in your
//  definition), but when it resizes, it returns a new pointer and you can't
//  use the old one, so you have to be careful to copy-in-out as necessary.
//
//  Use a NULL pointer as a 0-length array.
//
//     float *my_array = NULL, *temp;
//
//     // add elements on the end one at a time
//     stb_arr_push(my_array, 0.0f);
//     stb_arr_push(my_array, 1.0f);
//     stb_arr_push(my_array, 2.0f);
//
//     assert(my_array[1] == 2.0f);
//
//     // add an uninitialized element at the end, then assign it
//     *stb_arr_add(my_array) = 3.0f;
//
//     // add three uninitialized elements at the end
//     temp = stb_arr_addn(my_array,3);
//     temp[0] = 4.0f;
//     temp[1] = 5.0f;
//     temp[2] = 6.0f;
//
//     assert(my_array[5] == 5.0f);
//
//     // remove the last one
//     stb_arr_pop(my_array);
//
//     assert(stb_arr_len(my_array) == 6);


#ifdef STB_MALLOC_WRAPPER
  #define STB__PARAMS    , char *file, int line
  #define STB__ARGS      ,       file,     line
#else
  #define STB__PARAMS
  #define STB__ARGS
#endif

// calling this function allocates an empty stb_arr attached to p
// (whereas NULL isn't attached to anything)
STB_EXTERN void stb_arr_malloc(void **target, void *context);

// call this function with a non-NULL value to have all successive
// stbs that are created be attached to the associated parent. Note
// that once a given stb_arr is non-empty, it stays attached to its
// current parent, even if you call this function again.
// it turns the previous value, so you can restore it
STB_EXTERN void* stb_arr_malloc_parent(void *p);

#ifdef STB_PERSONAL
#define arrpush    stb_arr_push
#define arrlen     stb_arr_len
#define arrpop     stb_arr_pop
#define arrlast    stb_arr_last
#define arrlastn   stb_arr_lastn
#define arrfor     stb_arr_for
#define arraddn    stb_arr_addn
#define arrsetsize stb_arr_setsize
#endif

// simple functions written on top of other functions
#define stb_arr_empty(a)       (  stb_arr_len(a) == 0 )
#define stb_arr_add(a)         (  stb_arr_addn((a),1) )
#define stb_arr_push(a,v)      ( *stb_arr_add(a)=(v)  )


#ifdef __cplusplus
#define STB__CAST(x)           (x).cast()
#else
#define STB__CAST(x)           ((stb__arr *) (x))
#endif

typedef struct
{
   int len, limit;
   int stb_malloc;
   unsigned int signature;
} stb__arr;

#define stb_arr_signature      0x51bada7b  // ends with 0123 in decimal

// access the header block stored before the data
#define stb_arrhead(a)         /*lint --e(826)*/ (STB__CAST(a) - 1)
#define stb_arrhead2(a)        /*lint --e(826)*/ (((stb__arr *) (a)) - 1)

#ifdef STB_DEBUG
#define stb_arr_check(a)       assert(!a || stb_arrhead(a)->signature == stb_arr_signature)
#define stb_arr_check2(a)      assert(!a || stb_arrhead2(a)->signature == stb_arr_signature)
#else
#define stb_arr_check(a)       0
#define stb_arr_check2(a)      0
#endif

// ARRAY LENGTH

// get the array length; special case if pointer is NULL
#define stb_arr_len(a)         (STB__CAST(a) ? stb_arrhead(a)->len : 0)
#define stb_arr_len2(a)        ((stb__arr *) (a) ? stb_arrhead2(a)->len : 0)
#define stb_arr_lastn(a)       (stb_arr_len(a)-1)

// check whether a given index is valid -- tests 0 <= i < stb_arr_len(a) 
#define stb_arr_valid(a,i)     (STB__CAST(a) ? (unsigned int) (i) < stb_arrhead(a)->len : 0)

// change the array length so is is exactly N entries long, creating
// uninitialized entries as needed
#define stb_arr_setlen(a,n)  \
            ((a)=stb__arr_setlen((a), sizeof(a[0]), (n)))

// change the array length so that N is a valid index (that is, so
// it is at least N entries long), creating uninitialized entries as needed
#define stb_arr_makevalid(a,n)  \
            (stb_arr_len(a) < (n)+1 ? stb_arr_setlen((a),(n)+1) : (a))

// remove the last element of the array, returning it
#define stb_arr_pop(a)         ((stb_arr_check(a), (a))[--stb_arrhead(a)->len])

// access the last element in the array
#define stb_arr_last(a)        ((stb_arr_check(a), (a))[stb_arr_len(a)-1])

// is iterator at end of list?
#define stb_arr_end(a,i)       ((i) >= &(a)[stb_arr_len(a)])

// (internal) change the allocated length of the array
#define stb_arr__grow(a,n)     (stb_arr_check(a), stb_arrhead(a)->len += (n))

// add N new unitialized elements to the end of the array
#define stb_arr__addn(a,n)     /*lint --e(826)*/ \
                               ((stb_arr_len(a)+(n) > stb_arrcurmax(a))      \
                                 ? ((a)=stb__arr_addlen((a),sizeof(*a),(n))) \
                                 : ((stb_arr__grow(a,n), (a))))

// add N new unitialized elements to the end of the array, and return
// a pointer to the first new one
#define stb_arr_addn(a,n)      (stb_arr__addn(a,n),(a)+stb_arr_len(a)-(n))

// ARRAY STORAGE

// get the array maximum storage; special case if NULL
#define stb_arrcurmax(a)       (STB__CAST(a) ? stb_arrhead(a)->limit : 0)
#define stb_arrcurmax2(a)      ((a) ? stb_arrhead2(a)->limit : 0)

// set the maxlength of the array to n in anticipation of further growth
#define stb_arr_setsize(a,n)   (stb_arr_check(a), (a)=stb__arr_setsize(a,sizeof((a)[0]),n))

// make sure maxlength is large enough for at least N new allocations
#define stb_arr_atleast(a,n)   (stb_arr_len(a)+(n) > stb_arrcurmax(a)      \
                                 ? stb_arr_setsize((a), (n)) : 0)

// make a copy of a given array (copies contents via 'memcpy'!)
#define stb_arr_copy(a)        stb__arr_copy(a, sizeof((a)[0]))

// compute the storage needed to store all the elements of the array
#define stb_arr_storage(a)     (stb_arr_len(a) * sizeof((a)[0]))

#define stb_arr_for(v,arr)     for((v)=(arr); (v) < (arr)+stb_arr_len(arr); ++(v))

// IMPLEMENTATION

STB_EXTERN void *stb_arr_free_(void *p);
STB_EXTERN void *stb__arr_copy_(void *p, int elem_size);
STB_EXTERN void *stb__arr_setsize_(void *p, int size, int limit  STB__PARAMS);
STB_EXTERN void *stb__arr_setlen_(void *p, int size, int newlen  STB__PARAMS);
STB_EXTERN void *stb__arr_addlen_(void *p, int size, int addlen  STB__PARAMS);


#ifdef __cplusplus

// in C++, wrap all the above function calls in a template to avoid type error

#define STB__ARR(t)          stb_arr<t >

template<class T> struct stb_arr
{
   T *ptr;

   // constructors
   stb_arr<T>(void *p) { ptr = (T *) p; }
   stb_arr<T>() { ptr = NULL; }

   // operator overloads
   operator T*() { return ptr; }
   stb__arr * cast() { return (stb__arr *) ptr; }
   T& operator[] (unsigned i) { return ptr[i]; }
   T& operator[] (int i     ) { return ptr[i]; }

   stb_arr<T> stb_arr_free__()                { return stb_arr<T>(stb_arr_free_(ptr)); }

   stb_arr<T> stb__arr_copy__(int elem_size) { return stb_arr<T>(stb__arr_copy_(ptr,elem_size)); }
   stb_arr<T> stb__arr_setsize__(int size, int limit        STB__PARAMS)
   { return stb_arr<T>(stb__arr_setsize_(ptr, size, limit  STB__ARGS)); }
   stb_arr<T> stb__arr_setlen__(int size, int newlen        STB__PARAMS)
   { return stb_arr<T>(stb__arr_setlen_(ptr, size, newlen  STB__ARGS)); }
   stb_arr<T> stb__arr_addlen__(int size, int addlen        STB__PARAMS)
   { return stb_arr<T>(stb__arr_addlen_(ptr, size, addlen  STB__ARGS)); }
};

#define stb_arr_free(p)           (p).stb_arr_free__()
#define stb__arr_copy(p,e)        (p).stb__arr_copy__(e)

#ifndef STB_MALLOC_WRAPPER
  #define stb__arr_setsize(p,s,n)   (p).stb__arr_setsize__(s,n)
  #define stb__arr_setlen(p,s,n)    (p).stb__arr_setlen__(s,n)
  #define stb__arr_addlen(p,s,n)    (p).stb__arr_addlen__(s,n)
#else
  #define stb__arr_setsize(p,s,n)   (p).stb__arr_setsize__(s,n,__FILE__,__LINE__)
  #define stb__arr_setlen(p,s,n)    (p).stb__arr_setlen__(s,n,__FILE__,__LINE__)
  #define stb__arr_addlen(p,s,n)    (p).stb__arr_addlen__(s,n,__FILE__,__LINE__)
#endif

#else

#define STB__ARR(t)          t *

#define stb_arr_free               stb_arr_free_
#define stb__arr_copy              stb__arr_copy_

#ifndef STB_MALLOC_WRAPPER
  #define stb__arr_setsize         stb__arr_setsize_
  #define stb__arr_setlen          stb__arr_setlen_
  #define stb__arr_addlen          stb__arr_addlen_
#else
  #define stb__arr_addlen(p,s,n)   stb__arr_addlen_(p,s,n,__FILE__,__LINE__)
  #define stb__arr_setlen(p,s,n)   stb__arr_setlen_(p,s,n,__FILE__,__LINE__)
  #define stb__arr_setsize(p,s,n)  stb__arr_setsize_(p,s,n,__FILE__,__LINE__)
#endif

#endif

#define stb_arr(x)           STB__ARR(x)

#ifdef STB_DEFINE
static void *stb__arr_context;

void *stb_arr_malloc_parent(void *p)
{
   void *q = stb__arr_context;
   stb__arr_context = p;
   return q;
}

void stb_arr_malloc(void **target, void *context)
{
   stb__arr *q = (stb__arr *) stb_malloc(context, sizeof(*q));
   q->len = q->limit = 0;
   q->stb_malloc = 1;
   q->signature = stb_arr_signature;
   *target = (void *) (q+1);
}

static void * stb__arr_malloc(int size)
{
   if (stb__arr_context)
      return stb_malloc(stb__arr_context, size);
   return malloc(size);
}

void * stb__arr_copy_(void *p, int elem_size)
{
   stb__arr *q;
   if (p == NULL) return p;
   q = (stb__arr *) stb__arr_malloc(sizeof(*q) + elem_size * stb_arrhead2(p)->limit);
   stb_arr_check2(p);
   memcpy(q, stb_arrhead2(p), sizeof(*q) + elem_size * stb_arrhead2(p)->len);
   q->stb_malloc = !!stb__arr_context;
   return q+1;
}

void * stb_arr_free_(void *p)
{
   stb_arr_check2(p);
   if (p) {
      stb__arr *q = stb_arrhead2(p);
      if (q->stb_malloc)
         stb_free(q);
      else
         free(q);
   }
   return NULL;
}

static void *stb__arrsize_(void *p, int size, int limit, int len  STB__PARAMS)
{
   stb__arr *a;
   stb_arr_check2(p);
   if (p == NULL) {
      if (len == 0 && size == 0) return p;
      a = (stb__arr *) stb__arr_malloc(sizeof(*a) + size*limit);
      a->limit = limit;
      a->len   = len;
      a->stb_malloc = !!stb__arr_context;
      a->signature = stb_arr_signature;
   } else {
      a = stb_arrhead2(p);
      a->len = len;
      if (a->limit < limit) {
         void *p;
         if (a->limit >= 4 && limit < a->limit * 2)
            limit = a->limit * 2;
         if (a->stb_malloc)
            p = stb_realloc(a, sizeof(*a) + limit*size);
         else
            #ifdef STB_MALLOC_WRAPPER
            p = stb__realloc(a, sizeof(*a) + limit*size, file, line);
            #else
            p = realloc(a, sizeof(*a) + limit*size);
            #endif
         if (p) {
            a = (stb__arr *) p;
            a->limit = limit;
         } else {
            // throw an error!
         }
      }
   }
   a->len   = stb_min(a->len, a->limit);
   return a+1;
}

void *stb__arr_setsize_(void *p, int size, int limit  STB__PARAMS)
{
   stb_arr_check2(p);
   return stb__arrsize_(p, size, limit, stb_arr_len2(p)  STB__ARGS);
}

void *stb__arr_setlen_(void *p, int size, int newlen  STB__PARAMS)
{
   stb_arr_check2(p);
   if (stb_arrcurmax2(p) < newlen || p == NULL) {
      return stb__arrsize_(p, size, newlen, newlen  STB__ARGS);
   } else {
      stb_arrhead2(p)->len = newlen;
      return p;
   }
}

void *stb__arr_addlen_(void *p, int size, int addlen  STB__PARAMS)
{
   return stb__arr_setlen_(p, size, stb_arr_len2(p) + addlen  STB__ARGS);
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                               Hashing
//
//      typical use for this is to make a power-of-two hash table.
//
//      let N = size of table (2^n)
//      let H = stb_hash(str)
//      let S = stb_rehash(H) | 1
//
//      then hash probe sequence P(i) for i=0..N-1
//         P(i) = (H + S*i) & (N-1)
//
//      the idea is that H has 32 bits of hash information, but the
//      table has only, say, 2^20 entries so only uses 20 of the bits.
//      then by rehashing the original H we get 2^12 different probe
//      sequences for a given initial probe location. (So it's optimal
//      for 64K tables and its optimality decreases past that.)
//
//      ok, so I've added something that generates _two separate_
//      32-bit hashes simultaneously which should scale better to
//      very large tables.


STB_EXTERN unsigned int stb_hash(char *str);
STB_EXTERN unsigned int stb_hashptr(void *p);
STB_EXTERN unsigned int stb_hashlen(char *str, int len);
STB_EXTERN unsigned int stb_rehash_improved(unsigned int v);
STB_EXTERN unsigned int stb_hash_fast(void *p, int len);
STB_EXTERN unsigned int stb_hash2(char *str, unsigned int *hash2_ptr);

#define stb_rehash(x)  ((x) + ((x) >> 6) + ((x) >> 19))

#ifdef STB_DEFINE
unsigned int stb_hash(char *str)
{
   unsigned int hash = 0;
   while (*str)
      hash = (hash << 7) + (hash >> 25) + *str++;
   return hash + (hash >> 16);
}

unsigned int stb_hashlen(char *str, int len)
{
   unsigned int hash = 0;
   while (len-- > 0 && *str)
      hash = (hash << 7) + (hash >> 25) + *str++;
   return hash + (hash >> 16);
}

unsigned int stb_hashptr(void *p)
{
   unsigned int x = (unsigned int) p;

   // typically lacking in low bits and high bits
   x = stb_rehash(x);
   x += x << 16;

   // pearson's shuffle
   x ^= x << 3;
   x += x >> 5;
   x ^= x << 2;
   x += x >> 15;
   x ^= x << 10;
   return stb_rehash(x);
}

unsigned int stb_rehash_improved(unsigned int v)
{
   return stb_hashptr((void *) v);
}

unsigned int stb_hash2(char *str, unsigned int *hash2_ptr)
{
   unsigned int hash1 = 0x3141592c;
   unsigned int hash2 = 0x77f044ed;
   while (*str) {
      hash1 = (hash1 << 7) + (hash1 >> 25) + *str;
      hash2 = (hash2 << 11) + (hash2 >> 21) + *str;
      ++str;
   }
   *hash2_ptr = hash2 + (hash1 >> 16);
   return       hash1 + (hash2 >> 16);
}

// Paul Hsieh hash
#define stb__get16_slow(p) ((p)[0] + ((p)[1] << 8))
#if defined(_MSC_VER)
   #define stb__get16(p) (*((unsigned short *) (p)))
#else
   #define stb__get16(p) stb__get16_slow(p)
#endif

unsigned int stb_hash_fast(void *p, int len)
{
   unsigned char *q = (unsigned char *) p;
   unsigned int hash = len;

   if (len <= 0 || q == NULL) return 0;

   /* Main loop */
   if (((int) q & 1) == 0) {
      for (;len > 3; len -= 4) {
         unsigned int val;
         hash +=  stb__get16(q);
         val   = (stb__get16(q+2) << 11);
         hash  = (hash << 16) ^ hash ^ val;
         q    += 4;
         hash += hash >> 11;
      }
   } else {
      for (;len > 3; len -= 4) {
         unsigned int val;
         hash +=  stb__get16_slow(q);
         val   = (stb__get16_slow(q+2) << 11);
         hash  = (hash << 16) ^ hash ^ val;
         q    += 4;
         hash += hash >> 11;
      }
   }

   /* Handle end cases */
   switch (len) {
      case 3: hash += stb__get16_slow(q);
              hash ^= hash << 16;
              hash ^= q[2] << 18;
              hash += hash >> 11;
              break;
      case 2: hash += stb__get16_slow(q);
              hash ^= hash << 11;
              hash += hash >> 17;
              break;
      case 1: hash += q[0];
              hash ^= hash << 10;
              hash += hash >> 1;
              break;
      case 0: break;
   }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                     Perfect hashing for ints/pointers
//
//   This is mainly useful for making faster pointer-indexed tables
//   that don't change frequently. E.g. for stb_ischar().
//

typedef struct
{
   stb_uint32  addend;
   stb_uint    multiplicand;
   stb_uint    b_mask;
   stb_uint8   small_bmap[16];
   stb_uint16  *large_bmap;

   stb_uint table_mask;
   stb_uint32 *table;
} stb_perfect;

STB_EXTERN int stb_perfect_create(stb_perfect *,unsigned int*,int n);
STB_EXTERN void stb_perfect_destroy(stb_perfect *);
STB_EXTERN int stb_perfect_hash(stb_perfect *, unsigned int x);
extern int stb_perfect_hash_max_failures;

#ifdef STB_DEFINE

int stb_perfect_hash_max_failures;

int stb_perfect_hash(stb_perfect *p, unsigned int x)
{
   stb_uint m = x * p->multiplicand;
   stb_uint y = x >> 16;
   stb_uint bv = (m >> 24) + y;
   stb_uint av = (m + y) >> 12;
   if (p->table == NULL) return -1;  // uninitialized table fails
   bv &= p->b_mask;
   av &= p->table_mask;
   if (p->large_bmap)
      av ^= p->large_bmap[bv];
   else
      av ^= p->small_bmap[bv];
   return p->table[av] == x ? av : -1;
}

static void stb__perfect_prehash(stb_perfect *p, stb_uint x, stb_uint16 *a, stb_uint16 *b)
{
   stb_uint m = x * p->multiplicand;
   stb_uint y = x >> 16;
   stb_uint bv = (m >> 24) + y;
   stb_uint av = (m + y) >> 12;
   bv &= p->b_mask;
   av &= p->table_mask;
   *b = bv;
   *a = av;
}

static unsigned long stb__perfect_rand(void)
{
   static unsigned long stb__rand;
   stb__rand = stb__rand * 2147001325 + 715136305;
   return 0x31415926 ^ ((stb__rand >> 16) + (stb__rand << 16));
}

typedef struct {
   unsigned short count;
   unsigned short b;
   unsigned short map;
   unsigned short *entries;
} stb__slot;

static int stb__slot_compare(const void *p, const void *q)
{
   stb__slot *a = (stb__slot *) p;
   stb__slot *b = (stb__slot *) q;
   return a->count > b->count ? -1 : a->count < b->count;  // sort large to small
}

int stb_perfect_create(stb_perfect *p, unsigned int *v, int n)
{
   unsigned int buffer1[64], buffer2[64], buffer3[64], buffer4[64], buffer5[32];
   unsigned short *as = (unsigned short *) stb_temp(buffer1, sizeof(*v)*n);
   unsigned short *bs = (unsigned short *) stb_temp(buffer2, sizeof(*v)*n);
   unsigned short *entries = (unsigned short *) stb_temp(buffer4, sizeof(*entries) * n);
   int size = 1 << stb_log2_ceil(n), bsize=8;
   int failure = 0,i,j,k;

   assert(n <= 32768);
   p->large_bmap = NULL;

   for(;;) {
      stb__slot *bcount = (stb__slot *) stb_temp(buffer3, sizeof(*bcount) * bsize);
      unsigned short *bloc = (unsigned short *) stb_temp(buffer5, sizeof(*bloc) * bsize);
      unsigned short *e;
      int bad=0;

      p->addend = stb__perfect_rand();
      p->multiplicand = stb__perfect_rand() | 1;
      p->table_mask = size-1;
      p->b_mask = bsize-1;
      p->table = (stb_uint32 *) malloc(size * sizeof(*p->table));

      for (i=0; i < bsize; ++i) {
         bcount[i].b     = i;
         bcount[i].count = 0;
         bcount[i].map   = 0;
      }
      for (i=0; i < n; ++i) {
         stb__perfect_prehash(p, v[i], as+i, bs+i);
         ++bcount[bs[i]].count;
      }
      qsort(bcount, bsize, sizeof(*bcount), stb__slot_compare);
      e = entries; // now setup up their entries index
      for (i=0; i < bsize; ++i) {
         bcount[i].entries = e;
         e += bcount[i].count;
         bcount[i].count = 0;
         bloc[bcount[i].b] = i;
      }
      // now fill them out
      for (i=0; i < n; ++i) {
         int b = bs[i];
         int w = bloc[b];
         bcount[w].entries[bcount[w].count++] = i;
      }
      stb_tempfree(buffer5,bloc);
      // verify
      for (i=0; i < bsize; ++i)
         for (j=0; j < bcount[i].count; ++j)
            assert(bs[bcount[i].entries[j]] == bcount[i].b);
      memset(p->table, 0, size*sizeof(*p->table));

      // check if any b has duplicate a
      for (i=0; i < bsize; ++i) {
         if (bcount[i].count > 1) {
            for (j=0; j < bcount[i].count; ++j) {
               if (p->table[as[bcount[i].entries[j]]])
                  bad = 1;
               p->table[as[bcount[i].entries[j]]] = 1;
            }
            for (j=0; j < bcount[i].count; ++j) {
               p->table[as[bcount[i].entries[j]]] = 0;
            }
            if (bad) break;
         }
      }

      if (!bad) {
         // go through the bs and populate the table, first fit
         for (i=0; i < bsize; ++i) {
            if (bcount[i].count) {
               // go through the candidate table[b] values
               for (j=0; j < size; ++j) {
                  // go through the a values and see if they fit
                  for (k=0; k < bcount[i].count; ++k) {
                     int a = as[bcount[i].entries[k]];
                     if (p->table[(a^j)&p->table_mask]) {
                        break; // fails
                     }
                  }
                  // if succeeded, stu__accept
                  if (k == bcount[i].count) {
                     bcount[i].map = j;
                     for (k=0; k < bcount[i].count; ++k) {
                        int a = as[bcount[i].entries[k]];
                        p->table[(a^j)&p->table_mask] = 1;
                     }
                     break;
                  }
               }
               if (j == size)
                  break; // no match for i'th entry, so break out in failure
            }
         }
         if (i == bsize) {
            // success... fill out map
            if (bsize <= 16 && size <= 256) {
               p->large_bmap = NULL;
               for (i=0; i < bsize; ++i)
                  p->small_bmap[bcount[i].b] = (stb_uint8) bcount[i].map;
            } else {
               p->large_bmap = (unsigned short *) malloc(sizeof(*p->large_bmap) * bsize);
               for (i=0; i < bsize; ++i)
                  p->large_bmap[bcount[i].b] = bcount[i].map;
            }

            // initialize table to v[0], so empty slots will fail
            for (i=0; i < size; ++i)
               p->table[i] = v[0];

            for (i=0; i < n; ++i)
               if (p->large_bmap)
                  p->table[as[i] ^ p->large_bmap[bs[i]]] = v[i];
               else
                  p->table[as[i] ^ p->small_bmap[bs[i]]] = v[i];

            // and now validate that none of them collided
            for (i=0; i < n; ++i)
               assert(stb_perfect_hash(p, v[i]) >= 0);

            stb_tempfree(buffer3, bcount);
            break;
         }
      }
      free(p->table);
      p->table = NULL;
      stb_tempfree(buffer3, bcount);

      ++failure;
      if (failure >= 4 && bsize < size) bsize *= 2;
      if (failure >= 8 && (failure & 3) == 0 && size < 4*n) {
         size *= 2;
         bsize *= 2;
      }
      if (failure == 6) {
         // make sure the input data is unique, so we don't infinite loop
         unsigned int *data = (unsigned int *) stb_temp(buffer3, n * sizeof(*data));
         memcpy(data, v, sizeof(*data) * n);
         qsort(data, n, sizeof(*data), stb_intcmp);
         for (i=1; i < n; ++i) {
            if (data[i] == data[i-1])
               size = 0; // size is return value, so 0 it
         }
         stb_tempfree(buffer3, data);
         if (!size) break;
      }
   }

   if (failure > stb_perfect_hash_max_failures)
      stb_perfect_hash_max_failures = failure;

   stb_tempfree(buffer1, as);
   stb_tempfree(buffer2, bs);
   stb_tempfree(buffer4, entries);

   return size;
}

void stb_perfect_destroy(stb_perfect *p)
{
   if (p->large_bmap) free(p->large_bmap);
   if (p->table     ) free(p->table);
   p->large_bmap = NULL;
   p->table      = NULL;
   p->b_mask     = 0;
   p->table_mask = 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                     Perfect hash clients

STB_EXTERN int    stb_ischar(char s, char *set);

#ifdef STB_DEFINE

int stb_ischar(char c, char *set)
{
   static unsigned char bit[8] = { 1,2,4,8,16,32,64,128 };
   static stb_perfect p;
   static unsigned char (*tables)[256];
   static STB__ARR(char *) sets = NULL;

   int z = stb_perfect_hash(&p, (int) set);
   if (z < 0) {
      int i,k,n,j,f;
      // special code that means free all existing data
      if (set == NULL) {
         sets = stb_arr_free(sets);
         free(tables);
         tables = NULL;
         stb_perfect_destroy(&p);
         return 0;
      }
      stb_arr_push(sets, set);
      stb_perfect_destroy(&p);
      n = stb_perfect_create(&p, (unsigned int *) (char **) sets, stb_arr_len(sets));
      assert(n != 0);
      k = (n+7) >> 3;
      tables = (unsigned char (*)[256]) realloc(tables, sizeof(*tables) * k);
      memset(tables, 0, sizeof(*tables) * k);
      for (i=0; i < stb_arr_len(sets); ++i) {
         k = stb_perfect_hash(&p, (int) sets[i]);
         assert(k >= 0);
         n = k >> 3;
         f = bit[k&7];
         for (j=0; !j || sets[i][j]; ++j) {
            tables[n][(unsigned char) sets[i][j]] |= f;
         }
      }
      z = stb_perfect_hash(&p, (int) set);
   }
   return tables[z >> 3][(unsigned char) c] & bit[z & 7];
}

#endif

//////////////////////////////////////////////////////////////////////////////
//
//                     Instantiated data structures
//
// This is an attempt to implement a templated data structure.
//
// Hash table: call stb_define_hash(TYPE,N,KEY,K1,K2,HASH,VALUE)
//     TYPE     -- will define a structure type containing the hash table
//     N        -- the name, will prefix functions named:
//                        N create
//                        N destroy
//                        N get
//                        N set, N add, N update,
//                        N remove
//     KEY      -- the type of the key. 'x == y' must be valid
//       K1,K2  -- keys never used by the app, used as flags in the hashtable
//       HASH   -- a piece of code ending with 'return' that hashes key 'k'
//     VALUE    -- the type of the value. 'x = y' must be valid
//
//  Note that stb_define_hash_base can be used to define more sophisticated
//  hash tables, e.g. those that make copies of the key or use special
//  comparisons (e.g. strcmp).

#define STB_(prefix,name)     stb__##prefix##name
#define STB__(prefix,name)    prefix##name
#define STB__use(x)           x
#define STB__skip(x)

#define stb_declare_hash(PREFIX,TYPE,N,KEY,VALUE) \
   typedef struct st_##TYPE TYPE;\
   PREFIX int STB__(N, init)(TYPE *h, int count);\
   PREFIX int STB__(N, memory_usage)(TYPE *h);\
   PREFIX TYPE * STB__(N, create)(void);\
   PREFIX TYPE * STB__(N, copy)(TYPE *h);\
   PREFIX void STB__(N, destroy)(TYPE *h);\
   PREFIX int STB__(N,get_flag)(TYPE *a, KEY k, VALUE *v);\
   PREFIX VALUE STB__(N,get)(TYPE *a, KEY k);\
   PREFIX int STB__(N, set)(TYPE *a, KEY k, VALUE v);\
   PREFIX int STB__(N, add)(TYPE *a, KEY k, VALUE v);\
   PREFIX int STB__(N, update)(TYPE*a,KEY k,VALUE v);\
   PREFIX int STB__(N, remove)(TYPE *a, KEY k, VALUE *v);

#define STB_nocopy(x)        (x)
#define STB_nodelete(x)      0
#define STB_nofields         
#define STB_nonullvalue(x)
#define STB_nullvalue(x)     x
#define STB_safecompare(x)   x
#define STB_nosafe(x)
#define STB_hasvalue(x)      x
#define STB_novalue(x)

#ifdef __GNUC__
#define STB__nogcc(x)
#else
#define STB__nogcc(x)  x
#endif

#define stb_define_hash_base(TYPE,FIELDS,N,LOAD_FACTOR,                       \
                             KEY,EMPTY,DEL,COPY,DISPOSE,SAFE,COMPARE,HASH,    \
                             VALUE,HASVNULL,VNULL)                            \
                                                                              \
typedef struct                                                                \
{                                                                             \
   KEY   k;                                                                   \
   VALUE v;                                                                   \
} STB_(N,_hashpair);                                                          \
                                                                              \
STB__nogcc( typedef struct st_##TYPE TYPE;  )                                 \
struct st_##TYPE {                                                            \
   FIELDS                                                                     \
   STB_(N,_hashpair) *table;                                                  \
   unsigned int mask;                                                         \
   int count, limit;                                                          \
   int deleted;                                                               \
                                                                              \
   int delete_threshhold;                                                     \
   int grow_threshhold;                                                       \
   int shrink_threshhold;                                                     \
   unsigned char alloced, has_empty, has_del;                                 \
   VALUE ev; VALUE dv;                                                        \
};                                                                            \
                                                                              \
static unsigned int STB_(N, hash)(KEY k)                                      \
{                                                                             \
   HASH                                                                       \
}                                                                             \
                                                                              \
int STB__(N, init)(TYPE *h, int count)                                        \
{                                                                             \
   int i;                                                                     \
   if (count < 4) count = 4;                                                  \
   h->limit = count;                                                          \
   h->count = 0;                                                              \
   h->mask  = count-1;                                                        \
   h->deleted = 0;                                                            \
   h->grow_threshhold = (int) (count * LOAD_FACTOR);                          \
   h->has_empty = h->has_del = 0;                                             \
   h->alloced = 0;                                                            \
   if (count <= 64)                                                           \
      h->shrink_threshhold = 0;                                               \
   else                                                                       \
      h->shrink_threshhold = (int) (count * (LOAD_FACTOR/2.25));              \
   h->delete_threshhold = (int) (count * (1-LOAD_FACTOR)/2);                  \
   h->table = (STB_(N,_hashpair)*) malloc(sizeof(h->table[0]) * count);       \
   if (h->table == NULL) return 0;                                            \
   /* ideally this gets turned into a memset32 automatically */               \
   for (i=0; i < count; ++i)                                                  \
      h->table[i].k = EMPTY;                                                  \
   return 1;                                                                  \
}                                                                             \
                                                                              \
int STB__(N, memory_usage)(TYPE *h)                                           \
{                                                                             \
   return sizeof(*h) + h->limit * sizeof(h->table[0]);                        \
}                                                                             \
                                                                              \
TYPE * STB__(N, create)(void)                                                 \
{                                                                             \
   TYPE *h = (TYPE *) malloc(sizeof(*h));                                     \
   if (h) {                                                                   \
      if (STB__(N, init)(h, 16))                                              \
         h->alloced = 1;                                                      \
      else { free(h); h=NULL; }                                               \
   }                                                                          \
   return h;                                                                  \
}                                                                             \
                                                                              \
void STB__(N, destroy)(TYPE *a)                                               \
{                                                                             \
   int i;                                                                     \
   for (i=0; i < a->limit; ++i)                                               \
      if (a->table[i].k != EMPTY && a->table[i].k != DEL)                     \
         DISPOSE(a->table[i].k);                                              \
   free(a->table);                                                            \
   if (a->alloced)                                                            \
      free(a);                                                                \
}                                                                             \
                                                                              \
static void STB_(N, rehash)(TYPE *a, int count);                              \
                                                                              \
int STB__(N,get_flag)(TYPE *a, KEY k, VALUE *v)                               \
{                                                                             \
   unsigned int h = STB_(N, hash)(k);                                         \
   unsigned int n = h & a->mask, s;                                           \
   if (k == EMPTY) { if (a->has_empty) *v = a->ev; return a->has_empty; }     \
   if (k == DEL  ) { if (a->has_del  ) *v = a->dv; return a->has_del;   }     \
   if (a->table[n].k==EMPTY) return 0;                                        \
   SAFE(if (a->table[n].k != DEL))                                            \
   if (COMPARE(a->table[n].k,k)) { *v = a->table[n].v; return 1; }            \
   s = stb_rehash(h) | 1;                                                     \
   for(;;) {                                                                  \
      n = (n + s) & a->mask;                                                  \
      if (a->table[n].k==EMPTY) return 0;                                     \
      SAFE(if (a->table[n].k == DEL) continue;)                               \
      if (COMPARE(a->table[n].k,k))                                           \
         { *v = a->table[n].v; return 1; }                                    \
   }                                                                          \
}                                                                             \
                                                                              \
HASVNULL(                                                                     \
   VALUE STB__(N,get)(TYPE *a, KEY k)                                         \
   {                                                                          \
      VALUE v;                                                                \
      if (STB__(N,get_flag)(a,k,&v)) return v;                                \
      else                           return VNULL;                            \
   }                                                                          \
)                                                                             \
                                                                              \
int STB__(N,getkey)(TYPE *a, KEY k, KEY *kout)                                \
{                                                                             \
   unsigned int h = STB_(N, hash)(k);                                         \
   unsigned int n = h & a->mask, s;                                           \
   if (k == EMPTY || k == DEL) return 0;                                      \
   if (a->table[n].k==EMPTY) return 0;                                        \
   SAFE(if (a->table[n].k != DEL))                                            \
   if (COMPARE(a->table[n].k,k)) { *kout = a->table[n].k; return 1; }         \
   s = stb_rehash(h) | 1;                                                     \
   for(;;) {                                                                  \
      n = (n + s) & a->mask;                                                  \
      if (a->table[n].k==EMPTY) return 0;                                     \
      SAFE(if (a->table[n].k == DEL) continue;)                               \
      if (COMPARE(a->table[n].k,k))                                           \
         { *kout = a->table[n].k; return 1; }                                 \
   }                                                                          \
}                                                                             \
                                                                              \
static int STB_(N,addset)(TYPE *a, KEY k, VALUE v,                            \
                             int allow_new, int allow_old, int copy)          \
{                                                                             \
   unsigned int h = STB_(N, hash)(k);                                         \
   unsigned int n = h & a->mask;                                              \
   int b = -1;                                                                \
   if (k == EMPTY)                                                            \
      if (a->has_empty ? allow_old : allow_new) {                             \
          n=a->has_empty; a->ev = v; a->has_empty = 1; return !n;             \
      } else return 0;                                                        \
   if (k == DEL)                                                              \
      if (a->has_del ? allow_old : allow_new) {                               \
          n=a->has_del; a->dv = v; a->has_del = 1; return !n;                 \
      } else return 0;                                                        \
   if (a->table[n].k != EMPTY) {                                              \
      unsigned int s;                                                         \
      if (a->table[n].k == DEL)                                               \
         b = n;                                                               \
      else if (COMPARE(a->table[n].k,k)) {                                    \
         if (allow_old)                                                       \
            a->table[n].v = v;                                                \
         return !allow_new;                                                   \
      }                                                                       \
      s = stb_rehash(h) | 1;                                                  \
      for(;;) {                                                               \
         n = (n + s) & a->mask;                                               \
         if (a->table[n].k == EMPTY) break;                                   \
         if (a->table[n].k == DEL) {                                          \
            if (b < 0) b = n;                                                 \
         } else if (COMPARE(a->table[n].k,k)) {                               \
            if (allow_old)                                                    \
               a->table[n].v = v;                                             \
            return !allow_new;                                                \
         }                                                                    \
      }                                                                       \
   }                                                                          \
   if (!allow_new) return 0;                                                  \
   if (b < 0) b = n; else --a->deleted;                                       \
   a->table[b].k = copy ? COPY(k) : k;                                        \
   a->table[b].v = v;                                                         \
   ++a->count;                                                                \
   if (a->count > a->grow_threshhold)                                         \
      STB_(N,rehash)(a, a->limit*2);                                          \
   return 1;                                                                  \
}                                                                             \
                                                                              \
int STB__(N, set)(TYPE *a, KEY k, VALUE v){return STB_(N,addset)(a,k,v,1,1,1);}\
int STB__(N, add)(TYPE *a, KEY k, VALUE v){return STB_(N,addset)(a,k,v,1,0,1);}\
int STB__(N, update)(TYPE*a,KEY k,VALUE v){return STB_(N,addset)(a,k,v,0,1,1);}\
                                                                              \
int STB__(N, remove)(TYPE *a, KEY k, VALUE *v)                                \
{                                                                             \
   unsigned int h = STB_(N, hash)(k);                                         \
   unsigned int n = h & a->mask, s;                                           \
   if (k == EMPTY) { if (a->has_empty) { *v = a->ev; a->has_empty=0; return 1; } return 0; } \
   if (k == DEL  ) { if (a->has_del  ) { *v = a->dv; a->has_del  =0; return 1; } return 0; } \
   if (a->table[n].k == EMPTY) return 0;                                      \
   if (SAFE(a->table[n].k == DEL || ) !COMPARE(a->table[n].k,k)) {            \
      s = stb_rehash(h) | 1;                                                  \
      for(;;) {                                                               \
         n = (n + s) & a->mask;                                               \
         if (a->table[n].k == EMPTY) return 0;                                \
         SAFE(if (a->table[n].k == DEL) continue;)                            \
         if (COMPARE(a->table[n].k,k)) break;                                 \
      }                                                                       \
   }                                                                          \
   DISPOSE(a->table[n].k);                                                    \
   a->table[n].k = DEL;                                                       \
   --a->count;                                                                \
   ++a->deleted;                                                              \
   if (v != NULL)                                                             \
      *v = a->table[n].v;                                                     \
   if (a->count < a->shrink_threshhold)                                       \
      STB_(N, rehash)(a, a->limit >> 1);                                      \
   else if (a->deleted > a->delete_threshhold)                                \
      STB_(N, rehash)(a, a->limit);                                           \
   return 1;                                                                  \
}                                                                             \
                                                                              \
TYPE * STB__(N, copy)(TYPE *a)                                                \
{                                                                             \
   int i;                                                                     \
   TYPE *h = (TYPE *) malloc(sizeof(*h));                                     \
   if (!h) return NULL;                                                       \
   if (!STB__(N, init)(h, a->count)) { free(h); return NULL; }                \
   h->count = a->count;                                                       \
   h->deleted = a->deleted;                                                   \
   h->alloced = 1;                                                            \
   h->ev = a->ev; h->dv = a->dv;                                              \
   h->has_empty = a->has_empty; h->has_del = a->has_del;                      \
   memcpy(h->table, a->table, h->limit * sizeof(h->table[0]));                \
   for (i=0; i < a->limit; ++i)                                               \
      if (h->table[i].k != EMPTY && h->table[i].k != DEL)                     \
         h->table[i].k = COPY(h->table[i].k);                                 \
   return h;                                                                  \
}                                                                             \
                                                                              \
static void STB_(N, rehash)(TYPE *a, int count)                               \
{                                                                             \
   int i;                                                                     \
   TYPE b;                                                                    \
   STB__(N, init)(&b, count);                                                 \
   for (i=0; i < a->limit; ++i)                                               \
      if (a->table[i].k != EMPTY && a->table[i].k != DEL)                     \
         STB_(N,addset)(&b, a->table[i].k, a->table[i].v,1,1,0);              \
   free(a->table);                                                            \
   a->table = b.table;\
   a->mask = b.mask;\
   a->count = b.count;\
   a->limit = b.limit;\
   a->deleted = b.deleted;\
   a->delete_threshhold = b.delete_threshhold;\
   a->grow_threshhold = b.grow_threshhold;\
   a->shrink_threshhold = b.shrink_threshhold;\
}

#define STB_equal(a,b)  ((a) == (b))

#define stb_define_hash(TYPE,N,KEY,EMPTY,DEL,HASH,VALUE)                      \
   stb_define_hash_base(TYPE,STB_nofields,N,0.85f,                            \
              KEY,EMPTY,DEL,STB_nocopy,STB_nodelete,STB_nosafe,STB_equal,HASH,\
              VALUE,STB_nullvalue,NULL)

//////////////////////////////////////////////////////////////////////////////
//
//                        stb_extra
//
// An stb_extra data structure lets you store "extra" data associated with
// pointers. You determine the size and type of the data to store; stb_extra
// just gives you O(1) access to it.

//$ SKIP!
stb_declare_hash(STB_EXTERN, stb_extra, stb_extra_, void *, void *)
//$ SKIP!
stb_declare_hash(STB_EXTERN, stb_idict, stb_idict_, stb_int32, stb_int32)

STB_EXTERN void        stb_extra_delete(stb_extra *e, void (*free_func)(void *));

STB_EXTERN stb_idict * stb_idict_new_size(int size);
STB_EXTERN void stb_idict_remove_all(stb_idict *e);

#ifdef STB_DEFINE

#define STB_EMPTY ((void *) 2)
#define STB_EDEL  ((void *) 6)

stb_define_hash_base(stb_extra, STB_nofields, stb_extra_,0.85f,       
              void *,STB_EMPTY,STB_EDEL,STB_nocopy,STB_nodelete,STB_nosafe,STB_equal,return stb_hashptr(k);,
              void *,STB_nullvalue,NULL)

void stb_extra_delete(stb_extra *e, void (*free_func)(void *))
{
   int i;
   if (free_func)
      for (i=0; i < e->limit; ++i)
         if (e->table[i].k != STB_EMPTY && e->table[i].k != STB_EDEL)
            if (free_func == free)
               free(e->table[i].v); // allow STB_MALLOC_WRAPPER to operate
            else
               free_func(e->table[i].v);
   stb_extra_destroy(e);
}

// extra fields needed for stua_dict
#define STB_IEMPTY  ((int) 1)
#define STB_IDEL    ((int) 3)
stb_define_hash_base(stb_idict, short type; short gc; STB_nofields, stb_idict_,0.85f,
              stb_int32,STB_IEMPTY,STB_IDEL,STB_nocopy,STB_nodelete,STB_nosafe,STB_equal,
              return stb_rehash_improved(k);,stb_int32,STB_nonullvalue,0)

stb_idict * stb_idict_new_size(int size)
{
   stb_idict *e = (stb_idict *) malloc(sizeof(*e));
   if (e) {
      if (!stb_is_pow2(size))
         size = 1 << stb_log2_ceil(size);
      stb_idict_init(e, size);
      e->alloced = 1;
   }
   return e;
}

void stb_idict_remove_all(stb_idict *e)
{
   int n;
   for (n=0; n < e->limit; ++n)
      e->table[n].k = STB_IEMPTY;
   e->has_empty = e->has_del = 0;
}
#endif



//////////////////////////////////////////////////////////////////////////////
//
//                  SDICT: Hash Table for Strings (symbol table)
//
//           if "use_arena=1", then strings will be copied
//           into blocks and never freed until the sdict is freed;
//           otherwise they're malloc()ed and free()d on the fly. 
//           (specify use_arena=1 if you never stb_sdict_remove/change)

//$ SKIP!
stb_declare_hash(STB_EXTERN, stb_sdict, stb_sdict_, char *, void *)

STB_EXTERN stb_sdict * stb_sdict_new(int use_arena);
STB_EXTERN void        stb_sdict_delete(stb_sdict *);
STB_EXTERN void *      stb_sdict_change(stb_sdict *, char *str, void *p);
STB_EXTERN int         stb_sdict_count(stb_sdict *d);

#define stb_sdict_for(d,i,q,v)                                                \
   for(i=0; i < (d)->limit ? q=(d)->p[i].str,v=(d)->p[i].val,1 : 0; ++i)      \
      if (q==NULL||q==(void *) 1);else   // reversed makes macro friendly

#ifdef STB_DEFINE

#define STB_DEL ((void *) 1)
#define STB_SDEL  ((char *) 1)

//#define stb_define_hash_base(TYPE,FIELDS,N,LOAD_FACTOR,                      
                               //KEY,EMPTY,DEL,COPY,DISPOSE,COMPARE,HASH,      
                               //VALUE,HASVNULL,VNULL)                          

#define stb_sdict__copy(x)                                       \
   strcpy(a->arena ? stb_malloc_string(a->arena, strlen(x)+1)    \
                   : (char *) malloc(strlen(x)+1), x)

#define stb_sdict__dispose(x)  if (!a->arena) free(x)

stb_define_hash_base(stb_sdict, void*arena;, stb_sdict_, 0.85f,
        char *, NULL, STB_SDEL, stb_sdict__copy, stb_sdict__dispose,
                        STB_safecompare, !strcmp, return stb_hash(k);,
        void *, STB_nullvalue, NULL)

int stb_sdict_count(stb_sdict *a)
{
   return a->count;
}

stb_sdict * stb_sdict_new(int use_arena)
{
   stb_sdict *d = stb_sdict_create();
   if (d == NULL) return NULL;
   d->arena = use_arena ? stb_malloc_global(1) : NULL;
   return d;
}

void stb_sdict_delete(stb_sdict *d)
{
   if (d->arena)
      stb_free(d->arena);
   stb_sdict_destroy(d);
}

void * stb_sdict_change(stb_sdict *d, char *str, void *p)
{
   void *q = stb_sdict_get(d, str);
   stb_sdict_set(d, str, p);
   return q;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                     Instantiated data structures
//
// This is an attempt to implement a templated data structure.
// What you do is define a struct foo, and then include several
// pointer fields to struct foo in your struct. Then you call
// the instantiator, which creates the functions that implement
// the data structure. This requires massive undebuggable #defines,
// so we limit the cases where we do this.
//
// AA tree is an encoding of a 2-3 tree whereas RB trees encode a 2-3-4 tree;
// much simpler code due to fewer cases.

#define stb__bst_parent(x)    x
#define stb__bst_noparent(x)

#define stb_bst_fields(N)                                   \
    *STB_(N,left), *STB_(N,right);                          \
    unsigned char STB_(N,level)

#define stb_bst_fields_parent(N)                            \
    *STB_(N,left), *STB_(N,right),  *STB_(N,parent);        \
    unsigned char STB_(N,level)

#define STB__level(N,x)         ((x) ? (x)->STB_(N,level) : 0)

#define stb_bst_base(TYPE, N, TREE, M, compare, PAR)                         \
                                                                             \
static int STB_(N,_compare)(TYPE *p, TYPE *q)                                \
{                                                                            \
   compare                                                                   \
}                                                                            \
                                                                             \
static void STB_(N,setleft)(TYPE *q, TYPE *v)                                \
{                                                                            \
   q->STB_(N,left) = v;                                                      \
   PAR(if (v) v->STB_(N,parent) = q;)                                        \
}                                                                            \
                                                                             \
static void STB_(N,setright)(TYPE *q, TYPE *v)                               \
{                                                                            \
   q->STB_(N,right) = v;                                                     \
   PAR(if (v) v->STB_(N,parent) = q;)                                        \
}                                                                            \
                                                                             \
static TYPE *STB_(N,skew)(TYPE *q)                                           \
{                                                                            \
   if (q == NULL) return q;                                                  \
   if (q->STB_(N,left)                                                       \
        && q->STB_(N,left)->STB_(N,level) == q->STB_(N,level)) {             \
      TYPE *p       = q->STB_(N,left);                                       \
      STB_(N,setleft)(q, p->STB_(N,right));                                  \
      STB_(N,setright)(p, q);                                                \
      return p;                                                              \
   }                                                                         \
   return q;                                                                 \
}                                                                            \
                                                                             \
static TYPE *STB_(N,split)(TYPE *p)                                          \
{                                                                            \
   TYPE *q = p->STB_(N,right);                                               \
   if (q && q->STB_(N,right)                                                 \
         && q->STB_(N,right)->STB_(N,level) == p->STB_(N,level)) {           \
      STB_(N,setright)(p, q->STB_(N,left));                                  \
      STB_(N,setleft)(q,p);                                                  \
      ++q->STB_(N,level);                                                    \
      return q;                                                              \
   }                                                                         \
   return p;                                                                 \
}                                                                            \
                                                                             \
TYPE *STB__(N,insert)(TYPE *tree, TYPE *item)                                \
{                                                                            \
   int c;                                                                    \
   if (tree == NULL) {                                                       \
      item->STB_(N,left) = NULL;                                             \
      item->STB_(N,right) = NULL;                                            \
      item->STB_(N,level) = 1;                                               \
      PAR(item->STB_(N,parent) = NULL;)                                      \
      return item;                                                           \
   }                                                                         \
   c = STB_(N,_compare)(item,tree);                                          \
   if (c == 0) {                                                             \
      if (item != tree) {                                                    \
         STB_(N,setleft)(item, tree->STB_(N,left));                          \
         STB_(N,setright)(item, tree->STB_(N,right));                        \
         item->STB_(N,level) = tree->STB_(N,level);                          \
         PAR(item->STB_(N,parent) = NULL;)                                   \
      }                                                                      \
      return item;                                                           \
   }                                                                         \
   if (c < 0)                                                                \
      STB_(N,setleft )(tree, STB__(N,insert)(tree->STB_(N,left), item));     \
   else                                                                      \
      STB_(N,setright)(tree, STB__(N,insert)(tree->STB_(N,right), item));    \
   tree = STB_(N,skew)(tree);                                                \
   tree = STB_(N,split)(tree);                                               \
   PAR(tree->STB_(N,parent) = NULL;)                                         \
   return tree;                                                              \
}                                                                            \
                                                                             \
TYPE *STB__(N,remove)(TYPE *tree, TYPE *item)                                \
{                                                                            \
   static TYPE *delnode, *leaf, *restore;                                    \
   if (tree == NULL) return NULL;                                            \
   leaf = tree;                                                              \
   if (STB_(N,_compare)(item, tree) < 0) {                                   \
      STB_(N,setleft)(tree, STB__(N,remove)(tree->STB_(N,left), item));      \
   } else {                                                                  \
      TYPE *r;                                                               \
      delnode = tree;                                                        \
      r = STB__(N,remove)(tree->STB_(N,right), item);                        \
      /* maybe move 'leaf' up to this location */                            \
      if (restore == tree) { tree = leaf; leaf = restore = NULL;  }          \
      STB_(N,setright)(tree,r);                                              \
      assert(tree->STB_(N,right) != tree);                                   \
   }                                                                         \
   if (tree == leaf) {                                                       \
      if (delnode == item) {                                                 \
         tree = tree->STB_(N,right);                                         \
         assert(leaf->STB_(N,left) == NULL);                                 \
         /* move leaf (the right sibling) up to delnode */                   \
         STB_(N,setleft )(leaf, item->STB_(N,left ));                        \
         STB_(N,setright)(leaf, item->STB_(N,right));                        \
         leaf->STB_(N,level) = item->STB_(N,level);                          \
         if (leaf != item)                                                   \
            restore = delnode;                                               \
      }                                                                      \
      delnode = NULL;                                                        \
   } else {                                                                  \
      if (STB__level(N,tree->STB_(N,left) ) < tree->STB_(N,level)-1 ||       \
          STB__level(N,tree->STB_(N,right)) < tree->STB_(N,level)-1) {       \
         --tree->STB_(N,level);                                              \
         if (STB__level(N,tree->STB_(N,right)) > tree->STB_(N,level))        \
            tree->STB_(N,right)->STB_(N,level) = tree->STB_(N,level);        \
         tree = STB_(N,skew)(tree);                                          \
         STB_(N,setright)(tree, STB_(N,skew)(tree->STB_(N,right)));          \
         if (tree->STB_(N,right))                                            \
            STB_(N,setright)(tree->STB_(N,right),                            \
                  STB_(N,skew)(tree->STB_(N,right)->STB_(N,right)));         \
         tree = STB_(N,split)(tree);                                         \
         if (tree->STB_(N,right))                                            \
            STB_(N,setright)(tree,  STB_(N,split)(tree->STB_(N,right)));     \
      }                                                                      \
   }                                                                         \
   PAR(if (tree) tree->STB_(N,parent) = NULL;)                               \
   return tree;                                                              \
}                                                                            \
                                                                             \
TYPE *STB__(N,last)(TYPE *tree)                                              \
{                                                                            \
   if (tree)                                                                 \
      while (tree->STB_(N,right)) tree = tree->STB_(N,right);                \
   return tree;                                                              \
}                                                                            \
                                                                             \
TYPE *STB__(N,first)(TYPE *tree)                                             \
{                                                                            \
   if (tree)                                                                 \
      while (tree->STB_(N,left)) tree = tree->STB_(N,left);                  \
   return tree;                                                              \
}                                                                            \
                                                                             \
TYPE *STB__(N,next)(TYPE *tree, TYPE *item)                                  \
{                                                                            \
   TYPE *next = NULL;                                                        \
   if (item->STB_(N,right))                                                  \
      return STB__(N,first)(item->STB_(N,right));                            \
   PAR(                                                                      \
      while(item->STB_(N,parent)) {                                          \
         TYPE *up = item->STB_(N,parent);                                    \
         if (up->STB_(N,left) == item) return up;                            \
         item = up;                                                          \
      }                                                                      \
      return NULL;                                                           \
   )                                                                         \
   while (tree != item) {                                                    \
      if (STB_(N,_compare)(item, tree) < 0) {                                \
         next = tree;                                                        \
         tree = tree->STB_(N,left);                                          \
      } else {                                                               \
         tree = tree->STB_(N,right);                                         \
      }                                                                      \
   }                                                                         \
   return next;                                                              \
}                                                                            \
                                                                             \
TYPE *STB__(N,prev)(TYPE *tree, TYPE *item)                                  \
{                                                                            \
   TYPE *next = NULL;                                                        \
   if (item->STB_(N,left))                                                   \
      return STB__(N,last)(item->STB_(N,left));                              \
   PAR(                                                                      \
      while(item->STB_(N,parent)) {                                          \
         TYPE *up = item->STB_(N,parent);                                    \
         if (up->STB_(N,right) == item) return up;                           \
         item = up;                                                          \
      }                                                                      \
      return NULL;                                                           \
   )                                                                         \
   while (tree != item) {                                                    \
      if (STB_(N,_compare)(item, tree) < 0) {                                \
         tree = tree->STB_(N,left);                                          \
      } else {                                                               \
         next = tree;                                                        \
         tree = tree->STB_(N,right);                                         \
      }                                                                      \
   }                                                                         \
   return next;                                                              \
}                                                                            \
                                                                             \
STB__DEBUG(                                                                  \
   void STB__(N,_validate)(TYPE *tree, int root)                             \
   {                                                                         \
      if (tree == NULL) return;                                              \
      PAR(if(root) assert(tree->STB_(N,parent) == NULL);)                    \
      assert(STB__level(N,tree->STB_(N,left) ) == tree->STB_(N,level)-1);    \
      assert(STB__level(N,tree->STB_(N,right)) <= tree->STB_(N,level));      \
      assert(STB__level(N,tree->STB_(N,right)) >= tree->STB_(N,level)-1);    \
      if (tree->STB_(N,right)) {                                             \
        assert(STB__level(N,tree->STB_(N,right)->STB_(N,right))              \
                                               !=    tree->STB_(N,level));   \
        PAR(assert(tree->STB_(N,right)->STB_(N,parent) == tree);)            \
      }                                                                      \
      PAR(if(tree->STB_(N,left)) assert(tree->STB_(N,left)->STB_(N,parent) == tree);) \
      STB__(N,_validate)(tree->STB_(N,left) ,0);                             \
      STB__(N,_validate)(tree->STB_(N,right),0);                             \
   }                                                                         \
)                                                                            \
                                                                             \
typedef struct                                                               \
{                                                                            \
   TYPE *root;                                                               \
} TREE;                                                                      \
                                                                             \
void  STB__(M,Insert)(TREE *tree, TYPE *item)                                \
{ tree->root = STB__(N,insert)(tree->root, item); }                          \
void  STB__(M,Remove)(TREE *tree, TYPE *item)                                \
{ tree->root = STB__(N,remove)(tree->root, item); }                          \
TYPE *STB__(M,Next)(TREE *tree, TYPE *item)                                  \
{ return STB__(N,next)(tree->root, item); }                                  \
TYPE *STB__(M,Prev)(TREE *tree, TYPE *item)                                  \
{ return STB__(N,prev)(tree->root, item); }                                  \
TYPE *STB__(M,First)(TREE *tree) { return STB__(N,first)(tree->root); }      \
TYPE *STB__(M,Last) (TREE *tree) { return STB__(N,last) (tree->root); }      \
void STB__(M,Init)(TREE *tree) { tree->root = NULL; }


#define stb_bst_find(N,tree,fcompare)                                        \
{                                                                            \
   int c;                                                                    \
   while (tree != NULL) {                                                    \
      fcompare                                                               \
      if (c == 0) return tree;                                               \
      if (c < 0)  tree = tree->STB_(N,left);                                 \
      else        tree = tree->STB_(N,right);                                \
   }                                                                         \
   return NULL;                                                              \
}

#define stb_bst_raw(TYPE,N,TREE,M,vfield,VTYPE,compare,PAR)                  \
   stb_bst_base(TYPE,N,TREE,M,                                               \
         VTYPE a = p->vfield; VTYPE b = q->vfield; return (compare);, PAR )  \
                                                                             \
TYPE *STB__(N,find)(TYPE *tree, VTYPE a)                                     \
   stb_bst_find(N,tree,VTYPE b = tree->vfield; c = (compare);)               \
TYPE *STB__(M,Find)(TREE *tree, VTYPE a)                                     \
{ return STB__(N,find)(tree->root, a); }

#define stb_bst(TYPE,N,TREE,M,vfield,VTYPE,compare) \
   stb_bst_raw(TYPE,N,TREE,M,vfield,VTYPE,compare,stb__bst_noparent)
#define stb_bst_parent(TYPE,N,TREE,M,vfield,VTYPE,compare) \
   stb_bst_raw(TYPE,N,TREE,M,vfield,VTYPE,compare,stb__bst_parent)


//////////////////////////////////////////////////////////////////////////////
//
//                             Stream Processing
//
//   Stream processing allows you to:
//       redirect file I/O to/from a memory buffer
//       turn on and off stream/compression
//           while compressing, you cannot ftell/fseek
//
//
//   implements:
//       fopen, fclose
//       fwrite, fread
//       fputc, fgetc
//       





//////////////////////////////////////////////////////////////////////////////
//
//                             File Processing
//


#ifdef _WIN32
  // if not win32, we can still get gcc style int64!
  // and this should probably be _MSC_VER ?
  typedef unsigned _int64 stb__64;
   
  #define stb_rename(x,y)   _wrename(stb__from_utf8(x), stb__from_utf8_alt(y))
  #define stb_mktemp   _mktemp
  STB_EXTERN void     stb_fput_varlen64(FILE *f, stb__64 v);
  STB_EXTERN stb__64  stb_fget_varlen64(FILE *f);
  STB_EXTERN int      stb_size_varlen64(stb__64 v);
#else
  #define stb_mktemp   mktemp
  #define stb_rename   rename
#endif


#define stb_filec    (char *) stb_file
#define stb_fileu    (unsigned char *) stb_file
STB_EXTERN void *  stb_file(char *filename, size_t *length);
STB_EXTERN size_t  stb_filelen(FILE *f);
STB_EXTERN char ** stb_stringfile(char *filename, int *len);
STB_EXTERN char ** stb_stringfile_trimmed(char *name, int *len, char comm);
STB_EXTERN char *  stb_fgets(char *buffer, int buflen, FILE *f);
STB_EXTERN char *  stb_fgets_malloc(FILE *f);
STB_EXTERN int     stb_fexists(char *filename);
STB_EXTERN void    stb_fwrite32(FILE *f, uint32 datum);

STB_EXTERN int     stb_fullpath(char *abs, int abs_size, char *rel);
STB_EXTERN FILE *  stb_fopen(char *filename, char *mode);
STB_EXTERN int     stb_fclose(FILE *f, int keep);

STB_EXTERN int     stb_copyfile(char *src, char *dest);

enum
{
   stb_keep_no = 0,
   stb_keep_yes = 1,
   stb_keep_if_different = 2,
};


#ifdef STB_DEFINE

void stb_fwrite32(FILE *f, uint32 x)
{
   fwrite(&x, 4, 1, f);
}

#ifdef _WIN32
   #define stb__stat   _stat
#else
   #define stb__stat   stat
#endif

int stb_fexists(char *filename)
{
   struct stb__stat buf;
   return stb__windows(
             _wstat(stb__from_utf8(filename), &buf),
               stat(filename,&buf)
          ) == 0;
}

size_t  stb_filelen(FILE *f)
{
   size_t len, pos;
   pos = ftell(f);
   fseek(f, 0, SEEK_END);
   len = ftell(f);
   fseek(f, pos, SEEK_SET);
   return len;
}

void *stb_file(char *filename, size_t *length)
{
   FILE *f = stb__fopen(filename, "rb");
   char *buffer;
   size_t len;
   if (!f) return NULL;
   len = stb_filelen(f);
   buffer = (char *) malloc(len+2); // nul + extra
   if (fread(buffer, 1, len, f) == len) {
      if (length) *length = len;
      buffer[len] = 0;
   } else {
      free(buffer);
      buffer = NULL;
   }
   fclose(f);
   return buffer;
}

void *  stb_file_max(char *filename, size_t *length)
{
   FILE *f = stb__fopen(filename, "rb");
   char *buffer;
   size_t len, maxlen;
   if (!f) return NULL;
   maxlen = *length;
   buffer = (char *) malloc(maxlen+1);
   len = fread(buffer, 1, maxlen, f);
   buffer[len] = 0;
   fclose(f);
   *length = len;
   return buffer;
}

char ** stb_stringfile(char *filename, int *plen)
{
   FILE *f = stb__fopen(filename, "rb");
   char *buffer, **list=NULL, *s;
   size_t len, count, i;

   if (!f) return NULL;
   len = stb_filelen(f);
   buffer = (char *) malloc(len+1);
   len = fread(buffer, 1, len, f);
   buffer[len] = 0;
   fclose(f);

   // two passes through: first time count lines, second time set them
   for (i=0; i < 2; ++i) {
      s = buffer;
      if (i == 1)
         list[0] = s;
      count = 1;
      while (*s) {
         if (*s == '\n' || *s == '\r') {
            // detect if both cr & lf are together
            int crlf = (s[0] + s[1]) == ('\n' + '\r');
            if (i == 1) *s = 0;
            if (crlf) ++s;
            if (s[1]) {  // it's not over yet
               if (i == 1) list[count] = s+1;
               ++count;
            }
         }
         ++s;
      }
      if (i == 0) {
         list = (char **) malloc(sizeof(*list) * (count+1) + len+1);
         if (!list) return NULL;
         list[count] = 0;
         // recopy the file so there's just a single allocation to free
         memcpy(&list[count+1], buffer, len+1);
         free(buffer);
         buffer = (char *) &list[count+1];
         if (plen) *plen = count;
      }
   }
   return list;
}

char ** stb_stringfile_trimmed(char *name, int *len, char comment)
{
   int i,n,o=0;
   char **s = stb_stringfile(name, &n);
   if (s == NULL) return NULL;
   for (i=0; i < n; ++i) {
      char *p = stb_skipwhite(s[i]);
      if (*p && *p != comment)
         s[o++] = p;
   }
   s[o] = NULL;
   if (len) *len = o;
   return s;
}

char * stb_fgets(char *buffer, int buflen, FILE *f)
{
   char *p;
   buffer[0] = 0;
   p = fgets(buffer, buflen, f);
   if (p) {
      int n = strlen(p)-1;
      if (n >= 0)
         if (p[n] == '\n')
            p[n] = 0;
   }
   return p;
}

char * stb_fgets_malloc(FILE *f)
{
   // avoid reallocing for small strings
   char quick_buffer[800];
   quick_buffer[sizeof(quick_buffer)-2] = 0;
   if (!fgets(quick_buffer, sizeof(quick_buffer), f))
      return NULL;

   if (quick_buffer[sizeof(quick_buffer)-2] == 0) {
      int n = strlen(quick_buffer);
      if (n > 0 && quick_buffer[n-1] == '\n')
         quick_buffer[n-1] = 0;
      return strdup(quick_buffer);
   } else {
      char *p;
      char *a = strdup(quick_buffer);
      int len = sizeof(quick_buffer)-1;

      while (!feof(f)) {
         if (a[len-1] == '\n') break;
         a = (char *) realloc(a, len*2);
         p = &a[len];
         p[len-2] = 0;
         if (!fgets(p, len, f))
            break;
         if (p[len-2] == 0) {
            len += strlen(p);
            break;
         }
         len = len + (len-1);
      }
      if (a[len-1] == '\n')
         a[len-1] = 0;
      return a;
   }
}

int stb_fullpath(char *abs, int abs_size, char *rel)
{
   #ifdef _WIN32
   return _fullpath(abs, rel, abs_size) != NULL;
   #else
   if (abs[0] == '/' || abs[0] == '~') {
      if ((int) strlen(rel) >= abs_size)
         return 0;
      strcpy(abs,rel);
      return TRUE;
   } else {
      int n;
      getcwd(abs, abs_size);
      n = strlen(abs);
      if (n+(int) strlen(rel)+2 <= abs_size) {
         abs[n] = '/';
         strcpy(abs+n+1, rel);
         return TRUE;
      } else {
         return FALSE;
      }
   }
   #endif
}

static int stb_fcmp_core(FILE *f, FILE *g)
{
   char buf1[1024],buf2[1024];
   int n1,n2, res=0;

   while (1) {
      n1 = fread(buf1, 1, sizeof(buf1), f);
      n2 = fread(buf2, 1, sizeof(buf2), g);
      res = memcmp(buf1,buf2,stb_min(n1,n2));
      if (res)
         break;
      if (n1 != n2) {
         res = n1 < n2 ? -1 : 1;
         break;
      }
      if (n1 == 0)
         break;
   }

   fclose(f);
   fclose(g);
   return res;
}

int stb_fcmp(char *s1, char *s2)
{
   FILE *f = stb__fopen(s1, "rb");
   FILE *g = stb__fopen(s2, "rb");

   if (f == NULL || g == NULL) {
      if (f) fclose(f);
      if (g) {
         fclose(g);
         return TRUE;
      }
      return f != NULL;
   }

   return stb_fcmp_core(f,g);
}

int stb_feq(char *s1, char *s2)
{
   FILE *f = stb__fopen(s1, "rb");
   FILE *g = stb__fopen(s2, "rb");

   if (f == NULL || g == NULL) {
      if (f) fclose(f);
      if (g) fclose(g);
      return f == g;
   }

   // feq is faster because it shortcuts if they're different length
   if (stb_filelen(f) != stb_filelen(g)) {
      fclose(f);
      fclose(g);
      return 0;
   }

   return !stb_fcmp_core(f,g);
}

static stb_extra *stb__files;

typedef struct
{
   char *temp_name;
   char *name;
   int   errors;
} stb__file_data;

FILE *  stb_fopen(char *filename, char *mode)
{
   char name_full[4096];
   char temp_full[sizeof(name_full) + 12];
   int j,p;
   if (mode[0] != 'w' && !strchr(mode, '+'))
      return stb__fopen(filename, mode);

   // save away the full path to the file so if the program
   // changes the cwd everything still works right! unix has
   // better ways to do this, but we have to work in windows
   if (stb_fullpath(name_full, sizeof(name_full), filename)==0)
      return 0;

   // try to generate a temporary file in the same directory
   p = strlen(name_full)-1;
   while (p > 0 && name_full[p] != '/' && name_full[p] != '\\'
                && name_full[p] != ':' && name_full[p] != '~')
      --p;
   ++p;

   memcpy(temp_full, name_full, p);

   // try multiple times to make a temp file... just in
   // case some other process makes the name first
   for (j=0; j < 32; ++j) {
      FILE *f;
      strcpy(temp_full+p, "stmpXXXXXX");
      if (stb_mktemp(temp_full) == NULL)
         return 0;

      f = fopen(temp_full, mode);
      if (f != NULL) {
         stb__file_data *d = (stb__file_data *) malloc(sizeof(*d));
         if (!d) { assert(0);  /* NOTREACHED */fclose(f); return NULL; }
         if (stb__files == NULL) stb__files = stb_extra_create();
         d->temp_name = strdup(temp_full);
         d->name      = strdup(name_full);
         d->errors    = 0;
         stb_extra_add(stb__files, f, d);
         return f;
      }
   }

   return NULL;
}

int     stb_fclose(FILE *f, int keep)
{
   stb__file_data *d;

   int ok = FALSE;
   if (f == NULL) return 0;

   if (ferror(f))
      keep = stb_keep_no;

   fclose(f);

   if (stb__files && stb_extra_remove(stb__files, f, (void **) &d)) {
      if (stb__files->count == 0) {
         stb_extra_destroy(stb__files);
         stb__files = NULL;
      }
   } else
      return TRUE; // not special

   if (keep == stb_keep_if_different) {
      // check if the files are identical
      if (stb_feq(d->name, d->temp_name)) {
         keep = stb_keep_no;
         ok = TRUE;  // report success if no change
      }
   }

   if (keep != stb_keep_no) {
      if (stb_fexists(d->name) && remove(d->name)) {
         // failed to delete old, so don't keep new
         keep = stb_keep_no;
      } else {
         if (!stb_rename(d->temp_name, d->name))
            ok = TRUE;
         else
            keep=stb_keep_no;
      }
   }

   if (keep == stb_keep_no)
      remove(d->temp_name);

   free(d->temp_name);
   free(d->name);
   free(d);

   return ok;
}

int stb_copyfile(char *src, char *dest)
{
   char raw_buffer[1024];
   char *buffer;
   int buf_size = 65536;

   FILE *f, *g;

   // if file already exists at destination, do nothing
   if (stb_feq(src, dest)) return TRUE;

   // open file
   f = stb__fopen(src, "rb");
   if (f == NULL) return FALSE;

   // open file for writing
   g = stb__fopen(dest, "wb");
   if (g == NULL) {
      fclose(f);
      return FALSE;
   }

   buffer = (char *) malloc(buf_size);
   if (buffer == NULL) {
      buffer = raw_buffer;
      buf_size = sizeof(raw_buffer);
   }

   while (!feof(f)) {
      int n = fread(buffer, 1, buf_size, f);
      if (n != 0)
         fwrite(buffer, 1, n, g);
   }

   fclose(f);
   if (buffer != raw_buffer)
      free(buffer);

   fclose(g);
   return TRUE;
}

#endif


//////////////////////////////////////////////////////////////////////////////
//
//                 Options parsing
//

STB_EXTERN char **stb_getopt_param(int *argc, char **argv, char *param);
STB_EXTERN char **stb_getopt(int *argc, char **argv);
STB_EXTERN void   stb_getopt_free(char **opts);

#ifdef STB_DEFINE

void   stb_getopt_free(char **opts)
{
   int i;
   STB__ARR(char *) o2 = opts;
   for (i=0; i < stb_arr_len(o2); ++i)
      free(o2[i]);
   stb_arr_free(o2);
}

char **stb_getopt(int *argc, char **argv)
{
   return stb_getopt_param(argc, argv, "");
}

char **stb_getopt_param(int *argc, char **argv, char *param)
{
   STB__ARR(char *) opts=NULL;
   int i,j=1;
   for (i=1; i < *argc; ++i) {
      if (argv[i][0] != '-') {
         argv[j++] = argv[i];
      } else {
         if (argv[i][1] == 0) { // plain - == don't parse further options
            ++i;
            while (i < *argc)
               argv[j++] = argv[i++];
            break;
         } else {
            int k;
            char *q = argv[i];  // traverse options list
            for (k=1; q[k]; ++k) {
               char *s;
               if (strchr(param, q[k])) {  // does it take a parameter?
                  char *t = &q[k+1], z = q[k];
                  int len=0;
                  if (*t == 0) {
                     if (i == *argc-1) { // takes a parameter, but none found
                        *argc = 0;
                        stb_getopt_free(opts);
                        return NULL;
                     }
                     t = argv[++i];
                  } else
                     k += strlen(t);
                  len = strlen(t);
                  s = (char *) malloc(len+2);
                  if (!s) return NULL;
                  s[0] = z;
                  strcpy(s+1, t);
               } else {
                  // no parameter
                  s = (char *) malloc(2);
                  if (!s) return NULL;
                  s[0] = q[k];
                  s[1] = 0;
               }
               stb_arr_push(opts, s);
            }
         }
      }
   }
   stb_arr_push(opts, NULL);
   *argc = j;
   return opts;
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
//                 Portable directory reading
//

STB_EXTERN char **stb_readdir_files  (char *dir);
STB_EXTERN char **stb_readdir_files_mask(char *dir, char *wild);
STB_EXTERN char **stb_readdir_subdirs(char *dir);
STB_EXTERN void   stb_readdir_free   (char **files);
STB_EXTERN char **stb_readdir_recursive(char *dir, char *filespec);
STB_EXTERN char **stb_readdir_recursive_n(char *dir, char **filespecs, int n);
STB_EXTERN void stb_delete_directory_recursive(char *dir);

#ifdef STB_DEFINE

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

void stb_readdir_free(char **files)
{
   STB__ARR(char *) f2 = files;
   int i;
   for (i=0; i < stb_arr_len(f2); ++i)
      free(f2[i]);
   stb_arr_free(f2);
}

STB_EXTERN int stb_wildmatchi(char *expr, char *candidate);
static double stb_readdir_size;
static char **readdir_raw(char *dir, int return_subdirs, char *mask)
{
   STB__ARR(char *) results = NULL;
   char buffer[512], with_slash[512];
   int n;

   #ifdef _WIN32
      stb__wchar *ws;
      struct _wfinddata_t data;
      const long none = -1;
      long z;
   #else
      const DIR *none = NULL;
      DIR *z;
   #endif

   strcpy(buffer,dir);
   stb_fixpath(buffer);
   n = strlen(buffer);

   if (n > 0 && (buffer[n-1] != '/')) {
      buffer[n++] = '/';
   }
   buffer[n] = 0;
   strcpy(with_slash, buffer);

   #ifdef _WIN32
      strcpy(buffer+n, "*.*");
      ws = stb__from_utf8(buffer);
      z = _wfindfirst(ws, &data);
   #else
      z = opendir(dir);
   #endif


   if (z != none) {
      int nonempty = TRUE;
      #ifndef _WIN32
      struct dirent *data = readdir(z);
      nonempty = (data != NULL);
      #endif

      if (nonempty) {

         do {
            int is_subdir;
            #ifdef _WIN32
            char *name = stb__to_utf8(data.name);
            if (name == NULL) {
               printf("Unable to convert '%S' to utf8!\n", data.name);
               continue;
            }
            is_subdir = !!(data.attrib & _A_SUBDIR);
            #else
            char *name = data->d_name;
            strcpy(buffer+n,name);
            DIR *y = opendir(buffer);
            is_subdir = (y != NULL);
            if (y != NULL) closedir(y);
            #endif
        
            if (is_subdir == return_subdirs) {
               if (!is_subdir || name[0] != '.') {
                  if (!mask || stb_wildmatchi(mask, name)) {
                     char buffer[512],*p=buffer;
                     sprintf(buffer, "%s%s", with_slash, name);
                     if (buffer[0] == '.' && buffer[1] == '/')
                        p = buffer+2;
                     stb_arr_push(results, strdup(p));
                     #ifdef _WIN32
                     if (!is_subdir)
                        stb_readdir_size += data.size;
                     #endif
                  }
               }
            }
         }
         #ifdef _WIN32
         while (0 == _wfindnext(z, &data));
         #else
         while ((data = readdir(z)) != NULL);
         #endif
      }
      #ifdef _WIN32
         _findclose(z);
      #else
         closedir(z);
      #endif
   }
   return results;
}

char **stb_readdir_files  (char *dir) { return readdir_raw(dir, 0, NULL); }
char **stb_readdir_subdirs(char *dir) { return readdir_raw(dir, 1, NULL); }
char **stb_readdir_files_mask(char *dir, char *wild) { return readdir_raw(dir, 0, wild); }

static char **stb_readdir_rec(STB__ARR(char *) sofar, char *dir, char **filespecs, int num_specs)
{
   int i, n = strcmp(dir, ".") ? strlen(dir)+1 : 0;
   STB__ARR(char *) files;
   STB__ARR(char *) dirs;
   char **p;
   files = stb_readdir_files(dir);
   if (filespecs == NULL) num_specs = 0;
   stb_arr_for(p, files) {
      for (i=0; i < num_specs; ++i)
         if (stb_wildmatchi(filespecs[i], *p + n))
            break;
      if (num_specs == 0 || i < num_specs)
         stb_arr_push(sofar, strdup(*p));
   }
   stb_readdir_free(files);

   dirs = stb_readdir_subdirs(dir);
   stb_arr_for(p, dirs)
      sofar = stb_readdir_rec(sofar, *p, filespecs, num_specs);
   stb_readdir_free(dirs);
   return sofar;
}

char **stb_readdir_recursive_n(char *dir, char **filespecs, int num_specs)
{
   return stb_readdir_rec(NULL, dir, filespecs, num_specs);
}

char **stb_readdir_recursive(char *dir, char *filespec)
{
   char *filespecs[2] = { filespec, NULL };
   return stb_readdir_recursive_n(dir, filespecs, 1);
}

void stb_delete_directory_recursive(char *dir)
{
   STB__ARR(char *) list = stb_readdir_subdirs(dir);
   int i;
   for (i=0; i < stb_arr_len(list); ++i)
      stb_delete_directory_recursive(list[i]);
   stb_arr_free(list);
   list = stb_readdir_files(dir);
   for (i=0; i < stb_arr_len(list); ++i)
      if (!remove(list[i])) {
         // on windows, try again after making it writeable; don't ALWAYS
         // do this first since that would be slow in the normal case
         #ifdef _WIN32
         _chmod(list[i], _S_IWRITE);
         remove(list[i]);
         #endif
      }
   stb_arr_free(list);
   stb__windows(_rmdir,rmdir)(dir);
}

#endif


//////////////////////////////////////////////////////////////////////////////
//
//                 Checksums: CRC-32, ADLER32, SHA-1
//
//    CRC-32 and ADLER32 allow streaming blocks
//    SHA-1 requires either a complete buffer, max size 2^32 - 73
//          or it can checksum directly from a file, max 2^61

#define STB_ADLER32_SEED   1
#define STB_CRC32_SEED     0    // note that we logical NOT this in the code

STB_EXTERN stb_uint 
  stb_adler32(stb_uint adler32, stb_uchar *buffer, stb_uint buflen);
STB_EXTERN stb_uint 
  stb_crc32_block(stb_uint crc32, stb_uchar *buffer, stb_uint len);
STB_EXTERN stb_uint stb_crc32(unsigned char *buffer, stb_uint len);

STB_EXTERN void stb_sha1(
  unsigned char output[20], unsigned char *buffer, unsigned int len);
STB_EXTERN int stb_sha1_file(unsigned char output[20], char *file);

STB_EXTERN void stb_sha1_readable(char display[27], unsigned char sha[20]);

#ifdef STB_DEFINE
stb_uint stb_crc32_block(stb_uint crc, unsigned char *buffer, stb_uint len)
{
   static stb_uint crc_table[256];
   stb_uint i,j,s;
   crc = ~crc;

   if (crc_table[1] == 0)
      for(i=0; i < 256; i++) {
         for (s=i, j=0; j < 8; ++j)
            s = (s >> 1) ^ (s & 1 ? 0xedb88320 : 0);
         crc_table[i] = s;
      }
   for (i=0; i < len; ++i)
      crc = (crc >> 8) ^ crc_table[buffer[i] ^ (crc & 0xff)];
   return ~crc;
}

stb_uint stb_crc32(unsigned char *buffer, stb_uint len)
{
   return stb_crc32_block(0, buffer, len);
}

stb_uint stb_adler32(stb_uint adler32, stb_uchar *buffer, stb_uint buflen)
{
   const unsigned long ADLER_MOD = 65521;
   unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
   unsigned long blocklen, i;

   blocklen = buflen % 5552;
   while (buflen) {
      for (i=0; i + 7 < blocklen; i += 8) {
         s1 += buffer[0], s2 += s1;
         s1 += buffer[1], s2 += s1;
         s1 += buffer[2], s2 += s1;
         s1 += buffer[3], s2 += s1;
         s1 += buffer[4], s2 += s1;
         s1 += buffer[5], s2 += s1;
         s1 += buffer[6], s2 += s1;
         s1 += buffer[7], s2 += s1;

         buffer += 8;
      }

      for (; i < blocklen; ++i)
         s1 += *buffer++, s2 += s1;

      s1 %= ADLER_MOD, s2 %= ADLER_MOD;
      buflen -= blocklen;
      blocklen = 5552;
   }
   return (s2 << 16) + s1;
}

static void stb__sha1(stb_uchar *chunk, stb_uint h[5])
{
   int i;
   stb_uint a,b,c,d,e;
   stb_uint w[80];

   for (i=0; i < 16; ++i)
      w[i] = stb_big32(&chunk[i*4]);
   for (i=16; i < 80; ++i) {
      stb_uint t;
      t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
      w[i] = (t + t) | (t >> 31);
   }

   a = h[0];
   b = h[1];
   c = h[2];
   d = h[3];
   e = h[4];

   #define STB__SHA1(k,f)                                            \
   {                                                                 \
      stb_uint temp = (a << 5) + (a >> 27) + (f) + e + (k) + w[i];  \
      e = d;                                                       \
      d = c;                                                     \
      c = (b << 30) + (b >> 2);                               \
      b = a;                                              \
      a = temp;                                    \
   }

   i=0;
   for (; i < 20; ++i) STB__SHA1(0x5a827999, d ^ (b & (c ^ d))       );
   for (; i < 40; ++i) STB__SHA1(0x6ed9eba1, b ^ c ^ d               );
   for (; i < 60; ++i) STB__SHA1(0x8f1bbcdc, (b & c) + (d & (b ^ c)) );
   for (; i < 80; ++i) STB__SHA1(0xca62c1d6, b ^ c ^ d               );

   #undef STB__SHA1

   h[0] += a;
   h[1] += b;
   h[2] += c;
   h[3] += d;
   h[4] += e;
}

void stb_sha1(stb_uchar output[20], stb_uchar *buffer, stb_uint len)
{
   unsigned char final_block[128];
   stb_uint end_start, final_len, j;
   int i;

   stb_uint h[5];

   h[0] = 0x67452301;
   h[1] = 0xefcdab89;
   h[2] = 0x98badcfe;
   h[3] = 0x10325476;
   h[4] = 0xc3d2e1f0;

   // we need to write padding to the last one or two
   // blocks, so build those first into 'final_block'

   // we have to write one special byte, plus the 8-byte length

   // compute the block where the data runs out
   end_start = len & ~63;

   // compute the earliest we can encode the length
   if (((len+9) & ~63) == end_start) {
      // it all fits in one block, so fill a second-to-last block
      end_start -= 64;
   }

   final_len = end_start + 128;

   // now we need to copy the data in
   assert(end_start + 128 >= len+9);
   assert(end_start < len || len < 64-9);

   j = 0;
   if (end_start > len)
      j = (stb_uint) - (int) end_start;

   for (; end_start + j < len; ++j)
      final_block[j] = buffer[end_start + j];
   final_block[j++] = 0x80;
   while (j < 128-5) // 5 byte length, so write 4 extra padding bytes
      final_block[j++] = 0;
   // big-endian size
   final_block[j++] = len >> 29;
   final_block[j++] = len >> 21;
   final_block[j++] = len >> 13;
   final_block[j++] = len >>  5;
   final_block[j++] = len <<  3;
   assert(j == 128 && end_start + j == final_len);

   for (j=0; j < final_len; j += 64) { // 512-bit chunks
      if (j+64 >= end_start+64)
         stb__sha1(&final_block[j - end_start], h);
      else
         stb__sha1(&buffer[j], h);
   }

   for (i=0; i < 5; ++i) {
      output[i*4 + 0] = h[i] >> 24;
      output[i*4 + 1] = h[i] >> 16;
      output[i*4 + 2] = h[i] >>  8;
      output[i*4 + 3] = h[i] >>  0;
   }
}

#ifdef _WIN32
// @TODO: rewrite this to not use 64-bit numbers, e.g.
// manually use 2 32-bit ints (or get stb__64 ported)
int stb_sha1_file(stb_uchar output[20], char *file)
{
   int i;
   stb__64 length=0;
   unsigned char buffer[128];

   FILE *f = stb__fopen(file, "rb");
   stb_uint h[5];

   if (f == NULL) return 0; // file not found

   h[0] = 0x67452301;
   h[1] = 0xefcdab89;
   h[2] = 0x98badcfe;
   h[3] = 0x10325476;
   h[4] = 0xc3d2e1f0;

   for(;;) {
      int n = fread(buffer, 1, 64, f);
      if (n == 64) {
         stb__sha1(buffer, h);
         length += n;
      } else {
         int block = 64;

         length += n;

         buffer[n++] = 0x80;

         // if there isn't enough room for the length, double the block
         if (n + 8 > 64) 
            block = 128;

         // pad to end
         memset(buffer+n, 0, block-8-n);

         i = block - 8;
         buffer[i++] = (stb_uchar) (length >> 53);
         buffer[i++] = (stb_uchar) (length >> 45);
         buffer[i++] = (stb_uchar) (length >> 37);
         buffer[i++] = (stb_uchar) (length >> 29);
         buffer[i++] = (stb_uchar) (length >> 21);
         buffer[i++] = (stb_uchar) (length >> 13);
         buffer[i++] = (stb_uchar) (length >>  5);
         buffer[i++] = (stb_uchar) (length <<  3);
         assert(i == block);
         stb__sha1(buffer, h);
         if (block == 128)
            stb__sha1(buffer+64, h);
         else
            assert(block == 64);
         break;
      }
   }
   fclose(f);

   for (i=0; i < 5; ++i) {
      output[i*4 + 0] = h[i] >> 24;
      output[i*4 + 1] = h[i] >> 16;
      output[i*4 + 2] = h[i] >>  8;
      output[i*4 + 3] = h[i] >>  0;
   }

   return 1;
}
#endif

// client can truncate this wherever they like
void stb_sha1_readable(char display[27], unsigned char sha[20])
{
   char encoding[65] = "0123456789abcdefghijklmnopqrstuv"
                       "wxyzABCDEFGHIJKLMNOPQRSTUVWXYZ%$";
   int num_bits = 0, acc=0;
   int i=0,o=0;
   while (o < 26) {
      int v;
      // expand the accumulator
      if (num_bits < 6) {
         assert(i != 20);
         acc += sha[i++] << num_bits;
         num_bits += 8;
      }
      v = acc & ((1 << 6) - 1);
      display[o++] = encoding[v];
      acc >>= 6;
      num_bits -= 6;
   }
   assert(num_bits == 20*8 - 26*6);
   display[o++] = encoding[acc];   
}

#endif


//////////////////////////////////////////////////////////////////////////////
//
//  STB_MALLOC_WRAPPER
//
//    you can use the wrapper functions with your own malloc wrapper,
//    or define STB_MALLOC_WRAPPER project-wide to have
//    malloc/free/realloc/strdup all get vectored to it

#ifdef STB_DEFINE

typedef struct
{
   void *p;
   char *file;
   int  line;
   int  size;
} stb_malloc_record;

#ifndef STB_MALLOC_HISTORY_COUNT
#define STB_MALLOC_HISTORY_COUNT 50 // 800 bytes
#endif

stb_malloc_record *stb__allocations;
static int stb__alloc_size, stb__alloc_limit, stb__alloc_mask;
int stb__alloc_count;

stb_malloc_record stb__alloc_history[STB_MALLOC_HISTORY_COUNT];
int stb__history_pos;

static int stb__hashfind(void *p)
{
   uint32 h = stb_hashptr(p);
   int s,n = h & stb__alloc_mask;
   if (stb__allocations[n].p == p)
      return n;
   s = stb_rehash(h)|1;
   for(;;) {
      if (stb__allocations[n].p == NULL)
         return -1;
      n = (n+s) & stb__alloc_mask;
      if (stb__allocations[n].p == p)
         return n;
   }
}

int stb_wrapper_allocsize(void *p)
{
   int n = stb__hashfind(p);
   if (n < 0) return 0;
   return stb__allocations[n].size;
}

static int stb__historyfind(void *p)
{
   int n = stb__history_pos;
   int i;
   for (i=0; i < STB_MALLOC_HISTORY_COUNT; ++i) {
      if (--n < 0) n = STB_MALLOC_HISTORY_COUNT-1;
      if (stb__alloc_history[n].p == p)
         return n;
   }
   return -1;
}

static void stb__add_alloc(void *p, int sz, char *file, int line);
static void stb__grow_alloc(void)
{
   int i,old_num = stb__alloc_size;
   stb_malloc_record *old = stb__allocations;
   if (stb__alloc_size == 0)
      stb__alloc_size = 64;
   else
      stb__alloc_size *= 2;

   stb__allocations = (stb_malloc_record *) stb__realloc_raw(NULL, stb__alloc_size * sizeof(stb__allocations[0]));
   if (stb__allocations == NULL)
      stb_fatal("Internal error: couldn't grow malloc wrapper table");
   memset(stb__allocations, 0, stb__alloc_size * sizeof(stb__allocations[0]));
   stb__alloc_limit = (stb__alloc_size*3)>>2;
   stb__alloc_mask = stb__alloc_size-1;

   stb__alloc_count = 0;

   for (i=0; i < old_num; ++i)
      if (old[i].p > STB_DEL) {
         stb__add_alloc(old[i].p, old[i].size, old[i].file, old[i].line);
         assert(stb__hashfind(old[i].p) >= 0);
      }
   for (i=0; i < old_num; ++i)
      if (old[i].p > STB_DEL)
         assert(stb__hashfind(old[i].p) >= 0);
   stb__realloc_raw(old, 0);
}

static void stb__add_alloc(void *p, int sz, char *file, int line)
{
   uint32 h;
   int n, f=-1;
   if (stb__alloc_count >= stb__alloc_limit)
      stb__grow_alloc();
   h = stb_hashptr(p);
   n = h & stb__alloc_mask;
   if (stb__allocations[n].p > STB_DEL) {
      int s = stb_rehash(h)|1;
      do {
         n = (n+s) & stb__alloc_mask;
      } while (stb__allocations[n].p > STB_DEL);
   }
   assert(stb__allocations[n].p == NULL || stb__allocations[n].p == STB_DEL);
   stb__allocations[n].p = p;
   stb__allocations[n].size = sz;
   stb__allocations[n].line = line;
   stb__allocations[n].file = file;
   ++stb__alloc_count;
}

static void stb__remove_alloc(int n, char *file, int line)
{
   stb__alloc_history[stb__history_pos] = stb__allocations[n];
   stb__alloc_history[stb__history_pos].file = file;
   stb__alloc_history[stb__history_pos].line = line;
   if (++stb__history_pos == STB_MALLOC_HISTORY_COUNT)
      stb__history_pos = 0;
   stb__allocations[n].p = STB_DEL;
   --stb__alloc_count;
}

void stb_wrapper_malloc(void *p, int sz, char *file, int line)
{
   if (!p) return;
   stb__add_alloc(p,sz,file,line);
}

void stb_wrapper_free(void *p, char *file, int line)
{
   int n;
   
   if (p == NULL) return;

   n = stb__hashfind(p);

   if (n >= 0)
      stb__remove_alloc(n, file, line);
   else {
      // tried to free something we hadn't allocated!
      n = stb__historyfind(p);
      assert(0); /* NOTREACHED */
      if (n >= 0)
         stb_fatal("Attempted to free %d-byte block %p at %s:%d previously freed/realloced at %s:%d",
                       stb__alloc_history[n].size, p,
                       file, line,
                       stb__alloc_history[n].file, stb__alloc_history[n].line);
      else
         stb_fatal("Attempted to free unknown block %p at %s:%d", p, file,line);
   }
}

void stb_wrapper_check(void *p)
{
   int n;
   
   if (p == NULL) return;

   n = stb__hashfind(p);

   if (n >= 0) return;

   for (n=0; n < stb__alloc_size; ++n)
      if (stb__allocations[n].p == p)
         stb_fatal("Internal error: pointer %p was allocated, but hash search failed", p);

   // tried to free something that wasn't allocated!
   n = stb__historyfind(p);
   if (n >= 0)
      stb_fatal("Checked %d-byte block %p previously freed/realloced at %s:%d",
                    stb__alloc_history[n].size, p,
                    stb__alloc_history[n].file, stb__alloc_history[n].line);
   stb_fatal("Checked unknown block %p");
}

void stb_wrapper_realloc(void *p, void *q, int sz, char *file, int line)
{
   int n;
   if (p == NULL) { stb_wrapper_malloc(q, sz, file, line); return; }
   if (q == NULL) return; // nothing happened

   n = stb__hashfind(p);
   if (n == -1) {
      // tried to free something we hadn't allocated!
      // this is weird, though, because we got past the realloc!
      n = stb__historyfind(p);
      assert(0); /* NOTREACHED */
      if (n >= 0)
         stb_fatal("Attempted to realloc %d-byte block %p at %s:%d previously freed/realloced at %s:%d",
                       stb__alloc_history[n].size, p,
                       file, line,
                       stb__alloc_history[n].file, stb__alloc_history[n].line);
      else
         stb_fatal("Attempted to realloc unknown block %p at %s:%d", p, file,line);
   } else {
      if (q == p) {
         stb__allocations[n].size = sz;
         stb__allocations[n].file = file;
         stb__allocations[n].line = line;
      } else {
         stb__remove_alloc(n, file, line);
         stb__add_alloc(q,sz,file,line);
      }
   }
}

void stb_wrapper_listall(void (*func)(void *ptr, int sz, char *file, int line))
{
   int i;
   for (i=0; i < stb__alloc_size; ++i)
      if (stb__allocations[i].p > STB_DEL)
         func(stb__allocations[i].p   , stb__allocations[i].size,
              stb__allocations[i].file, stb__allocations[i].line);
}

void stb_wrapper_dump(char *filename)
{
   int i;
   FILE *f = fopen(filename, "w");
   if (!f) return;
   for (i=0; i < stb__alloc_size; ++i)
      if (stb__allocations[i].p > STB_DEL)
         fprintf(f, "%p %7d - %4d %s\n",
            stb__allocations[i].p   , stb__allocations[i].size,
            stb__allocations[i].line, stb__allocations[i].file);
}
#endif // STB_DEFINE


//////////////////////////////////////////////////////////////////////////////
//
//                  stb_pointer_set
//
//
// For data structures that support querying by key, data structure
// classes always hand-wave away the issue of what to do if two entries
// have the same key: basically, store a linked list of all the nodes
// which have the same key (a LISP-style list).
//
// The thing is, it's not that trivial. If you have an O(log n)
// lookup data structure, but then n/4 items have the same value,
// you don't want to spend O(n) time scanning that list when
// deleting an item if you already have a pointer to the item.
// (You have to spend O(n) time enumerating all the items with
// a given key, sure, and you can't accelerate deleting a particular
// item if you only have the key, not a pointer to the item.)
//
// I'm going to call this data structure, whatever it turns out to
// be, a "pointer set", because we don't store any associated data for
// items in this data structure, we just answer the question of
// whether an item is in it or not (it's effectively one bit per pointer).
// Technically they don't have to be pointers; you could cast ints
// to (void *) if you want, but you can't store 0 or 1 because of the
// hash table.
//
// Since the fastest data structure we might want to add support for
// identical-keys to is a hash table with O(1)-ish lookup time,
// that means that the conceptual "linked list of all items with
// the same indexed value" that we build needs to have the same
// performance; that way when we index a table we think is arbitrary
// ints, but in fact half of them are 0, we don't get screwed.
//
// Therefore, it needs to be a hash table, at least when it gets
// large. On the other hand, when the data has totally arbitrary ints
// or floats, there won't be many collisions, and we'll have tons of
// 1-item bitmaps. That will be grossly inefficient as hash tables;
// trade-off; the hash table is reasonably efficient per-item when
// it's large, but not when it's small. So we need to do something
// Judy-like and use different strategies depending on the size.
//
// Like Judy, we'll use the bottom bit to encode the strategy:
//
//      bottom bits:
//          00     -   direct tuple pointer
//          01     -   4-item bucket (16 bytes, no length, NULLs)
//          10     -   N-item array
//          11     -   hash table

typedef struct stb_ps stb_ps;

STB_EXTERN int      stb_ps_find  (stb_ps *ps, void *value);
STB_EXTERN stb_ps * stb_ps_add   (stb_ps *ps, void *value);
STB_EXTERN stb_ps * stb_ps_remove(stb_ps *ps, void *value);
STB_EXTERN stb_ps * stb_ps_remove_any(stb_ps *ps, void **value);
STB_EXTERN void     stb_ps_delete(stb_ps *ps);
STB_EXTERN int      stb_ps_count (stb_ps *ps);

STB_EXTERN stb_ps * stb_ps_copy  (stb_ps *ps);
STB_EXTERN int      stb_ps_subset(stb_ps *bigger, stb_ps *smaller);
STB_EXTERN int      stb_ps_eq    (stb_ps *p0, stb_ps *p1);

STB_EXTERN void ** stb_ps_getlist  (stb_ps *ps, int *count);
STB_EXTERN int     stb_ps_writelist(stb_ps *ps, void **list, int size );

// enum and fastlist don't allocate storage, but you must consume the
// list before there's any chance the data structure gets screwed up;
STB_EXTERN int     stb_ps_enum     (stb_ps *ps, void *data,
                                       int (*func)(void *value, void*data) );
STB_EXTERN void ** stb_ps_fastlist(stb_ps *ps, int *count);
//  result:
//     returns a list, *count is the length of that list,
//     but some entries of the list may be invalid;
//     test with 'stb_ps_fastlist_valid(x)'

#define stb_ps_fastlist_valid(x)   ((unsigned int) (x) > 1)

#ifdef STB_DEFINE

enum
{
   STB_ps_direct = 0,
   STB_ps_bucket = 1,
   STB_ps_array  = 2,
   STB_ps_hash   = 3,
};

#define STB_BUCKET_SIZE  4

typedef struct
{
   void *p[STB_BUCKET_SIZE];
} stb_ps_bucket;
#define GetBucket(p)    ((stb_ps_bucket *) ((char *) (p) - STB_ps_bucket))
#define EncodeBucket(p) ((stb_ps *) ((char *) (p) + STB_ps_bucket))

typedef char stb__verify_bucket_heap_size[sizeof(stb_ps_bucket) == 16];

static void stb_bucket_free(stb_ps_bucket *b)
{
   free(b);
}

static stb_ps_bucket *stb_bucket_create2(void *v0, void *v1)
{
   stb_ps_bucket *b = (stb_ps_bucket*) malloc(sizeof(*b));
   b->p[0] = v0;
   b->p[1] = v1;
   b->p[2] = NULL;
   b->p[3] = NULL;
   return b;
}

static stb_ps_bucket * stb_bucket_create3(void **v)
{
   stb_ps_bucket *b = (stb_ps_bucket*) malloc(sizeof(*b));
   b->p[0] = v[0];
   b->p[1] = v[1];
   b->p[2] = v[2];
   b->p[3] = NULL;
   return b;
}


// could use stb_arr, but this will save us memory
typedef struct
{
   int count;
   void *p[1];
} stb_ps_array;
#define GetArray(p)     ((stb_ps_array *) ((char *) (p) - STB_ps_array))
#define EncodeArray(p)  ((stb_ps *) ((char *) (p) + STB_ps_array))

static int stb_ps_array_max = 13;

typedef struct
{
   int size, mask;
   int count, count_deletes;
   int grow_threshhold;
   int shrink_threshhold;
   int rehash_threshhold;
   int any_offset;
   void *table[1];
} stb_ps_hash;
#define GetHash(p)      ((stb_ps_hash *) ((char *) (p) - STB_ps_hash))
#define EncodeHash(p)   ((stb_ps *) ((char *) (p) + STB_ps_hash))

#define stb_ps_empty(v)   (((uint32) v) <= 1)

static stb_ps_hash *stb_ps_makehash(int size, int old_size, void **old_data)
{
   int i;
   stb_ps_hash *h = (stb_ps_hash *) malloc(sizeof(*h) + (size-1) * sizeof(h->table[0]));
   assert(stb_is_pow2(size));
   h->size = size;
   h->mask = size-1;
   h->shrink_threshhold = (int) (0.3f * size);
   h->  grow_threshhold = (int) (0.8f * size);
   h->rehash_threshhold = (int) (0.9f * size);
   h->count = 0;
   h->count_deletes = 0;
   h->any_offset = 0;
   memset(h->table, 0, size * sizeof(h->table[0]));
   for (i=0; i < old_size; ++i)
      if (!stb_ps_empty(old_data[i]))
         stb_ps_add(EncodeHash(h), old_data[i]);
   return h;
}

void stb_ps_delete(stb_ps *ps)
{
   switch (3 & (int) ps) {
      case STB_ps_direct: break;
      case STB_ps_bucket: stb_bucket_free(GetBucket(ps)); break;
      case STB_ps_array : free(GetArray(ps)); break;
      case STB_ps_hash  : free(GetHash(ps)); break;
   }
}

stb_ps *stb_ps_copy(stb_ps *ps)
{
   int i;
   // not a switch: order based on expected performance/power-law distribution
   switch (3 & (int) ps) {
      case STB_ps_direct: return ps;
      case STB_ps_bucket: {
         stb_ps_bucket *n = (stb_ps_bucket *) malloc(sizeof(*n));
         *n = *GetBucket(ps);
         return EncodeBucket(n);
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         stb_ps_array *n = (stb_ps_array *) malloc(sizeof(*n) + stb_ps_array_max * sizeof(n->p[0]));
         n->count = a->count;
         for (i=0; i < a->count; ++i)
            n->p[i] = a->p[i];
         return EncodeArray(n);
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         stb_ps_hash *n = stb_ps_makehash(h->size, h->size, h->table);
         return EncodeHash(n);
      }
   }
   assert(0); /* NOTREACHED */
   return NULL;
}

int stb_ps_find(stb_ps *ps, void *value)
{
   int i, code = 3 & (int) ps;
    assert((3 & (int) value) == STB_ps_direct);
   assert(stb_ps_fastlist_valid(value));
   // not a switch: order based on expected performance/power-law distribution
   if (code == STB_ps_direct)
      return value == ps;
   if (code == STB_ps_bucket) {
      stb_ps_bucket *b = GetBucket(ps);
      assert(STB_BUCKET_SIZE == 4);
      if (b->p[0] == value || b->p[1] == value ||
          b->p[2] == value || b->p[3] == value)
          return TRUE;
      return FALSE;
   }
   if (code == STB_ps_array) {
      stb_ps_array *a = GetArray(ps);
      for (i=0; i < a->count; ++i)
         if (a->p[i] == value)
            return TRUE;
      return FALSE;
   } else {
      stb_ps_hash *h = GetHash(ps);
      uint32 hash = stb_hashptr(value);
      uint32 s, n = hash & h->mask;
      void **t = h->table;
      if (t[n] == value) return TRUE;
      if (t[n] == NULL) return FALSE;
      s = stb_rehash(hash) | 1;
      do {
         n = (n + s) & h->mask;
         if (t[n] == value) return TRUE;
      } while (t[n] != NULL);
      return FALSE;
   }
}

stb_ps *  stb_ps_add   (stb_ps *ps, void *value)
{
   #ifdef STB_DEBUG
   assert(!stb_ps_find(ps,value));
   #endif
   if (value == NULL) return ps; // ignore NULL adds to avoid bad breakage
   assert((3 & (int) value) == STB_ps_direct);
   assert(stb_ps_fastlist_valid(value));
   assert(value != STB_DEL);     // STB_DEL is less likely

   switch (3 & (int) ps) {
      case STB_ps_direct:
         if (ps == NULL) return (stb_ps *) value;
         return EncodeBucket(stb_bucket_create2(ps,value));

      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         stb_ps_array  *a;
         assert(STB_BUCKET_SIZE == 4);
         if (b->p[0] == NULL) { b->p[0] = value; return ps; }
         if (b->p[1] == NULL) { b->p[1] = value; return ps; }
         if (b->p[2] == NULL) { b->p[2] = value; return ps; }
         if (b->p[3] == NULL) { b->p[3] = value; return ps; }
         a = (stb_ps_array *) malloc(sizeof(*a) + 7 * sizeof(a->p[0])); // 8 slots, must be 2^k
         memcpy(a->p, b, sizeof(*b));
         a->p[4] = value;
         a->count = 5;
         stb_bucket_free(b);
         return EncodeArray(a);
      }

      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         if (a->count == stb_ps_array_max) {
            // promote from array to hash
            stb_ps_hash *h = stb_ps_makehash(2 << stb_log2_ceil(a->count), a->count, a->p);
            free(a);
            return stb_ps_add(EncodeHash(h), value);
         }
         // do we need to resize the array? the array doubles in size when it
         // crosses a power-of-two
         if ((a->count & (a->count-1))==0) {
            int newsize = a->count*2;
            // clamp newsize to max if:
            //    1. it's larger than max
            //    2. newsize*1.5 is larger than max (to avoid extra resizing)
            if (newsize + a->count > stb_ps_array_max)
               newsize = stb_ps_array_max;
            a = (stb_ps_array *) realloc(a, sizeof(*a) + (newsize-1) * sizeof(a->p[0]));
         }
         a->p[a->count++] = value;
         return EncodeArray(a);
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         uint32 hash = stb_hashptr(value);
         uint32 n = hash & h->mask;
         void **t = h->table;
         // find first NULL or STB_DEL entry
         if (!stb_ps_empty(t[n])) {
            uint32 s = stb_rehash(hash) | 1;
            do {
               n = (n + s) & h->mask;
            } while (!stb_ps_empty(t[n]));
         }
         if (t[n] == STB_DEL)
            -- h->count_deletes;
         t[n] = value;
         ++ h->count;
         if (h->count == h->grow_threshhold) {
            stb_ps_hash *h2 = stb_ps_makehash(h->size*2, h->size, t);
            free(h);
            return EncodeHash(h2);
         }
         if (h->count + h->count_deletes == h->rehash_threshhold) {
            stb_ps_hash *h2 = stb_ps_makehash(h->size, h->size, t);
            free(h);
            return EncodeHash(h2);
         }
         return ps;
      }
   }
   return NULL; /* NOTREACHED */
}

stb_ps *stb_ps_remove(stb_ps *ps, void *value)
{
   #ifdef STB_DEBUG
   assert(stb_ps_find(ps, value));
   #endif
   assert((3 & (int) value) == STB_ps_direct);
   if (value == NULL) return ps; // ignore NULL removes to avoid bad breakage
   switch (3 & (int) ps) {
      case STB_ps_direct:
         return ps == value ? NULL : ps;
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         int count=0;
         assert(STB_BUCKET_SIZE == 4);
         if (b->p[0] == value) b->p[0] = NULL; else count += (b->p[0] != NULL);
         if (b->p[1] == value) b->p[1] = NULL; else count += (b->p[1] != NULL);
         if (b->p[2] == value) b->p[2] = NULL; else count += (b->p[2] != NULL);
         if (b->p[3] == value) b->p[3] = NULL; else count += (b->p[3] != NULL);
         if (count == 1) { // shrink bucket at size 1
            value = b->p[0];
            if (value == NULL) value = b->p[1];
            if (value == NULL) value = b->p[2];
            if (value == NULL) value = b->p[3];
            assert(value != NULL);
            stb_bucket_free(b);
            return (stb_ps *) value; // return STB_ps_direct of value
         }
         return ps;
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         int i;
         for (i=0; i < a->count; ++i) {
            if (a->p[i] == value) {
               a->p[i] = a->p[--a->count];
               if (a->count == 3) { // shrink to bucket!
                  stb_ps_bucket *b = stb_bucket_create3(a->p);
                  free(a);
                  return EncodeBucket(b);
               }
               return ps;
            }
         }
         return ps;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         uint32 hash = stb_hashptr(value);
         uint32 s, n = hash & h->mask;
         void **t = h->table;
         if (t[n] != value) {
            s = stb_rehash(hash) | 1;
            do {
               n = (n + s) & h->mask;
            } while (t[n] != value);
         }
         t[n] = STB_DEL;
         -- h->count;
         ++ h->count_deletes;
         // should we shrink down to an array?
         if (h->count < stb_ps_array_max) {
            int n = 1 << stb_log2_floor(stb_ps_array_max);
            if (h->count < n) {
               stb_ps_array *a = (stb_ps_array *) malloc(sizeof(*a) + (n-1) * sizeof(a->p[0]));
               int i,j=0;
               for (i=0; i < h->size; ++i)
                  if (!stb_ps_empty(t[i]))
                     a->p[j++] = t[i];
               assert(j == h->count);
               a->count = j;
               free(h);
               return EncodeArray(a);
            }
         }
         if (h->count == h->shrink_threshhold) {
            stb_ps_hash *h2 = stb_ps_makehash(h->size >> 1, h->size, t);
            free(h);
            return EncodeHash(h2);
         }
         return ps;
      }
   }
   return ps; /* NOTREACHED */
}

stb_ps *stb_ps_remove_any(stb_ps *ps, void **value)
{
   assert(ps != NULL);
   switch (3 & (int) ps) {
      case STB_ps_direct:
         *value = ps;
         return NULL;
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         int count=0, slast=0, last=0;
         assert(STB_BUCKET_SIZE == 4);
         if (b->p[0]) { ++count;               last = 0; }
         if (b->p[1]) { ++count; slast = last; last = 1; }
         if (b->p[2]) { ++count; slast = last; last = 2; }
         if (b->p[3]) { ++count; slast = last; last = 3; }
         *value = b->p[last];
         b->p[last] = 0;
         if (count == 2) {
            void *leftover = b->p[slast]; // second to last
            stb_bucket_free(b);
            return (stb_ps *) leftover; 
         }
         return ps;
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         *value = a->p[a->count-1];
         if (a->count == 4)
            return stb_ps_remove(ps, *value);
         --a->count;
         return ps;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         void **t = h->table;
         uint32 n = h->any_offset;
         while (stb_ps_empty(t[n]))
            n = (n + 1) & h->mask;
         *value = t[n];
         h->any_offset = (n+1) & h->mask;
         // check if we need to skip down to the previous type
         if (h->count-1 < stb_ps_array_max || h->count-1 == h->shrink_threshhold)
            return stb_ps_remove(ps, *value);
         t[n] = STB_DEL;
         -- h->count;
         ++ h->count_deletes;
         return ps;
      }
   }
   return ps; /* NOTREACHED */
}


void ** stb_ps_getlist(stb_ps *ps, int *count)
{
   int i,n=0;
   void **p;
   switch (3 & (int) ps) {
      case STB_ps_direct:
         if (ps == NULL) { *count = 0; return NULL; }
         p = (void **) malloc(sizeof(*p) * 1);
         p[0] = ps;
         *count = 1;
         return p;
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         p = (void **) malloc(sizeof(*p) * STB_BUCKET_SIZE);
         for (i=0; i < STB_BUCKET_SIZE; ++i)
            if (b->p[i] != NULL)
               p[n++] = b->p[i];
         break;
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         p = (void **) malloc(sizeof(*p) * a->count);
         memcpy(p, a->p, sizeof(*p) * a->count);
         *count = a->count;
         return p;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         p = (void **) malloc(sizeof(*p) * h->count);
         for (i=0; i < h->size; ++i)
            if (!stb_ps_empty(h->table[i]))
               p[n++] = h->table[i];
         break;
      }
   }
   *count = n;
   return p;
}

int stb_ps_writelist(stb_ps *ps, void **list, int size )
{
   int i,n=0;
   switch (3 & (int) ps) {
      case STB_ps_direct:
         if (ps == NULL || size <= 0) return 0;
         list[0] = ps;
         return 1;
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         for (i=0; i < STB_BUCKET_SIZE; ++i)
            if (b->p[i] != NULL && n < size)
               list[n++] = b->p[i];
         return n;
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         n = stb_min(size, a->count);
         memcpy(list, a->p, sizeof(*list) * n);
         return n;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         if (size <= 0) return 0;
         for (i=0; i < h->count; ++i) {
            if (!stb_ps_empty(h->table[i])) {
               list[n++] = h->table[i];
               if (n == size) break;
            }
         }
         return n;
      }
   }
   return 0; /* NOTREACHED */
}

int stb_ps_enum(stb_ps *ps, void *data, int (*func)(void *value, void *data))
{
   int i;
   switch (3 & (int) ps) {
      case STB_ps_direct:
         if (ps == NULL) return TRUE;
         return func(ps, data);
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         for (i=0; i < STB_BUCKET_SIZE; ++i)
            if (b->p[i] != NULL)
               if (!func(b->p[i], data))
                  return FALSE;
         return TRUE;
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         for (i=0; i < a->count; ++i)
            if (!func(a->p[i], data))
               return FALSE;
         return TRUE;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         for (i=0; i < h->count; ++i)
            if (!stb_ps_empty(h->table[i]))
               if (!func(h->table[i], data))
                  return FALSE;
         return TRUE;
      }
   }
   return TRUE; /* NOTREACHED */
}

int stb_ps_count (stb_ps *ps)
{
   switch (3 & (int) ps) {
      case STB_ps_direct:
         return ps != NULL;
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         return (b->p[0] != NULL) + (b->p[1] != NULL) +
                (b->p[2] != NULL) + (b->p[3] != NULL);
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         return a->count;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         return h->count;
      }
   }
   return 0;
}

void ** stb_ps_fastlist(stb_ps *ps, int *count)
{
   static void *storage;

   switch (3 & (int) ps) {
      case STB_ps_direct:
         if (ps == NULL) { *count = 0; return NULL; }
         storage = ps;
         *count = 1;
         return &storage;
      case STB_ps_bucket: {
         stb_ps_bucket *b = GetBucket(ps);
         *count = STB_BUCKET_SIZE;
         return b->p;
      }
      case STB_ps_array: {
         stb_ps_array *a = GetArray(ps);
         *count = a->count;
         return a->p;
      }
      case STB_ps_hash: {
         stb_ps_hash *h = GetHash(ps);
         *count = h->size;
         return h->table;
      }
   }
   return NULL; /* NOTREACHED */
}

int stb_ps_subset(stb_ps *bigger, stb_ps *smaller)
{
   int i, listlen;
   void **list = stb_ps_fastlist(smaller, &listlen);
   for(i=0; i < listlen; ++i)
      if (stb_ps_fastlist_valid(list[i]))
         if (!stb_ps_find(bigger, list[i]))
            return 0;
   return 1;
}

int stb_ps_eq(stb_ps *p0, stb_ps *p1)
{
   if (stb_ps_count(p0) != stb_ps_count(p1))
      return 0;
   return stb_ps_subset(p0, p1);
}

#undef GetBucket
#undef GetArray
#undef GetHash

#undef EncodeBucket
#undef EncodeArray
#undef EncodeHash

#endif


//////////////////////////////////////////////////////////////////////////////
//
//                        stb_dupe
//
// stb_dupe is a duplicate-finding system for very, very large data
// structures--large enough that sorting is too slow, but not so large
// that we can't keep all the data in memory. using it works as follows:
//
//     1. create an stb_dupe:
//          provide a hash function
//          provide an equality function
//          provide an estimate for the size
//          optionally provide a comparison function
//
//     2. traverse your data, 'adding' pointers to the stb_dupe
//
//     3. finish and ask for duplicates
//
//        the stb_dupe will discard its intermediate data and build
//        a collection of sorted lists of duplicates, with non-duplicate
//        entries omitted entirely
//
//
//  Implementation strategy:
//
//     while collecting the N items, we keep a hash table of approximate
//     size sqrt(N). (if you tell use the N up front, the hash table is
//     just that size exactly)
//
//     each entry in the hash table is just an stb__arr of pointers (no need
//     to use stb_ps, because we don't need to delete from these)
//
//     for step 3, for each entry in the hash table, we apply stb_dupe to it
//     recursively. once the size gets small enough (or doesn't decrease
//     significantly), we switch to either using qsort() on the comparison
//     function, or else we just do the icky N^2 gather


typedef struct stb_dupe stb_dupe;

typedef int (*stb_compare_func)(void *a, void *b);
typedef int (*stb_hash_func)(void *a);

STB_EXTERN void stb_dupe_free(stb_dupe *sd);
STB_EXTERN stb_dupe *stb_dupe_create(stb_hash_func hash,
                          stb_compare_func eq, int size, stb_compare_func ineq);
STB_EXTERN void stb_dupe_add(stb_dupe *sd, void *item);
STB_EXTERN void stb_dupe_finish(stb_dupe *sd);
STB_EXTERN int stb_dupe_numsets(stb_dupe *sd);
STB_EXTERN void **stb_dupe_set(stb_dupe *sd, int num);
STB_EXTERN int stb_dupe_set_count(stb_dupe *sd, int num);

#ifdef STB_DEFINE

struct stb_dupe
{
   STB__ARR(void *) *hash_table;
   int hash_size;
   int size_log2;
   int population;

   int hash_shift;
   stb_hash_func     hash;

   stb_compare_func  eq;
   stb_compare_func  ineq;

   STB__ARR(STB__ARR(void*)) dupes;
};

int stb_dupe_numsets(stb_dupe *sd)
{
   assert(sd->hash_table == NULL);
   return stb_arr_len(sd->dupes);
}

void **stb_dupe_set(stb_dupe *sd, int num)
{
   assert(sd->hash_table == NULL);
   return sd->dupes[num];
}

int stb_dupe_set_count(stb_dupe *sd, int num)
{
   assert(sd->hash_table == NULL);
   return stb_arr_len(sd->dupes[num]);
}

stb_dupe *stb_dupe_create(stb_hash_func hash, stb_compare_func eq, int size,
                                              stb_compare_func ineq)
{
   int i, hsize;
   stb_dupe *sd = (stb_dupe *) malloc(sizeof(*sd));

   sd->size_log2 = 4;
   hsize = 1 << sd->size_log2;
   while (hsize * hsize < size) {
      ++sd->size_log2;
      hsize *= 2;
   }

   sd->hash = hash;
   sd->eq   = eq;
   sd->ineq = ineq;
   sd->hash_shift = 0;

   sd->population = 0;
   sd->hash_size = hsize;
   sd->hash_table = (STB__ARR(void *)*) malloc(sizeof(*sd->hash_table) * hsize);
   for (i=0; i < hsize; ++i)
      sd->hash_table[i] = NULL;

   sd->dupes = NULL;

   return sd;
}

void stb_dupe_add(stb_dupe *sd, void *item)
{
   uint32 hash = sd->hash(item) >> sd->hash_shift;
   int z = hash & (sd->hash_size-1);
   stb_arr_push(sd->hash_table[z], item);
   ++sd->population;
}

void stb_dupe_free(stb_dupe *sd)
{
   int i;
   for (i=0; i < stb_arr_len(sd->dupes); ++i)
      if (sd->dupes[i])
         stb_arr_free(sd->dupes[i]);
   stb_arr_free(sd->dupes);
   free(sd);
}

static stb_compare_func stb__compare;

static int stb__dupe_compare(const void *a, const void *b)
{
   void *p = *(void **) a;
   void *q = *(void **) b;

   return stb__compare(p,q);
}

void stb_dupe_finish(stb_dupe *sd)
{
   int i,j,k;
   assert(sd->dupes == NULL);
   for (i=0; i < sd->hash_size; ++i) {
      STB__ARR(void *) list = sd->hash_table[i];
      if (list != NULL) {                                          
         int n = stb_arr_len(list);
         // @TODO: measure to find good numbers instead of just making them up!
         int thresh = (sd->ineq ? 200 : 20);
         // if n is large enough to be worth it, and n is smaller than
         // before (so we can guarantee we'll use a smaller hash table);
         // and there are enough hash bits left, assuming full 32-bit hash
         if (n > thresh && n < (sd->population >> 3) && sd->hash_shift + sd->size_log2*2 < 32) {

            // recursively process this row using stb_dupe, O(N log log N)

            stb_dupe *d = stb_dupe_create(sd->hash, sd->eq, n, sd->ineq);
            d->hash_shift = sd->hash_shift + sd->size_log2;
            for (j=0; j < n; ++j)
               stb_dupe_add(d, list[j]);
            sd->hash_table[i] = stb_arr_free(sd->hash_table[i]);
            stb_dupe_finish(d);
            for (j=0; j < stb_arr_len(d->dupes); ++j) {
               stb_arr_push(sd->dupes, d->dupes[j]);
               d->dupes[j] = NULL; // take over ownership
            }
            stb_dupe_free(d);

         } else if (sd->ineq) {

            // process this row using qsort(), O(N log N)
            stb__compare = sd->ineq;
            qsort(list, n, sizeof(list[0]), stb__dupe_compare);

            // find equal subsequences of the list
            for (j=0; j < n-1; ) {
               // find a subsequence from j..k
               for (k=j; k < n; ++k)
                  // only use ineq so eq can be left undefined
                  if (sd->ineq(list[j], list[k]))
                     break;
               // k is the first one not in the subsequence
               if (k-j > 1) {
                  STB__ARR(void *) mylist = NULL;
                  stb_arr_setlen(mylist, k-j);
                  memcpy(mylist, list+j, sizeof(list[j]) * (k-j));
                  stb_arr_push(sd->dupes, mylist);
               }
               j = k;
            }
            sd->hash_table[i] = stb_arr_free(sd->hash_table[i]);
         } else {

            // process this row using eq(), O(N^2)
            for (j=0; j < n; ++j) {
               if (list[j] != NULL) {
                  STB__ARR(void *) output  = NULL;
                  for (k=j+1; k < n; ++k) {
                     if (sd->eq(list[j], list[k])) {
                        if (output == NULL)
                           stb_arr_push(output, list[j]);
                        stb_arr_push(output, list[k]);
                        list[k] = NULL;
                     }
                  }
                  list[j] = NULL;
                  if (output)
                     stb_arr_push(sd->dupes, output);
               }
            }
            sd->hash_table[i] = stb_arr_free(sd->hash_table[i]);
         }
      }
   }
   free(sd->hash_table);
   sd->hash_table = NULL;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//                       templatized Sort routine
//
// This is an attempt to implement a templated sorting algorithm.
// To use it, you have to explicitly instantiate it as a _function_,
// then you call that function. This allows the comparison to be inlined,
// giving the sort similar performance to C++ sorts.
//
// It implements quicksort with three-way-median partitioning (generally
// well-behaved), with a final insertion sort pass.
//
// When you define the compare expression, you should assume you have
// elements of your array pointed to by 'a' and 'b', and perform the comparison
// on those. OR you can use one or more statements; first say '0;', then
// write whatever code you want, and compute the result into a variable 'c'.

#define stb_declare_sort(FUNCNAME, TYPE)    \
                       void FUNCNAME(TYPE *p, int n)
#define stb_define_sort(FUNCNAME,TYPE,COMPARE) \
                       stb__define_sort(       void, FUNCNAME,TYPE,COMPARE)
#define stb_define_sort_static(FUNCNAME,TYPE,COMPARE) \
                       stb__define_sort(static void, FUNCNAME,TYPE,COMPARE)

#define stb__define_sort(MODE, FUNCNAME, TYPE, COMPARE)                       \
                                                                              \
static void STB_(FUNCNAME,_ins_sort)(TYPE *p, int n)                          \
{                                                                             \
   int i,j;                                                                   \
   for (i=1; i < n; ++i) {                                                    \
      TYPE t = p[i], *a = &t;                                                 \
      j = i;                                                                  \
      while (j > 0) {                                                         \
         TYPE *b = &p[j-1];                                                   \
         int c = COMPARE;                                                     \
         if (!c) break;                                                       \
         p[j] = p[j-1];                                                       \
         --j;                                                                 \
      }                                                                       \
      if (i != j)                                                             \
         p[j] = t;                                                            \
   }                                                                          \
}                                                                             \
                                                                              \
static void STB_(FUNCNAME,_quicksort)(TYPE *p, int n)                         \
{                                                                             \
   /* threshhold for transitioning to insertion sort */                       \
   while (n > 12) {                                                           \
      TYPE *a,*b,t;                                                           \
      int c01,c12,c,m,i,j;                                                    \
                                                                              \
      /* compute median of three */                                           \
      m = n >> 1;                                                             \
      a = &p[0];                                                              \
      b = &p[m];                                                              \
      c = COMPARE;                                                            \
      c01 = c;                                                                \
      a = &p[m];                                                              \
      b = &p[n-1];                                                            \
      c = COMPARE;                                                            \
      c12 = c;                                                                \
      /* if 0 >= mid >= end, or 0 < mid < end, then use mid */                \
      if (c01 != c12) {                                                       \
         /* otherwise, we'll need to swap something else to middle */         \
         int z;                                                               \
         a = &p[0];                                                           \
         b = &p[n-1];                                                         \
         c = COMPARE;                                                         \
         /* 0>mid && mid<n:  0>n => n; 0<n => 0 */                            \
         /* 0<mid && mid>n:  0>n => 0; 0<n => n */                            \
         z = (c == c12) ? 0 : n-1;                                            \
         t = p[z];                                                            \
         p[z] = p[m];                                                         \
         p[m] = t;                                                            \
      }                                                                       \
      /* now p[m] is the median-of-three */                                   \
      /* swap it to the beginning so it won't move around */                  \
      t = p[0];                                                               \
      p[0] = p[m];                                                            \
      p[m] = t;                                                               \
                                                                              \
      /* partition loop */                                                    \
      i=1;                                                                    \
      j=n-1;                                                                  \
      for(;;) {                                                               \
         /* handling of equality is crucial here */                           \
         /* for sentinels & efficiency with duplicates */                     \
         b = &p[0];                                                           \
         for (;;++i) {                                                        \
            a=&p[i];                                                          \
            c = COMPARE;                                                      \
            if (!c) break;                                                    \
         }                                                                    \
         a = &p[0];                                                           \
         for (;;--j) {                                                        \
            b=&p[j];                                                          \
            c = COMPARE;                                                      \
            if (!c) break;                                                    \
         }                                                                    \
         /* make sure we haven't crossed */                                   \
         if (i >= j) break;                                                   \
         t = p[i];                                                            \
         p[i] = p[j];                                                         \
         p[j] = t;                                                            \
                                                                              \
         ++i;                                                                 \
         --j;                                                                 \
      }                                                                       \
      /* recurse on smaller side, iterate on larger */                        \
      if (j < (n-i)) {                                                        \
         STB_(FUNCNAME,_quicksort)(p,j);                                       \
         p = p+i;                                                             \
         n = n-i;                                                             \
      } else {                                                                \
         STB_(FUNCNAME,_quicksort)(p+i, n-i);                                  \
         n = j;                                                               \
      }                                                                       \
   }                                                                          \
}                                                                             \
                                                                              \
MODE FUNCNAME(TYPE *p, int n)                                                 \
{                                                                             \
   STB_(FUNCNAME, _quicksort)(p, n);                                           \
   STB_(FUNCNAME, _ins_sort)(p, n);                                            \
}                                                                             \

//////////////////////////////////////////////////////////////////////////////
//
//               Random Numbers via Meresenne Twister or LCG
//

STB_EXTERN unsigned long stb_srandLCG(unsigned long seed);
STB_EXTERN unsigned long stb_randLCG(void);
STB_EXTERN double        stb_frandLCG(void);

STB_EXTERN void          stb_srand(unsigned long seed);
STB_EXTERN unsigned long stb_rand(void);
STB_EXTERN double        stb_frand(void);
STB_EXTERN void          stb_shuffle(void *p, size_t n, size_t sz,
                                        unsigned long seed);
STB_EXTERN void stb_reverse(void *p, size_t n, size_t sz);

#define stb_rand_define(x,y)                                         \
                                                                     \
   unsigned long x(void)                                             \
   {                                                                 \
      static unsigned long stb__rand = y;                            \
      stb__rand = stb__rand * 2147001325 + 715136305; /* BCPL */     \
      return 0x31415926 ^ ((stb__rand >> 16) + (stb__rand << 16));   \
   }


#ifdef STB_DEFINE
static unsigned long stb__rand_seed=0;

unsigned long stb_srandLCG(unsigned long seed)
{
   unsigned long previous = stb__rand_seed;
   stb__rand_seed = seed;
   return previous;
}

unsigned long stb_randLCG(void)
{
   stb__rand_seed = stb__rand_seed * 2147001325 + 715136305; // BCPL generator
   // shuffle non-random bits to the middle, and xor to decorrelate with seed
   return 0x31415926 ^ ((stb__rand_seed >> 16) + (stb__rand_seed << 16));
}

double stb_frandLCG(void)
{
   return stb_randLCG() / ((double) (1 << 16) * (1 << 16));
}

void stb_shuffle(void *p, size_t n, size_t sz, unsigned long seed)
{
   char *a;
   unsigned long old_seed;
   int i;
   if (seed)
      old_seed = stb_srandLCG(seed);
   a = (char *) p + (n-1) * sz;

   for (i=n; i > 1; --i) {
      int j = stb_randLCG() % i;
      stb_swap(a, (char *) p + j * sz, sz);
      a -= sz;
   }
   if (seed)
      stb_srandLCG(old_seed);
}

void stb_reverse(void *p, size_t n, size_t sz)
{
   int i,j = n-1;
   for (i=0; i < j; ++i,--j) {
      stb_swap((char *) p + i * sz, (char *) p + j * sz, sz);
   }
}

// public domain Mersenne Twister by Michael Brundage
#define STB__MT_LEN       624

int stb__mt_index = STB__MT_LEN*sizeof(unsigned long)+1;
unsigned long stb__mt_buffer[STB__MT_LEN];

void stb_srand(unsigned long seed)
{
   int i;
   unsigned long old = stb_srandLCG(seed);
   for (i = 0; i < STB__MT_LEN; i++)
      stb__mt_buffer[i] = stb_randLCG();
   stb_srandLCG(old);
   stb__mt_index = STB__MT_LEN*sizeof(unsigned long);
}

#define STB__MT_IA           397
#define STB__MT_IB           (STB__MT_LEN - STB__MT_IA)
#define STB__UPPER_MASK      0x80000000
#define STB__LOWER_MASK      0x7FFFFFFF
#define STB__MATRIX_A        0x9908B0DF
#define STB__TWIST(b,i,j)    ((b)[i] & STB__UPPER_MASK) | ((b)[j] & STB__LOWER_MASK)
#define STB__MAGIC(s)        (((s)&1)*STB__MATRIX_A)

unsigned long stb_rand()
{
   unsigned long * b = stb__mt_buffer;
   int idx = stb__mt_index;
   unsigned long s,r;
   int i;
	
   if (idx >= STB__MT_LEN*sizeof(unsigned long)) {
      if (idx > STB__MT_LEN*sizeof(unsigned long))
         stb_srand(0);
      idx = 0;
      i = 0;
      for (; i < STB__MT_IB; i++) {
         s = STB__TWIST(b, i, i+1);
         b[i] = b[i + STB__MT_IA] ^ (s >> 1) ^ STB__MAGIC(s);
      }
      for (; i < STB__MT_LEN-1; i++) {
         s = STB__TWIST(b, i, i+1);
         b[i] = b[i - STB__MT_IB] ^ (s >> 1) ^ STB__MAGIC(s);
      }
      
      s = STB__TWIST(b, STB__MT_LEN-1, 0);
      b[STB__MT_LEN-1] = b[STB__MT_IA-1] ^ (s >> 1) ^ STB__MAGIC(s);
   }
   stb__mt_index = idx + sizeof(unsigned long);
   
   r = *(unsigned long *)((unsigned char *)b + idx);
   
   r ^= (r >> 11);
   r ^= (r << 7) & 0x9D2C5680;
   r ^= (r << 15) & 0xEFC60000;
   r ^= (r >> 18);
   
   return r;
}

double stb_frand(void)
{
   return stb_rand() / ((double) (1 << 16) * (1 << 16));
}

#endif


//////////////////////////////////////////////////////////////////////////////
//
//      stb_bitset   an array of booleans indexed by integers
//

typedef stb_uint32 stb_bitset;

STB_EXTERN  stb_bitset *stb_bitset_new(int value, int len);

#define stb_bitset_clearall(arr,len)     (memset(arr,   0, 4 * (len)))
#define stb_bitset_setall(arr,len)       (memset(arr, 255, 4 * (len)))

#define stb_bitset_setbit(arr,n)         ((arr)[(n) >> 5] |=  (1 << (n & 31)))
#define stb_bitset_clearbit(arr,n)       ((arr)[(n) >> 5] &= ~(1 << (n & 31)))
#define stb_bitset_testbit(arr,n)        ((arr)[(n) >> 5] &   (1 << (n & 31)))

STB_EXTERN  stb_bitset *stb_bitset_union(stb_bitset *p0, stb_bitset *p1, int len);

STB_EXTERN  int *stb_bitset_getlist(stb_bitset *out, int start, int end);

STB_EXTERN  int  stb_bitset_eq(stb_bitset *p0, stb_bitset *p1, int len);
STB_EXTERN  int  stb_bitset_disjoint(stb_bitset *p0, stb_bitset *p1, int len);
STB_EXTERN  int  stb_bitset_disjoint_0(stb_bitset *p0, stb_bitset *p1, int len);
STB_EXTERN  int  stb_bitset_subset(stb_bitset *bigger, stb_bitset *smaller, int len);
STB_EXTERN  int  stb_bitset_unioneq_changed(stb_bitset *p0, stb_bitset *p1, int len);

#ifdef STB_DEFINE
int stb_bitset_eq(stb_bitset *p0, stb_bitset *p1, int len)
{
   int i;
   for (i=0; i < len; ++i)
      if (p0[i] != p1[i]) return 0;
   return 1;
}

int stb_bitset_disjoint(stb_bitset *p0, stb_bitset *p1, int len)
{
   int i;
   for (i=0; i < len; ++i)
      if (p0[i] & p1[i]) return 0;
   return 1;
}

int stb_bitset_disjoint_0(stb_bitset *p0, stb_bitset *p1, int len)
{
   int i;
   for (i=0; i < len; ++i)
      if ((p0[i] | p1[i]) != 0xffffffff) return 0;
   return 1;
}

int stb_bitset_subset(stb_bitset *bigger, stb_bitset *smaller, int len)
{
   int i;
   for (i=0; i < len; ++i)
      if ((bigger[i] & smaller[i]) != smaller[i]) return 0;
   return 1;
}

stb_bitset *stb_bitset_union(stb_bitset *p0, stb_bitset *p1, int len)
{
   int i;
   stb_bitset *d = (stb_bitset *) malloc(sizeof(*d) * len);
   for (i=0; i < len; ++i) d[i] = p0[i] | p1[i];
   return d;
}

int stb_bitset_unioneq_changed(stb_bitset *p0, stb_bitset *p1, int len)
{
   int i, changed=0;
   for (i=0; i < len; ++i) {
      stb_bitset d = p0[i] | p1[i];
      if (d != p0[i]) {
         p0[i] = d;
         changed = 1;
      }
   }
   return changed;
}

stb_bitset *stb_bitset_new(int value, int len)
{
   int i;
   stb_bitset *d = (stb_bitset *) malloc(sizeof(*d) * len);
   if (value) value = 0xffffffff;
   for (i=0; i < len; ++i) d[i] = value;
   return d;
}

int *stb_bitset_getlist(stb_bitset *out, int start, int end)
{
   STB__ARR(int) list = NULL;
   int i;
   for (i=start; i < end; ++i)
      if (stb_bitset_testbit(out, i))
         stb_arr_push(list, i);
   return list;
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//      stb_wordwrap    quality word-wrapping for fixed-width fonts
//

STB_EXTERN int stb_wordwrap(int *pairs, int pair_max, int count, char *str);
STB_EXTERN int *stb_wordwrapalloc(int count, char *str);

#ifdef STB_DEFINE

int stb_wordwrap(int *pairs, int pair_max, int count, char *str)
{
   int n=0,i=0, start=0,nonwhite=0;
   if (pairs == NULL) pair_max = 0x7ffffff0;
   else pair_max *= 2;
   // parse 
   for(;;) {
      int s=i; // first whitespace char; last nonwhite+1
      int w;   // word start
      // stu__accept whitespace
      while (isspace(str[i])) {
         if (str[i] == '\n' || str[i] == '\r') {
            if (str[i] + str[i+1] == '\n' + '\r') ++i;
            if (n >= pair_max) return -1;
            if (pairs) pairs[n] = start, pairs[n+1] = s-start;
            n += 2;
            nonwhite=0;
            start = i+1;
            s = start;
         }
         ++i;
      }
      if (i >= start+count) {
         // we've gone off the end using whitespace
         if (nonwhite) {
            if (n >= pair_max) return -1;
            if (pairs) pairs[n] = start, pairs[n+1] = s-start;
            n += 2;
            start = s = i;
            nonwhite=0;
         } else {
            // output all the whitespace
            while (i >= start+count) {
               if (n >= pair_max) return -1;
               if (pairs) pairs[n] = start, pairs[n+1] = count;
               n += 2;
               start += count;
            }
            s = start;
         }
      }

      if (str[i] == 0) break;
      // now scan out a word and see if it fits
      w = i;
      while (str[i] && !isspace(str[i])) {
         ++i;
      }
      // wrapped?
      if (i > start + count) {
         // huge?
         if (i-s <= count) {
            if (n >= pair_max) return -1;
            if (pairs) pairs[n] = start, pairs[n+1] = s-start;
            n += 2;
            start = w;
         } else {
            // This word is longer than one line. If we wrap it onto N lines
            // there are leftover chars. do those chars fit on the cur line?
            // But if we have leading whitespace, we force it to start here.
            if ((w-start) + ((i-w) % count) <= count || !nonwhite) {
               // output a full line
               if (n >= pair_max) return -1;
               if (pairs) pairs[n] = start, pairs[n+1] = count;
               n += 2;
               start += count;
               w = start;
            } else {
               // output a partial line, trimming trailing whitespace
               if (s != start) {
                  if (n >= pair_max) return -1;
                  if (pairs) pairs[n] = start, pairs[n+1] = s-start;
                  n += 2;
                  start = w;
               }
            }
            // now output full lines as needed
            while (start + count <= i) {
               if (n >= pair_max) return -1;
               if (pairs) pairs[n] = start, pairs[n+1] = count;
               n += 2;
               start += count;
            }
         }
      }
      nonwhite=1;
   }
   if (start < i) {
      if (n >= pair_max) return -1;
      if (pairs) pairs[n] = start, pairs[n+1] = i-start;
      n += 2;
   }
   return n>>1;
}

int *stb_wordwrapalloc(int count, char *str)
{
   int n = stb_wordwrap(NULL,0,count,str);
   STB__ARR(int ) z = NULL;
   stb_arr_setlen(z, n*2);
   stb_wordwrap(z, n, count, str);
   return z;
}
#endif


//////////////////////////////////////////////////////////////////////////////
//
//         stb_match:    wildcards and regexping
//

STB_EXTERN int stb_wildmatch (char *expr, char *candidate);
STB_EXTERN int stb_wildmatchi(char *expr, char *candidate);
STB_EXTERN int stb_wildfind  (char *expr, char *candidate);
STB_EXTERN int stb_wildfindi (char *expr, char *candidate);

STB_EXTERN int stb_regex(char *regex, char *candidate);

typedef struct stb_matcher stb_matcher;

STB_EXTERN stb_matcher *stb_regex_matcher(char *regex);
STB_EXTERN int stb_matcher_match(stb_matcher *m, char *str);
STB_EXTERN int stb_matcher_find(stb_matcher *m, char *str);
STB_EXTERN void stb_matcher_free(stb_matcher *f);

STB_EXTERN stb_matcher *stb_lex_matcher(void);
STB_EXTERN int stb_lex_item(stb_matcher *m, char *str, int result);
STB_EXTERN int stb_lex_item_wild(stb_matcher *matcher, char *regex, int result);
STB_EXTERN int stb_lex(stb_matcher *m, char *str, int *len);



#ifdef STB_DEFINE

static int stb__match_qstring(char *candidate, char *qstring, int qlen, int insensitive)
{
   int i;
   if (insensitive) {
      for (i=0; i < qlen; ++i)
         if (qstring[i] == '?') {
            if (!candidate[i]) return 0;
         } else
            if (tolower(qstring[i]) != tolower(candidate[i]))
               return 0;
   } else {
      for (i=0; i < qlen; ++i)
         if (qstring[i] == '?') {
            if (!candidate[i]) return 0;
         } else
            if (qstring[i] != candidate[i])
               return 0;
   }
   return 1;
}

static int stb__find_qstring(char *candidate, char *qstring, int qlen, int insensitive)
{
   char c;

   int offset=0;
   while (*qstring == '?') {
      ++qstring;
      --qlen;
      ++candidate;
      if (qlen == 0) return 0;
      if (*candidate == 0) return -1;
   }

   c = *qstring++;
   --qlen;
   if (insensitive) c = tolower(c);

   while (candidate[offset]) {
      if (c == (insensitive ? tolower(candidate[offset]) : candidate[offset]))
         if (stb__match_qstring(candidate+offset+1, qstring, qlen, insensitive))
            return offset;
      ++offset;
   }

   return -1;
}

int stb__wildmatch_raw2(char *expr, char *candidate, int search, int insensitive)
{
   int where=0;
   int start = -1;
   
   if (!search) {
      // parse to first '*'
      if (*expr != '*')
         start = 0;
      while (*expr != '*') {
         if (!*expr)
            return *candidate == 0 ? 0 : -1;
         if (*expr == '?') {
            if (!*candidate) return -1;
         } else {
            if (insensitive) {
               if (tolower(*candidate) != tolower(*expr))
                  return -1;
            } else 
               if (*candidate != *expr)
                  return -1;
         }
         ++candidate, ++expr, ++where;
      }
   } else {
      // 0-length search string
      if (!*expr)
         return 0;
   }

   assert(search || *expr == '*');
   if (!search)
      ++expr;

   // implicit '*' at this point
      
   while (*expr) {
      int o=0;
      // combine redundant * characters
      while (expr[0] == '*') ++expr;

      // ok, at this point, expr[-1] == '*',
      // and expr[0] != '*'

      if (!expr[0]) return start >= 0 ? start : 0;

      // now find next '*'
      o = 0;
      while (expr[o] != '*') {
         if (expr[o] == 0)
            break;
         ++o;
      }
      // if no '*', scan to end, then match at end
      if (expr[o] == 0 && !search) {
         int z;
         for (z=0; z < o; ++z)
            if (candidate[z] == 0)
               return -1;
         while (candidate[z])
            ++z;
         // ok, now check if they match
         if (stb__match_qstring(candidate+z-o, expr, o, insensitive))
            return start >= 0 ? start : 0;
         return -1; 
      } else {
         // if yes '*', then do stb__find_qmatch on the intervening chars
         int n = stb__find_qstring(candidate, expr, o, insensitive);
         if (n < 0)
            return -1;
         if (start < 0)
            start = where + n;
         expr += o;
         candidate += n+o;
      }

      if (*expr == 0) {
         assert(search);
         return start;
      }

      assert(*expr == '*');
      ++expr;
   }

   return start >= 0 ? start : 0;
}

int stb__wildmatch_raw(char *expr, char *candidate, int search, int insensitive)
{
   char buffer[256];
   // handle multiple search strings
   char *s = strchr(expr, ';');
   char *last = expr;
   while (s) {
      int z;
      // need to allow for non-writeable strings... assume they're small
      if (s - last < 256) {
         stb_strncpy(buffer, last, s-last+1);
         z = stb__wildmatch_raw2(buffer, candidate, search, insensitive);
      } else {
         *s = 0;
         z = stb__wildmatch_raw2(last, candidate, search, insensitive);
         *s = ';';
      }
      if (z >= 0) return z;
      last = s+1;
      s = strchr(last, ';');
   }
   return stb__wildmatch_raw2(last, candidate, search, insensitive);
}

int stb_wildmatch(char *expr, char *candidate)
{
   return stb__wildmatch_raw(expr, candidate, 0,0) >= 0;
}

int stb_wildmatchi(char *expr, char *candidate)
{
   return stb__wildmatch_raw(expr, candidate, 0,1) >= 0;
}

int stb_wildfind(char *expr, char *candidate)
{
   return stb__wildmatch_raw(expr, candidate, 1,0);
}

int stb_wildfindi(char *expr, char *candidate)
{
   return stb__wildmatch_raw(expr, candidate, 1,1);
}

typedef struct
{
   int16 transition[256];
} stb_dfa;

// an NFA node represents a state you're in; it then has
// an arbitrary number of edges dangling off of it
// note this isn't utf8-y
typedef struct
{
   int16  match; // character/set to match
   uint16 node;  // output node to go to
} stb_nfa_edge;

typedef struct
{
   int16 goal;   // does reaching this win the prize?
   uint8 active; // is this in the active list
   STB__ARR(stb_nfa_edge) out;
   STB__ARR(uint16)       eps;  // list of epsilon closures
} stb_nfa_node;

#define STB__DFA_UNDEF  -1
#define STB__DFA_GOAL   -2
#define STB__DFA_END    -3
#define STB__DFA_MGOAL  -4
#define STB__DFA_VALID  0

#define STB__NFA_STOP_GOAL -1

// compiled regexp
struct stb_matcher
{
   uint16 start_node;
   int16 dfa_start;
   uint32 *charset;
   int num_charset;
   int match_start;
   STB__ARR(stb_nfa_node) nodes;
   int does_lex;

   // dfa matcher
   STB__ARR(stb_dfa) dfa;
   STB__ARR(uint32 ) dfa_mapping;
   STB__ARR(int16  ) dfa_result;
   int num_words_per_dfa;
};

static int stb__add_node(stb_matcher *matcher)
{
   stb_nfa_node z;
   z.active = 0;
   z.eps    = 0;
   z.goal   = 0;
   z.out    = 0;
   stb_arr_push(matcher->nodes, z);
   return stb_arr_len(matcher->nodes)-1;
}

static void stb__add_epsilon(stb_matcher *matcher, int from, int to)
{
   assert(from != to);
   if (matcher->nodes[from].eps == NULL)
      stb_arr_malloc((void **) &matcher->nodes[from].eps, matcher);
   stb_arr_push(matcher->nodes[from].eps, to);
}

static void stb__add_edge(stb_matcher *matcher, int from, int to, int type)
{
   stb_nfa_edge z = { type, to };
   if (matcher->nodes[from].out == NULL)
      stb_arr_malloc((void **) &matcher->nodes[from].out, matcher);
   stb_arr_push(matcher->nodes[from].out, z);
}

static char *stb__reg_parse_alt(stb_matcher *m, int s, char *r, uint16 *e);
static char *stb__reg_parse(stb_matcher *matcher, int start, char *regex, uint16 *end)
{
   int n;
   int last_start = -1;
   uint16 last_end = start;

   while (*regex) {
      switch (*regex) {
         case '(':
            last_start = last_end;
            regex = stb__reg_parse_alt(matcher, last_end, regex+1, &last_end);
            if (regex == NULL || *regex != ')')
               return NULL;
            ++regex;
            break;

         case '|':
         case ')':
            *end = last_end;
            return regex;

         case '?':
            if (last_start < 0) return NULL;
            stb__add_epsilon(matcher, last_start, last_end);
            ++regex;
            break;

         case '*':
            if (last_start < 0) return NULL;
            stb__add_epsilon(matcher, last_start, last_end);

            // fall through

         case '+':
            if (last_start < 0) return NULL;
            stb__add_epsilon(matcher, last_end, last_start);
            // prevent links back to last_end from chaining to last_start
            n = stb__add_node(matcher);
            stb__add_epsilon(matcher, last_end, n);
            last_end = n;
            ++regex;
            break;

         case '{':   // not supported!
            // @TODO: given {n,m}, clone last_start to last_end m times,
            // and include epsilons from start to first m-n blocks
            return NULL; 

         case '\\':
            ++regex;
            if (!*regex) return NULL;

            // fallthrough
         default: // match exactly this character
            n = stb__add_node(matcher);
            stb__add_edge(matcher, last_end, n, *regex);
            last_start = last_end;
            last_end = n;
            ++regex;
            break;

         case '$':
            n = stb__add_node(matcher);
            stb__add_edge(matcher, last_end, n, '\n');
            last_start = last_end;
            last_end = n;
            ++regex;
            break;

         case '.':
            n = stb__add_node(matcher);
            stb__add_edge(matcher, last_end, n, -1);
            last_start = last_end;
            last_end = n;
            ++regex;
            break;

         case '[': {
            uint8 flags[256];
            int invert = 0,z;
            ++regex;
            if (matcher->num_charset == 0) {
               matcher->charset = (stb_uint *) stb_malloc(matcher, sizeof(*matcher->charset) * 256);
               memset(matcher->charset, 0, sizeof(*matcher->charset) * 256);
            }

            memset(flags,0,sizeof(flags));

            // leading ^ is special
            if (*regex == '^')
               ++regex, invert = 1;

            // leading ] is special
            if (*regex == ']') {
               flags[']'] = 1;
               ++regex;
            }
            while (*regex != ']') {
               uint a;
               if (!*regex) return NULL;
               a = *regex++;
               if (regex[0] == '-' && regex[1] != ']') {
                  uint i,b = regex[1];
                  regex += 2;
                  if (b == 0) return NULL;
                  if (a > b) return NULL;
                  for (i=a; i <= b; ++i)
                     flags[i] = 1;
               } else
                  flags[a] = 1;
            }
            ++regex;
            if (invert) {
               int i;
               for (i=0; i < 256; ++i)
                  flags[i] = 1-flags[i];
            }

            // now check if any existing charset matches
            for (z=0; z < matcher->num_charset; ++z) {
               int i, k[2] = { 0, 1 << z};
               for (i=0; i < 256; ++i) {
                  unsigned int f = k[flags[i]];
                  if ((matcher->charset[i] & k[1]) != f)
                     break;
               }
               if (i == 256) break;
            }

            if (z == matcher->num_charset) {
               int i;
               ++matcher->num_charset;
               if (matcher->num_charset > 32) {
                  assert(0); /* NOTREACHED */
                  return NULL; // too many charsets, oops
               }
               for (i=0; i < 256; ++i)
                  if (flags[i])
                     matcher->charset[i] |= (1 << z);
            }

            n = stb__add_node(matcher);
            stb__add_edge(matcher, last_end, n, -2 - z);
            last_start = last_end;
            last_end = n;
            break;
         }
      }
   }
   *end = last_end;
   return regex;
}

static char *stb__reg_parse_alt(stb_matcher *matcher, int start, char *regex, uint16 *end)
{
   uint16 last_end = start;
   uint16 main_end;

   int head, tail;

   head = stb__add_node(matcher);
   stb__add_epsilon(matcher, start, head);

   regex = stb__reg_parse(matcher, head, regex, &last_end);
   if (regex == NULL) return NULL;
   if (*regex == 0 || *regex == ')') {
      *end = last_end;
      return regex;
   }

   main_end = last_end;
   tail = stb__add_node(matcher);

   stb__add_epsilon(matcher, last_end, tail);

   // start alternatives from the same starting node; use epsilon
   // transitions to combine their endings
   while(*regex && *regex != ')') {
      assert(*regex == '|');
      head = stb__add_node(matcher);
      stb__add_epsilon(matcher, start, head);
      regex = stb__reg_parse(matcher, head, regex+1, &last_end);
      if (regex == NULL)
         return NULL;
      stb__add_epsilon(matcher, last_end, tail);
   }

   *end = tail;
   return regex;
}

static char *stb__wild_parse(stb_matcher *matcher, int start, char *str, uint16 *end)
{
   int n;
   uint16 last_end;

   last_end = stb__add_node(matcher);
   stb__add_epsilon(matcher, start, last_end);

   while (*str) {
      switch (*str) {
            // fallthrough
         default: // match exactly this character
            n = stb__add_node(matcher);
            if (toupper(*str) == tolower(*str)) {
               stb__add_edge(matcher, last_end, n, *str);
            } else {
               stb__add_edge(matcher, last_end, n, tolower(*str));
               stb__add_edge(matcher, last_end, n, toupper(*str));
            }
            last_end = n;
            ++str;
            break;

         case '?':
            n = stb__add_node(matcher);
            stb__add_edge(matcher, last_end, n, -1);
            last_end = n;
            ++str;
            break;

         case '*':
            n = stb__add_node(matcher);
            stb__add_edge(matcher, last_end, n, -1);
            stb__add_epsilon(matcher, last_end, n);
            stb__add_epsilon(matcher, n, last_end);
            last_end = n;
            ++str;
            break;
      }
   }

   // now require end of string to match
   n = stb__add_node(matcher);
   stb__add_edge(matcher, last_end, n, 0);
   last_end = n;

   *end = last_end;
   return str;
}

static int stb__opt(stb_matcher *m, int n)
{
   for(;;) {
      stb_nfa_node *p = &m->nodes[n];
      if (p->goal)                  return n;
      if (stb_arr_len(p->out))      return n;
      if (stb_arr_len(p->eps) != 1) return n;
      n = p->eps[0];
   }
}

static void stb__optimize(stb_matcher *m)
{
   // if the target of any edge is a node with exactly
   // one out-epsilon, shorten it
   int i,j;
   for (i=0; i < stb_arr_len(m->nodes); ++i) {
      stb_nfa_node *p = &m->nodes[i];
      for (j=0; j < stb_arr_len(p->out); ++j)
         p->out[j].node = stb__opt(m,p->out[j].node);
      for (j=0; j < stb_arr_len(p->eps); ++j)
         p->eps[j]      = stb__opt(m,p->eps[j]     );
   }
   m->start_node = stb__opt(m,m->start_node);
}

void stb_matcher_free(stb_matcher *f)
{
   stb_free(f);
}

static stb_matcher *stb__alloc_matcher(void)
{
   stb_matcher *matcher = (stb_matcher *) stb_malloc(0,sizeof(*matcher));

   matcher->start_node  = 0;
   stb_arr_malloc((void **) &matcher->nodes, matcher);
   matcher->num_charset = 0;
   matcher->match_start = 0;
   matcher->does_lex    = 0;

   matcher->dfa_start   = STB__DFA_UNDEF;
   stb_arr_malloc((void **) &matcher->dfa, matcher);
   stb_arr_malloc((void **) &matcher->dfa_mapping, matcher);
   stb_arr_malloc((void **) &matcher->dfa_result, matcher);

   stb__add_node(matcher);

   return matcher;
}

static void stb__lex_reset(stb_matcher *matcher)
{
   // flush cached dfa data
   stb_arr_setlen(matcher->dfa, 0);
   stb_arr_setlen(matcher->dfa_mapping, 0);
   stb_arr_setlen(matcher->dfa_result, 0);
   matcher->dfa_start = STB__DFA_UNDEF;
}

stb_matcher *stb_regex_matcher(char *regex)
{
   void *c = stb__arr_context;
   char *z;
   uint16 end;
   stb_matcher *matcher = stb__alloc_matcher();
   if (*regex == '^') {
      matcher->match_start = 1;
      ++regex;
   }

   z = stb__reg_parse_alt(matcher, matcher->start_node, regex, &end);

   if (!z || *z) {
      stb_free(matcher);
      return NULL;
   }

   ((matcher->nodes)[(int) end]).goal = STB__NFA_STOP_GOAL;

   return matcher;
}

stb_matcher *stb_lex_matcher(void)
{
   stb_matcher *matcher = stb__alloc_matcher();

   matcher->match_start = 1;
   matcher->does_lex    = 1;

   return matcher;
}

int stb_lex_item(stb_matcher *matcher, char *regex, int result)
{
   char *z;
   uint16 end;

   z = stb__reg_parse_alt(matcher, matcher->start_node, regex, &end);

   if (z == NULL)
      return 0;

   stb__lex_reset(matcher);

   matcher->nodes[(int) end].goal = result;
   return 1;
}

int stb_lex_item_wild(stb_matcher *matcher, char *regex, int result)
{
   char *z;
   uint16 end;

   z = stb__wild_parse(matcher, matcher->start_node, regex, &end);

   if (z == NULL)
      return 0;

   stb__lex_reset(matcher);

   matcher->nodes[(int) end].goal = result;
   return 1;
}

static void stb__clear(stb_matcher *m, STB__ARR(uint16) list)
{
   int i;
   for (i=0; i < stb_arr_len(list); ++i)
      m->nodes[(int) list[i]].active = 0;
}

static int stb__clear_goalcheck(stb_matcher *m, STB__ARR(uint16) list)
{
   int i, t=0;
   for (i=0; i < stb_arr_len(list); ++i) {
      t += m->nodes[(int) list[i]].goal;
      m->nodes[(int) list[i]].active = 0;
   }
   return t;
}

static uint16 * stb__add_if_inactive(stb_matcher *m, STB__ARR(uint16) list, int n)
{
   if (!m->nodes[n].active) {
      stb_arr_push(list, n);
      m->nodes[n].active = 1;
   }
   return list;
}

static uint16 * stb__eps_closure(stb_matcher *m, STB__ARR(uint16) list)
{
   int i,n = stb_arr_len(list);

   for(i=0; i < n; ++i) {
      STB__ARR(uint16) e = m->nodes[(int) list[i]].eps;
      if (e) {
         int j,k = stb_arr_len(e);
         for (j=0; j < k; ++j)
            list = stb__add_if_inactive(m, list, e[j]);
         n = stb_arr_len(list);
      }
   }

   return list;
}

int stb_matcher_match(stb_matcher *m, char *str)
{
   int result = 0;
   int i,j,y,z;
   STB__ARR(uint16) previous = NULL;
   STB__ARR(uint16) current = NULL;
   uint16 *temp;

   stb_arr_setsize(previous, 4);
   stb_arr_setsize(current, 4);

   previous = stb__add_if_inactive(m, previous, m->start_node);
   previous = stb__eps_closure(m,previous);
   stb__clear(m, previous);

   while (*str && stb_arr_len(previous)) {
      y = stb_arr_len(previous);
      for (i=0; i < y; ++i) {
         stb_nfa_node *n = &m->nodes[(int) previous[i]];
         z = stb_arr_len(n->out);
         for (j=0; j < z; ++j) {
            if (n->out[j].match >= 0) {
               if (n->out[j].match == *str)
                  current = stb__add_if_inactive(m, current, n->out[j].node);
            } else if (n->out[j].match == -1) {
               if (*str != '\n')
                  current = stb__add_if_inactive(m, current, n->out[j].node);
            } else if (n->out[j].match < -1) {
               int z = -n->out[j].match - 2;
               if (m->charset[(uint8) *str] & (1 << z))
                  current = stb__add_if_inactive(m, current, n->out[j].node);
            }
         }
      }
      stb_arr_setlen(previous, 0);

      temp = previous;
      previous = current;
      current = temp;

      previous = stb__eps_closure(m,previous);
      stb__clear(m, previous);

      ++str;
   }

   // transition to pick up a '$' at the end
   y = stb_arr_len(previous);
   for (i=0; i < y; ++i)
      m->nodes[(int) previous[i]].active = 1;

   for (i=0; i < y; ++i) {
      stb_nfa_node *n = &m->nodes[(int) previous[i]];
      z = stb_arr_len(n->out);
      for (j=0; j < z; ++j) {
         if (n->out[j].match == '\n')
            current = stb__add_if_inactive(m, current, n->out[j].node);
      }
   }

   previous = stb__eps_closure(m,previous);
   stb__clear(m, previous);

   y = stb_arr_len(previous);
   for (i=0; i < y; ++i)
      if (m->nodes[(int) previous[i]].goal)
         result = 1;

   stb_arr_free(previous);
   stb_arr_free(current);

   return result && *str == 0;
}

int16 stb__get_dfa_node(stb_matcher *m, STB__ARR(uint16) list)
{
   uint16 node;
   uint32 data[8], *state, *newstate;
   int i,j,n;

   state = (uint32 *) stb_temp(data, m->num_words_per_dfa * 4);
   memset(state, 0, m->num_words_per_dfa*4);

   n = stb_arr_len(list);
   for (i=0; i < n; ++i) {
      int x = list[i];
      state[x >> 5] |= 1 << (x & 31);
   }

   // @TODO use a hash table
   n = stb_arr_len(m->dfa_mapping);
   i=j=0;
   for(; j < n; ++i, j += m->num_words_per_dfa) {
      // @TODO special case for <= 32
      if (!memcmp(state, m->dfa_mapping + j, m->num_words_per_dfa*4)) {
         node = i;
         goto done;
      }
   }

   assert(stb_arr_len(m->dfa) == i);
   node = i;

   newstate = stb_arr_addn(m->dfa_mapping, m->num_words_per_dfa);
   memcpy(newstate, state, m->num_words_per_dfa*4);

   // set all transitions to 'unknown'
   stb_arr_add(m->dfa);
   memset(m->dfa[i].transition, -1, sizeof(m->dfa[i].transition));

   if (m->does_lex) {
      int result = -1;
      n = stb_arr_len(list);
      for (i=0; i < n; ++i) {
         if (m->nodes[(int) list[i]].goal > result)
            result = m->nodes[(int) list[i]].goal;
      }

      stb_arr_push(m->dfa_result, result);
   }

done:
   stb_tempfree(data, state);
   return node;
}

static int stb__matcher_dfa(stb_matcher *m, char *str_c, int *len)
{
   stb_uint8 *str = (stb_uint8 *) str_c;
   int16 node,prevnode;
   stb_dfa *trans;
   int match_length = 0;
   stb_int16 match_result=0;

   if (m->dfa_start == STB__DFA_UNDEF) {
      STB__ARR(stb_uint16) list;

      m->num_words_per_dfa = (stb_arr_len(m->nodes)+31) >> 5;
      stb__optimize(m);

      list = stb__add_if_inactive(m, NULL, m->start_node);
      list = stb__eps_closure(m,list);
      if (m->does_lex) {
         m->dfa_start = stb__get_dfa_node(m,list);
         stb__clear(m, list);
         // DON'T allow start state to be a goal state!
         // this allows people to specify regexes that can match 0
         // characters without them actually matching (also we don't
         // check _before_ advancing anyway
         if (m->dfa_start <= STB__DFA_MGOAL)
            m->dfa_start = -(m->dfa_start - STB__DFA_MGOAL);
      } else {
         if (stb__clear_goalcheck(m, list))
            m->dfa_start = STB__DFA_GOAL;
         else
            m->dfa_start = stb__get_dfa_node(m,list);
      }
      stb_arr_free(list);
   }

   prevnode = STB__DFA_UNDEF;
   node = m->dfa_start;
   trans = m->dfa;

   if (m->dfa_start == STB__DFA_GOAL)
      return 1;

   for(;;) {
      assert(node >= STB__DFA_VALID);

      // fast inner DFA loop; especially if STB__DFA_VALID is 0

      do {
         prevnode = node;
         node = trans[node].transition[*str++];
      } while (node >= STB__DFA_VALID);

      assert(node >= STB__DFA_MGOAL - stb_arr_len(m->dfa));
      assert(node < stb_arr_len(m->dfa));

      // special case for lex: need _longest_ match, so notice goal
      // state without stopping
      if (node <= STB__DFA_MGOAL) {
         match_length = str - (stb_uint8 *) str_c;
         node = -(node - STB__DFA_MGOAL);
         match_result = node;
         continue;
      }

      // slow NFA->DFA conversion

      // or we hit the goal or the end of the string, but those
      // can only happen once per search...

      if (node == STB__DFA_UNDEF) {
         // build a list  -- @TODO special case <= 32 states
         // heck, use a more compact data structure for <= 16 and <= 8 ?!

         // @TODO keep states/newstates around instead of reallocating them
         STB__ARR(uint16) states = NULL;
         STB__ARR(uint16) newstates = NULL;
         int i,j,y,z;
         uint32 *flags = &m->dfa_mapping[prevnode * m->num_words_per_dfa];
         assert(prevnode != STB__DFA_UNDEF);
         stb_arr_setsize(states, 4);
         stb_arr_setsize(newstates,4);
         for (j=0; j < m->num_words_per_dfa; ++j) {
            for (i=0; i < 32; ++i) {
               if (*flags & (1 << i))
                  stb_arr_push(states, j*32+i);
            }
            ++flags;
         }
         // states is now the states we were in in the previous node;
         // so now we can compute what node it transitions to on str[-1]

         y = stb_arr_len(states);
         for (i=0; i < y; ++i) {
            stb_nfa_node *n = &m->nodes[(int) states[i]];
            z = stb_arr_len(n->out);
            for (j=0; j < z; ++j) {
               if (n->out[j].match >= 0) {
                  if (n->out[j].match == str[-1] || (str[-1] == 0 && n->out[j].match == '\n'))
                     newstates = stb__add_if_inactive(m, newstates, n->out[j].node);
               } else if (n->out[j].match == -1) {
                  if (str[-1] != '\n' && str[-1])
                     newstates = stb__add_if_inactive(m, newstates, n->out[j].node);
               } else if (n->out[j].match < -1) {
                  int z = -n->out[j].match - 2;
                  if (m->charset[str[-1]] & (1 << z))
                     newstates = stb__add_if_inactive(m, newstates, n->out[j].node);
               }
            }
         }
         // AND add in the start state!
         if (!m->match_start || (str[-1] == '\n' && !m->does_lex))
            newstates = stb__add_if_inactive(m, newstates, m->start_node);
         // AND epsilon close it
         newstates = stb__eps_closure(m, newstates);
         // if it's a goal state, then that's all there is to it
         if (stb__clear_goalcheck(m, newstates)) {
            if (m->does_lex) {
               match_length = str - (stb_uint8 *) str_c;
               node = stb__get_dfa_node(m,newstates);
               match_result = node;
               node = -node + STB__DFA_MGOAL;
               trans = m->dfa; // could have gotten realloc()ed
            } else
               node = STB__DFA_GOAL;
         } else if (str[-1] == 0 || stb_arr_len(newstates) == 0) {
            node = STB__DFA_END;
         } else {
            node = stb__get_dfa_node(m,newstates);
            trans = m->dfa; // could have gotten realloc()ed
         }
         trans[prevnode].transition[str[-1]] = node;
         if (node <= STB__DFA_MGOAL)
            node = -(node - STB__DFA_MGOAL);
         stb_arr_free(newstates);
         stb_arr_free(states);
      }

      if (node == STB__DFA_GOAL) {
         return 1;
      }
      if (node == STB__DFA_END) {
         if (m->does_lex) {
            if (match_result) {
               if (len) *len = match_length;
               return m->dfa_result[(int) match_result];
            }
         }
         return 0;
      }

      assert(node != STB__DFA_UNDEF);
   }
}

int stb_matcher_find(stb_matcher *m, char *str)
{
   assert(m->does_lex == 0);
   return stb__matcher_dfa(m, str, NULL);
}

int stb_lex(stb_matcher *m, char *str, int *len)
{
   assert(m->does_lex);
   return stb__matcher_dfa(m, str, len);
}

int stb_regex(char *regex, char *str)
{
   static stb_perfect p;
   static STB__ARR(stb_matcher *) matchers;
   static STB__ARR(char        *) regexps;
   static STB__ARR(char        *) regexp_cache;
   static unsigned short *mapping;
   int z = stb_perfect_hash(&p, (int) regex);
   if (z >= 0) {
      if (strcmp(regex, regexp_cache[(int) mapping[z]])) {
         int i = mapping[z];
         stb_matcher_free(matchers[i]);
         free(regexp_cache[i]);
         regexps[i] = regex;
         regexp_cache[i] = strdup(regex);
         matchers[i] = stb_regex_matcher(regex);
      }
   } else {
      int i,n;
      if (regex == NULL) {
         for (i=0; i < stb_arr_len(matchers); ++i) {
            stb_matcher_free(matchers[i]);
            free(regexp_cache[i]);
         }
         matchers = stb_arr_free(matchers);
         regexps = stb_arr_free(regexps);
         regexp_cache = stb_arr_free(regexp_cache);
         stb_perfect_destroy(&p);
         free(mapping); mapping = NULL;
         return -1;
      }
      stb_arr_push(regexps, regex);
      stb_arr_push(regexp_cache, strdup(regex));
      stb_arr_push(matchers, stb_regex_matcher(regex));
      stb_perfect_destroy(&p);
      n = stb_perfect_create(&p, (unsigned int *) (char **) regexps, stb_arr_len(regexps));
      mapping = (unsigned short *) realloc(mapping, n * sizeof(*mapping));
      for (i=0; i < stb_arr_len(regexps); ++i)
         mapping[stb_perfect_hash(&p, (int) regexps[i])] = i;
      z = stb_perfect_hash(&p, (int) regex);
   }
   return stb_matcher_find(matchers[(int) mapping[z]], str);
}

#endif // STB_DEFINE


#if 0
//////////////////////////////////////////////////////////////////////////////
//
//                      C source-code introspection
//

typedef struct
{
   char *name;
   char *type;
   char *comment;
   int   size;
   int   offset;
   int   arrcount[2];
} stb_info_field;

typedef struct
{
   char *structname;
   int size;
   int num_fields;
   stb_info_field *fields;
} stb_info_struct;

extern stb_info_struct stb__introspect_output[];

STB_EXTERN void stb__introspect(char *path, char *file, stb_info_struct *compiled);

#define stb_introspect_ship()            stb__introspect(NULL, NULL, stb__introspect_output)

#ifdef STB_SHIP
#define stb_introspect()                 stb_introspect_ship()
#define stb_introspect_path(p)           stb_introspect_ship()
#else
// bootstrapping: define stb_introspect_bootstrap() the first time
#define stb_introspect_bootstrap()       stb__introspect(NULL, __FILE__, NULL)
#define stb_introspect()                 stb__introspect(NULL, __FILE__, stb__introspect_output)

#define stb_introspect_path_bootstrap(p) stb__introspect(p, __FILE__, NULL)
#define stb_introspect_path(p)           stb__introspect(p, __FILE__, NULL)
#endif

#ifdef STB_DEFINE

#ifndef STB_INTROSPECT_CPP
   #ifdef __cplusplus
   #define STB_INTROSPECT_CPP 1
   #else
   #define STB_INTROSPECT_CPP 0
   #endif
#endif

static void stb__introspect_filename(char *buffer, char *path)
{
   #if STB_INTROSPECT_CPP
   sprintf(buffer, "%s/stb_introspect.cpp", path);
   #else
   sprintf(buffer, "%s/stb_introspect.c", path);
   #endif
}

static void stb__introspect_compute(char *path, char *file)
{
   int i;
   STB__ARR(char *) include_list = NULL;
   STB__ARR(char *) introspect_list = NULL;
   FILE *f;
   f = fopen(file, "w");
   if (!f) return;

   fputs("// if you get compiler errors, uncomment the following line:\n", f);
   fputs("//#define STB_INTROSPECT_INVALID\n\n", f);
   fputs("// this will force the code to compile, and force the introspector\n", f);
   fputs("// to run and then exit, allowing you to recompile\n\n\n", f);
   fputs("#include \"stb.h\"\n",f );
   fputs("#ifdef STB_INTROSPECT_INVALID\n", f);
   fputs("   stb_info_struct stb__introspect_output[] = { (void *) 1 }\n", f);
   fputs("#else\n", f);
   for (i=0; i < stb_arr_len(include_list); ++i)
      fprintf(f, " #include \"%s\"\n", include_list[i]);

   fputs(" stb_info_struct stb__introspect_output[] =\n{\n", f);
   for (i=0; i < stb_arr_len(introspect_list); ++i)
      fprintf(f, "  stb_introspect_%s,\n", introspect_list[i]);
   fputs(" };\n", f);
   fputs("#endif\n", f);
   fclose(f);
}

static stb_info_struct *stb__introspect_info;

#ifndef STB_SHIP

#endif

void stb__introspect(char *path, char *file, stb_info_struct *compiled)
{
   static int first=1;
   if (!first) return;
   first=0;

   stb__introspect_info = compiled;

   #ifndef STB_SHIP
   if (path || file) {
      int bail_flag = compiled && compiled[0].structname == (void *) 1;
      int needs_building = bail_flag;
      struct stb__stat st;
      char buffer[1024], buffer2[1024];
      if (!path) {
         stb_splitpath(buffer, file, STB_PATH);
         path = buffer;
      }
      // bail if the source path doesn't exist
      if (!stb_fexists(path)) return;

      stb__introspect_filename(buffer2, path);

      // get source/include files timestamps, compare to output-file timestamp;
      // if mismatched, regenerate 

      if (stb__stat(buffer2, &st))
         needs_building = TRUE;
      
      {
         // find any file that contains an introspection command and is newer
         // if needs_building is already true, we don't need to do this test,
         // but we still need these arrays, so go ahead and get them
         STB__ARR(char *) all[3];
         all[0] = stb_readdir_files_mask(path, "*.h");
         all[1] = stb_readdir_files_mask(path, "*.c");
         all[2] = stb_readdir_files_mask(path, "*.cpp");
         int i,j;
         if (needs_building) {
            for (j=0; j < 3; ++j) {
               for (i=0; i < stb_arr_len(all[j]); ++i) {
                  struct stb__stat st2;
                  if (!stb__stat(all[j][i], &st2)) {
                     if (st.st_mtime < st2.st_mtime) {
                        char *z = stb_filec(all[j][i], NULL);
                        int found=FALSE;
                        while (y) {
                           y = strstr(y, "//si");
                           if (y && isspace(y[4])) {
                              found = TRUE;
                              break;
                           }
                        }
                        needs_building = TRUE;
                        goto done;
                     }
                  }
               }
            }
           done:;
         }
               char *z = stb_filec(all[i], NULL), *y = z;
               int found=FALSE;
               while (y) {
                  y = strstr(y, "//si");
                  if (y && isspace(y[4])) {
                     found = TRUE;
                     break;
                  }
               }
               if (found)
                  stb_arr_push(introspect_h, strdup(all[i]));
               free(z);
            }
         }
         stb_readdir_free(all);
         if (!needs_building) {
            for (i=0; i < stb_arr_len(introspect_h); ++i) {
               struct stb__stat st2;
               if (!stb__stat(introspect_h[i], &st2))
                  if (st.st_mtime < st2.st_mtime)
                     needs_building = TRUE;
            }
         }

         if (needs_building) {
            stb__introspect_compute(path, buffer2);
         }
      }
   }
   #endif
}
#endif
#endif



#ifdef STB_STUA
//////////////////////////////////////////////////////////////////////////
//
//  stua: little scripting language
//
//     define STB_STUA to compile it
//
//     see http://nothings.org/stb_stua.html for documentation
//
//  basic parsing model:
//
//   lexical analysis
//      use stb_lex() to parse tokens; keywords get their own tokens
//
//   parsing:
//      recursive descent parser. too much of a hassle to make an unambiguous
//      LR(1) grammar, and one-pass generation is clumsier (recursive descent
//      makes it easier to e.g. compile nested functions). on the other hand,
//      dictionary syntax required hackery to get extra lookahead.
//
//   codegen:
//      output into an evaluation tree, using array indices as 'pointers'
//
//   run:
//      traverse the tree; support for 'break/continue/return' is tricky
//
//   garbage collection:
//      stu__mark and sweep; explicit stack with non-stu__compile_global_scope roots

typedef stb_int32 stua_obj;

typedef stb_idict stua_dict;

STB_EXTERN void stua_run_script(char *s);
STB_EXTERN void stua_uninit(void);

extern stua_obj stua_globals;

STB_EXTERN double   stua_number(stua_obj z);

STB_EXTERN stua_obj stua_getnil(void);
STB_EXTERN stua_obj stua_getfalse(void);
STB_EXTERN stua_obj stua_gettrue(void);
STB_EXTERN stua_obj stua_string(char *z);
STB_EXTERN stua_obj stua_make_number(double d);
STB_EXTERN stua_obj stua_box(int type, void *data, int size);

enum
{
   STUA_op_negate=129,
   STUA_op_shl,   STUA_op_ge,
   STUA_op_shr,   STUA_op_le,
   STUA_op_shru,
   STUA_op_last
};

#define STUA_NO_VALUE   2     // equivalent to a tagged NULL
STB_EXTERN stua_obj (*stua_overload)(int op, stua_obj a, stua_obj b, stua_obj c);

STB_EXTERN stua_obj stua_error(char *err, ...);

STB_EXTERN stua_obj stua_pushroot(stua_obj o);
STB_EXTERN void     stua_poproot (   void   );


#ifdef STB_DEFINE
// INTERPRETER

// 31-bit floating point implementation
//   force the (1 << 30) bit (2nd highest bit) to be zero by re-biasing the exponent;
//   then shift and set the bottom bit

static stua_obj stu__floatp(float *f)
{
   unsigned int n = *(unsigned int *) f;
   unsigned int e = n & (0xff << 23);

   assert(sizeof(int) == 4 && sizeof(float) == 4);

   if (!e)                    // zero?
      n = n;                  //   no change
   else if (e < (64 << 23))   // underflow of the packed encoding?
      n = (n & 0x80000000);   //   signed 0
   else if (e > (190 << 23))  // overflow of the encoding? (or INF or NAN)
      n = (n & 0x80000000) + (127 << 23); // new INF encoding
   else
      n -= 0x20000000;

   // now we need to shuffle the bits so that the spare bit is at the bottom
   assert((n & 0x40000000) == 0);
   return (n & 0x80000000) + (n << 1) + 1;
}

static unsigned char stu__getfloat_addend[256];
static float stu__getfloat(stua_obj v)
{
   unsigned int n;
   unsigned int e = ((unsigned int) v) >> 24;

   n = (int) v >> 1;  // preserve high bit
   n += stu__getfloat_addend[e] << 24;
   return *(float *) &n;
}

stua_obj stua_float(float f) 
{
   return stu__floatp(&f);
}

static void stu__float_init(void)
{
   int i;
   stu__getfloat_addend[0]    = 0;   // do nothing to biased exponent of 0
   for (i=1; i < 127; ++i)
      stu__getfloat_addend[i] = 32;  // undo the -0x20000000
   stu__getfloat_addend[127]  = 64;  // convert packed INF to INF (0x3f -> 0x7f)

   for (i=0; i < 128; ++i) // for signed floats, remove the bit we just shifted down
      stu__getfloat_addend[128+i] = stu__getfloat_addend[i] - 64;
}

// Tagged data type implementation

                                                 // TAGS:
#define stu__int_tag          0  // of 2 bits    //   00   int
#define stu__float_tag        1  // of 1 bit     //   01   float
#define stu__ptr_tag          2  // of 2 bits    //   10   boxed
                                                 //   11   float

#define stu__tag(x)           ((x) & 3)
#define stu__number(x)        (stu__tag(x) != stu__ptr_tag)
#define stu__isint(x)         (stu__tag(x) == stu__int_tag)

#define stu__int(x)           ((x) >> 2)
#define stu__float(x)         (stu__getfloat(x))

#define stu__makeint(v)       ((v)*4+stu__int_tag)

// boxed data, and tag support for boxed data

enum
{
   STU___float    = 1,   STU___int      = 2,
   STU___number   = 3,   STU___string   = 4,
   STU___function = 5,   STU___dict     = 6,
   STU___boolean  = 7,   STU___error    = 8,
};

// boxed data
#define STU__BOX  short type, stua_gc
typedef struct stu__box { STU__BOX; } stu__box;

stu__box stu__nil   = { 0, 1 };
stu__box stu__true  = { STU___boolean, 1, };
stu__box stu__false = { STU___boolean, 1, };

#define stu__makeptr(v)  ((stua_obj)     (v) + stu__ptr_tag)

#define stua_nil    stu__makeptr(&stu__nil)
#define stua_true   stu__makeptr(&stu__true)
#define stua_false  stu__makeptr(&stu__false)

stua_obj stua_getnil(void)   { return stua_nil; }
stua_obj stua_getfalse(void) { return stua_false; }
stua_obj stua_gettrue(void)  { return stua_true; }

#define stu__ptr(x)      ((stu__box *) ((x) - stu__ptr_tag))

#define stu__checkt(t,x) ((t) == STU___float  ? ((x) & 1) == stu__float_tag : \
                          (t) == STU___int    ? stu__isint(x)               : \
                          (t) == STU___number ? stu__number(x)              : \
                          stu__tag(x) == stu__ptr_tag && stu__ptr(x)->type == (t))

typedef struct
{
   STU__BOX;
   void *ptr;
} stu__wrapper;

// implementation of a 'function' or function + closure

typedef struct stu__func
{
   STU__BOX;
   stua_obj closure_source;  // 0 - regular function; 4 - C function
                             // if closure, pointer to source function
   union {
      stua_obj closure_data; // partial-application data
      void *store;           // pointer to free that holds 'code'
      stua_obj (*func)(stua_dict *context);
   } f;
   // closure ends here
   short *code;
   int num_param;
   stua_obj *param;  // list of parameter strings
} stu__func;

// apply this to 'short *code' to get at data
#define stu__const(f)  ((stua_obj *) (f))

static void stu__free_func(stu__func *f)
{
   if (f->closure_source == 0)          free(f->f.store);
   if ((uint) f->closure_source <= 4)   free(f->param);
   free(f);
}

#define stu__pd(x)       ((stua_dict *)    stu__ptr(x))
#define stu__pw(x)       ((stu__wrapper *) stu__ptr(x))
#define stu__pf(x)       ((stu__func *)    stu__ptr(x))


// garbage-collection


static STB__ARR(stu__box *) stu__gc_ptrlist;
static STB__ARR(stua_obj) stu__gc_root_stack;

stua_obj stua_pushroot(stua_obj o) { stb_arr_push(stu__gc_root_stack, o); return o; }
void     stua_poproot (   void   ) { stb_arr_pop(stu__gc_root_stack); }

static stb_sdict *stu__strings;
static void stu__mark(stua_obj z)
{
   int i;
   stu__box *p = stu__ptr(z);
   if (p->stua_gc == 1) return; // already marked
   assert(p->stua_gc == 0);
   p->stua_gc = 1;
   switch(p->type) {
      case STU___function: {
         stu__func *f = (stu__func *) p;
         if ((uint) f->closure_source <= 4) {
            if (f->closure_source == 0) {
               for (i=1; i <= f->code[0]; ++i)
                  if (!stu__number(((stua_obj *) f->code)[-i]))
                     stu__mark(((stua_obj *) f->code)[-i]);
            }
            for (i=0; i < f->num_param; ++i)
               stu__mark(f->param[i]);
         } else {
            stu__mark(f->closure_source);
            stu__mark(f->f.closure_data);
         }
         break;
      }
      case STU___dict: {
         stua_dict *e = (stua_dict *) p;
         for (i=0; i < e->limit; ++i)
            if (e->table[i].k != STB_IEMPTY && e->table[i].k != STB_IDEL) {
               if (!stu__number(e->table[i].k)) stu__mark((int) e->table[i].k);
               if (!stu__number(e->table[i].v)) stu__mark((int) e->table[i].v);
            }
         break;
      }
   }
}

static int stu__num_allocs, stu__size_allocs;
static stua_obj stu__flow_val = stua_nil; // used for break & return

static void stua_gc(int force)
{
   int i;
   if (!force && stu__num_allocs == 0 && stu__size_allocs == 0) return;
   stu__num_allocs = stu__size_allocs = 0;
   //printf("[gc]\n");

   // clear marks
   for (i=0; i < stb_arr_len(stu__gc_ptrlist); ++i)
       stu__gc_ptrlist[i]->stua_gc = 0;

   // stu__mark everything reachable
   stu__nil.stua_gc = stu__true.stua_gc = stu__false.stua_gc = 1;
   stu__mark(stua_globals);
   if (!stu__number(stu__flow_val)) 
      stu__mark(stu__flow_val);
   for (i=0; i < stb_arr_len(stu__gc_root_stack); ++i)
      if (!stu__number(stu__gc_root_stack[i]))
         stu__mark(stu__gc_root_stack[i]);

   // sweep unreachables
   for (i=0; i < stb_arr_len(stu__gc_ptrlist);) {
      stu__box *z = stu__gc_ptrlist[i];         
      if (!z->stua_gc) {
         switch (z->type) {
            case STU___dict:        stb_idict_destroy((stua_dict *) z); break;
            case STU___error:       free(((stu__wrapper *) z)->ptr); break;
            case STU___string:      stb_sdict_remove(stu__strings, (char*) ((stu__wrapper *) z)->ptr, NULL); free(z); break;
            case STU___function:    stu__free_func((stu__func *) z); break;
         }
         // swap in the last item over this, and repeat
         z = stb_arr_pop(stu__gc_ptrlist);
         stu__gc_ptrlist[i] = z;         
      } else
         ++i;
   }
}

static void stu__consider_gc(stua_obj x)
{
   if (stu__size_allocs < 100000) return;
   if (stu__num_allocs < 10 && stu__size_allocs < 1000000) return;
   stb_arr_push(stu__gc_root_stack, x);
   stua_gc(0);
   stb_arr_pop(stu__gc_root_stack);
}

static stua_obj stu__makeobj(int type, void *data, int size, int safe_to_gc)
{
   stua_obj x = stu__makeptr(data);
   ((stu__box *) data)->type = type;
   stb_arr_push(stu__gc_ptrlist, (stu__box *) data);
   stu__num_allocs  += 1;
   stu__size_allocs += size;
   if (safe_to_gc) stu__consider_gc(x);
   return x;
}

stua_obj stua_box(int type, void *data, int size)
{
   stu__wrapper *p = (stu__wrapper *) malloc(sizeof(*p));
   p->ptr = data;
   return stu__makeobj(type, p, size, 0);
}

// a stu string can be directly compared for equality, because
// they go into a hash table
stua_obj stua_string(char *z)
{
   stu__wrapper *b = (stu__wrapper *) stb_sdict_get(stu__strings, z);
   if (b == NULL) {
      int o = stua_box(STU___string, NULL, strlen(z) + sizeof(*b));
      b = stu__pw(o);
      stb_sdict_add(stu__strings, z, b);
      stb_sdict_getkey(stu__strings, z, (char **) &b->ptr);
   }
   return stu__makeptr(b);
}

// stb_obj dictionary is just an stb_extra
static void     stu__set(stua_dict *d, stua_obj k, stua_obj v)
{ if (stb_idict_set(d, k, v)) stu__size_allocs += 8; }

static stua_obj stu__get(stua_dict *d, stua_obj k, stua_obj res)
{
   stb_idict_get_flag(d, k, &res);
   return res;
}

static stua_obj make_string(char *z, int len)
{
   stua_obj s;
   char temp[256], *q = (char *) stb_temp(temp, len+1), *p = q;
   while (len > 0) {
      if (*z == '\\') {
              if (z[1] == 'n') *p = '\n';
         else if (z[1] == 'r') *p = '\r';
         else if (z[1] == 't') *p = '\t';
         else                  *p = z[1];
         p += 1; z += 2; len -= 2;
      } else {
         *p++ = *z++; len -= 1;
      }
   }
   *p = 0;
   s = stua_string(q);
   stb_tempfree(temp, q);
   return s;
}

enum token_names
{
   T__none=128,
   ST_shl = STUA_op_shl,    ST_ge  = STUA_op_ge,
   ST_shr = STUA_op_shr,    ST_le = STUA_op_le,
   ST_shru = STUA_op_shru,  STU__negate = STUA_op_negate,
   ST__reset_numbering = STUA_op_last,
   ST_white,
   ST_id, ST_float, ST_decimal, ST_hex, ST_char,ST_string, ST_number,
   // make sure the keywords come _AFTER_ ST_id, so stb_lex prefer them
   ST_if,      ST_while,    ST_for,     ST_eq,  ST_nil,
   ST_then,    ST_do,       ST_in,      ST_ne,  ST_true,
   ST_else,    ST_break,    ST_let,     ST_and, ST_false,
   ST_elseif,  ST_continue, ST_into,    ST_or,  ST_repeat,
   ST_end,     ST_as,       ST_return,  ST_var, ST_func,
   ST_catch,   ST__frame,
   ST__max_terminals,

   STU__defaultparm, STU__seq,
};

static stua_dict  * stu__globaldict;
       stua_obj     stua_globals;

static enum
{
   FLOW_normal,  FLOW_continue,   FLOW_break,  FLOW_return,  FLOW_error,
} stu__flow;

stua_obj stua_error(char *z, ...)
{
   stua_obj a;
   char temp[4096], *x;
   va_list v; va_start(v,z); vsprintf(temp, z, v); va_end(v);
   x = strdup(temp);
   a = stua_box(STU___error, x, strlen(x));
   stu__flow = FLOW_error;
   stu__flow_val = a;
   return stua_nil;
}

double stua_number(stua_obj z)
{
   return stu__tag(z) == stu__int_tag ? stu__int(z) : stu__float(z);
}

stua_obj stua_make_number(double d)
{
   double e = floor(d);
   if (e == d && e < (1 << 29) && e >= -(1 << 29))
      return stu__makeint((int) e);
   else
      return stua_float((float) d);
}

stua_obj (*stua_overload)(int op, stua_obj a, stua_obj b, stua_obj c) = NULL;

static stua_obj stu__op(int op, stua_obj a, stua_obj b, stua_obj c)
{
   stua_obj r = STUA_NO_VALUE;
   if (op == '+') {
      if (stu__checkt(STU___string, a) && stu__checkt(STU___string, b)) {
         ;// @TODO: string concatenation
      } else if (stu__checkt(STU___function, a) && stu__checkt(STU___dict, b)) {
         stu__func *f = (stu__func *) malloc(12);
         assert(offsetof(stu__func, code)==12);
         f->closure_source = a;
         f->f.closure_data = b;
         return stu__makeobj(STU___function, f, 16, 1);
      }
   }
   if (stua_overload) r = stua_overload(op,a,b,c);
   if (stu__flow != FLOW_error && r == STUA_NO_VALUE)
      stua_error("Typecheck for operator %d", op), r=stua_nil;
   return r;
}

#define STU__EVAL2(a,b)             \
          a = stu__eval(stu__f[n+1]);  if (stu__flow) break; stua_pushroot(a); \
          b = stu__eval(stu__f[n+2]);  stua_poproot(); if (stu__flow) break;

#define STU__FB(op)              \
          STU__EVAL2(a,b)           \
          if (stu__tag(a) == stu__int_tag && stu__tag(b) == stu__int_tag) \
             return ((a) op (b));                 \
          if (stu__number(a) && stu__number(b)) \
             return stua_make_number(stua_number(a) op stua_number(b)); \
          return stu__op(stu__f[n], a,b, stua_nil)

#define STU__F(op)              \
          STU__EVAL2(a,b)           \
          if (stu__number(a) && stu__number(b)) \
             return stua_make_number(stua_number(a) op stua_number(b)); \
          return stu__op(stu__f[n], a,b, stua_nil)

#define STU__I(op)               \
          STU__EVAL2(a,b)           \
          if (stu__tag(a) == stu__int_tag && stu__tag(b) == stu__int_tag) \
             return stu__makeint(stu__int(a) op stu__int(b));                 \
          return stu__op(stu__f[n], a,b, stua_nil)

#define STU__C(op)               \
          STU__EVAL2(a,b)           \
          if (stu__number(a) && stu__number(b)) \
             return (stua_number(a) op stua_number(b)) ? stua_true : stua_false; \
          return stu__op(stu__f[n], a,b, stua_nil)

#define STU__CE(op)              \
          STU__EVAL2(a,b)           \
          return (a op b) ? stua_true : stua_false

static short *stu__f;
static stua_obj  stu__f_obj;
static stua_dict       *stu__c;
static stua_obj stu__funceval(stua_obj fo, stua_obj co);

static int stu__cond(stua_obj x)
{
   if (stu__flow) return 0;
   if (!stu__checkt(STU___boolean, x))
      x = stu__op('!', x, stua_nil, stua_nil);
   if (x == stua_true ) return 1;
   if (x == stua_false) return 0;
   stu__flow = FLOW_error;
   return 0;
}

// had to manually eliminate tailcall recursion for debugging complex stuff
#define TAILCALL(x)   n = (x); goto top;
static stua_obj stu__eval(int n)
{
top:
   if (stu__flow >= FLOW_return) return stua_nil; // is this needed?
   if (n < 0) return stu__const(stu__f)[n];
   assert(n != 0 && n != 1);
   switch (stu__f[n]) {
      stua_obj a,b,c;
      case ST_catch:   a = stu__eval(stu__f[n+1]);
                       if (stu__flow == FLOW_error) { a=stu__flow_val; stu__flow = FLOW_normal; }
                       return a;
      case ST_var:     b = stu__eval(stu__f[n+2]); if (stu__flow) break;
                       stu__set(stu__c, stu__const(stu__f)[stu__f[n+1]], b);
                       return b;
      case STU__seq:   stu__eval(stu__f[n+1]); if (stu__flow) break;
                       TAILCALL(stu__f[n+2]);
      case ST_if:      if (!stu__cond(stu__eval(stu__f[n+1]))) return stua_nil;
                       TAILCALL(stu__f[n+2]);
      case ST_else:    a = stu__cond(stu__eval(stu__f[n+1]));
                       TAILCALL(stu__f[n + 2 + !a]);
                       #define STU__HANDLE_BREAK            \
                          if (stu__flow >= FLOW_break) {    \
                             if (stu__flow == FLOW_break) { \
                                a = stu__flow_val;          \
                                stu__flow = FLOW_normal;    \
                                stu__flow_val = stua_nil;   \
                                return a;                   \
                             }                              \
                             return stua_nil;               \
                          }
      case ST_as:      stu__eval(stu__f[n+3]);
                       STU__HANDLE_BREAK
                       // fallthrough!
      case ST_while:   a = stua_nil; stua_pushroot(a);
                       while (stu__cond(stu__eval(stu__f[n+1]))) {
                          stua_poproot();
                          a = stu__eval(stu__f[n+2]);
                          STU__HANDLE_BREAK
                          stu__flow = FLOW_normal;  // clear 'continue' flag
                          stua_pushroot(a);
                          if (stu__f[n+3]) stu__eval(stu__f[n+3]);
                          STU__HANDLE_BREAK
                          stu__flow = FLOW_normal;  // clear 'continue' flag
                       }
                       stua_poproot();
                       return a;
      case ST_break:   stu__flow = FLOW_break;  stu__flow_val = stu__eval(stu__f[n+1]); break;
      case ST_continue:stu__flow = FLOW_continue; break;
      case ST_return:  stu__flow = FLOW_return; stu__flow_val = stu__eval(stu__f[n+1]); break;
      case ST__frame:  return stu__f_obj;
      case '[':        STU__EVAL2(a,b);
                       if (stu__checkt(STU___dict, a))
                          return stu__get(stu__pd(a), b, stua_nil);
                       return stu__op(stu__f[n], a, b, stua_nil);
      case '=':        a = stu__eval(stu__f[n+2]); if (stu__flow) break;
                       n = stu__f[n+1];
                       if (stu__f[n] == ST_id) {
                          if (!stb_idict_update(stu__c, stu__const(stu__f)[stu__f[n+1]], a))
                             if (!stb_idict_update(stu__globaldict, stu__const(stu__f)[stu__f[n+1]], a))
                                return stua_error("Assignment to undefined variable");
                       } else if (stu__f[n] == '[') {
                          stua_pushroot(a);
                          b = stu__eval(stu__f[n+1]); if (stu__flow) { stua_poproot(); break; }
                          stua_pushroot(b);
                          c = stu__eval(stu__f[n+2]); stua_poproot(); stua_poproot();
                          if (stu__flow) break;
                          if (!stu__checkt(STU___dict, b)) return stua_nil;
                          stu__set(stu__pd(b), c, a);
                       } else {
                          return stu__op(stu__f[n], stu__eval(n), a, stua_nil);
                       }
                       return a;
      case STU__defaultparm:
                       a = stu__eval(stu__f[n+2]);
                       stu__flow = FLOW_normal;
                       if (stb_idict_add(stu__c, stu__const(stu__f)[stu__f[n+1]], a))
                          stu__size_allocs += 8;
                       return stua_nil;
      case ST_id:      a = stu__get(stu__c, stu__const(stu__f)[stu__f[n+1]], STUA_NO_VALUE); // try local variable
                       return a != STUA_NO_VALUE       // else try stu__compile_global_scope variable
                            ? a : stu__get(stu__globaldict, stu__const(stu__f)[stu__f[n+1]], stua_nil);
      case STU__negate:a = stu__eval(stu__f[n+1]); if (stu__flow) break;
                       return stu__isint(a) ? -a : stu__op(stu__f[n], a, stua_nil, stua_nil);
      case '~':        a = stu__eval(stu__f[n+1]); if (stu__flow) break;
                       return stu__isint(a) ? (~a)&~3 : stu__op(stu__f[n], a, stua_nil, stua_nil);
      case '!':        a = stu__eval(stu__f[n+1]); if (stu__flow) break;
                       a = stu__cond(a); if (stu__flow) break;
                       return a ? stua_true : stua_false;
      case ST_eq: STU__CE(==); case ST_le: STU__C(<=); case '<': STU__C(<);
      case ST_ne: STU__CE(!=); case ST_ge: STU__C(>=); case '>': STU__C(>);
      case '+' : STU__FB(+);  case '*': STU__F(*);  case '&': STU__I(&); case ST_shl: STU__I(<<);
      case '-' : STU__FB(-);  case '/': STU__F(/);  case '|': STU__I(|); case ST_shr: STU__I(>>);
                             case '%': STU__I(%);  case '^': STU__I(^);
      case ST_shru:    STU__EVAL2(a,b);
                       if (stu__tag(a) == stu__int_tag && stu__tag(b) == stu__int_tag)
                          return stu__makeint((unsigned) stu__int(a) >> stu__int(b));
                       return stu__op(stu__f[n], a,b, stua_nil);
      case ST_and:      a = stu__eval(stu__f[n+1]); b = stu__cond(a); if (stu__flow) break;
                       return a ? stu__eval(stu__f[n+2]) : a;
      case ST_or :      a = stu__eval(stu__f[n+1]); b = stu__cond(a); if (stu__flow) break;
                       return a ? b : stu__eval(stu__f[n+2]);
      case'(':case':': STU__EVAL2(a,b);
                       if (!stu__checkt(STU___function, a))
                           return stu__op(stu__f[n], a,b, stua_nil);
                       if (!stu__checkt(STU___dict, b))
                           return stua_nil;
                       if (stu__f[n] == ':')
                          b = stu__makeobj(STU___dict, stb_idict_copy(stu__pd(b)), stb_idict_memory_usage(stu__pd(b)), 0);
                       a = stu__funceval(a,b);
                       return a;
      case '{' :    {
                       stua_dict *d;
                       d = stb_idict_new_size(stu__f[n+1] > 40 ? 64 : 16);
                       if (d == NULL)
                          return stua_nil; // breakpoint fodder
                       c = stu__makeobj(STU___dict, d, 32, 1);
                       stua_pushroot(c);
                       a = stu__f[n+1];
                       for (b=0; b < a; ++b) {
                          stua_obj x = stua_pushroot(stu__eval(stu__f[n+2 + b*2 + 0]));
                          stua_obj y = stu__eval(stu__f[n+2 + b*2 + 1]);
                          stua_poproot();
                          if (stu__flow) { stua_poproot(); return stua_nil; }
                          stu__set(d, x, y);
                       }
                       stua_poproot();
                       return c;
                    }
      default:         if (stu__f[n] < 0) return stu__const(stu__f)[stu__f[n]];
                       assert(0); /* NOTREACHED */ // internal error!
   }
   return stua_nil;
}

int stb__stua_nesting;
static stua_obj stu__funceval(stua_obj fo, stua_obj co)
{
   stu__func *f = stu__pf(fo);
   stua_dict *context = stu__pd(co);
   int i,j;
   stua_obj p;
   short *tf = stu__f;     // save previous function
   stua_dict *tc = stu__c;

   if (stu__flow == FLOW_error) return stua_nil;
   assert(stu__flow == FLOW_normal);

   stua_pushroot(fo);
   stua_pushroot(co);
   stu__consider_gc(stua_nil);

   while ((uint) f->closure_source > 4) {
      // add data from closure to context
      stua_dict *e = (stua_dict *) stu__pd(f->f.closure_data);
      for (i=0; i < e->limit; ++i)
         if (e->table[i].k != STB_IEMPTY && e->table[i].k != STB_IDEL)
            if (stb_idict_add(context, e->table[i].k, e->table[i].v))
               stu__size_allocs += 8;
            // use add so if it's already defined, we don't override it; that way
            // explicit parameters win over applied ones, and most recent applications
            // win over previous ones
      f = stu__pf(f->closure_source);
   }

   for (j=0, i=0; i < f->num_param; ++i)
      // if it doesn't already exist, add it from the numbered parameters
      if (stb_idict_add(context, f->param[i], stu__get(context, stu__int(j), stua_nil)))
         ++j;

   // @TODO: if (stu__get(context, stu__int(f->num_param+1)) != STUA_NO_VALUE) // error: too many parameters
   // @TODO: ditto too few parameters

   if (f->closure_source == 4)
      p = f->f.func(context);
   else {
      stu__f = f->code, stu__c = context;
      stu__f_obj = co;
      ++stb__stua_nesting;
      if (stu__f[1]) 
         p = stu__eval(stu__f[1]);
      else
         p = stua_nil;
      --stb__stua_nesting;
      stu__f = tf, stu__c = tc;  // restore previous function
      if (stu__flow == FLOW_return) {
         stu__flow = FLOW_normal;
         p = stu__flow_val;
         stu__flow_val = stua_nil;
      }
   }

   stua_poproot();
   stua_poproot();

   return p;
}

// Parser

static int stu__tok;
static stua_obj stu__tokval;

static char *stu__curbuf, *stu__bufstart;

static stb_matcher *stu__lex_matcher;

static unsigned char stu__prec[ST__max_terminals], stu__end[ST__max_terminals];

static void stu__nexttoken(void)
{
   int len;

retry:
   stu__tok = stb_lex(stu__lex_matcher, stu__curbuf, &len);
   if (stu__tok == 0)
      return;
   switch(stu__tok) {
      case ST_white  : stu__curbuf += len; goto retry;
      case T__none  : stu__tok = *stu__curbuf; break;
      case ST_string:  stu__tokval = make_string(stu__curbuf+1, len-2); break;
      case ST_id    :  stu__tokval = make_string(stu__curbuf, len); break;
      case ST_hex    : stu__tokval = stu__makeint(strtol(stu__curbuf+2,NULL,16)); stu__tok = ST_number; break;
      case ST_decimal: stu__tokval = stu__makeint(strtol(stu__curbuf  ,NULL,10)); stu__tok = ST_number; break;
      case ST_float  : stu__tokval = stua_float((float) atof(stu__curbuf))       ; stu__tok = ST_number; break;
      case ST_char   : stu__tokval = stu__curbuf[2] == '\\' ? stu__curbuf[3] : stu__curbuf[2];
                      if (stu__curbuf[3] == 't') stu__tokval = '\t';
                      if (stu__curbuf[3] == 'n') stu__tokval = '\n';
                      if (stu__curbuf[3] == 'r') stu__tokval = '\r';
                      stu__tokval = stu__makeint(stu__tokval);
                      stu__tok  = ST_number;
                      break;
   }
   stu__curbuf += len;
}

static struct { int stu__tok; char *regex; } stu__lexemes[] =
{
   ST_white  , "([ \t\n\r]|/\\*(.|\n)*\\*/|//[^\r\n]*([\r\n]|$))+",
   ST_id     , "[_a-zA-Z][_a-zA-Z0-9]*",
   ST_hex    , "0x[0-9a-fA-F]+",
   ST_decimal, "[0-9]+[0-9]*",
   ST_float  , "[0-9]+\\.?[0-9]*([eE][-+]?[0-9]+)?",
   ST_float  , "\\.[0-9]+([eE][-+]?[0-9]+)?",
   ST_char   , "c'(\\\\.|[^\\'])'",
   ST_string , "\"(\\\\.|[^\\\"\n\r])*\"",
   ST_string , "\'(\\\\.|[^\\\'\n\r])*\'",

   #define stua_key4(a,b,c,d)  ST_##a, #a, ST_##b, #b, ST_##c, #c, ST_##d, #d,
   stua_key4(if,then,else,elseif)    stua_key4(while,do,for,in)
   stua_key4(func,var,let,break)     stua_key4(nil,true,false,end)
   stua_key4(return,continue,as,repeat) stua_key4(_frame,catch,catch,catch)

   ST_shl, "<<",   ST_and, "&&",  ST_eq,  "==",  ST_ge, ">=", 
   ST_shr, ">>",   ST_or , "||",  ST_ne,  "!=",  ST_le, "<=",
   ST_shru,">>>",  ST_into, "=>",
   T__none, ".",
};

typedef struct
{
   STB__ARR(stua_obj) data;    // constants being compiled
   STB__ARR(short)    code;    // code being compiled
   stua_dict         *locals;
   STB__ARR(short)    non_local_refs;
} stu__comp_func;

static stu__comp_func stu__pfunc;
static STB__ARR(stu__comp_func) func_stack = NULL;
static void stu__push_func_comp(void)
{
   stb_arr_push(func_stack, stu__pfunc);
   stu__pfunc.data = NULL;
   stu__pfunc.code = NULL;
   stu__pfunc.locals = stb_idict_new_size(16);
   stu__pfunc.non_local_refs = NULL;
   stb_arr_push(stu__pfunc.code, 0); // number of data items
   stb_arr_push(stu__pfunc.code, 1); // starting execution address
}

static void stu__pop_func_comp(void)
{
   stb_arr_free(stu__pfunc.code);
   stb_arr_free(stu__pfunc.data);   
   stb_idict_destroy(stu__pfunc.locals);
   stb_arr_free(stu__pfunc.non_local_refs);
   stu__pfunc = stb_arr_pop(func_stack);
}

// if an id is a reference to an outer lexical scope, this
// function returns the "name" of it, and updates the stack
// structures to make sure the names are propogated in.
static int stu__nonlocal_id(stua_obj var_obj)
{
   stua_obj dummy, var = var_obj;
   int i, n = stb_arr_len(func_stack), j,k;
   if (stb_idict_get_flag(stu__pfunc.locals, var, &dummy)) return 0;
   for (i=n-1; i > 1; --i) {
      if (stb_idict_get_flag(func_stack[i].locals, var, &dummy))
         break;
   }
   if (i <= 1) return 0; // stu__compile_global_scope
   j = i; // need to access variable from j'th frame
   for (i=0; i < stb_arr_len(stu__pfunc.non_local_refs); ++i)
      if (stu__pfunc.non_local_refs[i] == j) return j-n;
   stb_arr_push(stu__pfunc.non_local_refs, j-n);
   // now make sure all the parents propogate it down
   for (k=n-1; k > 1; --k) {
      if (j-k >= 0) return j-n; // comes direct from this parent
      for(i=0; i < stb_arr_len(func_stack[k].non_local_refs); ++i)
         if (func_stack[k].non_local_refs[i] == j-k)
            return j-n;
      stb_arr_push(func_stack[k].non_local_refs, j-k);
   }
   assert (k != 1);

   return j-n;
}

static int stu__off(void)                { return stb_arr_len(stu__pfunc.code); }
static void stu__cc(int a)
{
   assert(a >= -2000 && a < 5000);
   stb_arr_push(stu__pfunc.code, a);
}
static int stu__cc1(int a)                      { stu__cc(a); return stu__off()-1; }
static int stu__cc2(int a, int b)               { stu__cc(a); stu__cc(b); return stu__off()-2; }
static int stu__cc3(int a, int b, int c)        {
 if (a == '=') assert(c != 0);
 stu__cc(a); stu__cc(b); stu__cc(c); return stu__off()-3; }
static int stu__cc4(int a, int b, int c, int d) { stu__cc(a); stu__cc(b); stu__cc(c); stu__cc(d); return stu__off()-4; }

static int stu__cdv(stua_obj p)
{
   int i;
   assert(p != STUA_NO_VALUE);
   for (i=0; i < stb_arr_len(stu__pfunc.data); ++i)
      if (stu__pfunc.data[i] == p)
         break;
   if (i == stb_arr_len(stu__pfunc.data))
      stb_arr_push(stu__pfunc.data, p);
   return ~i;
}

static int stu__cdt(void)
{
   int z = stu__cdv(stu__tokval);
   stu__nexttoken();
   return z;
}

static int stu__seq(int a, int b)
{
   return !a ? b : !b ? a : stu__cc3(STU__seq, a,b);
}

static char stu__comp_err_str[1024];
static int stu__comp_err_line;
static int stu__err(char *str, ...)
{
   va_list v;
   char *s = stu__bufstart;
   stu__comp_err_line = 1;
   while (s < stu__curbuf) {
      if (s[0] == '\n' || s[0] == '\r') {
         if (s[0]+s[1] == '\n' + '\r') ++s;
         ++stu__comp_err_line;
      }
      ++s;
   }
   va_start(v, str);
   vsprintf(stu__comp_err_str, str, v);
   va_end(v);
   return 0;
}

static int stu__accept(int p)
{
   if (stu__tok != p) return 0;
   stu__nexttoken();
   return 1;
}

static int stu__demand(int p)
{
   if (stu__accept(p)) return 1;
   return stu__err("Didn't find expected stu__tok");
}

static int stu__demandv(int p, stua_obj *val)
{
   if (stu__tok == p || p==0) {
      *val = stu__tokval;
      stu__nexttoken();
      return 1;
   } else
      return 0;
}

static int stu__expr(int p);
int stu__nexpr(int p) { stu__nexttoken(); return stu__expr(p); }
static int stu__statements(int once, int as);

static int stu__parse_if(void)      // parse both ST_if and ST_elseif
{
   int b,c,a;
   a = stu__nexpr(1);               if (!a) return 0;
   if (!stu__demand(ST_then))       return stu__err("expecting THEN");
   b = stu__statements(0,0);        if (!b) return 0;
   if (b == 1) b = -1;

   if (stu__tok == ST_elseif) {
      return stu__parse_if();
   } else if (stu__accept(ST_else)) {
      c = stu__statements(0,0); if (!c) return 0;
      if (!stu__demand(ST_end)) return stu__err("expecting END after else clause");
      return stu__cc4(ST_else, a, b, c);
   } else {
      if (!stu__demand(ST_end)) return stu__err("expecting END in if statement");
      return stu__cc3(ST_if, a, b);
   }
}

int stu__varinit(int z, int in_globals)
{
   int a,b;
   stu__nexttoken();
   while (stu__demandv(ST_id, &b)) {
      if (!stb_idict_add(stu__pfunc.locals, b, 1))
         if (!in_globals) return stu__err("Redefined variable %s.", stu__pw(b)->ptr);
      if (stu__accept('=')) {
         a = stu__expr(1);       if (!a) return 0;
      } else
         a = stu__cdv(stua_nil);
      z = stu__seq(z, stu__cc3(ST_var, stu__cdv(b), a));
      if (!stu__accept(',')) break;
   }
   return z;
}

static int stu__compile_unary(int z, int outparm, int require_inparm)
{
   int op = stu__tok, a, b;
   stu__nexttoken();
   if (outparm) {
      if (require_inparm || (stu__tok && stu__tok != ST_end && stu__tok != ST_else && stu__tok != ST_elseif && stu__tok !=';')) {
         a = stu__expr(1); if (!a) return 0;
      } else
         a = stu__cdv(stua_nil);
      b = stu__cc2(op, a);
   } else
      b = stu__cc1(op);
   return stu__seq(z,b);
}

static int stu__assign(void)
{
   int z;
   stu__accept(ST_let);
   z = stu__expr(1); if (!z) return 0;
   if (stu__accept('=')) {
      int y,p = (z >= 0 ? stu__pfunc.code[z] : 0);
      if (z < 0 || (p != ST_id && p != '[')) return stu__err("Invalid lvalue in assignment");
      y = stu__assign();         if (!y) return 0;
      z = stu__cc3('=', z, y);
   }
   return z;
}

static int stu__statements(int once, int stop_while)
{
   int a,b, c, z=0;
   for(;;) {
      switch (stu__tok) {
         case ST_if     : a = stu__parse_if(); if (!a) return 0;
                          z = stu__seq(z, a);
                          break;
         case ST_while  : if (stop_while) return (z ? z:1);
                          a = stu__nexpr(1); if (!a) return 0;
                          if (stu__accept(ST_as)) c = stu__statements(0,0); else c = 0;
                          if (!stu__demand(ST_do)) return stu__err("expecting DO");
                          b = stu__statements(0,0); if (!b) return 0;
                          if (!stu__demand(ST_end)) return stu__err("expecting END");
                          if (b == 1) b = -1;
                          z = stu__seq(z, stu__cc4(ST_while, a, b, c));
                          break;
         case ST_repeat : stu__nexttoken();
                          c = stu__statements(0,1); if (!c) return 0;
                          if (!stu__demand(ST_while)) return stu__err("expecting WHILE");
                          a = stu__expr(1); if (!a) return 0;
                          if (!stu__demand(ST_do)) return stu__err("expecting DO");
                          b = stu__statements(0,0); if (!b) return 0;
                          if (!stu__demand(ST_end)) return stu__err("expecting END");
                          if (b == 1) b = -1;
                          z = stu__seq(z, stu__cc4(ST_as, a, b, c));
                          break;
         case ST_catch  : a = stu__nexpr(1); if (!a) return 0;
                          z = stu__seq(z, stu__cc2(ST_catch, a));
                          break;
         case ST_var    : z = stu__varinit(z,0); break;
         case ST_return : z = stu__compile_unary(z,1,1); break;
         case ST_continue:z = stu__compile_unary(z,0,0); break;
         case ST_break  : z = stu__compile_unary(z,1,0); break;
         case ST_into   : if (z == 0 && !once) return stu__err("=> cannot be first statement in block");
                          a = stu__nexpr(99);
                          b = (a >= 0? stu__pfunc.code[a] : 0);
                          if (a < 0 || (b != ST_id && b != '[')) return stu__err("Invalid lvalue on right side of =>");
                          z = stu__cc3('=', a, z);
                          break;
         default        : if (stu__end[stu__tok]) return once ? 0 : (z ? z:1);
                          a = stu__assign(); if (!a) return 0;
                          stu__accept(';');
                          if (stu__tok && !stu__end[stu__tok]) {
                             if (a < 0)
                                return stu__err("Constant has no effect");
                             if (stu__pfunc.code[a] != '(' && stu__pfunc.code[a] != '=')
                                return stu__err("Expression has no effect");
                          }
                          z = stu__seq(z, a);
                          break;
      }
      if (!z) return 0;
      stu__accept(';');
      if (once && stu__tok != ST_into) return z;
   }
}

static int stu__postexpr(int z, int p);
static int stu__dictdef(int end, int *count)
{
   int z,n=0,i,flags=0;
   STB__ARR(short) dict=NULL;
   stu__nexttoken();
   while (stu__tok != end) {
      if (stu__tok == ST_id) {
         stua_obj id = stu__tokval;
         stu__nexttoken();
         if (stu__tok == '=') {
            flags |= 1;
            stb_arr_push(dict, stu__cdv(id));
            z = stu__nexpr(1); if (!z) return 0;
         } else {
            z = stu__cc2(ST_id, stu__cdv(id));
            z = stu__postexpr(z,1); if (!z) return 0;
            flags |= 2;
            stb_arr_push(dict, stu__cdv(stu__makeint(n++)));
         }
      } else {
         z = stu__expr(1); if (!z) return 0;
         flags |= 2;
         stb_arr_push(dict, stu__cdv(stu__makeint(n++)));
      }
      if (end != ')' && flags == 3) { z=stu__err("can't mix initialized and uninitialized defs"); goto done;}
      stb_arr_push(dict, z);
      if (!stu__accept(',')) break;
   }
   if (!stu__demand(end))
      return stu__err(end == ')' ? "Expecting ) at end of function call" 
                                 : "Expecting } at end of dictionary definition");
   z = stu__cc2('{', stb_arr_len(dict)/2);
   for (i=0; i < stb_arr_len(dict); ++i)
      stu__cc(dict[i]);
   if (count) *count = n;
done:
   stb_arr_free(dict);
   return z;
}

static int stu__comp_id(void)
{
   int z,d;
   d = stu__nonlocal_id(stu__tokval);
   if (d == 0)
      return z = stu__cc2(ST_id, stu__cdt());
   // access a non-local frame by naming it with the appropriate int
   assert(d < 0);
   z = stu__cdv(d);            // relative frame # is the 'variable' in our local frame
   z = stu__cc2(ST_id, z);     // now access that dictionary
   return stu__cc3('[', z, stu__cdt()); // now access the variable from that dir
}

static stua_obj stu__funcdef(stua_obj *id, stua_obj *func);
static int stu__expr(int p)
{
   int z;
   // unary
   switch (stu__tok) {
      case ST_number: z = stu__cdt(); break;
      case ST_string: z = stu__cdt(); break;  // @TODO - string concatenation like C
      case ST_id    : z = stu__comp_id(); break;
      case ST__frame: z = stu__cc1(ST__frame); stu__nexttoken(); break;
      case ST_func  : z = stu__funcdef(NULL,NULL); break;
      case ST_if    : z = stu__parse_if(); break;
      case ST_nil   : z = stu__cdv(stua_nil); stu__nexttoken(); break;
      case ST_true  : z = stu__cdv(stua_true); stu__nexttoken(); break;
      case ST_false : z = stu__cdv(stua_false); stu__nexttoken(); break;
      case '-'      : z = stu__nexpr(99); if (z) z=stu__cc2(STU__negate,z); else return z; break;
      case '!'      : z = stu__nexpr(99); if (z) z=stu__cc2('!',z); else return z; break;
      case '~'      : z = stu__nexpr(99); if (z) z=stu__cc2('~',z); else return z; break;
      case '{'      : z = stu__dictdef('}', NULL); break;
      default       : return stu__err("Unexpected token");
      case '('      : stu__nexttoken(); z = stu__statements(0,0); if (!stu__demand(')')) return stu__err("Expecting )");
   }
   return stu__postexpr(z,p);
}

static int stu__postexpr(int z, int p)
{
   int q;
   // postfix
   while (stu__tok == '(' || stu__tok == '[' || stu__tok == '.') {
      if (stu__accept('.')) {
         // MUST be followed by a plain identifier! use [] for other stuff
         if (stu__tok != ST_id) return stu__err("Must follow . with plain name; try [] instead");
         z = stu__cc3('[', z, stu__cdv(stu__tokval));
         stu__nexttoken();
      } else if (stu__accept('[')) {
         while (stu__tok != ']') {
            int r = stu__expr(1); if (!r) return 0;
            z = stu__cc3('[', z, r);
            if (!stu__accept(',')) break;
         }
         if (!stu__demand(']')) return stu__err("Expecting ]");
      } else {
         int n, p = stu__dictdef(')', &n); if (!p) return 0;
         #if 0 // this is incorrect!
         if (z > 0 && stu__pfunc.code[z] == ST_id) {
            stua_obj q = stu__get(stu__globaldict, stu__pfunc.data[-stu__pfunc.code[z+1]-1], stua_nil);
            if (stu__checkt(STU___function, q))
               if ((stu__pf(q))->num_param != n)
                  return stu__err("Incorrect number of parameters");
         }
         #endif
         z = stu__cc3('(', z, p);
      }
   }
   // binop - this implementation taken from lcc
   for (q=stu__prec[stu__tok]; q >= p; --q) {
      while (stu__prec[stu__tok] == q) {
         int o = stu__tok, y = stu__nexpr(p+1); if (!y) return 0;
         z = stu__cc3(o,z,y);
      }
   }
   return z;
}

static stua_obj stu__finish_func(STB__ARR(stua_obj) param, int start)
{
   int n, size;
   stu__func *f = (stu__func *) malloc(sizeof(*f));
   f->closure_source = 0;
   f->num_param = stb_arr_len(param);
   f->param = (int *) stb_copy(param, f->num_param * sizeof(*f->param));
   size = stb_arr_storage(stu__pfunc.code) + stb_arr_storage(stu__pfunc.data) + sizeof(*f) + 8;
   f->f.store = malloc(stb_arr_storage(stu__pfunc.code) + stb_arr_storage(stu__pfunc.data));
   f->code = (short *) ((char *) f->f.store + stb_arr_storage(stu__pfunc.data));
   memcpy(f->code, stu__pfunc.code, stb_arr_storage(stu__pfunc.code));
   f->code[1] = start;
   f->code[0] = stb_arr_len(stu__pfunc.data);
   for (n=0; n < f->code[0]; ++n)
      ((stua_obj *) f->code)[-1-n] = stu__pfunc.data[n];
   return stu__makeobj(STU___function, f, size, 0);
}

static int stu__funcdef(stua_obj *id, stua_obj *result)
{
   int n,z=0,i,q;
   STB__ARR(stua_obj) param = NULL;
   STB__ARR(short) nonlocal;
   stua_obj v,f=stua_nil;
   assert(stu__tok == ST_func);
   stu__nexttoken();
   if (id) { 
      if (!stu__demandv(ST_id, id)) return stu__err("Expecting function name");
   } else
      stu__accept(ST_id);
   if (!stu__demand('(')) return stu__err("Expecting ( for function parameter");
   stu__push_func_comp();
   while (stu__tok != ')') {
      if (!stu__demandv(ST_id, &v)) { z=stu__err("Expecting parameter name"); goto done; }
      stb_idict_add(stu__pfunc.locals, v, 1);
      if (stu__tok == '=') {
         n = stu__nexpr(1); if (!n) { z=0; goto done; }
         z = stu__seq(z, stu__cc3(STU__defaultparm, stu__cdv(v), n));
      } else
         stb_arr_push(param, v);
      if (!stu__accept(',')) break;
   }
   if (!stu__demand(')'))   { z=stu__err("Expecting ) at end of parameter list"); goto done; }
   n = stu__statements(0,0);   if (!n) { z=0; goto done; }
   if (!stu__demand(ST_end)) { z=stu__err("Expecting END at end of function"); goto done; }
   if (n == 1) n = 0;
   n = stu__seq(z,n);
   f = stu__finish_func(param, n);
   if (result) { *result = f; z=1; stu__pop_func_comp(); }
   else {
      nonlocal = stu__pfunc.non_local_refs;
      stu__pfunc.non_local_refs = NULL;
      stu__pop_func_comp();
      z = stu__cdv(f);
      if (nonlocal) {  // build a closure with references to the needed frames
         STB__ARR(short) initcode = NULL;
         for (i=0; i < stb_arr_len(nonlocal); ++i) {
            int k = nonlocal[i], p;
            stb_arr_push(initcode, stu__cdv(k));
            if (k == -1) p = stu__cc1(ST__frame);
            else { p = stu__cdv(stu__makeint(k+1)); p = stu__cc2(ST_id, p); }
            stb_arr_push(initcode, p);
         }
         q = stu__cc2('{', stb_arr_len(nonlocal));
         for (i=0; i < stb_arr_len(initcode); ++i)
            stu__cc(initcode[i]);
         z = stu__cc3('+', z, q);
         stb_arr_free(initcode);
      }
      stb_arr_free(nonlocal);
   }
done:
   stb_arr_free(param);
   if (!z) stu__pop_func_comp();
   return z;
}

static int stu__compile_global_scope(void)
{
   stua_obj o;
   int z=0;

   stu__push_func_comp();
   while (stu__tok != 0) {
      if (stu__tok == ST_func) {
         stua_obj id, f;
         if (!stu__funcdef(&id,&f))
            goto error;
         stu__set(stu__globaldict, id, f);
      } else if (stu__tok == ST_var) {
         z = stu__varinit(z,1); if (!z) goto error;
      } else {
         int y = stu__statements(1,0); if (!y) goto error;
         z = stu__seq(z,y);
      }
      stu__accept(';');
   }
   o = stu__finish_func(NULL, z);
   stu__pop_func_comp();

   o = stu__funceval(o, stua_globals); // initialize stu__globaldict
   if (stu__flow == FLOW_error)
      printf("Error: %s\n", ((stu__wrapper *) stu__ptr(stu__flow_val))->ptr);
   return 1;
error:
   stu__pop_func_comp();
   return 0;
}

stua_obj stu__myprint(stua_dict *context)
{
   stua_obj x = stu__get(context, stua_string("x"), stua_nil);
   if ((x & 1) == stu__float_tag) printf("%f", stu__getfloat(x));
   else if (stu__tag(x) == stu__int_tag) printf("%d", stu__int(x));
   else {
       stu__wrapper *s = stu__pw(x);
       if (s->type == STU___string || s->type == STU___error)
          printf("%s", s->ptr);
       else if (s->type == STU___dict) printf("{{dictionary}}");
       else if (s->type == STU___function) printf("[[function]]");
       else
          printf("[[ERROR:%s]]", s->ptr);
   }
   return x;
}

void stua_init(void)
{
   if (!stu__globaldict) {
      int i;
      stua_obj s;
      stu__func *f;

      stu__prec[ST_and] = stu__prec[ST_or] =                     1;
      stu__prec[ST_eq ] = stu__prec[ST_ne] = stu__prec[ST_le] =
       stu__prec[ST_ge] = stu__prec['>' ]  = stu__prec['<'] =    2;
      stu__prec[':']    =                                        3;
      stu__prec['&']    = stu__prec['|']   = stu__prec['^'] =    4;
      stu__prec['+']    = stu__prec['-']   =                     5;
      stu__prec['*']    = stu__prec['/']   = stu__prec['%'] =
       stu__prec[ST_shl]= stu__prec[ST_shr]= stu__prec[ST_shru]= 6;

      stu__end[')']   = stu__end[ST_end] = stu__end[ST_else] = 1;
      stu__end[ST_do] = stu__end[ST_elseif] = 1;

      stu__float_init();
      stu__lex_matcher = stb_lex_matcher();
      for (i=0; i < sizeof(stu__lexemes)/sizeof(stu__lexemes[0]); ++i)
         stb_lex_item(stu__lex_matcher, stu__lexemes[i].regex, stu__lexemes[i].stu__tok);

      stu__globaldict = stb_idict_new_size(64);
      stua_globals    = stu__makeobj(STU___dict, stu__globaldict, 0,0);
      stu__strings    = stb_sdict_new(0);

      stu__curbuf = stu__bufstart = "func _print(x) end\n"
      "func print()\n  var x=0 while _frame[x] != nil as x=x+1 do _print(_frame[x]) end end\n";
      stu__nexttoken();
      if (!stu__compile_global_scope())
         printf("Compile error in line %d: %s\n", stu__comp_err_line, stu__comp_err_str);

      s = stu__get(stu__globaldict, stua_string("_print"), stua_nil);
      if (stu__tag(s) == stu__ptr_tag && stu__ptr(s)->type == STU___function) {
         f = stu__pf(s);
         free(f->f.store);
         f->closure_source = 4;
         f->f.func = stu__myprint;
         f->code = NULL;
      }
   }
}

void stua_uninit(void)
{
   if (stu__globaldict) {
      stb_idict_remove_all(stu__globaldict);
      stb_arr_setlen(stu__gc_root_stack, 0);
      stua_gc(1);
      stb_idict_destroy(stu__globaldict);
      stb_sdict_delete(stu__strings);
      stb_matcher_free(stu__lex_matcher);
      stu__gc_ptrlist = stb_arr_free(stu__gc_ptrlist);
      func_stack = stb_arr_free(func_stack);
      stu__gc_root_stack = stb_arr_free(stu__gc_root_stack);
      stu__globaldict = NULL;
   }
}

void stua_run_script(char *s)
{
   stua_init();

   stu__curbuf = stu__bufstart = s;
   stu__nexttoken();

   stu__flow = FLOW_normal;

   if (!stu__compile_global_scope())
      printf("Compile error in line %d: %s\n", stu__comp_err_line, stu__comp_err_str);
   stua_gc(1);
}
#endif // STB_DEFINE

#endif // STB_STUA


//////////////////////////////////////////////////////////////////////////////
//
//             STB-C sliding-window dictionary compression
//
//  This uses a DEFLATE-style sliding window, but no bitwise entropy.
//  Everything is on byte boundaries, so you could then apply a byte-wise
//  entropy code, though that's nowhere near as effective.
//
//  An STB-C stream begins with a 16-byte header:
//      4 bytes: 0x57 0xBC 0x00 0x00
//      8 bytes: big-endian size of decompressed data, 64-bits
//      4 bytes: big-endian size of window (how far back decompressor may need)
//
//  The following symbols appear in the stream (these were determined ad hoc,
//  not by analysis):
//
//  [dict]      00000100 yyyyyyyy yyyyyyyy yyyyyyyy xxxxxxxx xxxxxxxx
//  [END]       00000101 11111010 cccccccc cccccccc cccccccc cccccccc
//  [dict]      00000110 yyyyyyyy yyyyyyyy yyyyyyyy xxxxxxxx
//  [literals]  00000111 zzzzzzzz zzzzzzzz
//  [literals]  00001zzz zzzzzzzz
//  [dict]      00010yyy yyyyyyyy yyyyyyyy xxxxxxxx xxxxxxxx
//  [dict]      00011yyy yyyyyyyy yyyyyyyy xxxxxxxx
//  [literals]  001zzzzz
//  [dict]      01yyyyyy yyyyyyyy xxxxxxxx
//  [dict]      1xxxxxxx yyyyyyyy
//
//      xxxxxxxx: match length - 1
//      yyyyyyyy: backwards distance - 1
//      zzzzzzzz: num literals - 1
//      cccccccc: adler32 checksum of decompressed data
//   (all big-endian)


STB_EXTERN stb_uint stb_decompress_length(stb_uchar *input);
STB_EXTERN stb_uint stb_decompress(stb_uchar *out,stb_uchar *in,stb_uint len);
STB_EXTERN stb_uint stb_compress  (stb_uchar *out,stb_uchar *in,stb_uint len);
STB_EXTERN void stb_compress_window(int z);
STB_EXTERN void stb_compress_hashsize(unsigned int z);

STB_EXTERN int stb_compress_tofile(char *filename, char *in,  stb_uint  len);
STB_EXTERN int stb_compress_intofile(FILE *f, char *input,    stb_uint  len);
STB_EXTERN char *stb_decompress_fromfile(char *filename,      stb_uint *len);

STB_EXTERN int stb_compress_stream_start(FILE *f);
STB_EXTERN void stb_compress_stream_end(int close);
STB_EXTERN void stb_write(char *data, int data_len);

#ifdef STB_DEFINE

stb_uint stb_decompress_length(stb_uchar *input)
{
   return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

////////////////////           decompressor         ///////////////////////

// simple implementation that just writes whole thing into big block

static unsigned char *stb_barrier;
static unsigned char *stb_barrier_2;
static unsigned char *stb_barrier_3;
static unsigned char *stb_barrier_4;

static stb_uchar *stb__dout;
static void stb__match(stb_uchar *data, stb_uint length)
{
   // INVERSE of memmove... write each byte before copying the next...
   assert (stb__dout + length <= stb_barrier);
   if (stb__dout + length > stb_barrier) { stb__dout += length; return; }
   if (data < stb_barrier_4) { stb__dout = stb_barrier+1; return; }
   while (length--) *stb__dout++ = *data++;
}

static void stb__lit(stb_uchar *data, stb_uint length)
{
   assert (stb__dout + length <= stb_barrier);
   if (stb__dout + length > stb_barrier) { stb__dout += length; return; }
   if (data < stb_barrier_2) { stb__dout = stb_barrier+1; return; }
   memcpy(stb__dout, data, length);
   stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static stb_uchar *stb_decompress_token(stb_uchar *i)
{
   if (*i >= 0x20) { // use fewer if's for cases that expand small
      if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
      else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
      else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
   } else { // more ifs for cases that expand large, since overhead is amortized
      if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
      else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
      else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
      else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
      else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
      else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
   }
   return i;
}

stb_uint stb_decompress(stb_uchar *output, stb_uchar *i, stb_uint length)
{
   stb_uint olen;
   if (stb__in4(0) != 0x57bC0000) return 0;
   if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
   olen = stb_decompress_length(i);
   stb_barrier_2 = i;
   stb_barrier_3 = i+length;
   stb_barrier = output + olen;
   stb_barrier_4 = output;
   i += 16;

   stb__dout = output;
   while (1) {
      stb_uchar *old_i = i;
      i = stb_decompress_token(i);
      if (i == old_i) {
         if (*i == 0x05 && i[1] == 0xfa) {
            assert(stb__dout == output + olen);
            if (stb__dout != output + olen) return 0;
            if (stb_adler32(1, output, olen) != (stb_uint) stb__in4(2))
               return 0;
            return olen;
         } else {
            assert(0); /* NOTREACHED */
            return 0;
         }
      }
      assert(stb__dout <= output + olen); 
      if (stb__dout > output + olen)
         return 0;
   }
}

char *stb_decompress_fromfile(char *filename, unsigned int *len)
{
   unsigned int n;
   char *q;
   unsigned char *p;
   FILE *f = fopen(filename, "rb");   if (f == NULL) return NULL;
   fseek(f, 0, SEEK_END);
   n = ftell(f);
   fseek(f, 0, SEEK_SET);
   p = (unsigned char * ) malloc(n); if (p == NULL) return NULL;
   fread(p, 1, n, f);
   fclose(f);
   if (p == NULL) return NULL;
   if (p[0] != 0x57 || p[1] != 0xBc || p[2] || p[3]) { free(p); return NULL; }
   q = (char *) malloc(stb_decompress_length(p)+1);
   if (!q) { free(p); free(p); return NULL; }
   *len = stb_decompress((unsigned char *) q, p, n);
   if (*len) q[*len] = 0;
   free(p);
   return q;
}

#if 0
//  streaming decompressor

static struct
{
   stb__uchar *in_buffer;
   stb__uchar *match;

   stb__uint pending_literals;
   stb__uint pending_match;
} xx;



static void stb__match(stb_uchar *data, stb_uint length)
{
   // INVERSE of memmove... write each byte before copying the next...
   assert (stb__dout + length <= stb_barrier);
   if (stb__dout + length > stb_barrier) { stb__dout += length; return; }
   if (data < stb_barrier_2) { stb__dout = stb_barrier+1; return; }
   while (length--) *stb__dout++ = *data++;
}

static void stb__lit(stb_uchar *data, stb_uint length)
{
   assert (stb__dout + length <= stb_barrier);
   if (stb__dout + length > stb_barrier) { stb__dout += length; return; }
   if (data < stb_barrier_2) { stb__dout = stb_barrier+1; return; }
   memcpy(stb__dout, data, length);
   stb__dout += length;
}

static void sx_match(stb_uchar *data, stb_uint length)
{
   xx.match = data;
   xx.pending_match = length;
}

static void sx_lit(stb_uchar *data, stb_uint length)
{
   xx.pending_lit = length;
}

static int stb_decompress_token_state(void)
{
   stb__uchar *i = xx.in_buffer;

   if (*i >= 0x20) { // use fewer if's for cases that expand small
      if (*i >= 0x80)       sx_match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
      else if (*i >= 0x40)  sx_match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
      else /* *i >= 0x20 */ sx_lit(i+1, i[0] - 0x20 + 1), i += 1;
   } else { // more ifs for cases that expand large, since overhead is amortized
      if (*i >= 0x18)       sx_match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
      else if (*i >= 0x10)  sx_match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
      else if (*i >= 0x08)  sx_lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2;
      else if (*i == 0x07)  sx_lit(i+3, stb__in2(1) + 1), i += 3;
      else if (*i == 0x06)  sx_match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
      else if (*i == 0x04)  sx_match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
      else return 0;
   }
   xx.in_buffer = i;
   return 1;
}
#endif



////////////////////           compressor         ///////////////////////

static unsigned int stb_matchlen(stb_uchar *m1, stb_uchar *m2, stb_uint maxlen)
{
   stb_uint i;
   for (i=0; i < maxlen; ++i)
      if (m1[i] != m2[i]) return i;
   return i;
}

// simple implementation that just takes the source data in a big block

static stb_uchar *stb__out;
static FILE      *stb__outfile;
static stb_uint   stb__outbytes;

static void stb__write(unsigned char v)
{
   fputc(v, stb__outfile);
   ++stb__outbytes;
}

#define stb_out(v)    (stb__out ? *stb__out++ = (stb_uchar) (v) : stb__write((stb_uchar) (v)))

static void stb_out2(stb_uint v)
{
   stb_out(v >> 8);
   stb_out(v);
}

static void stb_out3(stb_uint v) { stb_out(v >> 16); stb_out(v >> 8); stb_out(v); }
static void stb_out4(stb_uint v) { stb_out(v >> 24); stb_out(v >> 16);
                                   stb_out(v >> 8 ); stb_out(v);                  }

static void outliterals(stb_uchar *in, int numlit)
{
   while (numlit > 65536) {
      outliterals(in,65536);
      in     += 65536;
      numlit -= 65536;
   }

   if      (numlit ==     0)    ;
   else if (numlit <=    32)    stb_out (0x000020 + numlit-1);
   else if (numlit <=  2048)    stb_out2(0x000800 + numlit-1);
   else /*  numlit <= 65536) */ stb_out3(0x070000 + numlit-1);

   if (stb__out) {
      memcpy(stb__out,in,numlit);
      stb__out += numlit;
   } else
      fwrite(in, 1, numlit, stb__outfile);
}

static int stb__window = 0x40000; // 256K
void stb_compress_window(int z)
{
   if (z >= 0x1000000) z = 0x1000000; // limit of implementation
   if (z <      0x100) z = 0x100;   // insanely small
   stb__window = z;
}

static int stb_not_crap(int best, int dist)
{
   return   ((best > 2  &&  dist <= 0x00100)     
          || (best > 5  &&  dist <= 0x04000)
          || (best > 7  &&  dist <= 0x80000));
}

static  stb_uint stb__hashsize = 32768;
void stb_compress_hashsize(unsigned int y)
{
   unsigned int z = 1024;
   while (z < y) z <<= 1;
   stb__hashsize = z >> 2;   // pass in bytes, store #pointers
}

// note that you can play with the hashing functions all you
// want without needing to change the decompressor
#define stb__hc(q,h,c)      (((h) << 7) + ((h) >> 25) + q[c])
#define stb__hc2(q,h,c,d)   (((h) << 14) + ((h) >> 18) + (q[c] << 7) + q[d])
#define stb__hc3(q,c,d,e)   ((q[c] << 14) + (q[d] << 7) + q[e])

static stb_uint32 stb__running_adler;

static int stb_compress_chunk(stb_uchar *history,
                              stb_uchar *start,
                              stb_uchar *end,
                              int length,
                              int *pending_literals,
                              stb_uchar **chash,
                              stb_uint mask)
{
   int window = stb__window;
   stb_uint match_max;
   stb_uchar *lit_start = start - *pending_literals;
   stb_uchar *q = start;

   #define STB__SCRAMBLE(h)   (((h) + ((h) >> 16)) & mask)

   // stop short of the end so we don't scan off the end doing
   // the hashing; this means we won't compress the last few bytes
   // unless they were part of something longer
   while (q < start+length && q+12 < end) {
      int m;
      stb_uint h1,h2,h3,h4, h;
      stb_uchar *t;
      int best = 2, dist=0;

      if (q+65536 > end)
         match_max = end-q;
      else
         match_max = 65536;

      #define stb__nc(b,d)  ((d) <= window && ((b) > 9 || stb_not_crap(b,d)))

      #define STB__TRY(t,p)  /* avoid retrying a match we already tried */ \
                      if (p ? dist != q-t : 1)                             \
                      if ((m = stb_matchlen(t, q, match_max)) > best)     \
                      if (stb__nc(m,q-(t)))                                \
                          best = m, dist = q - (t)

      // rather than search for all matches, only try 4 candidate locations,
      // chosen based on 4 different hash functions of different lengths.
      // this strategy is inspired by LZO; hashing is unrolled here using the
      // 'hc' macro
      h = stb__hc3(q,0, 1, 2); h1 = STB__SCRAMBLE(h);
                                      t = chash[h1]; if (t) STB__TRY(t,0);
      h = stb__hc2(q,h, 3, 4); h2 = STB__SCRAMBLE(h);
      h = stb__hc2(q,h, 5, 6);        t = chash[h2]; if (t) STB__TRY(t,1);
      h = stb__hc2(q,h, 7, 8); h3 = STB__SCRAMBLE(h);
      h = stb__hc2(q,h, 9,10);        t = chash[h3]; if (t) STB__TRY(t,1);
      h = stb__hc2(q,h,11,12); h4 = STB__SCRAMBLE(h);
                                      t = chash[h4]; if (t) STB__TRY(t,1);

      // because we use a shared hash table, can only update it
      // _after_ we've probed all of them
      chash[h1] = chash[h2] = chash[h3] = chash[h4] = q;

      if (best > 2)
         assert(dist > 0);

      // see if our best match qualifies
      if (best < 3) { // fast path literals
         ++q;
      } else if (best > 2  &&  best <= 0x80    &&  dist <= 0x100) {
         outliterals(lit_start, q-lit_start); lit_start = (q += best);
         stb_out(0x80 + best-1);
         stb_out(dist-1);
      } else if (best > 5  &&  best <= 0x100   &&  dist <= 0x4000) {
         outliterals(lit_start, q-lit_start); lit_start = (q += best);
         stb_out2(0x4000 + dist-1);       
         stb_out(best-1);
      } else if (best > 7  &&  best <= 0x100   &&  dist <= 0x80000) {
         outliterals(lit_start, q-lit_start); lit_start = (q += best);
         stb_out3(0x180000 + dist-1);     
         stb_out(best-1);
      } else if (best > 8  &&  best <= 0x10000 &&  dist <= 0x80000) {
         outliterals(lit_start, q-lit_start); lit_start = (q += best);
         stb_out3(0x100000 + dist-1);     
         stb_out2(best-1);
      } else if (best > 9                      &&  dist <= 0x1000000) {
         outliterals(lit_start, q-lit_start); lit_start = (q += best);
         if (best <= 0x100) {
            stb_out(0x06);
            stb_out3(dist-1);
            stb_out(best-1);
         } else {
            stb_out(0x04);
            stb_out3(dist-1);
            stb_out2(best-1);
         }
      } else {  // fallback literals if no match was a balanced tradeoff
         ++q;
      }
   }

   // if we didn't get all the way, add the rest to literals
   if (q-start < length)
      q = start+length;

   // the literals are everything from lit_start to q
   *pending_literals = (q - lit_start);

   stb__running_adler = stb_adler32(stb__running_adler, start, q - start);
   return q - start;
}

static int stb_compress_inner(stb_uchar *input, stb_uint length)
{
   int literals = 0;
   stb_uint len,i;

   stb_uchar **chash;
   chash = (stb_uchar**) malloc(stb__hashsize * sizeof(stb_uchar*));
   if (chash == NULL) return 0; // failure
   for (i=0; i < stb__hashsize; ++i)
      chash[i] = NULL;

   // stream signature
   stb_out(0x57); stb_out(0xbc);
   stb_out2(0);

   stb_out4(0);       // 64-bit length requires 32-bit leading 0
   stb_out4(length);
   stb_out4(stb__window);

   stb__running_adler = 1;

   len = stb_compress_chunk(input, input, input+length, length, &literals, chash, stb__hashsize-1);
   assert(len == length);

   outliterals(input+length - literals, literals);

   free(chash);

   stb_out2(0x05fa); // end opcode

   stb_out4(stb__running_adler);

   return 1; // success
}

stb_uint stb_compress(stb_uchar *out, stb_uchar *input, stb_uint length)
{
   stb__out = out;
   stb__outfile = NULL;

   stb_compress_inner(input, length);

   return stb__out - out;
}

int stb_compress_tofile(char *filename, char *input, unsigned int length)
{
   int maxlen = length + 512 + (length >> 2); // total guess
   char *buffer = (char *) malloc(maxlen);
   int blen = stb_compress((stb_uchar*)buffer, (stb_uchar*)input, length);
   
   stb__out = NULL;
   stb__outfile = fopen(filename, "wb");
   if (!stb__outfile) return 0;

   stb__outbytes = 0;

   if (!stb_compress_inner((stb_uchar*)input, length))
      return 0;

   fclose(stb__outfile);

   return stb__outbytes;
}

int stb_compress_intofile(FILE *f, char *input, unsigned int length)
{
   int maxlen = length + 512 + (length >> 2); // total guess
   //char *buffer = (char*)malloc(maxlen);
   //int blen = stb_compress((stb_uchar*)buffer, (stb_uchar*)input, length);
   
   stb__out = NULL;
   stb__outfile = f;
   if (!stb__outfile) return 0;

   stb__outbytes = 0;

   if (!stb_compress_inner((stb_uchar*)input, length))
      return 0;

   return stb__outbytes;
}

//////////////////////    streaming I/O version    /////////////////////


static stb_uint stb_out_backpatch_id(void)
{
   if (stb__out)
      return (stb_uint) stb__out;
   else
      return ftell(stb__outfile);
}

static void stb_out_backpatch(stb_uint id, stb_uint value)
{
   stb_uchar data[4] = { value >> 24, value >> 16, value >> 8, value };
   if (stb__out) {
      memcpy((void *) id, data, 4);
   } else {
      stb_uint where = ftell(stb__outfile);
      fseek(stb__outfile, id, SEEK_SET);
      fwrite(data, 4, 1, stb__outfile);
      fseek(stb__outfile, where, SEEK_SET);
   }
}

// ok, the wraparound buffer was a total failure. let's instead
// use a copying-in-place buffer, which lets us share the code.
// This is way less efficient but it'll do for now.

static struct
{
   stb_uchar *buffer;
   int size;           // physical size of buffer in bytes

   int valid;          // amount of valid data in bytes
   int start;          // bytes of data already output

   int window;
   int fsize;

   int pending_literals; // bytes not-quite output but counted in start
   int length_id;

   stb_uint total_bytes;

   stb_uchar **chash;
   stb_uint    hashmask;
} xtb;

static int stb_compress_streaming_start(void)
{
   stb_uint i;
   xtb.size = stb__window * 3;
   xtb.buffer = (stb_uchar*)malloc(xtb.size);
   if (!xtb.buffer) return 0;

   xtb.chash = (stb_uchar**)malloc(sizeof(*xtb.chash) * stb__hashsize);
   if (!xtb.chash) {
      free(xtb.buffer);
      return 0;
   }

   for (i=0; i < stb__hashsize; ++i)
      xtb.chash[i] = NULL;

   xtb.hashmask = stb__hashsize-1;

   xtb.valid        = 0;
   xtb.start        = 0;
   xtb.window       = stb__window;
   xtb.fsize        = stb__window;
   xtb.pending_literals = 0;
   xtb.total_bytes  = 0;

      // stream signature
   stb_out(0x57); stb_out(0xbc); stb_out2(0);

   stb_out4(0);       // 64-bit length requires 32-bit leading 0

   xtb.length_id = stb_out_backpatch_id();
   stb_out4(0);       // we don't know the output length yet

   stb_out4(stb__window);

   stb__running_adler = 1;

   return 1;
}

static int stb_compress_streaming_end(void)
{
   // flush out any remaining data
   stb_compress_chunk(xtb.buffer, xtb.buffer+xtb.start, xtb.buffer+xtb.valid,
                      xtb.valid-xtb.start, &xtb.pending_literals, xtb.chash, xtb.hashmask);

   // write out pending literals
   outliterals(xtb.buffer + xtb.valid - xtb.pending_literals, xtb.pending_literals);

   stb_out2(0x05fa); // end opcode
   stb_out4(stb__running_adler);

   stb_out_backpatch(xtb.length_id, xtb.total_bytes);

   free(xtb.buffer);
   free(xtb.chash);
   return 1;
}

void stb_write(char *data, int data_len)
{
   stb_uint i;

   // @TODO: fast path for filling the buffer and doing nothing else
   //   if (xtb.valid + data_len < xtb.size)

   xtb.total_bytes += data_len;

   while (data_len) {
      // fill buffer
      if (xtb.valid < xtb.size) {
         int amt = xtb.size - xtb.valid;
         if (data_len < amt) amt = data_len;
         memcpy(xtb.buffer + xtb.valid, data, amt);
         data_len -= amt;
         data     += amt;
         xtb.valid += amt;
      }
      if (xtb.valid < xtb.size)
         return;

      // at this point, the buffer is full

      // if we can process some data, go for it; make sure
      // we leave an 'fsize's worth of data, though
      if (xtb.start + xtb.fsize < xtb.valid) {
         int amount = (xtb.valid - xtb.fsize) - xtb.start;
         int n;
         assert(amount > 0);
         n = stb_compress_chunk(xtb.buffer, xtb.buffer + xtb.start, xtb.buffer + xtb.valid,
                                amount, &xtb.pending_literals, xtb.chash, xtb.hashmask);
         xtb.start += n;
      }

      assert(xtb.start + xtb.fsize >= xtb.valid);
      // at this point, our future size is too small, so we
      // need to flush some history. we, in fact, flush exactly
      // one window's worth of history

      {
         int flush = xtb.window;
         assert(xtb.start >= flush);
         assert(xtb.valid >= flush);

         // if 'pending literals' extends back into the shift region,
         // write them out
         if (xtb.start - xtb.pending_literals < flush) {
            outliterals(xtb.buffer + xtb.start - xtb.pending_literals, xtb.pending_literals);
            xtb.pending_literals = 0;
         }

         // now shift the window
         memmove(xtb.buffer, xtb.buffer + flush, xtb.valid - flush);
         xtb.start -= flush;
         xtb.valid -= flush;
   
         for (i=0; i <= xtb.hashmask; ++i)
            if (xtb.chash[i] < xtb.buffer + flush)
               xtb.chash[i] = NULL;
            else
               xtb.chash[i] -= flush;
      }
      // and now that we've made room for more data, go back to the top
   }
}

int stb_compress_stream_start(FILE *f)
{
   stb__out = NULL;
   stb__outfile = f;

   if (f == NULL)
      return 0;

   if (!stb_compress_streaming_start())
      return 0;

   return 1;
}

void stb_compress_stream_end(int close)
{
   stb_compress_streaming_end();
   if (close && stb__outfile) {
      fclose(stb__outfile);
   }
}

#endif // STB_DEFINE


//////////////////////////////////////////////////////////////////////////////
//
//                         Threads
//


typedef void * (*stb_thread_func)(void *);

// do not rely on these types, this is an implementation detail.
// compare against STB_THREAD_NULL and ST_SEMAPHORE_NULL
typedef void *stb_thread;
typedef void *stb_semaphore;
typedef void *stb_mutex;
#define STB_SEMAPHORE_NULL    NULL
#define STB_THREAD_NULL       NULL
#define STB_MUTEX_NULL        NULL

// get the number of processors (limited to those in the affinity mask for this process).
STB_EXTERN int stb_processor_count(void);

// stb_work functions: queue up work to be done by some worker threads

// set number of threads to serve the queue; you can change this on the fly,
// but if you decrease it, it won't decrease until things currently on the
// queue are finished
STB_EXTERN void          stb_work_numthreads(int n);
// set maximum number of units in the queue; you can only set this BEFORE running any work functions
STB_EXTERN int           stb_work_maxunits(int n);
// enqueue some work to be done (can do this from any thread, or even from a piece of work);
// return value of f is stored in *return_code if non-NULL
STB_EXTERN int           stb_work(stb_thread_func f, void *d, volatile void **return_code);
// as above, but stb_sem_release is called on 'rel' after work is complete
STB_EXTERN int           stb_work_sem(stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel);


// support for independent queues with their own threads

typedef struct stb__workqueue stb_workqueue;

STB_EXTERN stb_workqueue*stb_workq_new(int numthreads, int max_units);
STB_EXTERN stb_workqueue*stb_workq_new_flags(int numthreads, int max_units, int no_add_mutex, int no_remove_mutex);
STB_EXTERN void          stb_workq_delete(stb_workqueue *q);
STB_EXTERN void          stb_workq_numthreads(stb_workqueue *q, int n);
STB_EXTERN int           stb_workq(stb_workqueue *q, stb_thread_func f, void *d, volatile void **return_code);
STB_EXTERN int           stb_workq_sem(stb_workqueue *q, stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel);
STB_EXTERN int           stb_workq_length(stb_workqueue *q);

STB_EXTERN stb_thread    stb_create_thread (stb_thread_func f, void *d);
STB_EXTERN stb_thread    stb_create_thread2(stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel);
STB_EXTERN void          stb_destroy_thread(stb_thread t);

STB_EXTERN stb_semaphore stb_sem_new(int max_val, int is_blocked_flag);
STB_EXTERN void          stb_sem_delete (stb_semaphore s);
STB_EXTERN void          stb_sem_waitfor(stb_semaphore s);
STB_EXTERN void          stb_sem_release(stb_semaphore s);

STB_EXTERN stb_mutex     stb_mutex_new(void);
STB_EXTERN void          stb_mutex_delete(stb_mutex m);
STB_EXTERN void          stb_mutex_begin(stb_mutex m);
STB_EXTERN void          stb_mutex_end(stb_mutex m);

#ifdef STB_DEFINE

typedef struct
{
   stb_thread_func f;
   void *d;
   volatile void **return_val;
   stb_semaphore sem;
} stb__thread;

// initialize this in lowest-level create thread routine if not initted
static stb_semaphore stb__threadsem;

#ifdef _WIN32

// avoid including windows.h -- note that our definitions aren't
// exactly the same (we don't define the security descriptor struct)
// so if you want to include windows.h, make sure you do it first.
#include <process.h>

#ifndef _WINDOWS_  // check windows.h guard
#define STB__IMPORT   STB_EXTERN __declspec(dllimport)
#define STB__DW       unsigned long

STB__IMPORT int     __stdcall TerminateThread(void *, STB__DW);
STB__IMPORT void *  __stdcall CreateSemaphoreA(void *sec, long,long,char*);
STB__IMPORT int     __stdcall CloseHandle(void *);
STB__IMPORT STB__DW __stdcall WaitForSingleObject(void *, STB__DW);
STB__IMPORT int     __stdcall ReleaseSemaphore(void *, long, long *);
STB__IMPORT void    __stdcall Sleep(STB__DW);
#endif

static void stb__thread_run(void *t)
{
   void *res;
   stb__thread info = * (stb__thread *) t;
   free(t);
   res = info.f(info.d);
   if (info.return_val)
      *info.return_val = res;
   if (info.sem != STB_SEMAPHORE_NULL)
      stb_sem_release(info.sem);
}

static stb_thread stb_create_thread_raw(stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel)
{
#ifdef _MT
#if defined(STB_FASTMALLOC) && !defined(STB_FASTMALLOC_ITS_OKAY_I_ONLY_MALLOC_IN_ONE_THREAD)
   stb_fatal("Error! Cannot use STB_FASTMALLOC with threads.\n");
   return STB_THREAD_NULL;
#else
   unsigned long id;
   stb__thread *data = (stb__thread *) malloc(sizeof(*data));
   if (!data) return NULL;
   if (stb__threadsem == STB_SEMAPHORE_NULL)
      stb__threadsem = stb_sem_new(1,0);
   data->f = f;
   data->d = d;
   data->return_val = return_code;
   data->sem = rel;
   id = _beginthread(stb__thread_run, 0, data);
   if (id == -1) return NULL;
   return (void *) id;
#endif
#else
   stb_fatal("Must compile mult-threaded to use stb_thread/stb_work.");
   return NULL;
#endif
}

// trivial win32 wrappers
void          stb_destroy_thread(stb_thread t)   { TerminateThread(t,0); }
stb_semaphore stb_sem_new(int maxv,int blocked)  { return CreateSemaphoreA(NULL,blocked?0:maxv,maxv,NULL); }
void          stb_sem_delete(stb_semaphore s)    { if (s != NULL) CloseHandle(s); }
void          stb_sem_waitfor(stb_semaphore s)   { WaitForSingleObject(s, 0xffffffff); } // INFINITE
void          stb_sem_release(stb_semaphore s)   { ReleaseSemaphore(s,1,NULL); }
static void   stb__thread_sleep(int ms)          { Sleep(ms); }

#ifndef _WINDOWS_
STB__IMPORT int __stdcall GetProcessAffinityMask(void *, STB__DW *, STB__DW *);
STB__IMPORT void * __stdcall GetCurrentProcess(void);
#endif

int stb_processor_count(void)
{
   unsigned long proc,sys;
   GetProcessAffinityMask(GetCurrentProcess(), &proc, &sys);
   return stb_bitcount(proc);
}

#endif // _WIN32

stb_thread stb_create_thread2(stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel)
{
   return stb_create_thread_raw(f,d,return_code,rel);
}

stb_thread stb_create_thread(stb_thread_func f, void *d)
{
   return stb_create_thread2(f,d,NULL,STB_SEMAPHORE_NULL);
}

// mutex implemented by wrapping semaphore
stb_mutex stb_mutex_new(void)            { return stb_sem_new(1,0); }
void      stb_mutex_delete(stb_mutex m)  { stb_sem_delete (m);      }
void      stb_mutex_begin(stb_mutex m)   { stb_sem_waitfor(m);      }
void      stb_mutex_end(stb_mutex m)     { stb_sem_release(m);      }


static int stb__work_maxitems = 64;

typedef struct
{
   stb_thread_func f;
   void *d;
   volatile void **retval;
   stb_semaphore sem;
} stb__workinfo;

static volatile stb__workinfo *stb__work;

struct stb__workqueue
{
   int maxitems, numthreads;
   int oldest, newest;
   stb_semaphore add_mutex, remove_mutex, available;
   stb__workinfo *work;
};

static void *stb__thread_workloop(void *p)
{
   volatile stb_workqueue *q = (volatile stb_workqueue *) p;
   for(;;) {
      void *z;
      stb__workinfo w;
      stb_sem_waitfor(q->available);
      stb_sem_waitfor(q->remove_mutex);
      memcpy(&w, (void *) &q->work[q->oldest], sizeof(w)); // C++ won't copy
      if (++q->oldest == q->maxitems)
         q->oldest = 0;
      stb_sem_release(q->remove_mutex);
      if (w.f == NULL) // null work is a signal to end the thread
         return NULL;
      z = w.f(w.d);
      if (w.retval) *w.retval = z;
      if (w.sem != STB_SEMAPHORE_NULL) stb_sem_release(w.sem);
   }
}

static void *stb__thread_workloop_nomutex(void *p)
{
   volatile stb_workqueue *q = (volatile stb_workqueue *) p;
   for(;;) {
      void *z;
      stb__workinfo w;
      stb_sem_waitfor(q->available);
      memcpy(&w, (void *) &q->work[q->oldest], sizeof(w)); // C++ won't copy
      if (++q->oldest == q->maxitems)
         q->oldest = 0;
      if (w.f == NULL) // null work is a signal to end the thread
         return NULL;
      z = w.f(w.d);
      if (w.retval) *w.retval = z;
      if (w.sem != STB_SEMAPHORE_NULL) stb_sem_release(w.sem);
   }
}

static stb_workqueue *stb__work_global;

stb_workqueue *stb_workq_new(int num_threads, int max_units)
{
   return stb_workq_new_flags(num_threads, max_units, 0,0);
}

void stb__workq_delete_raw(stb_workqueue *q)
{
   free(q->work);
   stb_sem_delete(q->add_mutex);
   stb_sem_delete(q->remove_mutex);
   stb_sem_delete(q->available);
   free(q);
}

stb_workqueue *stb_workq_new_flags(int numthreads, int max_units, int no_add_mutex, int no_remove_mutex)
{
   stb_workqueue *q = (stb_workqueue *) malloc(sizeof(*q));
   if (q == NULL) return NULL;
   q->available    = stb_sem_new(stb__work_maxitems,1);
   q->add_mutex    = no_add_mutex    ? STB_SEMAPHORE_NULL : stb_sem_new(1,0);
   q->remove_mutex = no_remove_mutex ? STB_SEMAPHORE_NULL : stb_sem_new(1,0);
   q->maxitems = max_units < 1 ? 1 : max_units;
   ++q->maxitems; // since head cannot equal tail, we need one extra
   q->work = (stb__workinfo *) malloc(q->maxitems * sizeof(*q->work));
   q->newest = q->oldest = 0;
   q->numthreads = 0;
   if (q->work == NULL || q->available == STB_SEMAPHORE_NULL ||
       (q->add_mutex == STB_SEMAPHORE_NULL && !no_add_mutex) ||
       (q->remove_mutex == STB_SEMAPHORE_NULL && !no_remove_mutex)) {
      stb__workq_delete_raw(q);
      return NULL;
   }
   stb_workq_numthreads(q, numthreads);
   return q;
}

static void stb_work_init(int num_threads)
{
   if (stb__work_global == NULL) {
      if (stb__threadsem != STB_SEMAPHORE_NULL) stb_sem_waitfor(stb__threadsem);
      if (stb__work_global == NULL) {
         stb__work_global = stb_workq_new(num_threads, stb__work_maxitems);
      }
      if (stb__threadsem != STB_SEMAPHORE_NULL) stb_sem_release(stb__threadsem);
   }
}

void stb_workq_delete(stb_workqueue *q)
{
   for(;;) {
      stb_sem_waitfor(q->add_mutex);
      if (q->oldest == q->newest) {
         stb_sem_release(q->add_mutex);
         stb__workq_delete_raw(q);
         return;
      }
      stb__thread_sleep(1);
   }
}

int stb_work_maxunits(int n)
{
   if (stb__work_global == NULL) {
      stb__work_maxitems = n;
      stb_work_init(1);
   }
   return stb__work_maxitems;
}

static int stb__work_raw(stb_workqueue *q, stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel)
{
   int n, res;
   stb__workinfo w;
   if (q == NULL) {
      stb_work_init(1);
      q = stb__work_global;
   }
   w.f = f;
   w.d = d;
   w.retval = return_code;
   w.sem = rel;
   stb_sem_waitfor(q->add_mutex);
   n = q->newest+1; if (n == q->maxitems) n=0;
   if (n == q->oldest) {
      // wraparound, bail!
      res = 0;
   } else {
      res = 1;
      memcpy((void *) &q->work[q->newest], &w, sizeof(w));  // C++ won't copy
      q->newest = n;
   }
   stb_sem_release(q->add_mutex);
   if (res)
      stb_sem_release(q->available);
   return res;
}

int stb_workq_length(stb_workqueue *q)
{
   int o,n;
   if (q->remove_mutex) stb_sem_waitfor(q->remove_mutex);
   o = q->oldest;
   n = q->newest;
   if (q->remove_mutex) stb_sem_release(q->remove_mutex);
   if (n > o) o += q->maxitems;
   return o-n;
}

int stb_workq(stb_workqueue *q, stb_thread_func f, void *d, volatile void **return_code)
{
   if (f == NULL) return 0;
   return stb_workq_sem(q, f, d, return_code, NULL);
}

int stb_workq_sem(stb_workqueue *q, stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel)
{
   if (f == NULL) return 0;
   return stb__work_raw(q, f, d, return_code, rel);
}

void stb_workq_numthreads(stb_workqueue *q, int n)
{
   stb_sem_waitfor(stb__threadsem);
   while (q->numthreads < n) {
      if (q->remove_mutex == STB_SEMAPHORE_NULL)
         stb_create_thread(stb__thread_workloop_nomutex, q);
      else
         stb_create_thread(stb__thread_workloop, q);
      ++q->numthreads;
   }
   while (q->numthreads > n) {
      stb__work_raw(q, NULL, NULL, NULL, NULL);
      --q->numthreads;
   }
   stb_sem_release(stb__threadsem);
}

int stb_work(stb_thread_func f, void *d, volatile void **return_code)
{
   return stb_workq(stb__work_global, f,d,return_code);
}

int stb_work_sem(stb_thread_func f, void *d, volatile void **return_code, stb_semaphore rel)
{
   return stb_workq_sem(stb__work_global, f,d,return_code,rel);
}

void stb_work_numthreads(int n)
{
   if (stb__work_global == NULL)
      stb_work_init(n);
   else
      stb_workq_numthreads(stb__work_global, n);
}
#endif // STB_DEFINE

//////////////////////////////////////////////////////////////////////////////
//
//                         Fast malloc implementation
//
//   This is a clone of TCMalloc, but without the thread support.
//      1. large objects are allocated directly, page-aligned
//      2. small objects are allocated in homogeonous heaps, 0 overhead
//
//   We keep an allocation table for pages a la TCMalloc. This would
//   require 4MB for the entire address space, but we only allocate
//   the parts that are in use. The overhead from using homogenous heaps
//   everywhere is 3MB. (That is, if you allocate 1 object of each size,
//   you'll use 3MB.)

#if defined(STB_DEFINE) && (defined(_WIN32) || defined(STB_FASTMALLOC))

#ifdef _WIN32
   #ifndef _WINDOWS_
   STB__IMPORT void * __stdcall VirtualAlloc(void *p, unsigned long size, unsigned long type, unsigned long protect);
   STB__IMPORT int   __stdcall VirtualFree(void *p, unsigned long size, unsigned long freetype);
   #endif
   #define stb__alloc_pages_raw(x)     (stb_uint32) VirtualAlloc(NULL, (x), 0x3000, 0x04)
   #define stb__dealloc_pages_raw(p)   VirtualFree((void *) p, 0, 0x8000)
#else
   #error "Platform not currently supported"
#endif

typedef struct stb__span
{
   int                start, len;
   struct stb__span  *next, *prev;
   void              *first_free;
   unsigned short     list; // 1..256 free; 257..511 sizeclass; 0=large block
   short              allocations; // # outstanding allocations for sizeclass
} stb__span;  // 24

static stb__span **stb__span_for_page;
static int stb__firstpage, stb__lastpage;
static void stb__update_page_range(int first, int last)
{
   stb__span **sfp;
   int i, f,l;
   if (first >= stb__firstpage && last <= stb__lastpage) return;
   if (stb__span_for_page == NULL) {
      f = first;
      l = f+stb_max(last-f, 16384);
      l = stb_min(l, 1<<20);
   } else if (last > stb__lastpage) {
      f = stb__firstpage;
      l = f + (stb__lastpage - f) * 2;
      l = stb_clamp(last, l,1<<20);
   } else {
      l = stb__lastpage;
      f = l - (l - stb__firstpage) * 2;
      f = stb_clamp(f, 0,first);
   }
   sfp = (stb__span **) stb__alloc_pages_raw(sizeof(void *) * (l-f));
   for (i=f; i < stb__firstpage; ++i) sfp[i - f] = NULL;
   for (   ; i < stb__lastpage ; ++i) sfp[i - f] = stb__span_for_page[i - stb__firstpage];
   for (   ; i < l             ; ++i) sfp[i - f] = NULL;
   if (stb__span_for_page) stb__dealloc_pages_raw(stb__span_for_page);
   stb__firstpage = f;
   stb__lastpage  = l;
   stb__span_for_page = sfp;
}

static stb__span *stb__span_free=NULL;
static stb__span *stb__span_first, *stb__span_end;
static stb__span *stb__span_alloc(void)
{
   stb__span *s = stb__span_free;
   if (s)
      stb__span_free = s->next;
   else {
      if (!stb__span_first) {
         stb__span_first = (stb__span *) stb__alloc_pages_raw(65536);
         if (stb__span_first == NULL) return NULL;
         stb__span_end = stb__span_first + (65536 / sizeof(stb__span));
      }
      s = stb__span_first++;
      if (stb__span_first == stb__span_end) stb__span_first = NULL;
   }
   return s;
}

static stb__span *stb__spanlist[512];

static void stb__spanlist_unlink(stb__span *s)
{
   if (s->prev)
      s->prev->next = s->next;
   else {
      int n = s->list;
      assert(stb__spanlist[n] == s);
      stb__spanlist[n] = s->next;
   }
   if (s->next)
      s->next->prev = s->prev;
   s->next = s->prev = NULL;
   s->list = 0;
}

static void stb__spanlist_add(int n, stb__span *s)
{
   s->list = n;
   s->next = stb__spanlist[n];
   s->prev = NULL;
   stb__spanlist[n] = s;
   if (s->next) s->next->prev = s;
}

#define stb__page_shift       12
#define stb__page_size        (1 << stb__page_shift)
#define stb__page_number(x)   ((x) >> stb__page_shift)
#define stb__page_address(x)  ((x) << stb__page_shift)

static void stb__set_span_for_page(stb__span *s)
{
   int i;
   for (i=0; i < s->len; ++i)
      stb__span_for_page[s->start + i - stb__firstpage] = s;
}

static stb__span *stb__coalesce(stb__span *a, stb__span *b)
{
   assert(a->start + a->len == b->start);
   if (a->list) stb__spanlist_unlink(a);
   if (b->list) stb__spanlist_unlink(b);
   a->len += b->len;
   b->len = 0;
   b->next = stb__span_free;
   stb__span_free = b;
   stb__set_span_for_page(a);
   return a;
}

static void stb__free_span(stb__span *s)
{
   stb__span *n = NULL;
   if (s->start > stb__firstpage) {
      n = stb__span_for_page[s->start-1 - stb__firstpage];
      if (n && n->allocations == -2 && n->start + n->len == s->start) s = stb__coalesce(n,s);
   }
   if (s->start + s->len < stb__lastpage) {
      n = stb__span_for_page[s->start + s->len - stb__firstpage];
      if (n && n->allocations == -2 && s->start + s->len == n->start) s = stb__coalesce(s,n);
   }
   s->allocations = -2;
   stb__spanlist_add(s->len > 256 ? 256 : s->len, s);
}

static stb__span *stb__alloc_pages(int num)
{
   stb__span *s = stb__span_alloc();
   int p;
   if (!s) return NULL;
   p = stb__alloc_pages_raw(num << stb__page_shift);
   if (p == 0) { s->next = stb__span_free; stb__span_free = s; return 0; }
   assert(stb__page_address(stb__page_number(p)) == p);
   p = stb__page_number(p);
   stb__update_page_range(p, p+num);
   s->start = p;
   s->len   = num;
   s->next  = NULL;
   s->prev  = NULL;
   stb__set_span_for_page(s);
   return s;
}

static stb__span *stb__alloc_span(int pagecount)
{
   int i;
   stb__span *p = NULL;
   for(i=pagecount; i < 256; ++i)
      if (stb__spanlist[i]) {
         p = stb__spanlist[i];
         break;
      }
   if (!p) {
      p = stb__spanlist[256];
      while (p && p->len < pagecount)
         p = p->next;
   }
   if (!p) {
      p = stb__alloc_pages(pagecount < 16 ? 16 : pagecount);
      if (p == NULL) return 0;
   } else
      stb__spanlist_unlink(p);
      
   if (p->len > pagecount) {
      stb__span *q = stb__span_alloc();
      if (q) {
         q->start = p->start + pagecount;
         q->len   = p->len   - pagecount;
         p->len   = pagecount;
         for (i=0; i < q->len; ++i)
            stb__span_for_page[q->start+i - stb__firstpage] = q;
         stb__spanlist_add(q->len > 256 ? 256 : q->len, q);
      }
   }
   return p;
}

#define STB__MAX_SMALL_SIZE     32768
#define STB__MAX_SIZE_CLASSES   256

static unsigned char stb__class_base[32];
static unsigned char stb__class_shift[32];
static unsigned char stb__pages_for_class[STB__MAX_SIZE_CLASSES];
static           int stb__size_for_class[STB__MAX_SIZE_CLASSES];

stb__span *stb__get_nonempty_sizeclass(int c)
{
   int s = c + 256, i, size, tsize; // remap to span-list index
   char *z;
   void *q;
   stb__span *p = stb__spanlist[s];
   if (p) {
      if (p->first_free) return p; // fast path: it's in the first one in list
      for (p=p->next; p; p=p->next)
         if (p->first_free) {
            // move to front for future queries
            stb__spanlist_unlink(p);
            stb__spanlist_add(s, p);
            return p;
         }
   }
   // no non-empty ones, so allocate a new one
   p = stb__alloc_span(stb__pages_for_class[c]);
   if (!p) return NULL;
   // create the free list up front
   size = stb__size_for_class[c];
   tsize = stb__pages_for_class[c] << stb__page_shift;
   i = 0;
   z = (char *) stb__page_address(p->start);
   q = NULL;
   while (i + size <= tsize) {
      * (void **) z = q; q = z;
      z += size;
      i += size;
   }
   p->first_free = q;
   p->allocations = 0;
   stb__spanlist_add(s,p);
   return p;
}

static int stb__sizeclass(size_t sz)
{
   int z = stb_log2_floor(sz); // -1 below to group e.g. 13,14,15,16 correctly
   return stb__class_base[z] + ((sz-1) >> stb__class_shift[z]);
}

static void stb__init_sizeclass(void)
{
   int i, size, overhead;
   int align_shift = 2;  // allow 4-byte and 12-byte blocks as well, vs. TCMalloc
   int next_class = 1;
   int last_log = 0;

   for (i = 0; i < align_shift; i++) {
      stb__class_base [i] = next_class;
      stb__class_shift[i] = align_shift;
   }

   for (size = 1 << align_shift; size <= STB__MAX_SMALL_SIZE; size += 1 << align_shift) {
      i = stb_log2_floor(size);
      if (i > last_log) {
         if (size == 16) ++align_shift; // switch from 4-byte to 8-byte alignment
         else if (size >= 128 && align_shift < 8) ++align_shift;
         stb__class_base[i]  = next_class - ((size-1) >> align_shift);
         stb__class_shift[i] = align_shift;
         last_log = i;
      }
      stb__size_for_class[next_class++] = size;
   }

   for (i=1; i <= STB__MAX_SMALL_SIZE; ++i)
      assert(i <= stb__size_for_class[stb__sizeclass(i)]);

   overhead = 0;
   for (i = 1; i < next_class; i++) {
      int s = stb__size_for_class[i];
      size = stb__page_size;
      while (size % s > size >> 3)
         size += stb__page_size;
      stb__pages_for_class[i] = (unsigned char) (size >> stb__page_shift);
      overhead += size;
   }
   assert(overhead < (4 << 20)); // make sure it's under 4MB of overhead
}

#ifdef STB_DEBUG
#define stb__smemset(a,b,c)  memset((void *) a, b, c)
#elif defined(STB_FASTMALLOC_INIT)
#define stb__smemset(a,b,c)  memset((void *) a, b, c)
#else
#define stb__smemset(a,b,c)
#endif
void *stb_smalloc(size_t sz)
{
   stb__span *s;
   if (sz == 0) return NULL;
   if (stb__size_for_class[1] == 0) stb__init_sizeclass();
   if (sz > STB__MAX_SMALL_SIZE) {
      s = stb__alloc_span((sz + stb__page_size - 1) >> stb__page_shift);
      if (s == NULL) return NULL;
      s->list = 0;
      s->next = s->prev = NULL;
      s->allocations = -32767;
      stb__smemset(stb__page_address(s->start), 0xcd, (sz+3)&~3);
      return (void *) stb__page_address(s->start);
   } else {
      void *p;
      int c = stb__sizeclass(sz);
      s = stb__spanlist[256+c];
      if (!s || !s->first_free)
         s = stb__get_nonempty_sizeclass(c);
      if (s == NULL) return NULL;
      p = s->first_free;
      s->first_free = * (void **) p;
      ++s->allocations;
      stb__smemset(p,0xcd, sz);
      return p;
   }
}

int stb_ssize(void *p)
{
   stb__span *s;
   if (p == NULL) return 0;
   s = stb__span_for_page[stb__page_number((stb_uint) p) - stb__firstpage];
   if (s->list >= 256) {
      return stb__size_for_class[s->list - 256];
   } else {
      assert(s->list == 0);
      return s->len << stb__page_shift;
   }
}

void stb_sfree(void *p)
{
   stb__span *s;
   if (p == NULL) return;
   s = stb__span_for_page[stb__page_number((stb_uint) p) - stb__firstpage];
   if (s->list >= 256) {
      stb__smemset(p, 0xfe, stb__size_for_class[s->list-256]);
      * (void **) p = s->first_free;
      s->first_free = p;
      if (--s->allocations == 0) {
         stb__spanlist_unlink(s);
         stb__free_span(s);
      }
   } else {
      assert(s->list == 0);
      stb__smemset(p, 0xfe, stb_ssize(p));
      stb__free_span(s);
   }
}

void *stb_srealloc(void *p, size_t sz)
{
   size_t cur_size;
   if (p == NULL) return stb_smalloc(sz);
   if (sz == 0) { stb_sfree(p); return NULL; }
   cur_size = stb_ssize(p);
   if (sz > cur_size || sz <= (cur_size >> 1)) {
      void *q;
      if (sz > cur_size && sz < (cur_size << 1)) sz = cur_size << 1;
      q = stb_smalloc(sz); if (q == NULL) return NULL;
      memcpy(q, p, sz < cur_size ? sz : cur_size);
      stb_sfree(p);
      return q;
   }
   return p;
}

void *stb_scalloc(size_t n, size_t sz)
{
   void *p;
   if (n == 0 || sz == 0) return NULL;
   if (stb_log2_ceil(n) + stb_log2_ceil(n) >= 32) return NULL;
   p = stb_smalloc(n*sz);
   if (p) memset(p, 0, n*sz);
   return p;
}

char *stb_sstrdup(char *s)
{
   int n = strlen(s);
   char *p = (char *) stb_smalloc(n+1);
   if (p) strcpy(p,s);
   return p;
}
#endif // STB_DEFINE


#undef STB_EXTERN
#endif // STB_INCLUDE_STB_H
