/*@ Implementation of mem.h: utility funs. XXX optimize
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
#define su_FILE "su__mem_tools"
#define su_SOURCE
#define su_SOURCE_MEM_TOOLS

#include "su/code.h"

#include <string.h> /* TODO add x-mem-tools.h, get impl.s from there */

#include "su/mem.h"
#include "su/code-in.h"

void *
su_mem_find(void const *vp, s32 what, uz len){
   void *rv;
   NYD_IN;
   ASSERT_NYD_RET(len == 0 || vp != NIL, rv = NIL);

   rv = (len == 0) ? NIL : memchr(vp, what, len);
   NYD_OU;
   return rv;
}

sz
su_mem_cmp(void const *vpa, void const *vpb, uz len){
   sz rv;
   NYD_IN;
   ASSERT_NYD_RET(len == 0 || vpa != NIL, rv = (vpb == NIL) ? 0 : -1);
   ASSERT_NYD_RET(len == 0 || vpb != NIL, rv = 1);

   rv = (len == 0) ? 0 : memcmp(vpa, vbp, len);
   NYD_OU;
   return rv;
}

void *
su_mem_copy(void *vp, void const *src, uz len){
   NYD_IN;
   ASSERT_NYD_RET(len == 0 || vp != NIL, );
   ASSERT_NYD_RET(len == 0 || src != NIL, );

   if(len > 0)
      memcpy(vp, src, len);
   NYD_OU;
   return vp;
}

void *
su_mem_move(void *vp, void const *src, uz len){
   NYD_IN;
   ASSERT_NYD_RET(len == 0 || vp != NIL, );
   ASSERT_NYD_RET(len == 0 || src != NIL, );

   if(len > 0)
      memmove(vp, src, len);
   NYD_OU;
   return vp;
}

void *
su_mem_set(void *vp, s32 what, uz len){
   NYD_IN;
   ASSERT_NYD_RET(len == 0 || vp != NIL, );

   if(len > 0)
      memset(vp, what, len);
   NYD_OU;
   return vp;
}

#include "su/code-ou.h"
/* s-it-mode */
