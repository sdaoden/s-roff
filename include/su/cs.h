/*@ Anything (locale agnostic: ASCII or EBCDIC) around char and char*.
 *
 * Copyright (c) 2001 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef su_CS_H
#define su_CS_H

#include <su/code.h>
#define su_A_T_T_DECL_ONLY
#include <su/a-t-t.h>

#define su_HEADER
#include <su/code-in.h>
C_DECL_BEGIN

enum su_cs_class{
   su_CS_CLASS_NONE,
   su_CS_CLASS_ALNUM = 1u<<0,
   su_CS_CLASS_ALPHA = 1u<<1,
   su_CS_CLASS_CNTRL = 1u<<2,
   su_CS_CLASS_DIGIT = 1u<<3,
   su_CS_CLASS_GRAPH = 1u<<4,
   su_CS_CLASS_LOWER = 1u<<5,
   su_CS_CLASS_PRINT = 1u<<6,
   su_CS_CLASS_PUNCT = 1u<<7,
   su_CS_CLASS_SPACE = 1u<<8,
   su_CS_CLASS_UPPER = 1u<<9,
   su_CS_CLASS_XDIGIT = 1u<<10,

   su__CS_CLASS_MAXSHIFT = 11u,
   su__CS_CLASS_MASK = (1u<<su__CS_CLASS_MAXSHIFT) - 1
};

EXPORT_DATA struct toolbox const su_cs_toolbox;

/* Character classification FIXME su_library_init()+inline */
#define su_cs_is_alnum(C) su__c_is_class(C, su_CS_CLASS_ALNUM)
#define su_cs_is_alpha(C) su__c_is_class(C, su_CS_CLASS_ALPHA)
#define su_cs_is_cntrl(C) su__c_is_class(C, su_CS_CLASS_CNTRL)
#define su_cs_is_digit(C) su__c_is_class(C, su_CS_CLASS_DIGIT)
#define su_cs_is_graph(C) su__c_is_class(C, su_CS_CLASS_GRAPH)
#define su_cs_is_lower(C) su__c_is_class(C, su_CS_CLASS_LOWER)
#define su_cs_is_print(C) su__c_is_class(C, su_CS_CLASS_PRINT)
#define su_cs_is_punct(C) su__c_is_class(C, su_CS_CLASS_PUNCT)
#define su_cs_is_space(C) su__c_is_class(C, su_CS_CLASS_SPACE)
#define su_cs_is_upper(C) su__c_is_class(C, su_CS_CLASS_UPPER)
#define su_cs_is_xdigit(C) su__c_is_class(C, su_CS_CLASS_XDIGIT)
EXPORT boole su__cs_is_class(s32 c, enum su_cs_class cclass);

/* String comparison, byte-based, case-insensitive */
EXPORT sz su_cs_casecmp(char const *cp1, char const *cp2);
EXPORT sz su_cs_casecmp_n(char const *cp1, char const *cp2, uz n);

/* Hash a string (buffer), case-insensitively.
 * This should be considered an attackable hash, for now Chris Torek's hash
 * algorithm is used.  TODO Add _strong_hash (with, e.g., siphash algo) */
EXPORT uz su_cs_casehash_cbuf(char const *buf, uz len);
#define su_cs_casehash(CP) su_cs_casehash_cbuf(CP, UZ_MAX)

/* String comparison, byte-based, case-sensitive */
EXPORT sz su_cs_cmp(char const *cp1, char const *cp2);
EXPORT sz su_cs_cmp_n(char const *cp1, char const *cp2, uz n);

/* Copy at most n bytes of src to dst and return dst again.
 * Returns NIL if dst is not large enough; dst will always be
 * terminated unless n was 0 on entry */
EXPORT char *su_cs_copy_n(char *dst, char const *src, uz n);

/* Duplicate string or buffer into su_MEM_TALLOC()ated duplicate.
 * The latter will call cs_len() if len==UZ_MAX, otherwise embedded NULs will
 * be included in the copy.
 * Return NIL for su_STATE_ERR_NOMEM or su_STATE_ERR_OVERFLOW.
 * Note: mem.h is not included. */
EXPORT char *su_cs_dup(char const *cp);
EXPORT char *su_cs_dup_cbuf(char const *buf, uz len);

/* Search x within cp, return pointer to location or NIL.
 * For the non _c() versions, return cp if x is the empty buffer */
EXPORT char *su_cs_find_c(char const *cp, s32 x);
EXPORT char *su_cs_find(char const *cp, char const *x);

/* Hash a string (buffer).
 * This should be considered an attackable hash, for now Chris Torek's hash
 * algorithm is used.  TODO Add _strong_hash (with, e.g., siphash algo) */
EXPORT uz su_cs_hash_cbuf(char const *buf, uz len);
#define su_cs_hash(CP) su_cs_hash_cbuf(CP, UZ_MAX)

