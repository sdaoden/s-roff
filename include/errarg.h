/*@
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2002 Free Software Foundation, Inc.
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
#ifndef _ERRARG_H
#define _ERRARG_H

class errarg
{
  enum { EMPTY, STRING, CHAR, INTEGER, UNSIGNED_INTEGER, DOUBLE } type;
  union {
    const char *s;
    int n;
    unsigned int u;
    char c;
    double d;
  };

public:
  errarg();
  errarg(const char *);
  errarg(char);
  errarg(unsigned char);
  errarg(int);
  errarg(unsigned int);
  errarg(double);
  int empty() const;
  void print() const;
};

extern errarg empty_errarg;

extern void errprint(const char *,
		     const errarg &arg1 = empty_errarg,
		     const errarg &arg2 = empty_errarg,
		     const errarg &arg3 = empty_errarg);

#endif // _ERRARG_H
// s-it2-mode
