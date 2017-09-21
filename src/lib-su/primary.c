/*@ Implementation of primary.h.
 *@ TODO Log domain should be configurable
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
#define su_FILE "su__primary"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h> /* TODO Get rid? */
#include <stdlib.h>

#include "su/primary.h"
#include "su/code-in.h"

uiz su_state;

void
su_log(enum su_log_level lvl, char const *fmt, ...){
   va_list va;
   NYD_IN;

   va_start(va, fmt);
   if((S(ui32,lvl) & su__SF_LOGL_MASK) <= (su_state & su__SF_LOGL_MASK) ||
         (lvl <= su_LOGL_NOTICE && (su_state & su_SF_D_V)))
      vfprintf(stderr, fmt, *S(va_list*)vp);
   va_end(va);
   NYD_OU;
}

void
su_vlog(enum su_log_level lvl, char const *fmt, void *vp){
   NYD_IN;
   if((S(ui32,lvl) & su__SF_LOGL_MASK) <= (su_state & su__SF_LOGL_MASK) ||
         (lvl <= su_LOGL_NOTICE && (su_state & su_SF_D_V)))
      vfprintf(stderr, fmt, *S(va_list*)vp);
   NYD_OU;
}

void
su_alert(char const *fmt, ...){
   va_list va;
   NYD2_IN;

   if(su_LOGL_ALERT <= (su_state & su__SF_LOGL_MASK)){
      va_start(va, fmt);
      vfprintf(stderr, fmt, va);
      va_end(va);
   }
   NYD2_OU;
}

void
su_err(char const *fmt, ...){
   va_list va;
   NYD2_IN;

   if(su_LOGL_ERR <= (su_state & su__SF_LOGL_MASK)){
      va_start(va, fmt);
      vfprintf(stderr, fmt, va);
      va_end(va);
   }
   NYD2_OU;
}

void
su_debug(char const *fmt, ...){
   va_list va;
   NYD2_IN;

   if((su_state & su_SF_D_V) ||
         su_LOGL_DEBUG <= (su_state & su__SF_LOGL_MASK)){
      va_start(va, fmt);
      vfprintf(stderr, fmt, va);
      va_end(va);
   }
   NYD2_OU;
}

void
su_panic(char const *fmt, ...){
   va_list va;
   NYD2_IN;

   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
   abort();
   NYD2_OU;
}

void
su_perr(char const *cp, si32 errno_or_0){
   NYD2_IN;
   if(errno_or_0 == 0)
      errno_or_0 = errno;

   fprintf(stderr, "%s: %s\n", cp, strerror(errno_or_0)); /* TODO strerror */
   NYD2_OU;
}

#include "su/code-ou.h"
/* s-it-mode */
