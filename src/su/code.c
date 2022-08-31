/*@ Implementation of code.h: (unavoidable) basics.
 *@ TODO Log: domain should be configurable
 *@ TODO Assert: the C++ lib has per-thread assertion states, s_nolog to
 *@ TODO    suppress log, test_state(), test_and_clear_state(): for unit tests!
 *@ TODO su_program: if set, the PID should be logged, too!
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
#define su_FILE "su__code"
#define su_MASTER
#define su_SOURCE
#define su_SOURCE_CODE

#include <stdarg.h>
#include <stdio.h> /* TODO Get rid? */
#include <stdlib.h>

#include "su/code.h"
#include "su/code-in.h"

#define a_PRIMARY_DOLOG(LVL) \
   ((S(u32,LVL) & su__STATE_LOG_MASK) <= (su_state & su__STATE_LOG_MASK) ||\
      (su_state & su__STATE_D_V))

static char const a_c_lvlnames[][8] = { /* TODO no level name stuff yet*/
   FIELD_INITI(su_LOG_EMERG) "emerg",
   FIELD_INITI(su_LOG_ALERT) "alert",
   FIELD_INITI(su_LOG_CRIT) "crit",
   FIELD_INITI(su_LOG_ERR) "error",
   FIELD_INITI(su_LOG_WARN) "warning\0",
   FIELD_INITI(su_LOG_NOTICE) "notice",
   FIELD_INITI(su_LOG_INFO) "info",
   FIELD_INITI(su_LOG_DEBUG) "debug"
};
/* You can deduce the value from the offset */
CTAV(su_LOG_EMERG == 0);
CTAV(su_LOG_DEBUG == 7);

u16 const su_bom = 0xFEFFu;
char const su_empty[] = "";

uz su__state;
char const *su_program;

s32
su_state_err(uz state, char const *msg_or_nil){
   static char const intro_nomem[] = N_("Out of memory: %s"),
      intro_overflow[] = N_("Datatype overflow: %s");

   enum su_log_lvl lvl;
   char const *introp;
   s32 err;
   NYD2_IN;
   ASSERT((state & su__STATE_ERR_MASK) && !(state & ~su__STATE_ERR_MASK));

   switch(state & su_STATE_ERR_TYPE_MASK){
   default:
      ASSERT(0);
      /* FALLTHRU */
   case su_STATE_ERR_NOMEM:
      err = su_ERR_NOMEM;
      introp = intro_nonem;
      break;
   case su_STATE_ERR_OVERFLOW:
      err = su_ERR_OVERFLOW;
      introp = intro_overflow;
      break;
   }
   if(msg_or_nil == NIL)
      msg_or_nil = N_("no error information available");

   if(state & su_STATE_ERR_NOPASS)
      goto jemerg;
   if(state & su_STATE_ERR_PASS){
      lvl = su_LOG_DEBUG;
      goto jlog_check;
   }else if(su_state_has(state)){
      lvl = su_LOG_ALERT;
jlog_check:
      if(a_PRIMARY_DOLOG(lvl))
         goto jdolog;
   }else{
jemerg:
      lvl = su_LOG_EMERG;
jdolog:
      su_log(lvl, V_(introp), V_(msg_or_nil));
   }

   if(!(state & su_STATE_ERR_NOERRNO)
      su_err_set_no(err);
   NYD2_OU;
   return err;
}

void
su_log(enum su_log_level lvl, char const *fmt, ...){
   va_list va;
   NYD_IN;

   if(a_PRIMARY_DOLOG(lvl)){
      if(su_program != NIL)
         fprintf(stderr, "%s: ", su_program);
      va_start(va, fmt);
      vfprintf(stderr, fmt, va);
      va_end(va);

      if(lvl == su_LOG_EMERG)
         abort(); /* TODO configurable */
   }
   NYD_OU;
}

void
su_vlog(enum su_log_level lvl, char const *fmt, void *vp){
   NYD_IN;
   if(a_PRIMARY_DOLOG(lvl)){
      if(su_program != NIL)
         fprintf(stderr, "%s: ", su_program);
      vfprintf(stderr, fmt, *S(va_list*,vp));

      if(lvl == su_LOG_EMERG)
         abort(); /* TODO configurable */
   }
   NYD_OU;
}

#include "su/code-ou.h"
/* s-it-mode */
