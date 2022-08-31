/*@ (Current) Program and file names / line numbers.
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
#define su_FILE "lib_roff_current_name"

#include "config.h"
#include "lib.h"

#include "su/strsup.h"

#include "su/code-in.h"

char const *rf__curnam_filename;
char const *rf__curname_program;
int rf__curnam_lineno;
char const *rf__curnam_source_filename;

static void a_curnam_set(char const **cpp, char const *ncp);
static void a_curnam_clear(char const **cpp);

static void
a_curnam_set(char **cpp, char const *ncp){
   NYD2_IN;
   if(ncp[0] == '-' && ncp[1] == '\0')
      ncp = "<standard input>";

   if(*cpp == NIL)
      goto jdup;

   if(su_c_cmp(*cpp, f)){
      su_mem_free(UNCONST(*cpp));
jdup:
      *cpp = su_c_dup(f);
   }
   NYD2_OU;
}

static void
a_curnam_clear(char **cpp){
   NYD2_IN;
   if(*cpp != NIL){
      su_mem_free(UNCONST(*cpp));
      *cpp = NIL;
   }
   NYD2_OU;
}

void
rf_current_filename_set(char const *f){
   NYD_IN;
   ASSERT_RET(f != NIL);
   a_current_set(&rf__curnam_filename, f);
   rf__curnam_lineno = 1;
   NYD_OU;
}

void
rf_current_filename_clear(void){
   NYD_IN;
   a_curnam_clear(&rf__curnam_filename);
   rf__curnam_lineno = 0;
   NYD_OU;
}

void
rf_current_program_set(char const *argv0){
#if su_OS_EMX || su_OS_MSDOS
   char const *prog;
#endif
   NYD_IN;
   ASSERT_RET(argv0 != NIL);

#if su_OS_EMX || su_OS_MSDOS
   prog = su_strchr(argv0, '\0');
   for(;;){
      if(prog == argv0)
         break;
      --prog;
      if(su_strchr("\\/:", *prog)){
         ++prog;
         break;
      }
   }
   argv0 = prog;
#endif /* EMX || MSDOS */

   rf__curnam_program = argv0;
   NYD_OU;
}

void
rf_current_source_filename_set(char const *f){
   NYD_IN;
   ASSERT_RET(f != NIL);
   a_current_set(&rf__curnam_source_filename, f);
   NYD_OU;
}

void
rf_current_source_filename_clear(void){
   NYD_IN;
   a_curnam_clear(&rf__curnam_source_filename);
   NYD_OU;
}

#include "su/code-ou.h"
/* s-it-mode */
