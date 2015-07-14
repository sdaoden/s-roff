/*@ Defines the class and methods implemented within pushback.cpp.
 *
 * Copyright (c) 2014 - 2015 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
 *
 * Copyright (C) 2000 - 2004 Free Software Foundation, Inc.
 *      Written by Gaius Mulley (gaius@glam.ac.uk).
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
#ifndef _PUSHBACK_H
#define _PUSHBACK_H

#include "config.h"
#include "html-config.h"

#define eof         (char)-1

class pushBackBuffer
{
  char  *charStack;
  int   stackPtr;   // index to push back stack
  int   debug;
  int   verbose;
  int   eofFound;
  char  *fileName;
  int   lineNo;
  int   stdIn;

public:
  pushBackBuffer(char *);
  ~pushBackBuffer(void);

  char    getPB(void);
  char    putPB(char ch);
  void    skipUntilToken(void);
  void    skipToNewline(void);
  double  readNumber(void);
  int     readInt(void);
  char *  readString(void);
  int     isString(char const *string);
};

#endif // _PUSHBACK_H
// s-it2-mode
