/*@ Implementation of cs.h: classification and case conversion.
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
#define su_FILE "su__cs_class"
#define su_SOURCE
#define su_SOURCE_CS_CLASS

#include "su/code.h"

#include <ctype.h> /* FIXME as below */

#include "su/cs.h"
#include "su/code-in.h"

static boole a_csc_is_init;
static u16 a_csc_types[U8_MAX + 1];
static u8 a_csc_tolower[U8_MAX + 1];
static u8 a_csc_toupper[U8_MAX + 1];

/* */
static void a_csc_init(void); /* FIXME su_library_init() -> .. */

static void
a_csc_init(void){
   /* FIXME a_csc_type_init(void): <> su_library_init()!
    * FIXME then inline all the ctype things
    * FIXME This actually requires/assumes ASCII (original had isascii() first)
    * FIXME therefore all the ctype stuff can very well be compile-time
    * FIXME constant just as is for S-mailx, maybe care for EBCDIC here too
    * FIXME Thus, built-in EBCDIC and ASCII and allow choosing which to use! */
   u8 i;
   NYD_IN;

   i = 0;
   do{
      u16 c;

      c = su_C_CLASS_NONE;
      if(isalnum(i))
         c |= su_C_CLASS_ALNUM;
      if(isalpha(i))
         c |= su_C_CLASS_ALPHA;
      if(iscntrl(i))
         c |= su_C_CLASS_CNTRL;
      if(isdigit(i))
         c |= su_C_CLASS_DIGIT;
      if(isgraph(i))
         c |= su_C_CLASS_GRAPH;
      if(islower(i))
         c |= su_C_CLASS_LOWER;
      if(isprint(i))
         c |= su_C_CLASS_PRINT;
      if(ispunct(i))
         c |= su_C_CLASS_PUNCT;
      if(isspace(i))
         c |= su_C_CLASS_SPACE;
      if(isupper(i))
         c |= su_C_CLASS_UPPER;
      if(isxdigit(i))
         c |= su_C_CLASS_XDIGIT;
      a_ssup_types[i] = c;

      a_csc_toupper[i] = (c & su_C_CLASS_LOWER) ? S(u8,toupper(i)) : i;
      a_csc_tolower[i] = (c & su_C_CLASS_UPPER) ? S(u8,tolower(i)) : i;
   }while(i++ != U8_MAX);

   a_csc_is_init = TRU1;
   NYD_OU;
}

boole
su__cs_is_class(s32 c, enum su_cs_class cclass){ /* FIXME su_library_init() */
   boole rv;
   u32 u;
   NYD2_IN;

   if(UNLIKELY(!a_csc_isinit))
      a_csc_init();

   u = c;
   rv = (LIKELY(u <= U8_MAX) && (a_csc_types[u] & cclass) != 0);
   NYD2_OU;
   return rv;
}

sz
su_cs_casecmp(char const *cp1, char const *cp2){
   sz rv;
   NYD_IN;
   ASSERT_NYD_RET(cp1 != NIL, rv = (cp2 == NIL) ? 0 : -1);
   ASSERT_NYD_RET(cp2 != NIL, rv = 1);

   if(UNLIKELY(!a_csc_isinit))
      a_csc_init();

   for(;;){
      u8 c1, c2;

      c1 = *s1++;
      c2 = *s2++;
      if((rv = a_csc_tolower[c1] - a_csc_tolower[c2]) != 0 || c1 == '\0')
         break;
   }
   NYD_OU;
   return rv;
}

sz
su_cs_casecmp_n(char const *cp1, char const *cp2, uz n){
   sz rv;
   NYD_IN;
   ASSERT_NYD_RET(cp1 != NIL, rv = (cp2 == NIL) ? 0 : -1);
   ASSERT_NYD_RET(cp2 != NIL, rv = 1);

   if(UNLIKELY(!a_csc_isinit))
      a_csc_init();

   for(rv = 0; n != 0; --n){
      u8 c1, c2;

      c1 = *s1++;
      c2 = *s2++;
      rv = a_csc_tolower[c1];
      rv -= a_csc_tolower[c2];
      if(rv != 0 || c1 == '\0')
         break;
   }
   NYD_OU;
   return rv;
}

s32
su__cs_to_lower(s32 c){ /* FIXME su_library_init() -> inline! */
   u32 u;
   NYD_IN;

   if(UNLIKELY(!a_csc_isinit))
      a_csc_init();

   u = c;
   if(LIKELY(u <= U8_MAX))
      c = a_csc_tolower[u];
   NYD_OU;
   return c;
}

s32
su__cs_to_upper(s32 c){ /* FIXME su_library_init() -> inline! */
   u32 u;
   NYD_IN;

   if(UNLIKELY(!a_csc_isinit))
      a_csc_init();

   u = c;
   if(LIKELY(u <= U8_MAX))
      c = a_csc_toupper[u];
   NYD_OU;
   return c;
}

#include "su/code-ou.h"
/* s-it-mode */
