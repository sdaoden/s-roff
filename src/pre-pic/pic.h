/*@
 * Copyright (c) 2014 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
 *
 * Copyright (C) 1989 - 1992, 2000, 2001, 2003, 2005
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
#ifndef _PIC_H
#define _PIC_H

#include "config.h"
#include "pic-config.h"

#include <errno.h>
#include <math.h>
#include <stdlib.h>

#include "assert.h"
#include "cset.h"
#include "errarg.h"
#include "error.h"
#include "file_case.h"
#include "lib.h"
#include "output.h"
#include "position.h"
#include "stringclass.h"
#include "text.h"

#ifndef M_SQRT2 // FIXME lib.h
#define M_SQRT2	1.41421356237309504880
#endif

#ifndef M_PI // FIXME lib.h
#define M_PI 3.14159265358979323846
#endif

class input
{
  input *next;

public:
  input();
  virtual ~input();
  virtual int get() = 0;
  virtual int peek() = 0;
  virtual int get_location(const char **, int *);
  friend class input_stack;
  friend class copy_rest_thru_input;
};

class file_input
: public input
{
  file_case *_fcp;
  const char *filename;
  int lineno;
  string line;
  const char *ptr;
  int read_line();

public:
  file_input(file_case *, const char *);
  ~file_input();
  int get();
  int peek();
  int get_location(const char **, int *);
};

void lex_init(input *);
int get_location(char **, int *);

void do_copy(const char *file);
void parse_init();
void parse_cleanup();

void lex_error(const char *message,
	       const errarg &arg1 = empty_errarg,
	       const errarg &arg2 = empty_errarg,
	       const errarg &arg3 = empty_errarg);

void lex_warning(const char *message,
		 const errarg &arg1 = empty_errarg,
		 const errarg &arg2 = empty_errarg,
		 const errarg &arg3 = empty_errarg);

void lex_cleanup();

extern int flyback_flag;
extern int command_char;
// zero_length_line_flag is non-zero if zero-length lines are drawn
// as dots by the output device
extern int zero_length_line_flag;
extern int driver_extension_flag;
extern int compatible_flag;
extern int safer_flag;
extern char *graphname;

#endif // _PIC_H
// s-it2-mode
