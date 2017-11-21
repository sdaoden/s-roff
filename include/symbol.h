/*@
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2002, 2004
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
#ifndef _SYMBOL_H
#define _SYMBOL_H

#include "config.h"

#define DONT_STORE 1
#define MUST_ALREADY_EXIST 2

class symbol
{
  static const char **table;
  static int table_used;
  static int table_size;
  static char *block;
  static int block_size;
  static symbol *s_specials[3];

  const char *s;

public:
  symbol(void)
      : s(NULL){
  }

  symbol(const char *p, int how = 0);

  symbol(symbol const &t)
      : s(t.s){
  }

  // TODO DTOR (refcnt etc.????)

  symbol &assign(symbol const &t){
    if(this != &t)
      s = t.s;
    return *this;
  }

  symbol &operator=(symbol const &t){
    return assign(t);
  }

  rf_uip hash(void) const{
    return rf_R(rf_uip,s);
  }

  int operator==(symbol const &sym) const{ // TODO need s32 cmp()!
    return (s == sym.s);
  }

  int operator!=(symbol const &sym) const{
    return (s != sym.s);
  }

  char const *contents(void) const{
    return s;
  }

  rf_bool is_null(void) const{
    return (s == NULL);
  }

  rf_bool is_empty(void) const{
    return (s != NULL && *s == '\0';
  }

  static symbol const &get_default(void){
      return (s_specials[0] == NULL) ? s_specials_create(0) : *s_specials[0];
  }
  static symbol const &get_null(void){
      return (s_specials[1] == NULL) ? s_specials_create(1) : *s_specials[1];
  }
  static symbol const &get_empty(void){
      return (s_specials[2] == NULL) ? s_specials_create(2) : *s_specials[2];
  }

private:
   static symbol const &s_specials_create(rf_ui32 i);
};

symbol concat(symbol const &, symbol const &); // TODO make static fun!

#endif // _SYMBOL_H
// s-it2-mode
