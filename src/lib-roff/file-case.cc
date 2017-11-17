/*@ file_case: input file encapsulator. TODO do not use errno!
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#define su_FILE "lib_roff__file_case"
#define rf_SOURCE
#define rf_SOURCE_LIB_ROFF_FILE_CASE

#include "config.h"
#include "lib.h"

#include "su/cs.h"
#include "su/mem.h"

#include <stdio.h>
#include <stat.h>

#ifdef HAVE_ZLIB /* TODO ZSTD! */
# include <zlib.h>
#endif

#include "file-case.h"
#include "su/code-in.h"

NSPC_USE(rf)

#undef getc
#undef a_getc
#ifdef HAVE_STDIO_UNLOCKED
# define a_getc getc_unlocked
#else
# define a_getc fgetc
#endif

// Support decompression XXX configure should say `no popen() - no unpacking'
#ifdef POPEN_MISSING /* TODO POPEN_MISSING symbol */
# undef HAVE_UNPACK
#endif

// (Enclosed by HAVE_UNPACK) Directly support decompression library layer?
// XXX We yet only support a zlib a_LAYER, which is why we directly address zlib
// XXX functions instead of furtherly abstracting into a struct iolayer or sth.
// XXX If we would, that can only have read_buf() and close() and we should
// XXX deal with buffer handling entirely ourselfs, in which case even the
// XXX popen(3) code path could be enwrapped into struct iolayer; i.e., then
// XXX the entire public read interface could internally be driven by iolayer
#ifndef HAVE_ZLIB
# define HAVE_ZLIB 0
#endif
#if HAVE_ZLIB
# define a_LAYER
#endif

struct a_args{
   FILE *a_fp;
   void *a_layer;
   char const *a_path;
   uz a_path_len;
   char const *a_mode;  // Mode for fopen(3), if used
   u32 a_flags;
   s32 a_errno;
};

#ifdef HAVE_UNPACK
struct a_zproc{
   u8 zp_popen;
   u8 zp_ext_len;    // Extension including `.' (<period>)
   u8 zp_cmd_len;
   u8 zp_layer;      // Uses I/O layer (zlib)
   char zp_ext[5];
   char zp_cmd[15];
};
#endif

#ifdef HAVE_UNPACK
static a_zproc const a_zprocs[] = {
# define a_X(L,C,E) {TRU1, sizeof(E) -1, sizeof(C) -1, L, E "\0", C "\0"}
# ifdef HAVE_UNPACK_BZ2
   a_X(0, "bzip2 -cdf", ".bz2"),
# endif
# ifdef HAVE_UNPACK_GZ
   a_X(HAVE_ZLIB, "gzip -cdf", ".gz"),
# endif
# ifdef HAVE_UNPACK_XZ
   a_X(0, "xz -cdf", ".xz")
# endif
# ifdef HAVE_UNPACK_ZST
   a_X(0, "zstd -cdf", ".zst")
# endif
# undef a_X
};
#endif // HAVE_UNPACK

#ifdef HAVE_UNPACK
// Check whether path was explicitly specified with a packer extension.
// This returns a ternary: false only if we knew the extension, applied the
// zproc and that failed to perform, true otherwise (ap->a_fp is 2nd indicator)
static boole a_is_ext(a_args *ap);

// Plain file did not exist, iterate over the supported packer extensions
// and see if a matching file exists instead; NIL if not / on error.
// Note that ap->a_errno is ENOENT on entry and only overwritten if we run
// a zproc and that fails XXX ENOENT is blindly used in codebase, but not ISO C
static boole a_try_all_ext(a_args *ap);

// Create a file according to zp, return NIL on error
static a_args *a__run_zproc(a_args *ap, a_zproc const *zp);

// Callee needs seek()ing or STD I/O, unpack into temporary file, NIL on error
static a_args *a__unpack(a_args *ap);
#endif // HAVE_UNPACK

#ifdef HAVE_UNPACK
static boole
a_is_ext(a_args *ap){
   NYD2_IN;
   for(a_zproc const *zp = a_zprocs; zp < &a_zprocs[NELEM(a_zprocs)]; ++zp){
      uz el;

      if(ap->a_path_len <= (el = zp->zp_ext_len))
         continue;
      if(mem::cmp(&ap->a_path[ap->a_path_len - el], zp->zp_ext, el))
         continue;

      ap = a__run_zproc(ap, zp);
      break;
   }
   NYD2_OU;
   return (ap != NIL);
}

