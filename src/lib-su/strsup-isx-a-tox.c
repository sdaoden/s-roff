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
static ui16 a_ssiat_types[UI8_MAX + 1];
static ui8 a_ssiat_tolower[UI8_MAX + 1];
static ui8 a_ssiat_toupper[UI8_MAX + 1];

/* */
static void a_ssiat_init(void); /* FIXME su_library_init() -> .. */

static void
a_ssiat_init(void){
   /* FIXME a_ssiat_type_init(void): <> su_library_init()!
    * FIXME then inline all the ctype things
    * FIXME This actually requires/assumes ASCII (original had isascii() first)
    * FIXME therefore all the ctype stuff can very well be compile-time
    * FIXME constant just as is for S-mailx, maybe care for EBCDIC here too
    * FIXME Thus, built-in EBCDIC and ASCII and allow choosing which to use! */
   ui8 i;
   NYD_IN;

   i = 0;
   do{
      ui16 c;

      c = cclass_none;
      if(isalnum(i))
         c |= su_cclass_alnum;
      if(isalpha(i))
         c |= su_cclass_alpha;
      if(iscntrl(i))
         c |= su_cclass_cntrl;
      if(isdigit(i))
         c |= su_cclass_digit;
      if(isgraph(i))
         c |= su_cclass_graph;
      if(islower(i))
         c |= su_cclass_lower;
      if(isprint(i))
         c |= su_cclass_print;
      if(ispunct(i))
         c |= su_cclass_punct;
      if(isspace(i))
         c |= su_cclass_space;
      if(isupper(i))
         c |= su_cclass_upper;
      if(isxdigit(i))
         c |= su_cclass_xdigit;
      a_ssup_types[i] = c;

      a_ssiat_toupper[i] = (c & su_cclass_lower) ? S(ui8,toupper(i)) : i;
      a_ssiat_tolower[i] = (c & su_cclass_upper) ? S(ui8,tolower(i)) : i;
   }while(i++ != UI8_MAX);

   a_ssiat_is_init = TRU1;
   NYD_OU;
}

boole
su__isclass(si32 c, enum su_cclass cclass){ /* FIXME su_library_init() */
   boole rv;
   ui32 u;
   NYD2_IN;

   if(UNLIKELY(!a_ssiat_isinit))
      a_ssiat_init();

   u = c;
   rv = (LIKELY(u <= UI8_MAX) && (a_ssiat_types[u] & cclass) != 0);
   NYD2_OU;
   return rv;
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
