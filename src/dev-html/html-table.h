/*@ Provides the methods necessary to handle indentation and tab
 *@ positions using html tables.
 *
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 2002 - 2005, 2007 Free Software Foundation, Inc.
 * Written by Gaius Mulley (gaius@glam.ac.uk)
 */
/*
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
#ifndef _HTML_TABLE_H
#define _HTML_TABLE_H

#include "html.h"

typedef struct tab_position {
  char alignment;
  int  position;
  struct tab_position *next;
} tab_position;

class tabs
{
  void  delete_list (void);
  tab_position *tab;

public:
         tabs         ();
        ~tabs         ();
  void  clear         (void);
  int   compatible    (const char *s);
  void  init          (const char *s);
  void  check_init    (const char *s);
  int   find_tab      (int pos);
  int   get_tab_pos   (int n);
  char  get_tab_align (int n);
  void  dump_tabs     (void);
};

/*
 *  define a column
 */

typedef struct cols {
  int          left, right;
  int          no;
  char         alignment;
  struct cols *next;
} cols;

class html_table
{
  cols          *columns;      /* column entries */
  int            linelength;
  cols          *last_col;     /* last column started */
  int            start_space;  /* have we seen a `.sp' tag? */

  void  remove_cols (cols *c);

public:
      html_table          (simple_output *op, int linelen);
     ~html_table          (void);
  int   add_column        (int coln, int hstart, int hend, char align);
  cols *get_column        (int coln);
  int   insert_column     (int coln, int hstart, int hend, char align);
  int   modify_column     (cols *c, int hstart, int hend, char align);
  int   find_tab_column   (int pos);
  int   find_column       (int pos);
  int   get_tab_pos       (int n);
  char  get_tab_align     (int n);
  void  set_linelength    (int linelen);
  int   no_columns        (void);
  int   no_gaps           (void);
  int   is_gap            (cols *c);
  void  dump_table        (void);
  void  emit_table_header (int space);
  void  emit_col          (int n);
  void  emit_new_row      (void);
  void  emit_finish_table (void);
  int   get_right         (cols *c);
  void  add_indent        (int indent);
  void  finish_row        (void);
  int   get_effective_linelength (void);
  void  set_space         (int space);
  void  emit_colspan      (void);
  void  emit_td           (int percentage, const char *s = ">");

  tabs          *tab_stops;    /* tab stop positions */
  simple_output *out;
};

/*
 *  the indentation wrapper.
 *  Builds an indentation from a html-table.
 *  This table is only emitted if the paragraph is emitted.
 */

class html_indent
{
  void end    (void);
  int         is_used;
  int         pg;        // values of the registers as passed via initialization
  int         ll;
  int         in;
  html_table *table;

public:
  html_indent  (simple_output *op, int ind, int pageoffset, int linelength);
  ~html_indent (void);
  void begin   (int space);   // called if we need to use the indent
  void get_reg (int *ind, int *pageoffset, int *linelength);

  // the indent is shutdown when it is deleted
};

#endif // _HTML_TABLE_H
// s-it2-mode
