/*@ Forwards of and for all types.
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
#ifndef su_FWDS_H
#define su_FWDS_H

#include <su/code.h>

#define su_HEADER
#include <su/code-in.h>
C_DECL_BEGIN

struct su_array;
struct su_parray;

C_DECL_END

#if !C_LANG
NSPC_BEGIN(su)

template<class T> class array;

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_FWDS_H */
/* s-it-mode */