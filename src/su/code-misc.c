/*@ Implementation of code.h: what belongs nowhere else, really.
 *@ Also to reduce static object inter-dependencies.
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
#define su_FILE "su__code_misc"
#define su_SOURCE
#define su_SOURCE_CODE_MISC

#include <errno.h> /* TODO Get rid */
#include <stdio.h> /* TODO Get rid? */
#include <stdlib.h> /* TODO Get rid (abort()) */

#include "su/code.h"
#include "su/code-in.h"

void
su_perr(char const *msg, s32 err_no_or_0){
   NYD2_IN; /* XXX may clash - we may query su_err_no */
   if(err_no_or_0 == 0)
      err_no_or_0 = su_err_no();

   if(su_program != NIL)
      fprintf(stderr, "%s: ", su_program);
   fprintf(stderr, "%s: %s\n", msg, su_err_doc(err_no_or_0));
   NYD2_OU;
}

void
su_assert(char const *expr, char const *file, s32 line, char const *fun,
      boole crash){
   char const *pre;

   pre = (su_program != NIL) ? su_program : su_empty;
   fprintf(stderr,
      "%s: SU assert failed: %.60s\n"
      "%s:   File %.60s, line %d\n"
      "%s:   Function %.142s\n",
      expr, file, line, fun);

   if(crash)
      abort();
}

#include "su/code-ou.h"
/* s-it-mode */
