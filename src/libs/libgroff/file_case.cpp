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

#include "file_case.h"

bool
file_case::close(void)
{
  assert(_file != NULL);

  if (!(_flags & fc_const_path))
    a_delete _path;

  bool rv;
  if (_flags & fc_dont_close)
    rv = true;
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

  if (!(flags & (fc_const_path | fc_take_path))) {
    path = strsave(path);
    flags |= fc_take_path;
  }

  errno = 0;
  FILE *fp = fopen(path, ((flags & mux_need_binary) ? "rb" : "r"));
  int save_err = errno;

  flags &= ~mux_mask;
  file_case *fcp;
  if (fp != NULL)
    fcp = new file_case(fp, path, flags);
  else {
    if (!(flags & fc_const_path))
      a_delete path;
    errno = save_err;
    fcp = NULL;
  }
  return fcp;
}

/* s-it2-mode */