static boole
a_try_all_ext(a_args *ap){
   NYD2_IN;
   for(a_zproc const *zp = a_zprocs; zp < &a_zprocs[NELEM(a_zprocs)]; ++zp){
      char *np;

      if((np = su_TALLOC(char, ap->a_path_len + zp->zp_ext_len +1)) == NIL)
         break;
      mem::copy(np, ap->a_path, ap->a_path_len);

      struct ::stat sb;
      mem::copy(&np[ap->a_path_len], zp->zp_ext, zp->zp_ext_len +1);
      if(::stat(np, &sb)){
         su_FREE(np);
         continue;
      }

      // That's our zproc, let it make the deal
      char const *pb_save = ap->a_path;
      uz pl_save = ap->a_path_len;
      ap->a_path = np;
      ap->a_path_len = pl_save + zp->zp_ext_len;
      if((ap = a__run_zproc(ap, zp)) != NIL){
         ap->a_path = pb_save;
         ap->a_path_len = pl_save;
      }
      su_FREE(np);
      goto jleave;
   }

   ap = NIL;
jleave:
   NYD2_OU;
   return (ap != NIL);
}

static a_args *
a__run_zproc(a_args *ap, a_zproc const *zp){
   char *np;
   NYD2_IN;

# ifdef a_LAYER
   if(zp->zp_layer){
      if((ap->a_layer = gzopen(ap->a_path, "rb")) == NIL){
         ap->a_errno = err::no_via_errno();
         ap = NIL;
      }else if(ap->a_flags &
            (file_case::mux_need_seek | file_case::mux_need_stdio))
         ap = a__unpack(ap);
   }else
# endif
         if((np = su_TALLOC(char, zp->zp_cmd_len + 1 + ap->a_path_len +1)
         ) != NIL){
      uz l;

      mem::copy(np, zp->zp_cmd, l = zp->zp_cmd_len);
      np[l++] = ' ';
      mem::copy(&np[l], ap->a_path, ap->a_path_len +1);

      if((ap->a_fp = popen(np, "r")) == NIL){
         ap->a_errno = err::no_via_errno();
         ap = NIL;
      }else if(ap->a_flags & file_case::mux_need_seek)
         ap = a__unpack(ap);
      else
         ap->a_flags |= file_case::fc_pipe | file_case::fc_have_stdio;

      su_FREE(np);
   }else{
      ap->a_errno = err::no();
      ap = NIL;
   }

   NYD2_OU;
   return ap;
}

static a_args *
a__unpack(a_args *ap){
   uz const buf_len = (BUFSIZ + 0 > 1u << 15) ? BUFSIZ : 1u << 15;/* TODO */

   FILE *decomp, *decomp_save;
   u8 *buf;
   NYD2_IN;

   if((buf = su_TALLOC(u8, buf_len)) == NIL){
      ap->a_errno = err::no();
      ap = NIL;
      goto jleave;
   }

   // xtmpfile uses binary mode and fatal()s on error
   decomp = xtmpfile(NIL, "groff_unpack");
   decomp_save = decomp;

   for(;;){
      uz oc;

# ifdef a_LAYER
      if(ap->a_layer != NIL){
         int i;

         if((i = gzread(S(gzFile,ap->a_layer), buf, buf_len)) == -1){
            ap->a_errno = err::no_via_errno();
            decomp = NIL;
            break;
         }else if(i == 0)
            break;
         oc = S(uz,i);
      }else
# endif
            if((oc = fread(buf, sizeof *buf, buf_len, ap->a_fp)) == 0){
         if(!feof(ap->a_fp)){
            ap->a_errno = err::no_via_errno();
            decomp = NIL;
         }
         break;
      }

      if(decomp != NIL){
         for(u8 *target = buf; oc > 0;){
            uz i;

            if((i = fwrite(target, sizeof *buf, oc, decomp)) == 0)
               break;
            oc -= i;
            target += i;
         }
         if(oc > 0){
            ap->a_errno = err::no_via_errno();
            decomp = NIL;
         }
      }
   }

# ifdef a_LAYER
   if(ap->a_layer != NIL){
      if(gzclose(S(gzFile,ap->a_layer)) != Z_OK)
         error("decompressor gzclose(3) failed"); /* XXX */
      ap->a_layer = NIL;
   }else
# endif
         if(pclose(ap->a_fp) != 0)
      error("decompressor pipe pclose(3) didn't exit cleanly"); /* XXX */

   if(decomp != NIL){
      ap->a_flags |= file_case::fc_have_stdio;
      rewind(ap->a_fp = decomp);
   }else{
      fclose(decomp_save);
      ap->a_fp = NIL;
      ap = NIL;
   }

   su_FREE(buf);
jleave:
   NYD2_OU;
   return ap;
}
#endif // HAVE_UNPACK

