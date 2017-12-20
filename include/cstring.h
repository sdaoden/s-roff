/*@ FIXME rename cstring.h, class cstring;  we assume RF memory allocs!
 *
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
#ifndef _STRINGCLASS_H
#define _STRINGCLASS_H

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// Ensure that the first declaration of functions that are later
// declared as inline declares them as inline.

class string;

inline string operator+(const string &, const string &);
inline string operator+(const string &, const char *);
inline string operator+(const char *, const string &);
inline string operator+(const string &, char);
inline string operator+(char, const string &);
inline int operator==(const string &, const string &);
inline int operator!=(const string &, const string &);

class string
{
  char *ptr;
  int len;
  int sz;

  string(const char *, int, const char *, int);	// for use by operator+
  string &grow1(void);

public:
  string();
  string(const string &);
  string(const char *);
  string(const char *, int);
  string(char);

  ~string();

  string &operator=(const string &);
  string &assign(string const &t) {return (*this = t);}
  string &operator=(const char *);
  string &assign(char const *cp) {return (*this = cp);}
  string &operator=(char);
  string &assign(char c) {return (*this = c);}

  string &operator+=(const string &);
  string &append(string const &t) {return (*this += t);}
  cstring &push(cstring const &t) {return (*this += t);}
  string &operator+=(const char *);
  string &append(char const *cp) {return (*this += cp);}
  cstring &push(char const *cp) {return (*this += cp);}
  string &operator+=(char);
  string &append(char c) {return (*this += c);}
  cstring &push(char c) {return (*this += c);}
  string &append(const char *, int);

  int length() const;
len
  int empty() const;
  rf_bool is_empty(void) const {return len == 0;}
  int operator*() const;

  string substring(int i, int n) const;
substr

   char *data(void) {return m_ca.ca_data;}
   char const *data(void) const {return m_ca.ca_data;}

   char at(uiz off) {ASSERT(off < count()); return data()[off];}
   char const at(uiz off) const {ASSERT(off < count()); return data()[off];}
   char &ref_at(uiz off) {ASSERT(off < count()); return data()[off];}

   char &operator[](uiz off) {return ref_at(off);}
   char operator[](uiz off) const {return at(off);}

  string &set_length(int i);
drop vis S+R -> this is trunc()!
  char *data(void){
    return ptr;
  }
  // always valid terminated pointer
  char const *cp(void) const /*const TODO mutable */;
  char *cpdata(void);

  const char *contents() const;//FIXME remove
  int search(char) const;
  // Newly alloc copy without NULs TODO new[] yet!
  // FIXME extract-> // to_cp_dup()
  // FIXME with enum WHAT_TO_INCLUDE and/or to_cp_dup_strip_nuls()
  char *extract() const;

  char *release_data(rf_ui32 *lenp=NULL, rf_ui32 *sizep=NULL){
    if(lenp != NULL)
      *lenp = len;
    if(sizep != NULL)
      *sizep = sz;
    char *rv = ptr;
    if(rv != NULL) // XXX ensure termination
      cp();
    ptr = NULL;
    len = sz = 0;
    return rv;
  }

  string &remove_spaces();
drop -> trim(), squeeze()

  string &clear(void){ /* TODO should release mem -> truncate()! */
    len = 0;
    return *this;
  }
  string &trunc(rf_uiz newlen=0){
    rf_ASSERT(newlen <= rf_S(rf_ui32,length())); /* TODO rf_uiz/32 */
    len = newlen;
    return *this;
  }
  string &move(string &);
  string &reserve(rf_uiz i, rf_bool setlen=TRU1){
->book, book_at()
    int xlen = len; /* TODO int -> rf_uiz/32 */
    set_length(i + len);
    if(!setlen)
      len = xlen;
    return *this;
  }


//FIXME cmp, casecmp() with string, buf and char!  
//FIXME jsut use kind of my own C++ string and incorporate what is needed else


  friend string operator+(const string &, const string &);
  friend string operator+(const string &, const char *);
  friend string operator+(const char *, const string &);
  friend string operator+(const string &, char);
  friend string operator+(char, const string &);

  friend int operator==(const string &, const string &);
  friend int operator!=(const string &, const string &);
  friend int operator<=(const string &, const string &);
  friend int operator<(const string &, const string &);
  friend int operator>=(const string &, const string &);
  friend int operator>(const string &, const string &);
};

inline char &string::operator[](int i)
{
  assert(i >= 0 && i < len);
  return ptr[i];
}

inline char string::operator[](int i) const
{
  assert(i >= 0 && i < len);
  return ptr[i];
}

inline int string::length() const
{
  return len;
}

inline int string::empty() const
{
  return len == 0;
}

inline int string::operator*() const
{
  return len;
}

inline const char *string::contents() const
{
  return ptr;
}

inline string operator+(const string &s1, const string &s2)
{
  return string(s1.ptr, s1.len, s2.ptr, s2.len);
}

inline string operator+(const string &s1, const char *s2)
{
  return (s2 == NULL) ? s1 : string(s1.ptr, s1.len, s2, strlen(s2));
}

inline string operator+(const char *s1, const string &s2)
{
  return (s1 == NULL) ? s2 : string(s1, strlen(s1), s2.ptr, s2.len);
}

inline string operator+(const string &s, char c)
{
  return string(s.ptr, s.len, &c, 1);
}

inline string operator+(char c, const string &s)
{
  return string(&c, 1, s.ptr, s.len);
}

inline int operator==(const string &s1, const string &s2)
{
  return (s1.len == s2.len
	  && (s1.len == 0 || memcmp(s1.ptr, s2.ptr, s1.len) == 0));
}

inline int operator!=(const string &s1, const string &s2)
{
  return (s1.len != s2.len
	  || (s1.len != 0 && memcmp(s1.ptr, s2.ptr, s1.len) != 0));
}

inline string string::substring(int i, int n) const
{
  assert(i >= 0 && i + n <= len);
  return string(ptr + i, n);
}

inline string &string::operator+=(char c)
{
  if (len >= sz)
    grow1();
  ptr[len++] = c;
  return *this;
}

void put_string(const string &, FILE *);

string as_string(int);

#endif // _STRINGCLASS_H
// s-it2-mode
