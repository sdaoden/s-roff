/*@ Pivotal point of massive basic header injection etc., for ROFF.
 *@ TODO On the long run this should vanish in favour if explicit includes.
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 2003, 2005, 2006 Free Software Foundation, Inc.
 *    Written by James Clark (jjc@jclark.com)
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
#ifndef rf_LIB_H
#define rf_LIB_H

#include <config.h>

#include <su/primary.h>
#include <su/memory.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define __GETOPT_PREFIX groff_ /* FIXME drop this mess! */
#include <getopt.h> /* FIXME use that of sfsys! */

#include <su/code-in.h>

/* We simply use su facilities in r(of)f headers, so globally ensure the su
 * namepace is directly available */
#if !C_LANG
NSPC_BEGIN(rf)
NSPC_USE(su)
NSPC_END(rf)
#endif

char *strsave(const char *s);
int is_prime(unsigned);
double groff_hypot(double, double);

#ifndef HAVE_STRERROR
C_DECL_BEGIN
   char *strerror(int);
C_DECL_END
#endif

C_DECL_BEGIN
   const char *i_to_a(int);
   const char *ui_to_a(unsigned int);
   const char *if_to_a(int, int);
C_DECL_END

C_DECL_BEGIN
   /* The last six characters of template must be "XXXXXX";
    * they are replaced with a string that makes the filename unique.
    * Then open the file (or directory) and return a fd */
   c_decl int rf_mkstemp(char *template, boole wantdir);
C_DECL_END

FILE *xtmpfile(char **namep = 0,
             const char *postfix_long = 0, const char *postfix_short = 0,
             int do_unlink = 1);

#ifdef NEED_DECLARATION_POPEN
extern "C" { FILE *popen(const char *, const char *); }
#endif /* NEED_DECLARATION_POPEN */

#ifdef NEED_DECLARATION_PCLOSE
extern "C" { int pclose (FILE *); }
#endif /* NEED_DECLARATION_PCLOSE */

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

/* Maximum number of digits in the decimal representation of an int
    (not including the -). */

#define INT_DIGITS 10 /* TODO */

#ifdef PI
#undef PI /* FIXME */
#endif
const double PI = 3.14159265358979323846; /* FIXME */

#include <su/code-ou.h>
#endif /* rf_LIB_H */
/* s-it-mode */
