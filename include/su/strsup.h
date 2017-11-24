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
#ifndef su_STRSUP_H
#define su_STRSUP_H

#include <su/primary.h>

#include <string.h> /* TODO drop that if possible */

#include <su/code-in.h>
C_DECL_BEGIN

enum su_cclass{
   su_cclass_none,
   su_cclass_alnum = 1u<<0,
   su_cclass_alpha = 1u<<1,
   su_cclass_cntrl = 1u<<2,
   su_cclass_digit = 1u<<3,
   su_cclass_graph = 1u<<4,
   su_cclass_lower = 1u<<5,
   su_cclass_print = 1u<<6,
   su_cclass_punct = 1u<<7,
   su_cclass_space = 1u<<8,
   su_cclass_upper = 1u<<9,
   su_cclass_xdigit = 1u<<10,

   su__cclass_maxshift = 11u,
   su__cclass_mask = (1u<<su__cclass_maxshift) - 1
};

#define su_strcat strcat
#define su_strcpy strcpy
#define su_strchr strchr
#define su_strcmp strcmp
#define su_strlen strlen
#define su_strncat strncat
#define su_strncmp strncmp
#define su_strncpy strncpy
#define su_strrchr strrchr
#define su_strstr strstr
#define su_strtok strtok

/* Return string describing C error number eno */
c_decl char const *su_err_doc(si32 eno);

/* Character classification FIXME su_library_init()+inline */
#define su_isalnum(C) su__isclass(C, su_cclass_alnum)
#define su_isalpha(C) su__isclass(C, su_cclass_alpha)
#define su_iscntrl(C) su__isclass(C, su_cclass_cntrl)
#define su_isdigit(C) su__isclass(C, su_cclass_digit)
#define su_isgraph(C) su__isclass(C, su_cclass_graph)
#define su_islower(C) su__isclass(C, su_cclass_lower)
#define su_isprint(C) su__isclass(C, su_cclass_print)
#define su_ispunct(C) su__isclass(C, su_cclass_punct)
#define su_isspace(C) su__isclass(C, su_cclass_space)
#define su_isupper(C) su__isclass(C, su_cclass_upper)
#define su_isxdigit(C) su__isclass(C, su_cclass_xdigit)
c_decl boole su__isclass(si32 c, enum su_cclass cclass);

/* Copy src to dst, return pointer to NUL in dst */
c_decl char *su_stpcpy(char *dst, char const *src);

/* String comparison, byte-based, case-insensitive */
c_decl si32 su_strcasecmp(char const *cp1, char const *cp2);
c_decl si32 su_strncasecmp(char const *cp1, char const *cp2, uiz n);

/* Duplicate string into su_talloc()ated duplicate.
 * Return dup or NIL if input was NIL */
c_decl char *su_strdup(char const *cp);

/* Map to lower/upper or return unchanged */
#define su_tolower(C) su__tolower(C) /* FIXME su_library_init()+inline */
#define su_toupper(C) su__toupper(C) /* FIXME su_library_init()+inline */
c_decl si32 su__tolower(si32 c);
c_decl si32 su__toupper(si32 c);

C_DECL_END
#include <su/code-ou.h>
#endif /* su_STRSUP_H */
/* s-it-mode */
