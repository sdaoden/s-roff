/*@ Implementation of cs.h: anything which performs allocations.
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
#define su_FILE "su__cs_alloc"
#define su_SOURCE
#define su_SOURCE_CS_ALLOC

#include "su/code.h"
#include "su/mem.h"

#include "su/cs.h"
#include "su/code-in.h"

char *
su_cs_dup(char const *cp){
   char *rv;
   uz l;
   NYD_IN;
   ASSERT_EXEC(cp != NIL, cp = su_empty);

   if(LIKELY((l = su_cs_len(cp)) != UZ_MAX)){
      ++l;
      if((rv = su_TALLOC(char, l)) != NIL)
         su_mem_copy(rv, cp, l);
   }else{
      su_state_err(su_STATE_ERR_OVERFLOW, _("SU cs_dup: string too long"));
      rv = NIL;
   }
   NYD_OU;
   return rv;
}

char *
su_cs_dup_cbuf(char const *buf, uz len){
   char *rv;
   NYD_IN;
   ASSERT_EXEC(len == 0 || buf != NIL, len = 0);

   if(len == UZ_MAX)
      len = su_cs_len(buf);

   if(LIKELY(len != UZ_MAX)){
      if((rv = su_TALLOC(char, len +1)) != NIL){
         su_mem_copy(rv, buf, len);
         buf[len] = '\0';
      }
   }else{
      su_state_err(su_STATE_ERR_OVERFLOW, _("SU cs_dup: buffer too large"));
      rv = NIL;
   }
   NYD_OU;
   return rv;
}

#include "su/code-ou.h"
/* s-it-mode */
