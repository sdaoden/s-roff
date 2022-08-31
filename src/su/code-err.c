/*@ Implementation of code.h: errors.
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
#define su_FILE "su__code_err"
#define su_SOURCE
#define su_SOURCE_CODE_ERR

#include "su/cs.h"

#include "su/code.h"
#include "su/code-in.h"

struct a_ce_err_map{
   u32 ceem_hash;    /* Hash of name */
   u32 ceem_nameoff; /* Into a_ce_err_names[] */
   u32 ceem_docoff;  /* Into a_ce_err docs[] */
   s32 ceem_err_no;  /* The OS error value for this one */
};

/* Include the constant make-errors.sh output */
#include <gen-errors.h>

/* And these come from mk-config.h (config-time make-errors.sh output) */
static su__ERR_NUMBER_TYPE const a_ce_err_no2mapoff[][2] = {
#undef a_X
#define a_X(N,I) {N,I},
su__ERR_NUMBER_TO_MAPOFF
#undef a_X
};

/* Find the descriptive mapping of an error number, or _ERR_INVAL */
static struct a_ce_err_map const *a_ce_err_map_from_no(s32 eno);

static struct a_ce_err_map const *
a_ce_err_map_from_no(s32 eno){
   s32 ecmp;
   uz asz;
   su__ERR_NUMBER_TYPE const (*adat)[2], (*tmp)[2];
   struct a_ce_err_map const *ceemp;
   NYD2_IN;

   ceemp = &a_ce_err_map[su__ERR_NUMBER_VOIDOFF];

   if(UCMP(z, ABS(eno), <=, S(su__ERR_NUMBER_TYPE,-1))){
      for(adat = a_ce_err_no2mapoff, asz = NELEM(a_ce_err_no2mapoff);
            asz != 0; asz >>= 1){
         tmp = &adat[asz >> 1];
         if((ecmp = S(s32,S(su__ERR_NUMBER_TYPE,eno) - (*tmp)[0])) == 0){
            ceemp = &a_ce_err_map[(*tmp)[1]];
            break;
         }
         if(ecmp > 0){
            adat = &tmp[1];
            --asz;
         }
      }
   }
   NYD2_OU;
   return ceemp;
}

char const *
su_err_doc(s32 eno){
   char const *rv;
   struct a_ce_err_map const *ceemp;
   NYD2_IN;

   ceemp = a_ce_err_map_from_no(eno);
   rv = &a_ce_err_docs[ceemp->ceem_docoff];
   NYD2_OU;
   return rv;
}

char const *
su_err_name(s32 eno){
   char const *rv;
   struct a_ce_err_map const *ceemp;
   NYD2_IN;

   ceemp = a_ce_err_map_from_no(eno);
   rv = &a_ce_err_names[ceemp->ceem_nameoff];
   NYD2_OU;
   return rv;
}

s32
su_err_from_name(char const *name){
   struct a_ce_err_map const *ceemp;
   uz hash, i, j, x;
   s32 rv;
   NYD2_IN;

   hash = su_cs_hash(name);

   for(i = hash % a_SSM_ERR_REV_PRIME, j = 0; j <= a_SSM_ERR_REV_LONGEST; ++j){
      if((x = a_ce_err_revmap[i]) == a_SSM_ERR_REV_ILL)
         break;

      ceemp = &a_ce_err_map[x];
      if(ceemp->ceem_hash == hash &&
            !su_cs_cmp(&a_ce_err_names[ceemp->ceem_nameoff], name)){
         rv = ceemp->aem_err_no;
         goto jleave;
      }

      if(++i == a_SSM_ERR_REV_PRIME){
#ifdef a_SSM_ERR_REV_WRAPAROUND
         i = 0;
#else
         break;
#endif
      }
   }

   rv = -a_aux_err_map[n__ERR_NUMBER_VOIDOFF].aem_err_no;
jleave:
   NYD2_OU;
   return rv;
}

#include "su/code-ou.h"
/* s-it-mode */