boole
file_case::close(void){
   boole rv;
   NYD_IN;
   ASSERT((m_file != NIL && m_layer == NIL) ||
      (m_file == NIL && m_layer != NIL));

   if(!(m_flags & fc_const_path))
      su_FREE(m_path);

   if(m_flags & fc_dont_close)
      rv = TRU1;
#ifdef a_LAYER
   else if(m_layer != NIL)
      rv = (gzclose(S(gzFile,m_layer)) == Z_OK);
#endif
#ifdef HAVE_UNPACK
   else if(m_flags & fc_pipe)
      rv = (pclose(m_file) == 0);
#endif
   else
      rv = (fclose(m_file) == 0);

#ifdef HAVE_DEBUG
   m_path = NIL;
   m_file = NIL;
   m_layer = NIL;
   m_flags = fc_none;
#endif
   NYD_OU;
   return rv;
}

boole
file_case::is_eof(void) const{
   boole rv;
   NYD_IN;

#ifdef a_LAYER
   if(m_layer != NIL)
      rv = (gzeof(S(gzFile,m_layer)) != 0);
   else
#endif
      rv = (feof(m_file) != 0);
   NYD_OU;
   return rv;
}

s32
file_case::get_c(void){
   s32 rv;
   NYD2_IN;

#ifdef a_LAYER
   if(m_layer != NIL)
      rv = gzgetc(S(gzFile,m_layer));
   else
#endif
      rv = a_getc(m_file);
   NYD2_OU;
   return rv;
}

s32
file_case::unget_c(s32 c){
   s32 rv;
   NYD_IN;

   // The standard defines EOF "shall fail and the input stream shall be left
   // unchanged".  Extend this to any "non-char" and handle the case ourselve
   if(UCMP(u32, c, <=, U8_MAX)){
#ifdef a_LAYER
      if(m_layer != NIL)
         rv = gzungetc(c, S(gzFile,m_layer));
      else
#endif
         rv = ungetc(c, m_file);
   }else
      rv = EOF;
   NYD_OU;
   return rv;
}

char *
file_case::get_line(char *buf, uz buf_size){
   int i;
   NYD_IN;

   i = get_min<uz>(buf_size, S32_MAX);
#ifdef a_LAYER
   if(m_layer != NIL)
      buf = gzgets(S(gzFile,m_layer), buf, i);
   else
#endif
      buf = fgets(buf, i, m_file);
   NYD_OU;
   return buf;
}

uz
file_case::get_buf(void *buf, uz buf_size){
   uz rv;
   NYD_IN;

#ifdef a_LAYER
   if(m_layer != NIL){
      for(rv = 0; buf_size > 0;){
         int i, o;

         i = get_max<uz>(buf_size, S32_MAX);
         o = gzread(S(gzFile,m_layer), &S(u8*,buf)[rv], i);
         if(o <= 0)
            break;
         rv += o;
         if(o < i)
            break;
         buf_size -= o;
      }
   }else
#endif // a_LAYER
      rv = fread(buf, 1, buf_size, m_file);
   NYD_OU;
   return rv;
}

void
file_case::rewind(void){
   NYD_IN;
#ifdef a_LAYER
   if(m_layer != NIL)
      gzrewind(S(gzFile,m_layer));
   else
#endif
      ::rewind(m_file);
   NYD_OU;
}

