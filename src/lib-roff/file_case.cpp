/*@ file_case: input file encapsulator
 *
 * Copyright (c) 2014 - 2015 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
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

#include "config.h"
#include "lib.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#ifdef HAVE_ZLIB
# include <zlib.h>
#endif

#include "errarg.h"
#include "error.h"
#include "posix.h"
#include "nonposix.h"

#include "file_case.h"

#undef getc
#undef _getc
#ifdef HAVE_DECL_GETC_UNLOCKED
# define _getc  getc_unlocked
#else
# define _getc  fgetc
#endif

// Support decompression XXX configure should say `no popen() - no unpacking'
#ifdef POPEN_MISSING
# undef HAVE_UNPACK
#endif

// (Enclosed by HAVE_UNPACK) Directly support decompression library layer?
// XXX We yet only support a zlib _LAYER, which is why we directly address zlib
// XXX functions instead of furtherly abstracting into a struct iolayer or sth.
// XXX If we would, that can only have read_buf() and close() and we should
// XXX deal with buffer handling entirely ourselfs, in which case even the
// XXX popen(3) code path could be enwrapped into struct iolayer; i.e., then
// XXX the entire public read interface could internally be driven by iolayer
#ifndef HAVE_ZLIB
# define HAVE_ZLIB  0
#endif
#if HAVE_ZLIB
# define _LAYER
#endif

struct args {
  FILE        *a_fp;
  void        *a_layer;
  char const  *a_path;
  size_t      a_path_len;
  char const  *a_mode;      // Mode for fopen(3), if used
  uint32_t    a_flags;
  int32_t     a_errno;
};

#ifdef HAVE_UNPACK
struct zproc {
  uint8_t     zp_popen;
  uint8_t     zp_ext_len;   // Extension including `.' (<period>)
  uint8_t     zp_cmd_len;
  uint8_t     zp_layer;     // Uses I/O layer (zlib)
  char        zp_ext[5];
  char        zp_cmd[15];
};

static zproc const  _zprocs[] = {
# define __X(L,C,E) {true, sizeof(E) -1, sizeof(C) -1, L, E, C}
# ifdef HAVE_UNPACK_BZ2
  __X(0, "bzip2 -cdf", ".bz2"),
# endif
# ifdef HAVE_UNPACK_GZ
  __X(HAVE_ZLIB, "gzip -cdf", ".gz"),
# endif
# ifdef HAVE_UNPACK_XZ
  __X(0, "xz -cdf", ".xz")
# endif
# undef __X
};
#endif // HAVE_UNPACK

#ifdef HAVE_UNPACK
// Check wether path was explicitly specified with a packer extension.
// This returns a ternary: false only if we knew the extension, applied the
// zproc and that failed to perform, true otherwise (ap->a_fp is 2nd indicator)
static bool   _is_ext(args *ap);

// Plain file didn't exist, iterate over the supported packer extensions
// and see if a matching file exists instead; NULL if not / on error.
// Note that ap->a_errno is ENOENT on entry and only overwritten if we run
// a zproc and that fails XXX ENOENT is blindly used in codebase, but not ISO C
static bool   _try_all_ext(args *ap);

// Create a FILE* according to zp, return NULL on error
static args * __run_zproc(args *ap, zproc const *zp);

// Callee needs seek()ing or STD I/O, unpack into temporary file, NULL on error
static args * __unpack(args *ap);
#endif // HAVE_UNPACK

#ifdef HAVE_UNPACK
static bool
_is_ext(args *ap)
{
  for (zproc const *zp = _zprocs; zp < _zprocs + NELEM(_zprocs); ++zp) {
    size_t el = zp->zp_ext_len;

    if (ap->a_path_len <= el)
      continue;
    if (memcmp(ap->a_path + ap->a_path_len - el, zp->zp_ext, el))
      continue;

    ap = __run_zproc(ap, zp);
    break;
  }
  return (ap != NULL);
}

static bool
_try_all_ext(args *ap)
{
  for (zproc const *zp = _zprocs; zp < _zprocs + NELEM(_zprocs); ++zp) {
    char *np = new char[ap->a_path_len + zp->zp_ext_len +1];

    memcpy(np, ap->a_path, ap->a_path_len);

    struct ::stat sb;
    memcpy(np + ap->a_path_len, zp->zp_ext, zp->zp_ext_len +1);
    if (stat(np, &sb)) {
      a_delete np;
      continue;
    }

    // That's our zproc, let it make the deal
    char const *pb_save = ap->a_path;
    size_t pl_save = ap->a_path_len;
    ap->a_path = np;
    ap->a_path_len = pl_save + zp->zp_ext_len;
    if ((ap = __run_zproc(ap, zp)) != NULL) {
      ap->a_path = pb_save;
      ap->a_path_len = pl_save;
    }
    a_delete np;
    goto jleave;
  }
  ap = NULL;
jleave:
  return (ap != NULL);
}

static args *
__run_zproc(args *ap, zproc const *zp)
{
# ifdef _LAYER
  if (zp->zp_layer) {
    if ((ap->a_layer = gzopen(ap->a_path, "rb")) == NULL) {
      ap->a_errno = errno;
      ap = NULL;
    } else if (ap->a_flags &
        (file_case::mux_need_seek | file_case::mux_need_stdio))
      ap = __unpack(ap);
  } else {
# endif
    char *np = new char[zp->zp_cmd_len + 1 + ap->a_path_len +1];

    size_t l;
    memcpy(np, zp->zp_cmd, l = zp->zp_cmd_len);
    np[l++] = ' ';
    memcpy(np + l, ap->a_path, ap->a_path_len +1);

    if ((ap->a_fp = popen(np, "r")) == NULL) {
      ap->a_errno = errno;
      ap = NULL;
    } else if (ap->a_flags & file_case::mux_need_seek)
      ap = __unpack(ap);
    else
      ap->a_flags |= file_case::fc_pipe | file_case::fc_have_stdio;

    a_delete np;
# ifdef _LAYER
  }
# endif

  return ap;
}

static args *
__unpack(args *ap)
{
  size_t const buf_len = (BUFSIZ + 0 > 1 << 15) ? BUFSIZ : 1 << 15;
  uint8_t *buf = new uint8_t[buf_len];

  // xtmpfile uses binary mode and fatal()s on error
  FILE *decomp = xtmpfile(NULL, "groff_unpack"), *decomp_save = decomp;
  for (;;) {
    size_t oc;

# ifdef _LAYER
    if (ap->a_layer != NULL) {
      int i = gzread((gzFile)ap->a_layer, buf, buf_len);
      if (i == -1) {
        ap->a_errno = errno;
        decomp = NULL;
        break;
      } else if (i == 0)
        break;
      oc = (size_t)i;
    } else
# endif
    if ((oc = fread(buf, sizeof *buf, buf_len, ap->a_fp)) == 0) {
      if (!feof(ap->a_fp)) {
        ap->a_errno = errno;
        decomp = NULL;
      }
      break;
    }

    if (decomp != NULL) {
      for (uint8_t *target = buf; oc > 0;) {
        size_t i = fwrite(target, sizeof *buf, oc, decomp);
        if (i == 0)
          break;
        oc -= i;
        target += i;
      }
      if (oc > 0) {
        ap->a_errno = errno;
        decomp = NULL;
      }
    }
  }

# ifdef _LAYER
  if (ap->a_layer != NULL) {
    if (gzclose((gzFile)ap->a_layer) != Z_OK)
      error("decompressor gzclose(3) failed");
    ap->a_layer = NULL;
  } else
# endif
  if (pclose(ap->a_fp) != 0)
    error("decompressor pipe pclose(3) didn't exit cleanly");

  if (decomp != NULL) {
    ap->a_flags |= file_case::fc_have_stdio;
    rewind(ap->a_fp = decomp);
  } else {
    fclose(decomp_save);
    ap->a_fp = NULL;
    ap = NULL;
  }

  a_delete buf;
  return ap;
}
#endif // HAVE_UNPACK

bool
file_case::close(void)
{
  assert((_file != NULL && _layer == NULL) ||
    (_file == NULL && _layer != NULL));

  if (!(_flags & fc_const_path))
    a_delete _path;

  bool rv;
  if (_flags & fc_dont_close)
    rv = true;
#ifdef _LAYER
  else if (_layer != NULL)
    rv = (gzclose((gzFile)_layer) == Z_OK);
#endif
#ifdef HAVE_UNPACK
  else if (_flags & fc_pipe)
    rv = (pclose(_file) == 0);
#endif
  else
    rv = (fclose(_file) == 0);

#ifndef NDEBUG
  _path = NULL;
  _file = NULL;
  _layer = NULL;
  _flags = fc_none;
#endif
  return rv;
}

bool
file_case::is_eof(void) const
{
  bool rv;
#ifdef _LAYER
  if (_layer != NULL)
    rv = (gzeof((gzFile)_layer) != 0);
  else
#endif
    rv = (feof(_file) != 0);
  return rv;
}

int
file_case::get_c(void)
{
  int rv;
#ifdef _LAYER
  if (_layer != NULL)
    rv = gzgetc((gzFile)_layer);
  else
#endif
    rv = _getc(_file);
  return rv;
}

int
file_case::unget_c(int c)
{
  int rv;
#ifdef _LAYER
  if (_layer != NULL)
    rv = gzungetc(c, (gzFile)_layer);
  else
#endif
    rv = ungetc(c, _file);
  return rv;
}

char *
file_case::get_line(char *buf, size_t buf_size)
{
#ifdef _LAYER
  if (_layer != NULL)
    buf = gzgets((gzFile)_layer, buf, (int)buf_size);
  else
#endif
    buf = fgets(buf, (int)buf_size, _file);
  return buf;
}

size_t
file_case::get_buf(void *buf, size_t buf_size)
{
  size_t rv;
#ifdef _LAYER
  if (_layer != NULL) {
    int i = gzread((gzFile)_layer, buf, (unsigned int)buf_size);
    rv = (i <= 0) ? 0 : (size_t)i;
  } else
#endif
    rv = fread(buf, 1, buf_size, _file);
  return rv;
}

void
file_case::rewind(void)
{
#ifdef _LAYER
  if (_layer != NULL)
    gzrewind((gzFile)_layer);
  else
#endif
    ::rewind(_file);
}

int
file_case::seek(long offset, seek_whence whence)
{
  int x = (whence == seek_set ? SEEK_SET :
      (whence == seek_cur ? SEEK_CUR : SEEK_END));
#ifdef _LAYER
  if (_layer != NULL)
    x = gzseek((gzFile)_layer, (z_off_t)offset, x);
  else
#endif
    x = fseek(_file, offset, x);
  return x;
}

/*static*/ file_case *
file_case::muxer(char const *path, uint32_t flags)
{
  enum {tmpbit = 1<<(_mux_freebit+0)};

  assert(!(flags & (fc_dont_close | fc_pipe)));
  assert(!(flags & (fc_const_path | fc_take_path)) ||
      !(flags & fc_const_path) != !(flags & fc_take_path));
  assert(!(flags & (mux_unpack | mux_no_unpack)) ||
      !(flags & mux_unpack) != !(flags & mux_no_unpack));

  if (path == NULL || (path[0] == '-' && path[1] == '\0')) {
    path = "-";
    flags &= ~fc_take_path;
    flags |= fc_const_path | tmpbit;
  } else if (!(flags & (fc_const_path | fc_take_path))) {
    path = strsave(path);
    flags |= fc_take_path;
  }
  if (!(flags & (mux_unpack | mux_no_unpack)))
    flags |= _mux_unpack_default;

  file_case *fcp;
  args a;
  a.a_fp = NULL;
  a.a_layer = NULL;
  a.a_path_len = strlen(a.a_path = path);
  a.a_mode = (flags & mux_need_binary) ? "rb" : "r";
  a.a_flags = flags;
  a.a_errno = 0;

  // Shorthand: support "-" to mean stdin
  if (flags & tmpbit) {
    clearerr(stdin);
    if (flags & mux_need_binary)
      SET_BINARY(fileno(stdin));
    a.a_fp = stdin;
    a.a_flags |= fc_dont_close | fc_const_path | fc_have_stdio;
    goto jnew;
  }

  // If we support unpacking then check wether the path already includes
  // a packer's extension, i.e., explicitly.  Anyway unpack then, despite flags
#ifdef HAVE_UNPACK
  if (!_is_ext(&a)) {
    assert(a.a_fp == NULL && a.a_layer == NULL);
    goto jerror;
  }
  if (a.a_fp != NULL || a.a_layer != NULL)
    goto jnew;
#endif

  // Try a plain open
  errno = 0;
  if ((a.a_fp = fopen(a.a_path, a.a_mode)) != NULL) {
    a.a_flags |= fc_have_stdio;
jnew:
    assert((a.a_fp != NULL && a.a_layer == NULL) ||
      (a.a_fp == NULL && a.a_layer != NULL));
    fcp = new file_case(a.a_fp, path, a.a_flags & fc_mask); // XXX real path?
    fcp->_layer = a.a_layer;
    goto jleave;
  }
  a.a_errno = errno;

  // Then auto-expand the given path if so desired
#ifdef HAVE_UNPACK
  if (a.a_errno == ENOENT && (a.a_flags & mux_unpack) && _try_all_ext(&a))
    goto jnew;

jerror:
#endif
  if (!(a.a_flags & fc_const_path))
    a_delete a.a_path;
  errno = a.a_errno;
  fcp = NULL;
jleave:
  return fcp;
}

// s-it2-mode
