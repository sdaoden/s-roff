/*@ Arrays for POD, structures and * to these kind, or unmanaged objects.
 *@ TODO C++ type can have a view (we have iter-loop use cases yet!)
 *@ TODO doc
 *
 * Copyright (c) 2004 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef su_ARRAY_H
#define su_ARRAY_H

#include <su/code.h>

#define su_HEADER
#include <su/code-in.h>
C_DECL_BEGIN

struct su_array;
struct su_parray;

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

// With specialization for pointer-based T
template<class T> class array;

NSPC_END(su)
#endif
C_DECL_BEGIN

/* Very simple C non-pointer plain-old-data or struct container.
 * Mostly a foundation for array<T> indeed, as it does not know anything about
 * what it stores */
struct su_array{
   void *a_data;
   u32 a_count;
   u32 a_size;
   u32 a_tsizeof;
   boole a_errpass;  /* To be set manually after default constructor */
   u8 a__pad[3];
};

/* tsizeof: asserted not to be 0
 * t: t->a_errpass is inherited
 * setcount: whether zeroed memory shall be produced and .a_count'ed */
EXPORT struct su_array *su_array_create(struct su_array *self, u32 tsizeof);
EXPORT SHADOW struct su_array *su_array_create_copy(struct su_array *self,
      struct su_array const *t);
EXPORT SHADOW struct su_array *su_array_create_sized(struct su_array *self,
      u32 tsizeof, uz init_slots, boole setcount);
EXPORT void su_array_gut(struct su_array *self);

/* Does not assign t->a_errpass! */
EXPORT s32 su_array_assign(struct su_array *self, struct su_array const *t);

EXPORT struct su_array *su_array_clear(struct su_array *self);

/* Cheap (only swaps fields) */
EXPORT struct su_array *su_array_swap(struct su_array *self,
      struct su_array *t);

/* Reserve additional room at the end.
 * noof: number of slots to book: honoured _exactly_!
 *    Use book_at() to have a growing algorithm be applied instead
 * setcount: if set new slots become initialized to 0 and are .a_count'ed */
EXPORT s32 su_array_book(struct su_array *self, uz noof, boole setcount);

/* Reserve additional room at offset.
 * off: where to book; must be LE .a_count (0 is always valid)
 * noof: number to book; new members are zeroed and .a_count'ed.
 *    Builtin growing algorithm is used to minimize further growing */
EXPORT s32 su_array_book_at(struct su_array *self, uz off, uz noof);

EXPORT s32 su_array_prepare_unshift(struct su_array *self);
EXPORT s32 su_array_insert(struct su_array *self, uz off);

EXPORT struct su_array *su_array_finish_shift(struct su_array *self);
EXPORT struct su_array *su_array_finish_remove(struct su_array *self, uz off);

/* off: must be LE .a_count (0 is always valid)
 * cnt: can be any value */
EXPORT struct su_array *su_array_remove_range(struct su_array *self, uz off,
      uz cnt);

EXPORT struct su_array *su_array_take_data(struct su_array *self,
      void *data, uz cnt_a_size);

/* Very simple container for storing pointers.
 * Mostly a foundation for array<T*> indeed, as it does not know anything
 * about what it stores */
struct su_parray{
   void **pa_data;
   u32 pa_count;
   /* Effectively 31-bit limit so we can store "errpass" in high bit */
   u32 pa__size_ep;
};

#define su_PARRAY_SIZE(PAP) ((PAP)->pa__size_ep & su_S(su_u32,su_S32_MAX))
#define su_PARRAY_SET_SIZE(PAP,SZ) \
do (PAP)->pa__size_ep = ((PAP)->pa__size_ep & su_S(su_u32,su_S32_MIN)) |\
      ((SZ) /* overflow check by user! & su_S(su_u32,su_S32_MAX)*/);\
while(0)
#define su_PARRAY_IS_ERRPASS(PAP) \
   (((PAP)->pa__size_ep & su_S(su_u32,su_S32_MIN)) != 0)
#define su_PARRAY_SET_ERRPASS(PAP,BOOL) \
do (PAP)->pa__size_ep = ((PAP)->pa__size_ep & su_S(su_u32,su_S32_MAX)) |\
      ((BOOL) ? su_S(su_u32,su_S32_MIN) : 0);\
while(0)

/* t: t->a_errpass is inherited
 * setcount: whether zeroed memory shall be produced and .a_count'ed */
EXPORT struct su_parray *su_parray_create(struct su_parray *self);
EXPORT SHADOW struct su_parray *su_parray_create_copy(struct su_parray *self,
      struct su_parray const *t);
EXPORT SHADOW struct su_parray *su_parray_create_sized(struct su_parray *self,
      uz init_slots, boole setcount);
EXPORT void su_parray_gut(struct su_parray *self);

/* Does not assign t->a_errpass! */
EXPORT s32 su_parray_assign(struct su_parray *self, struct su_parray const *t);

EXPORT struct su_parray *su_parray_clear(struct su_parray *self);