/* */
EXPORT uz su_cs_len(char const *cp);

/* Copy src to dst, return pointer to NUL in dst.
 * The latter returns NIL if dst is not large enough; dst will always be
 * terminated unless n was 0 on entry */
EXPORT char *su_cs_pcopy(char *dst, char const *src);
EXPORT char *su_cs_pcopy_n(char *dst, char const *src, uz n);

/* Search x within cp, starting at end, return pointer to location or NIL.
 * For the non _c() versions, return cp if x is the empty buffer */
EXPORT char *su_cs_rfind_c(char const *cp, s32 x);

/* */
EXPORT char *su_cs_sep_c(char **iolist, char sep, boole ignore_empty);

/* Map to lower/upper or return unchanged */
#define su_cs_to_lower(C) su__cs_to_lower(C) /* FIXME su_library_init+inline */
#define su_cs_to_upper(C) su__cs_to_upper(C) /* FIXME su_library_init+inline */
EXPORT s32 su__cs_to_lower(s32 c);
EXPORT s32 su__cs_to_upper(s32 c);

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

// Instanceless static encapsulator.
class EXPORT cs{
public:
   static NSPC(su)type_toolbox<char*> const * const type_toolbox =
         R(NSPC(su)type_toolbox<char*> const*,&su_cs_toolbox);
   static NSPC(su)type_toolbox<char const*> const * const const_type_toolbox =
         R(NSPC(su)type_toolbox<char const*> const*,&su_cs_toolbox);

   static boole is_alnum(s32 c) {return su_cs_is_alnum(c);}
   static boole is_alpha(s32 c) {return su_cs_is_alpha(c);}
   static boole is_cntrl(s32 c) {return su_cs_is_cntrl(c);}
   static boole is_digit(s32 c) {return su_cs_is_digit(c);}
   static boole is_graph(s32 c) {return su_cs_is_graph(c);}
   static boole is_lower(s32 c) {return su_cs_is_lower(c);}
   static boole is_print(s32 c) {return su_cs_is_print(c);}
   static boole is_punct(s32 c) {return su_cs_is_punct(c);}
   static boole is_space(s32 c) {return su_cs_is_space(c);}
   static boole is_upper(s32 c) {return su_cs_is_upper(c);}
   static boole is_xdigit(s32 c) {return su_cs_is_xdigit(c);}

   static sz casecmp(char const *cp1, char const *cp2){
      return su_cs_casecmp(cp1, cp2);
   }
   static sz casecmp(char const *cp1, char const *cp2, uz n){
      return su_cs_casecmp_n(cp1, cp2, n);
   }

   static uz casehash(char const *buf, uz len){
      return su_cs_casehash_cbuf(buf, len);
   }
   static uz casehash(char const *cp) {return su_cs_casehash(cp);}

   static sz cmp(char const *cp1, char const *cp2){
      return su_cs_cmp(cp1, cp2);
   }
   static sz cmp(char const *cp1, char const *cp2, uz n){
      return su_cs_cmp_n(cp1, cp2);
   }

   static char *copy(char *dst, char const *src, uz n){
      return su_cs_copy_n(dst, src, n);
   }

   static char *dup(char const *cp) {return su_cs_dup(cp);}
   static char *dup(char const *buf, uz len) {return su_cs_dup_cbuf(buf, len);}

   static char *find(char const *cp, s32 x) {return su_cs_find_c(cp, x);}
   static char *find(char const *cp, char const *x) {return su_cs_find(cp, x);}

   static uz hash(char const *buf, uz len) {return su_cs_hash_cbuf(buf, len);}
   static uz hash(char const *cp) {return su_cs_hash(cp);}

   static uz len(char const *cp) {return su_cs_len(cp);}

   static char *pcopy(char *dst, char const *src){
      return su_cs_pcopy(dst, src);
   }
   static char *pcopy(char *dst, char const *src, uz n){
      return su_cs_pcopy(dst, src, n);
   }

   static char *rfind(char const *cp, s32 x) {return su_cs_rfind_c(cp, x);}

   static char *sep(char **iolist, char sep, boole ignore_empty){
      return su_cs_sep_c(iolist, sep, ignore_empty);
   }

   static s32 to_lower(s32 c) {return su_cs_to_lower(c);}
   static s32 to_upper(s32 c) {return su_cs_to_upper(c);}
};

template<>
class auto_type_toolbox<char*>{
public:
   static type_toolbox<char*> const *get_instance(void){
      return cs::type_toolbox;
   }
};

template<>
class auto_type_toolbox<char const*>{
public:
   static type_toolbox<char const*> const *get_instance(void){
      return cs::const_type_toolbox;
   }
};

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_CS_H */
/* s-it-mode */
