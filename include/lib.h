/*@
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 2003, 2005, 2006
 *    Free Software Foundation, Inc.
 *      Written by James Clark (jjc@jclark.com)
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with groff; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin St - Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef _LIB_H
#define _LIB_H

#include "config.h"

extern "C" {
#ifndef HAVE_STRERROR
  char *strerror(int);
#endif
  const char *i_to_a(int);
  const char *ui_to_a(unsigned int);
  const char *if_to_a(int, int);
}

#define __GETOPT_PREFIX groff_
#include <getopt.h>

#ifdef HAVE_SETLOCALE
#include <locale.h>
#define getlocale(category) setlocale(category, NULL)
#else /* !HAVE_SETLOCALE */
#define LC_ALL 0
#define LC_CTYPE 0
#define setlocale(category, locale) (void)(category, locale)
#define getlocale(category) ((void)(category), (char *)"C")
#endif /* !HAVE_SETLOCALE */

char *strsave(const char *s);
int is_prime(unsigned);
double groff_hypot(double, double);

#include <stdio.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <stdarg.h>

/* HP-UX 10.20 and LynxOS 4.0.0 don't declare snprintf() */
#if !defined(HAVE_SNPRINTF) || defined(NEED_DECLARATION_SNPRINTF)
extern "C" { int snprintf(char *, size_t, const char *, /*args*/ ...); }
#endif

/* LynxOS 4.0.0 has snprintf() but no vsnprintf() */
#if !defined(HAVE_VSNPRINTF) || defined(NEED_DECLARATION_VSNPRINTF)
extern "C" { int vsnprintf(char *, size_t, const char *, va_list); }
#endif

/* LynxOS 4.0.0 doesn't declare vfprintf() */
#ifdef NEED_DECLARATION_VFPRINTF
extern "C" { int vfprintf(FILE *, const char *, va_list); }
#endif

#ifndef HAVE_MKSTEMP
/* since mkstemp() is defined as a real C++ function if taken from
   groff's mkstemp.cpp we need a declaration */
int mkstemp(char *tmpl);
#endif /* HAVE_MKSTEMP */

int mksdir(char *tmpl);

FILE *xtmpfile(char **namep = 0,
	       const char *postfix_long = 0, const char *postfix_short = 0,
	       int do_unlink = 1);
char *xtmptemplate(const char *postfix_long, const char *postfix_short);

#ifdef NEED_DECLARATION_POPEN
extern "C" { FILE *popen(const char *, const char *); }
#endif /* NEED_DECLARATION_POPEN */

#ifdef NEED_DECLARATION_PCLOSE
extern "C" { int pclose (FILE *); }
#endif /* NEED_DECLARATION_PCLOSE */

size_t file_name_max(const char *fname);
size_t path_name_max();

int interpret_lf_args(const char *p);

extern char invalid_char_table[];

inline int invalid_input_char(int c)
{
  return c >= 0 && invalid_char_table[c];
}

#ifdef HAVE_STRCASECMP
#ifdef NEED_DECLARATION_STRCASECMP
// Ultrix4.3's string.h fails to declare this.
extern "C" { int strcasecmp(const char *, const char *); }
#endif /* NEED_DECLARATION_STRCASECMP */
#else /* !HAVE_STRCASECMP */
extern "C" { int strcasecmp(const char *, const char *); }
#endif /* HAVE_STRCASECMP */

#if !defined(_AIX) && !defined(sinix) && !defined(__sinix__)
#ifdef HAVE_STRNCASECMP
#ifdef NEED_DECLARATION_STRNCASECMP
// SunOS's string.h fails to declare this.
extern "C" { int strncasecmp(const char *, const char *, int); }
#endif /* NEED_DECLARATION_STRNCASECMP */
#else /* !HAVE_STRNCASECMP */
extern "C" { int strncasecmp(const char *, const char *, size_t); }
#endif /* HAVE_STRNCASECMP */
#endif /* !_AIX && !sinix && !__sinix__ */

#ifdef HAVE_CC_LIMITS_H
#include <limits.h>
#else /* !HAVE_CC_LIMITS_H */
#define INT_MAX 2147483647
#endif /* !HAVE_CC_LIMITS_H */

/* It's not safe to rely on people getting INT_MIN right (ie signed). */

#ifdef INT_MIN
#undef INT_MIN
#endif

#ifdef CFRONT_ANSI_BUG

/* This works around a bug in cfront 2.0 used with ANSI C compilers. */

#define INT_MIN ((long)(-INT_MAX-1))

#else /* !CFRONT_ANSI_BUG */

#define INT_MIN (-INT_MAX-1)

#endif /* !CFRONT_ANSI_BUG */

/* Maximum number of digits in the decimal representation of an int
   (not including the -). */

#define INT_DIGITS 10

#ifdef PI
#undef PI
#endif

const double PI = 3.14159265358979323846;

/* ad_delete deletes an array of objects with destructors;
   a_delete deletes an array of objects without destructors */

#ifdef ARRAY_DELETE_NEEDS_SIZE
/* for 2.0 systems */
#define ad_delete(size) delete [size]
#define a_delete delete
#else /* !ARRAY_DELETE_NEEDS_SIZE */
/* for ARM systems */
#define ad_delete(size) delete []
#define a_delete delete []
#endif /* !ARRAY_DELETE_NEEDS_SIZE */

#ifdef HAVE_CC_INTTYPES_H
# include <inttypes.h>
#endif
#if !defined UINT8_MAX && !defined uint8_t
# undef int8_t
typedef unsigned char     uint8_t;
typedef signed char       int8_t;
#endif
#if !defined UINT32_MAX && !defined uint32_t
# undef int32_t
# if INT_MAX == 2147483647
typedef unsigned int      uint32_t;
typedef signed int        int32_t;
# else
typedef unsigned long int uint32_t;
typedef signed long int   int32_t;
# endif
#endif

#ifndef NELEM
# define NELEM(X)             (sizeof(X) / sizeof((X)[0]))
#endif

#define CLASS_DISABLE_COPY(C) private: C(C const &); C &operator=(C const &)

#if 1
# define STRING(X)      #X
# define XSTRING(X)     STRING(X)
# define CONCAT(S1,S2)  _CONCAT(S1, S2)
# define _CONCAT(S1,S2) S1 ## S2
#else
# define STRING(X)      "X"
# define XSTRING        STRING
# define CONCAT(S1,S2)  S1/**/S2
#endif

/* Compile-Time-Assert */
#define CTA(TEST)       _CTA_1(TEST, __LINE__)
#define _CTA_1(TEST,L)  _CTA_2(TEST, L)
#define _CTA_2(TEST,L)  \
   typedef char COMPILE_TIME_ASSERT_failed_at_line_ ## L[(TEST) ? 1 : -1]

#undef ISPOW2
#define ISPOW2(X)       ((((X) - 1) & (X)) == 0)
#undef MIN
#define MIN(A, B)       ((A) < (B) ? (A) : (B))
#undef MAX
#define MAX(A, B)       ((A) < (B) ? (B) : (A))
#undef ABS
#define ABS(A)          ((A) < 0 ? -(A) : (A))

#undef DBG
#undef NDBG
#ifdef NDEBUG
# define DBG(X)
# define NDBG(X)        X
#else
# define DBG(X)         X
# define NDBG(X)
#endif

/* Translation */
#undef _
#undef N_
#undef V_
#define _(S)            (S)
#define N_(S)           (S)
#define V_(S)           (S)

#endif // _LIB_H
// s-it2-mode
