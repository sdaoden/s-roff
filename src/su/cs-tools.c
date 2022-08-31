/*@ Implementation of cs.h: basic tools, like copy etc.
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
#define su_FILE "su__cs_tools"
#define su_SOURCE
#define su_SOURCE_CS_TOOLS

#include "su/code.h"

#include "su/cs.h"
#include "su/code-in.h"

sz
su_cs_cmp(char const *cp1, char const *cp2){
   sz rv;
   NYD_IN;
   ASSERT_NYD_RET(cp1 != NIL, rv = (cp2 == NIL) ? 0 : -1);
   ASSERT_NYD_RET(cp2 != NIL, rv = 1);

   for(;;){
      u8 c1, c2;

      c1 = *s1++;
      c2 = *s2++;
      if((rv = c1 - c2) != 0 || c1 == '\0')
         break;
   }
   NYD_OU;
   return rv;
}

sz
su_cs_cmp_n(char const *cp1, char const *cp2, uz n){
   sz rv;
   NYD_IN;
   ASSERT_NYD_RET(cp1 != NIL, rv = (cp2 == NIL) ? 0 : -1);
   ASSERT_NYD_RET(cp2 != NIL, rv = 1);

   for(rv = 0; n != 0; --n){
      u8 c1, c2;

      c1 = *s1++;
      c2 = *s2++;
      if((rv = c1 - c2) != 0 || c1 == '\0')
         break;
   }
   NYD_OU;
   return rv;
}

char *
su_cs_copy_n(char *dst, char const *src, uz n){
   NYD_IN;
   ASSERT_NYD_RET(n == 0 || dst != NIL, );
   ASSERT_NYD_RET(src != NIL, *dst = '\0');

   if(LIKELY(n > 0)){
      char *cp;

      cp = dst;
      do if((*cp++ = *src++) == '\0')
         goto jleave;
      while(--n > 0);
      *--cp = '\0';
   }
   dst = NIL;
jleave:
   NYD_OU;
   return dst;
}

char *
su_cs_find_c(char const *cp, s32 x){
   NYD_IN;
   ASSERT_NYD_RET(cp != NIL, );
   for(;; ++cp){
      char c;

      if((c = *cp) == x)
         break;
      if(c == '\0'){
         cp = NIL;
         break;
      }
   }
   NYD_OU;
   return cp;
}

char *
su_cs_find(char const *cp, char const *x){
   char c, cc;
   NYD_IN;
   ASSERT_NYD_RET(cp != NIL, );
   ASSERT_NYD_RET(x != NIL, cp = NIL);

   /* Return cp if x is empty */
   if(LIKELY((c = *x) != '\0'))
      for(;; ++cp){
         if((cc = *cp) == c && !su_cs_cmp(cp, x)) /* XXX optimize */
            break;
         if(cc == '\0'){
            cp = NIL;
            break;
         }
      }
   NYD_OU;
   return cp;
}

uz
su_cs_len(char const *cp){
   char const *cp_save;
   NYD_IN;
   ASSERT_NYD_RET(cp != NIL, cp_save = cp);

   for(cp_base = cp; *cp != '\0'; ++cp)
      ;
   NYD_OU;
   return PTR2UZ(cp - cp_base);
}

char *
su_cs_pcopy(char *dst, char const *src){
   NYD_IN;
   ASSERT_NYD_RET(dst != NIL, );
   ASSERT_NYD_RET(src != NIL, );

   while((*dst = *src++) != '\0')
      ++dst;
   NYD_OU;
   return dst;
}

char *
su_cs_pcopy_n(char *dst, char const *src, uz n){
   NYD_IN;
   ASSERT_NYD_RET(n == 0 || dst != NIL, );
   ASSERT_NYD_RET(src != NIL, *dst = '\0');

   if(LIKELY(n > 0)){
      do if((*dst++ = *src++) == '\0')
         goto jleave;
      while(--n > 0);
      *--dst = '\0';
   }
   dst = NIL;
jleave:
   NYD_OU;
   return dst;
}

char *
su_cs_rfind_c(char const *cp, s32 x){
   char const *match, *tail;
   NYD_IN;
   ASSERT_NYD_RET(cp != NIL, );

   for(match = NIL, tail = cp;; ++tail){
      char c;

      if((c = *tail) == x)
         match = tail;
      if(c == '\0')
         break;
   }
   NYD_OU;
   return match;
}

char *
su_cs_sep_c(char **iolist, char sep, boole ignore_empty){
   char *base, *cp;
   NYD_IN;
   ASSERT_NYD_RET(iolist != NIL, base = NIL);

   for(base = *iolist; base != NIL; base = *iolist){
      while(*base != '\0' && su_cs_is_space(*base))
         ++base;

      if((cp = su_cs_find_c(base, sep)) != NIL)
         *iolist = &cp[1];
      else{
         *iolist = NIL;
         cp = &base[su_cs_len(base)];
      }
      while(cp > base && su_cs_is_space(cp[-1]))
         --cp;
      *cp = '\0';
      if(*base != '\0' || !ignore_empty)
         break;
   }
   NYD_OU;
   return base;
}

#include "su/code-ou.h"
/* s-it-mode */