/* Cheap (only swaps fields) */
EXPORT struct su_parray *su_parray_swap(struct su_parray *self,
      struct su_parray *t);

/* Reserve additional room at the end.
 * noof: number of slots to book: honoured _exactly_!
 *    Use book_at() to have a growing algorithm be applied instead
 * setcount: if set new slots become initialized to 0 and are .a_count'ed */
EXPORT s32 su_parray_book(struct su_parray *self, uz noof, boole setcount);

/* Reserve additional room at offset.
 * off: where to book; must be LE .a_count (0 is always valid)
 * noof: number to book; new members are zeroed and .a_count'ed.
 *    Builtin growing algorithm is used to minimize further growing */
EXPORT s32 su_parray_book_at(struct su_parray *self, uz off, uz noof);

EXPORT s32 su_parray_prepare_unshift(struct su_parray *self);
EXPORT s32 su_parray_insert(struct su_parray *self, uz off);

EXPORT struct su_parray *su_parray_finish_shift(struct su_parray *self);
EXPORT struct su_parray *su_parray_finish_remove(struct su_parray *self,
      uz off);

/* off: must be LE .a_count (0 is always valid)
 * cnt: can be any value */
EXPORT struct su_parray *su_parray_remove_range(struct su_parray *self, uz off,
      uz cnt);

/* Assert that cnt_a_size LE SZ_MAX */
EXPORT struct su_parray *su_parray_take_data(struct su_parray *self,
      void **data, uz cnt_a_size);

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

template<class T>
class array{
   su_array m_a;
public:
   array(void) : m_a() {su_array_create(&m_a, sizeof(T));}
   SHADOW array(array const &t) : m_a() {su_array_create_copy(&m_a, &t.m_a);}
   SHADOW explicit array(su_array const *t) : m_a(){
      ASSERT(t->a_tsizeof == sizeof(T));
      su_array_create_copy(&m_a, t);
   }
   SHADOW explicit array(uz init_slots, boole setcount=TRU1){
      su_array_create_sized(&m_a, sizeof(T), init_slots, setcount);
   }

   ~array(void) {su_array_gut(&m_a);}

   s32 assign(array const &t){
      return (this == &t) ? 0 : su_array_assign(&m_a, &t.m_a);
   }

   SHADOW array &operator=(array const &t) {(void)assign(t); return *this;}

   u32 count(void) const {return m_a.a_count;}
   boole is_empty(void) const {return(count() == 0);}

   u32 size(void) const {return m_a.a_size;}

   boole is_errpass(void) const {return m_a.a_errpass;}
   array &set_errpass(boole passerr) {m_a.a_errpass = !!passerr; return *this;}

   T *data(void) {return S(T*,m_a.a_data);}
   T const *data(void) const {return S(T const*,m_a.a_data);}

   T &at(uz off) {ASSERT(off < count()); return data()[off];}
   T &operator[](uz off) {return at(off);}
   T const &at(uz off) const {ASSERT(off < count()); return data()[off];}
   T const &operator[](uz off) const {return at(off);}

   array &trunc(uz newcount=0){
      ASSERT_RET(newcount <= count(), *this);
      m_a.a_count = S(u32,newcount);
      return *this;
   }

   array &clear(void) {su_array_clear(&m_a); return *this;}

   s32 book(uz noof, boole setcount=TRU1){
      return su_array_book(&m_a, noof, setcount);
   }

   s32 book_at(uz off, uz noof){
      ASSERT_RET(off <= count(), 0);
      return su_array_book_at(&m_a, off, noof);
   }

   s32 unshift(T const &dat){
      s32 rv;
      if(!(rv = su_array_prepare_unshift(&m_a)))
         data()[0] = dat;
      return rv;
   }

   s32 push(T const &dat){
      s32 rv;
      if(!(rv = (count() == size())) ||
            !(rv = su_array_book(&m_a, 1, FAL0)))
         data()[m_a.a_count++] = dat;
      return rv;
   }
   array &push_fast(T const &dat){
      ASSERT_RET(count() < size(), *this);
      data()[m_a.a_count++] = dat;
      return *this;
   }

   s32 insert(uz off, T const &dat){
      s32 rv;
      ASSERT_RET(off <= count(), 0);
      if(!(rv = su_array_insert(&m_a, off)))
         data()[off] = dat;
      return rv;
   }

   // Note: this is expensive
   array &shift(void){
      ASSERT_RET(!is_empty(), *this);
      if(--m_a.a_count > 0)
         su_array_finish_shift(&m_a);
      return *this;
   }

   array &pop(void){
      ASSERT_RET(!is_empty(), *this);
      --m_a.a_count;
      return *this;
   }

   array &remove(uz off){
      ASSERT_RET(off < count(), *this);
      if(off != --m_a.a_count)
         su_array_finish_remove(&m_a, off);
      return *this;
   }

   array &remove(uz off, uz cnt){
      ASSERT_RET(off == 0 || off < count(), *this);
      su_array_remove_range(&m_a, off, cnt);
      return *this;
   }

