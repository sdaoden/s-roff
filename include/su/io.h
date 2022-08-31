/*@ I/O and anything around that topic.
 * TODO needs most things from posix.h/nonposix.h, yet used like su_io_.
 *
 * Copyright (c) 2001 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef su_IO_H
#define su_IO_H

#include <su/code.h>

#define su_HEADER
#include <su/code-in.h>
C_DECL_BEGIN

/* _PC_NAME_MAX for files in directory dname */
EXPORT uz su_file_name_max(char const *dname);

/* _PC_PATH_MAX for files (in directory dname_or_nil) */
EXPORT uz su_path_name_max(char const *dname_or_nil);

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

inline uz file_name_max(char const *dname) {return su_file_name_max(dname);}

inline uz path_name_max(char const *dname_or_nil){
   return su_path_name_max(dname_or_nil);
}

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_IO_H */
/* s-it-mode */
