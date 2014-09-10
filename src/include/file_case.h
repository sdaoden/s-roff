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
  FILE        *_file;
  uint32_t    _flags;
public:
  // Flags for ctor / muxer()
  enum {
    fc_none       = 0,
    fc_dont_close = 1<<0,   // Don't close the file, if any
    fc_pipe       = 1<<1,   // _file is not seekable
    fc_const_path = 1<<2,   // Don't dup path, and don't a_delete it
    fc_take_path  = 1<<3,   // Don't dup path, but a_delete it
    _fc_freebit   = 4,
    fc_mask       = (1<<_fc_freebit) - 1
  };

  // Flags only for muxer()
  enum {
    mux_need_seek   = 1<<(_fc_freebit+0), // File must be seekable
    mux_need_binary = 1<<(_fc_freebit+1), // Need binary I/O
    mux_default     = fc_none,
    mux_mask        = ~fc_mask
  };

  file_case(FILE *fp, char const *path, uint32_t flags=fc_none);
  ~file_case(void);

  bool          close(void);
  char const *  path(void) const;
  FILE *        file(void) const;
  bool          is_pipe(void) const;

  // Factory muxer; note that fc_take_path will be honoured even on failure
  static file_case *  muxer(char const *path, uint32_t flags=mux_default);

  CLASS_DISABLE_COPY(file_case);
};

inline
file_case::file_case(FILE *fp, char const *path, uint32_t flags)
:
  _path(path), _file(fp), _flags(flags)
{
  assert(!(flags & (fc_const_path | fc_take_path)) ||
      !(flags & fc_const_path) != !(flags & fc_take_path));
  assert(!(flags & ~fc_mask));
}

inline
file_case::~file_case(void)
{
  if (_file != NULL)
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
  return _file;
}

inline bool
file_case::is_pipe(void) const
{
  return ((_flags & fc_pipe) != 0);
}

#endif
/* s-it2-mode */