   array &take_data(T *array, uz cnt_a_size){
      ASSERT_RET(cnt_a_size == 0 || array != NIL, *this);
      su_array_take_data(&m_a, array, cnt_a_size);
   }

   T *release_data(uz *cnt=NIL){
      if(cnt != NIL)
         *cnt = count();
      T *rv = data();
      m_a.a_data = NIL;
      m_a.a_count = m_a.a_size = 0;
      return rv;
   }
};

// Specialization for pointers
template<class T>
class array<T*>{
   su_parray m_pa;
public:
   array(void) : m_pa() {su_parray_create(&m_pa);}
   SHADOW array(array const &t) : m_pa(){
      su_parray_create_copy(&m_pa, &t.m_pa);
   }
   SHADOW explicit array(su_parray const *t) : m_pa(){
      su_parray_create_copy(&m_pa, t);
   }
   SHADOW explicit array(uz init_slots, boole setcount=TRU1){
      su_parray_create_sized(&m_pa, init_slots, setcount);
   }

   ~array(void) {su_parray_gut(&m_pa);}

   s32 assign(array const &t){
      return (this == &t) ? 0 : su_parray_assign(&m_pa, &t.m_pa);
   }

   SHADOW array &operator=(array const &t) {(void)assign(t); return *this;}

   u32 count(void) const {return m_pa.pa_count;}
   boole is_empty(void) const {return(count() == 0);}

   u32 size(void) const {return su_PARRAY_SIZE(&m_pa);}

   boole is_errpass(void) const {return su_PARRAY_IS_ERRPASS(&m_pa);}
   array &set_errpass(boole passerr){
      su_PARRAY_SET_ERRPASS(&m_pa, passerr);
      return *this;
   }

   T **data(void) {return S(T**,m_pa.pa_data);}
   T const * const *data(void) const{
      return S(T const*const*,m_pa.pa_data);
   }

   T *&at(uz off) {ASSERT(off < count()); return data()[off];}
   T *&operator[](uz off) {return at(off);}
   T const * const &at(uz off) const{
      ASSERT(off < count()); return data()[off];
   }
   T const * const &operator[](uz off) const {return at(off);}

   array &trunc(uz newcount=0){
      ASSERT_RET(newcount <= count(), *this);
      m_pa.pa_count = S(u32,newcount);
      return *this;
   }

   array &clear(void) {su_parray_clear(&m_pa); return *this;}

   s32 book(uz noof, boole setcount=TRU1){
      return su_parray_book(&m_pa, noof, setcount);
   }

   s32 book_at(uz off, uz noof){
      ASSERT_RET(off <= count(), 0);
      return su_parray_book_at(&m_pa, off, noof);
   }

   s32 unshift(T const *dat){
      s32 rv;
      if(!(rv = su_parray_prepare_unshift(&m_pa)))
         data()[0] = dat; /* TODO TypeTraits<T>::to_vp() */
      return rv;
   }

   s32 push(T const *dat){
      s32 rv;
      if(!(rv = (count() == size())) ||
            !(rv = su_parray_book(&m_pa, 1, FAL0)))
         data()[m_pa.a_count++] = dat; /* TODO TypeTraits<T>::to_vp() */
      return rv;
   }
   array &push_fast(T const *dat){
      ASSERT_RET(count() < size(), *this);
      data()[m_pa.pa_count++] = dat; /* TODO TypeTraits<T>::to_vp() */
      return *this;
   }

   s32 insert(uz off, T const &dat){
      s32 rv;
      ASSERT_RET(off <= count(), 0);
      if(!(rv = su_parray_insert(&m_pa, off)))
         data()[off] = dat;
      return rv;
   }

   // Note: this is expensive
   array &shift(void){
      ASSERT_RET(!is_empty(), *this);
      if(--m_pa.pa_count > 0)
         su_parray_finish_shift(&m_pa);
      return *this;
   }

   array &pop(void){
      ASSERT_RET(!is_empty(), *this);
      --m_pa.pa_count;
      return *this;
   }

   array &remove(uz off){
      ASSERT_RET(off < count(), *this);
      if(off != --m_pa.pa_count)
         su_parray_finish_remove(&m_pa, off);
      return *this;
   }

   array &remove(uz off, uz cnt){
      ASSERT_RET(off == 0 || off < count(), *this);
      su_parray_remove_range(&m_pa, off, cnt);
      return *this;
   }

   array &take_data(T **array, uz cnt_a_size){
      ASSERT_RET(cnt_a_size == 0 || array != NIL, *this);
      ASSERT_RET(UCMP(z, cnt_a_size, <=, max::s32), *this);
      su_parray_take_data(&m_pa, array, cnt_a_size);
   }

   T **release_data(uz *cnt=NIL){
      if(cnt != NIL)
         *cnt = count();
      T **rv = data();
      m_pa.pa_data = NIL;
      m_pa.pa_count = m_pa.pa_size = 0;
      return rv;
   }
};

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_ARRAY_H */
/* s-it-mode */
