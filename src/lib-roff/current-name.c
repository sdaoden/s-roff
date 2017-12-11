/*@ (Current) Program, device and file names / line numbers.
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
#define su_FILE "lib_roff__current_name"
#define rf_SOURCE
#define rf_SOURCE_LIB_ROFF_CURRENT_NAME

#include "config.h"
#include "lib.h"

#include "su/cs.h"
#include "su/mem.h"

#include "su/code-in.h"

struct rf_current_ctx rf__current;

static void a_curnam_set(char const **cpp, char const *ncp);
static void a_curnam_clear(char const **cpp);

static void
a_curnam_set(char **cpp, char const *ncp){
   NYD2_IN;
   if(ncp[0] == '-' && ncp[1] == '\0')
      ncp = "<standard input>";

   if(*cpp == NIL)
      goto jdup;

   if(su_cs_cmp(*cpp, f)){
      su_FREE(UNCONST(*cpp));
jdup:
      *cpp = su_cs_dup(f);
   }
   NYD2_OU;
}

static void
a_curnam_clear(char **cpp){
   NYD2_IN;
   if(*cpp != NIL){
      su_FREE(UNCONST(*cpp));
      *cpp = NIL;
   }
   NYD2_OU;
}

char const *
rf_current_device_set(char const *name){
   NYD_IN;
   if(name == NULL && (name = getenv(U_ROFF_TYPESETTER)) == NULL)
      name = VAL_DEVICE;
   rf__current.curc_device = name;
   NYD_OU;
   return rf__current.curc_device;
}

void
rf_current_filename_set(char const *f){
   NYD_IN;
   ASSERT_NYD_RET(f != NIL);
   a_curnam_set(&rf__current.curc_filename, f);
   rf__current.curc_lineno = 1;
   NYD_OU;
}

void
rf_current_filename_clear(void){
   NYD_IN;
   a_curnam_clear(&rf__current.curc_filename);
   rf__current.curc_lineno = 0;
   NYD_OU;
}

void
rf_current_program_set(char const *argv0){
#if su_OS_EMX || su_OS_MSDOS
   char const *prog;
#endif
   NYD_IN;
   ASSERT_NYD_RET(argv0 != NIL, );

#if su_OS_EMX || su_OS_MSDOS
   prog = su_cs_find_c(argv0, '\0');
   for(;;){
      if(prog == argv0)
         break;
      --prog;
      if(su_cs_find_c("\\/:", *prog)){
         ++prog;
         break;
      }
   }
   argv0 = prog;
#endif /* EMX || MSDOS */

   rf__current.curc_program = argv0;
   NYD_OU;
}

void
rf_current_source_filename_set(char const *f){
   NYD_IN;
   ASSERT_NYD_RET(f != NIL);
   a_curnam_set(&rf__current.curc_source_filename, f);
   NYD_OU;
}

void
rf_current_source_filename_clear(void){
   NYD_IN;
   a_curnam_clear(&rf__current.curc_source_filename);
   NYD_OU;
}

#include "su/code-ou.h"
/* s-it-mode */
