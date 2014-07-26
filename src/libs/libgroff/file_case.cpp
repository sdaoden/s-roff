/*@ file_case: input file encapsulator
 *
 * Copyright (c) 2014 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
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

#include "lib.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "errarg.h"
#include "error.h"
#include "posix.h"
#include "nonposix.h"

#include "file_case.h"

// Support decompression XXX configure should say `no popen() - no unpacking'
#ifdef POPEN_MISSING
# undef HAVE_UNPACK
#endif

struct args {
  FILE        *a_fp;
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
  char        zp_ext[5];
  char        zp_cmd[16];
};

static zproc const  _zprocs[] = {
# define __X(C,E) {true, sizeof(E) -1, sizeof(C) -1, E, C}
# ifdef HAVE_UNPACK_BZ2
  __X("bzip2 -cdf", ".bz2"),
# endif
# ifdef HAVE_UNPACK_GZ
  __X("gzip -cdf", ".gz"),
# endif
# ifdef HAVE_UNPACK_XZ
  __X("xz -cdf", ".xz")
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

// Callee needs seek()ing, unpack into temporary file, return NULL on error
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
    char *np = new char[zp->zp_cmd_len +1+ ap->a_path_len + zp->zp_ext_len +1];

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
    ap->a_flags |= file_case::fc_pipe;

  a_delete np;
  return ap;
}

static args *
__unpack(args *ap)
{
  size_t const buf_len = (BUFSIZ + 0) > 1 << 15 ? BUFSIZ : 1 << 15;
  uint8_t *buf = new uint8_t[buf_len];

  // xtmpfile uses binary mode and fatal()s on error
  FILE *decomp = xtmpfile(NULL, "groff_unpack"), *decomp_save = decomp;
  for (;;) {
    size_t oc = fread(buf, sizeof *buf, buf_len, ap->a_fp);
    if (oc == 0) {
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
  if (pclose(ap->a_fp) != 0)
    error("decompressor pipe pclose(3) didn't exit cleanly");

  if (decomp != NULL)
    rewind(ap->a_fp = decomp);
  else {
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
  assert(_file != NULL);

  if (!(_flags & fc_const_path))
    a_delete _path;

  bool rv;
  if (_flags & fc_dont_close)
    rv = true;
#ifdef HAVE_UNPACK
  else if (_flags & fc_pipe)
    rv = (pclose(_file) == 0);
#endif
  else
    rv = (fclose(_file) == 0);

#ifndef NDEBUG
  _file = NULL;
  _path = NULL;
  _flags = fc_none;
#endif
  return rv;
}

/*static*/ file_case *
file_case::muxer(char const *path, uint32_t flags)
{
  assert(!(flags & (fc_dont_close | fc_pipe)));
  assert(!(flags & (fc_const_path | fc_take_path)) ||
      !(flags & fc_const_path) != !(flags & fc_take_path));
  assert(!(flags & (mux_unpack | mux_no_unpack)) ||
      !(flags & mux_unpack) != !(flags & mux_no_unpack));

  if (!(flags & (fc_const_path | fc_take_path))) {
    path = strsave(path);
    flags |= fc_take_path;
  }
  if (!(flags & (mux_unpack | mux_no_unpack)))
    flags |= _mux_unpack_default;

  file_case *fcp;
  args a;
  a.a_fp = NULL;
  a.a_path_len = strlen(a.a_path = path);
  a.a_mode = (flags & mux_need_binary) ? "rb" : "r";
  a.a_flags = flags;
  a.a_errno = 0;

  // If we support unpacking then check wether the path already includes
  // a packer's extension, i.e., explicitly.  Anyway unpack then, despite flags
#ifdef HAVE_UNPACK
  if (!_is_ext(&a)) {
    assert(a.a_fp == NULL);
    goto jerror;
  }
  if (a.a_fp != NULL)
    goto jnew;
#endif

  // Try a plain open
  errno = 0;
  if ((a.a_fp = fopen(a.a_path, a.a_mode)) != NULL) {
#ifdef HAVE_UNPACK
jnew:
#endif
    assert(a.a_fp != NULL);
    fcp = new file_case(a.a_fp, path, a.a_flags & ~mux_mask); // XXX real path?
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

/* s-it2-mode */
