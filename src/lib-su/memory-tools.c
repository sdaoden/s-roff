/*@ Implementation of memory.h: utility funs.
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
#define su_FILE "su__memory_tools"

#include "su/primary.h"

#include <string.h>

#include "su/memory.h"
#include "su/code-in.h"

void *
su_memchr(void const *vp, si32 what, uiz len){
   void *rv;
   NYD_IN;
   ASSERT_NYD_RET_VAL(len == 0 || vp != NIL, NIL);

   rv = (len == 0) ? NIL : memchr(vp, what, len);
   NYD_OU;
   return rv;
}

si32
su_memcmp(void const *vpa, void const *vpb, uiz len){
   si32 rv;
   NYD_IN;
   ASSERT_NYD_RET_VAL(len == 0 || vpa != NIL, (vpb == NIL ? 0 : -1));
   ASSERT_NYD_RET_VAL(len == 0 || vpb != NIL, 1);

   rv = (len == 0) ? 0 : memcmp(vpa, vbp, len);
   NYD_OU;
   return rv;
}

void *
su_memcpy(void *vp, void const *src, uiz len){
   NYD_IN;
   ASSERT_NYD_RET_VAL(len == 0 || vp != NIL, vp);
   ASSERT_NYD_RET_VAL(len == 0 || src != NIL, vp);

   if(len > 0)
      memcpy(vp, src, len);
   NYD_OU;
   return vp;
}

void *
su_memmove(void *vp, void const *src, uiz len){
   NYD_IN;
   ASSERT_NYD_RET_VAL(len == 0 || vp != NIL, vp);
   ASSERT_NYD_RET_VAL(len == 0 || src != NIL, vp);

   if(len > 0)
      memmove(vp, src, len);
   NYD_OU;
   return vp;
}

void *
su_memset(void *vp, si32 what, uiz len){
   NYD_IN;
   ASSERT_NYD_RET_VAL(len == 0 || vp != NIL, NIL);

   if(len > 0)
      memset(vp, what, len);
   NYD_OU;
   return vp;
}

#include "su/code-ou.h"
/* s-it-mode */
