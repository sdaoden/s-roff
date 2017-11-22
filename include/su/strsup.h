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

/* Copy src to dst, return pointer to NUL in dst */
c_decl char *su_stpcpy(char *dst, char const *src);

/* String comparison, byte-based, case-insensitive */
c_decl si32 su_strcasecmp(char const *cp1, char const *cp2);
c_decl si32 su_strncasecmp(char const *cp1, char const *cp2, uiz n);

/* Duplicate string into su_talloc()ated duplicate.
 * Return dup or NIL if input was NIL */
c_decl char *su_strdup(char const *cp);

C_DECL_END
#include <su/code-ou.h>
#endif /* su_STRSUP_H */
/* s-it-mode */
