/*@ su_is*() and su_to*().
 *
 * Copyright (c) 2017 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#define su_FILE "su__strsup_isx_a_tox"

#include "su/primary.h"

#include <ctype.h>

#include "su/strsup.h"
#include "su/code-in.h"

static boole a_ssiat_is_init;
static ui8 a_ssiat_tolower[UI8_MAX + 1];
static ui8 a_ssiat_toupper[UI8_MAX + 1];

/* */
static void a_ssiat_init(void); /* FIXME su_library_init() -> .. */

static void
a_ssiat_init(void){
   ui8 i;
   NYD_IN;

   i = 0;
   do
      a_ssiat_tolower[i] = S(ui8,tolower(i));
   while(i++ != UI8_MAX);

   i = 0;
   do
      a_ssiat_toupper[i] = S(ui8,toupper(i));
   while(i++ != UI8_MAX);

   a_ssiat_is_init = TRU1;
   NYD_OU;
}

si32
su__tolower(si32 c){ /* FIXME su_library_init() -> inline! */
   ui32 u;
   NYD_IN;

   if(UNLIKELY(!a_ssiat_isinit))
      a_ssiat_init();

   u = c;
   if(LIKELY(u <= UI8_MAX))
      c = a_ssiat_tolower[u];
   NYD_OU;
   return c;
}

si32
su__toupper(si32 c){ /* FIXME su_library_init() -> inline! */
   ui32 u;
   NYD_IN;

   if(UNLIKELY(!a_ssiat_isinit))
      a_ssiat_init();

   u = c;
   if(LIKELY(u <= UI8_MAX))
      c = a_ssiat_toupper[u];
   NYD_OU;
   return c;
}

#include "su/code-ou.h"
/* s-it-mode */
