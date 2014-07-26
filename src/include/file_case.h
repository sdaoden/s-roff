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
#ifndef _FILE_CASE_H
#define _FILE_CASE_H

#include "lib.h"

#include <assert.h>
#include <stdio.h>

class file_case
{
  char const  *_path;
  FILE        *_file;       // fc_have_stdio
  void        *_layer;      // E.g., gzFile
  uint32_t    _flags;
  uint8_t     _dummy[4];
public:
  // Flags for ctor / muxer()
  enum {
    fc_none       = 0,
    fc_dont_close = 1<<0,   // Don't close the file, if any
    fc_pipe       = 1<<1,   // _file is not seekable
    fc_const_path = 1<<2,   // Don't dup path, and don't a_delete it
    fc_take_path  = 1<<3,   // Don't dup path, but a_delete it
    fc_have_stdio = 1<<4,   // .file() may be used
    _fc_freebit   = 5,
    fc_mask       = (1<<_fc_freebit) - 1
  };

  // Flags only for muxer()
  enum {
    mux_need_seek   = 1<<(_fc_freebit+0), // File must be seekable
    mux_need_binary = 1<<(_fc_freebit+1), // Need binary I/O
    mux_unpack      = 1<<(_fc_freebit+2), // Do auto-check for FILE{.gz,.bz2..}
    mux_no_unpack   = 1<<(_fc_freebit+3), // Do NOT auto-check
    mux_need_stdio  = 1<<(_fc_freebit+4), // Only then may .file() be used
    _mux_freebit    = _fc_freebit + 5,
    mux_mask        = ~fc_mask,
    mux_default     = fc_none,
    // Defines the global default strategy for dealing with packed files in case
    // none of the above has been given explicitly by a callee
    _mux_unpack_default = mux_unpack
  };

  enum seek_whence {
    seek_set,
    seek_cur,
    seek_end
  };

  file_case(FILE *fp, char const *path, uint32_t flags=fc_none);
  ~file_case(void);

  bool          close(void);
  char const *  path(void) const;
  FILE *        file(void) const;
  bool          is_pipe(void) const;

  bool          is_eof(void) const;
  int           get_c(void);
  int           unget_c(int c);
  char *        get_line(char *buf, size_t buf_size);
  size_t        get_buf(void *buf, size_t buf_size);
  void          rewind(void);
  int           seek(long offset, seek_whence whence=seek_set);

  // Factory muxer; note that fc_take_path will be honoured even on failure
  // If path is NULL or "-" we'll go for stdin
  static file_case *  muxer(char const *path=NULL, uint32_t flags=mux_default);

  CLASS_DISABLE_COPY(file_case);
};

inline
file_case::file_case(FILE *fp, char const *path, uint32_t flags)
:
  _path(path), _file(fp), _layer(NULL), _flags(flags)
{
  assert(!(flags & (fc_const_path | fc_take_path)) ||
      !(flags & fc_const_path) != !(flags & fc_take_path));
  assert(!(flags & ~fc_mask));
}

inline
file_case::~file_case(void)
{
  if (_file != NULL || _layer != NULL) // xxx (uintptr_t)a|(uintptr_t)b
    close();
}

inline char const *
file_case::path(void) const
{
  return _path;
}

inline FILE *
file_case::file(void) const
{
  assert(_flags & fc_have_stdio);
  return _file;
}

inline bool
file_case::is_pipe(void) const
{
  return ((_flags & fc_pipe) != 0);
}

#endif
/* s-it2-mode */
