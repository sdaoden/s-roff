/*@
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2004 Free Software Foundation, Inc.
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

#include "config.h"
#include "lib.h"

#include "su/cs.h"

#include "stringclass.h"

int interpret_lf_args(const char *p)
{
  while (*p == ' ')
    p++;
  if (!su_cs_is_digit(*p))
    return 0;
  int ln = 0;
  do {
    ln *= 10; /* TODO inline atoi */
    ln += *p++ - '0';
  } while (su_cs_is_digit(*p));
  if (*p != ' ' && *p != '\n' && *p != '\0')
    return 0;
  while (*p == ' ')
    p++;
  if (*p == '\0' || *p == '\n')  {
    rf_current_lineno_set(ln);
    return 1;
  }
  const char *q;
  for (q = p;
       *q != '\0' && *q != ' ' && *q != '\n' && *q != '\\';
       q++)
    ;
  string tem(p, q - p); /* FIXME false usage (of new cstring) */
  while (*q == ' ')
    q++;
  if (*q != '\n' && *q != '\0')
    return 0;
  tem += '\0';
  rf_current_filename_set(tem.contents());
  change_lineno(ln);
  return 1;
}

// s-it2-mode
