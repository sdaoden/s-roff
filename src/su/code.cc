/*@ C++ injection point of most things which need it.
 *
 * Copyright (c) 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#define su_FILE "su__code"
#define su_MASTER
#define su_SOURCE
#define su_SOURCE_CODE

#include "su/cs.h"

#include "su/code.h"
#include "su/code-in.h"

NSPC_USE(su)

// code.h

PUB STA u16 const endian::bom = 0xFEFFu;

void
log::log(level lvl, char const *fmt, ...){ // XXX unroll
   va_list va;
   NYD_IN;

   va_start(va, fmt);
   su_log(S(enum su_log_level,lvl), fmt, &va);
   va_end(va);
   NYD_OU;
}

// cs.h

PUB STA type_toolbox<char*> const * const cs::type_toolbox =
      R(;
PUB STA type_toolbox<char const*> const * const cs::const_type_toolbox =
      R(

#include "su/code-ou.h"
/* s-it-mode */