boole
file_case::seek(s64 offset, seek_whence whence){
   int x;
   NYD_IN;

   x = (whence == seek_set ? SEEK_SET :
         (whence == seek_cur ? SEEK_CUR : SEEK_END));
#ifdef a_LAYER
   if(m_layer != NIL){
      if(sizeof(z_off_t) == sizeof(s64) ||
            (offset >= S32_MIN && offset <= S32_MAX))
         x = (gzseek(S(gzFile,m_layer), S(z_off_t,offset), x) == -1);
      else
         goto jsimulate;
   }else
#endif
   if(sizeof(long) == sizeof(s64) ||
         (offset >= S32_MIN && offset <= S32_MAX))
      x = fseek(m_file, S(long,offset), x);
   else
      goto jsimulate;

jleave:
   NYD_OU;
   return (x == 0);

jsimulate:
   ASSERT(offset != 0);
   if(sizeof(long) != sizeof(s64))
      for(; offset != 0; whence = seek_cur, offset -= x)
         if(!seek(x = get_clip<s64>(offset, S32_MIN, S32_MAX), whence))
            break;
   x = (offset != 0); // reversed
   goto jleave;
}

STA file_case *
file_case::muxer(char const *path, u32 flags){
   enum {tmpbit = 1u<<(mux__freebit+0)};

   a_args a;
   file_case *fcp;
   NYD_IN;
   ASSERT(!(flags & (fc_dont_close | fc_pipe)));
   ASSERT(!(flags & (fc_const_path | fc_take_path)) ||
      !(flags & fc_const_path) != !(flags & fc_take_path));
   ASSERT(!(flags & (mux_unpack | mux_no_unpack)) ||
      !(flags & mux_unpack) != !(flags & mux_no_unpack));

   // Support "-" to mean stdin
   if(path == NIL || (path[0] == '-' && path[1] == '\0')){
      path = "-"; /* XXX su_hm or su_hymi! */
      flags &= ~fc_take_path;
      flags |= fc_const_path | tmpbit;
   }else if(!(flags & (fc_const_path | fc_take_path))){
      if((path = cs::dup(path)) == NIL){
         err::set_errno(err::no());
         fcp = NIL;
         goto jleave;
      }
      flags |= fc_take_path;
   }
   if(!(flags & (mux_unpack | mux_no_unpack)))
      flags |= mux__unpack_default;

   a.a_fp = NIL;
   a.a_layer = NIL;
   a.a_path_len = cs::len(a.a_path = path);
   a.a_mode = (flags & mux_need_binary) ? "rb" : "r";
   a.a_flags = flags;
   a.a_errno = 0;

   // Shortcut for the "-" / stdin case
   if(flags & tmpbit){
      clearerr(stdin);
      if(flags & mux_need_binary)
         SET_BINARY(fileno(stdin));
      a.a_fp = stdin;
      a.a_flags |= fc_dont_close | fc_const_path | fc_have_stdio;
      goto jnew;
   }

   // If we support unpacking then check whether the path already includes
   // a packer's extension, i.e., explicitly.  Anyway unpack, despite flags
#ifdef HAVE_UNPACK
   if(!a_is_ext(&a)){
      ASSERT_JUMP(a.a_fp == NIL && a.a_layer == NIL, jerror);
      goto jerror;
   }
   if(a.a_fp != NIL || a.a_layer != NIL)
      goto jnew;
#endif

   // Try a plain open
   err::set_errno(0);
   if((a.a_fp = fopen(a.a_path, a.a_mode)) != NIL){
      a.a_flags |= fc_have_stdio;
jnew:
      ASSERT((a.a_fp != NIL && a.a_layer == NIL) ||
         (a.a_fp == NIL && a.a_layer != NIL));
      // XXX real path?
      if((fcp = su_NEW(file_case)(a.a_fp, path, (a.a_flags & fc_mask))
            ) == NIL){
         a.a_errno = err::no();
         goto jerror;
      }
      fcp->m_layer = a.a_layer;
      goto jleave;
   }
   a.a_errno = err::no_via_errno();

   // Then auto-expand the given path if so desired
#ifdef HAVE_UNPACK
   if(a.a_errno == err::e_noent &&
         (a.a_flags & mux_unpack) && a_try_all_ext(&a))
      goto jnew;
#endif

jerror:
   if(!(a.a_flags & fc_const_path))
      su_FREE(a.a_path);
   err::set_errno(a.a_errno);
   fcp = NIL;
jleave:
   NYD_OU;
   return fcp;
}

#include "su/code-ou.h"
// s-it-mode
