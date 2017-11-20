/*@
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992 Free Software Foundation, Inc.
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
#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include "config.h"
#include "troff-config.h"

// there is no distinction between name with no value and name with NULL value
// null names are not permitted (they will be ignored).

class association
{
public:
  symbol s;
  void *v;

  association() :  v(0) {}
};

class dictionary;

class dictionary_iterator
{
  dictionary *dict;
  int i;

public:
  dictionary_iterator(dictionary &);
  int get(symbol *, void **);
};

class dictionary
{
  friend class dictionary_iterator;

  int size;
  int used;
  double threshold;
  double factor;
  association *table;

  void rehash(int);

public:
  dictionary(int);
  void *lookup(symbol s, void *v=0); // returns value associated with key
  void *lookup(const char *);
  // if second parameter not NULL, value will be replaced
  void *remove(symbol);
};

class object
{
  int rcount;

public:
  object();
  virtual ~object();
  void add_reference();
  void remove_reference();
};

class object_dictionary;

class object_dictionary_iterator
{
  dictionary_iterator di;

public:
  object_dictionary_iterator(object_dictionary &);
  int get(symbol *, object **);
};

class object_dictionary
{
  dictionary d;

public:
  object_dictionary(int);
  object *lookup(symbol nm);
  void define(symbol nm, object *obj);
  void rename(symbol oldnm, symbol newnm);
  void remove(symbol nm);
  int alias(symbol newnm, symbol oldnm);
  friend class object_dictionary_iterator;
};

inline int object_dictionary_iterator::get(symbol *sp, object **op)
{
  return di.get(sp, (void **)op);
}
#endif // _DICTIONARY_H
// s-it2-mode
