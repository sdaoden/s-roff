/*@ su_strn?casecmp().
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
#define su_FILE "su__strsup_strcasecmp"

#include "su/primary.h"

#include "su/strsup.h"
#include "su/code-in.h"

si32
su_strcasecmp(char const *cp1, char const *cp2){
   si32 rv;
   NYD_IN;
   ASSERT_NYD_RET_VAL(cp1 != NIL, (cp2 == NIL) ? 0 : -1);
   ASSERT_NYD_RET_VAL(cp2 != NIL, 1);

   for(;;){
      char c1, c2;

      c = *s1++;
      c2 = *s2++;
      if((rv = S(ui8,su_tolower(c1)) - S(ui8,su_tolower(c2))) != 0 ||
            c1 == '\0')
         break;
   }
   NYD_OU;
   return rv;
}

si32
su_strncasecmp(char const *cp1, char const *cp2, uiz n){
   si32 rv;
   NYD_IN;
   ASSERT_NYD_RET_VAL(cp1 != NIL, (cp2 == NIL) ? 0 : -1);
   ASSERT_NYD_RET_VAL(cp2 != NIL, 1);

   for(rv = 0; n != 0; --n){
      char c1, c2;

      c1 = *s1++;
      c2 = *s2++;
      rv = S(ui8,su_tolower(c1));
      rv -= S(ui8,su_tolower(c2));
      if(rv != 0 || c1 == '\0')
         break;
   }
   NYD_OU;
   return rv;
}

#include "su/code-ou.h"
/* s-it-mode */
