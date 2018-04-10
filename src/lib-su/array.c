/*@ Implementation of array.h.
 *@ TODO Unit test!
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
#define su_FILE "su__array"
#define su_SOURCE
#define su_SOURCE_ARRAY

#include "su/code.h"
#include "su/mem.h"

#include "su/array.h"
#include "su/code-in.h"

struct su_array *
su_array_create(struct su_array *self, u32 tsizeof){ /* XXX inline */
   NYD_IN;
   ASSERT(self);
   ASSERT(tsizeof != 0);

   su_mem_set(self, 0, sizeof *self);
   self->a_tsizeof = tsizeof;
   NYD_OU;
   return self;
}

SHADOW struct su_array *
su_array_create_copy(struct su_array *self, struct su_array const *t){
   NYD_IN;
   ASSERT(self);

   su_mem_set(self, 0, sizeof *self);

   ASSERT_NYD_RET(t != NIL, self->a_tsizeof = 1/* XXX ?? */);

   self->a_errpass = t->a_errpass;
   if(t->a_count > 0)
      (void)su_array_assign(self, t);
   else
      self->a_tsizeof = t->a_tsizeof;
   NYD_OU;
   return self;
}

SHADOW struct su_array *
su_array_create_sized(struct su_array *self, u32 tsizeof, uz init_slots,
      boole setcount){
   NYD_IN;
   ASSERT(self);
   ASSERT(tsizeof != 0);

   su_mem_set(self, 0, sizeof *self);
   self->a_tsizeof = tsizeof;
   (void)su_array_book(self, init_slots, setcount);
   NYD_OU;
   return self;
}

void
su_array_gut(struct su_array *self){
   NYD_IN;
   ASSERT(self);

   if(self->a_data != NIL)
      su_FREE(self->a_data);

   DBG( su_mem_set(self, 0377, sizeof *self); )
   NYD_OU;
}

s32
su_array_assign(struct su_array *self, struct su_array const *t){
   s32 rv;
   NYD_IN;
   ASSERT(self);

   rv = 0;

   if(self != t){
      uiz s;

      ASSERT_NYD_RET(t != NIL, su_array_clear(self));

      if((s = self->a_tsizeof) != t->a_tsizeof){
         s *= self->a_size;
         s /= (self->a_tsizeof = t->a_tsizeof);
         self->a_size = s;
      }
      self->a_count = 0;
      /* self->a_errpass = t->a_errpass; No! */

      if(t->a_count > 0 && !(rv = su_array_book(self, t->a_count, TRU1)))
         su_mem_copy(self->a_data, t->a_data, self->a_count * self->a_tsizeof);
   }
   NYD_OU;
   return rv;
}

struct su_array *
su_array_clear(struct su_array *self){
   NYD_IN;
   ASSERT(self);

   if(self->a_data != NIL){
      su_FREE(self->a_data);
      self->a_data = NIL;
      self->a_count = self->a_size = 0;
   }
   NYD_OU;
   return self;
}

struct su_array *
su_array_swap(struct su_array *self, struct su_array *t){
   union {void *vp; u32 i; boole b;} u;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(t != NIL, self = su_array_clear(self));

   u.vp = self->a_data;
      self->a_data = t->a_data;
      t->a_data = u.vp;
   u.i = self->a_count;
      self->a_count = t->a_count;
      t->a_count = u.i;
   u.i = self->a_size;
      self->a_size = t->a_size;
      t->a_size = u.i;
   u.i = self->a_tsizeof;
      self->a_tsizeof = t->a_tsizeof;
      t->a_tsizeof = u.i;
   u.b = self->a_errpass;
      self->a_errpass = t->a_errpass;
      t->a_errpass = u.b;
   NYD_OU;
   return self;
}

s32
su_array_book(struct su_array *self, uz noof, boole setcount){
   u32 c, nc, i;
   s32 rv;
   NYD_IN;
   ASSERT(self);

   rv = 0;

   if(UNLIKELY(noof == 0))
      goto jleave;
   if(UNLIKELY(U32_MAX - noof <= (c = self->a_count)) ||
         UNLIKELY((nc = c + noof) >= U32_MAX / self->a_tsizeof)){
      rv = su_state_err((su_STATE_ERR_OVERFLOW |
               (self->a_errpass ? su_STATE_ERR_PASS : 0)),
            _("SU array: book request"));
      goto jleave;
   }

   if(nc > self->a_size){
      i = nc * self->a_tsizeof;
      i = su_mem_get_usable_size_32(i);
      self->a_size = i / self->a_tsizeof;
      if(UNLIKELY((self->a_data = su_MEM_REALLOCATE(self->a_data, i, 1,
            (self->a_errpass ? su_MEM_ALLOC_NOMEM_OK : 0))) == NIL)){
         self->a_count = self->a_size = 0;
         rv = su_ERR_NOMEM;
         goto jleave;
      }
   }

   if(setcount){
      self->a_count = nc;
      su_mem_set(&S(u8*,self->a_data)[c * self->a_tsizeof], 0,
         noof * self->a_tsizeof);
   }
jleave:
   NYD_OU;
   return rv;
}

s32
su_array_book_at(struct su_array *self, uz off, uz noof){
   u32 c, nc, i;
   s32 rv;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off <= self->a_count, rv = 0);

   rv = 0;

   if(UNLIKELY(noof == 0))
      goto jleave;
   if(UNLIKELY(U32_MAX - noof <= (c = self->a_count)) ||
         UNLIKELY((nc = c + noof) >= U32_MAX / self->a_tsizeof)){
jeoverflow:
      rv = su_state_err((su_STATE_ERR_OVERFLOW |
               (self->a_errpass ? su_STATE_ERR_PASS : 0)),
            _("SU array: book_at request"));
      goto jleave;
   }

   if(nc > self->a_size){
      i = nc * self->a_tsizeof;

      /* Try minimize further resizes xxx nonsense without a_tsizeof.. */
      if(i < 256)
         i = (i << 1) - (i >> 2);
      else{
         uz j;

         j = i >> 3;
         if(UNLIKELY(U32_MAX - j <= i))
            goto jeoverflow;
         i += j;
      }
      i = su_mem_get_usable_size_32(i);

      self->a_size = i / self->a_tsizeof;
      if(UNLIKELY((self->a_data = su_MEM_REALLOCATE(self->a_data, i, 1,
            (self->a_errpass ? su_MEM_ALLOC_NOMEM_OK : 0))) == NIL)){
         self->a_count = self->a_size = 0;
         rv = su_ERR_NOMEM;
         goto jleave;
      }
   }

   self->a_count = nc;
   if(c > 0 && c != off)
      su_mem_move(&S(u8*,self->a_data)[(off + noof) * self->a_tsizeof],
         &S(u8*,self->a_data)[off * self->a_tsizeof],
         (c - off) * self->a_tsizeof);
   su_mem_set(&S(u8*,self->a_data)[off * self->a_tsizeof], 0,
      noof * self->a_tsizeof);
jleave:
   NYD_OU;
   return rv;
}

s32
su_array_prepare_unshift(struct su_array *self){
   s32 rv;
   NYD_IN;
   ASSERT(self);

   if(self->a_count == self->a_size)
      rv = su_array_book_at(self, 0, 1);
   else{
      if(self->a_count > 0)
         su_mem_move(&S(u8*,self->a_data)[self->a_tsizeof], self->a_data,
            self->a_count * self->a_tsizeof);
      rv = 0;
   }

   if(!rv)
      ++self->a_count;
   NYD_OU;
   return rv;
}

s32
su_array_insert(struct su_array *self, uz off){
   s32 rv;
   u32 c, x;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off <= self->a_count, rv = 0);

   if((c = self->a_count) == self->a_size)
      rv = su_array_book_at(self, off, 1);
   else{
      if(off != c){
         x = off * self->a_tsizeof;
         su_mem_move(&S(u8*,self->a_data)[x + self->a_tsizeof],
            &S(u8*,self->a_data)[x], (c - off) * self->a_tsizeof);
      }
      rv = 0;
   }

   if(!rv)
      ++self->a_count;
   NYD_OU;
   return rv;
}

struct su_array *
su_array_finish_shift(struct su_array *self){
   NYD_IN;
   ASSERT(self);

   su_mem_move(self->a_data, &S(u8*,self->a_data)[self->a_tsizeof],
      self->a_count * self->a_tsizeof);
   NYD_OU;
   return self;
}

struct su_array *
su_array_finish_remove(struct su_array *self, uz off){
   u32 x;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off < self->a_count, );

   x = off * self->a_tsizeof;
   su_mem_move(&S(u8*,self->a_data)[x],
      &S(u8*,self->a_data)[x + self->a_tsizeof],
      (self->a_count - off) * self->a_tsizeof);
   NYD_OU;
   return self;
}

struct su_array *
su_array_remove_range(struct su_array *self, uz off, uz cnt){
   u32 i;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off == 0 || off < self->a_count, );

   i = self->a_count - off;
   if(cnt > i)
      cnt = i;

   if(cnt > 0 && (self->a_count -= cnt) != 0){
      i -= cnt;
      su_mem_move(&S(u8*,self->a_data)[off * self->a_tsizeof],
         &S(u8*,self->a_data)[(off + count) * self->a_tsizeof],
         i * self->a_tsizeof);
   }
   NYD_OU;
   return self;
}

struct su_array *
su_array_take_data(struct su_array *self, void *data, uz cnt_a_size){
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(cnt_a_size == 0 || data != NIL, su_array_clear(self));

   if(self->a_data != NIL)
      su_FREE(self->a_data);

   self->a_data = data;
   self->a_count = self->a_size = cnt_a_size;
   NYD_OU;
   return self;
}

struct su_parray *
su_parray_create(struct su_parray *self){ /* XXX inline */
   NYD_IN;
   ASSERT(self);

   su_mem_set(self, 0, sizeof *self);
   NYD_OU;
   return self;
}

SHADOW struct su_parray *
su_parray_create_copy(struct su_parray *self, struct su_parray const *t){
   NYD_IN;
   ASSERT(self);

   su_mem_set(self, 0, sizeof *self);

   ASSERT_NYD_RET(t != NIL, );

   if(su_PARRAY_IS_ERRPASS(t))
      self->pa__size_ep = S(u32,S32_MIN);
   if(t->pa_count > 0)
      (void)su_parray_assign(self, t);
   NYD_OU;
   return self;
}

SHADOW struct su_parray *
su_parray_create_sized(struct su_parray *self, uz init_slots, boole setcount){
   NYD_IN;
   ASSERT(self);

   su_mem_set(self, 0, sizeof *self);
   (void)su_parray_book(self, init_slots, setcount);
   NYD_OU;
   return self;
}

void
su_parray_gut(struct su_parray *self){
   NYD_IN;
   ASSERT(self);

   if(self->pa_data != NIL)
      su_FREE(self->pa_data);

   DBG( su_mem_set(self, 0377, sizeof *self); )
   NYD_OU;
}

s32
su_parray_assign(struct su_parray *self, struct su_parray const *t){
   s32 rv;
   NYD_IN;
   ASSERT(self);

   rv = 0;

   if(self != t){
      uiz s;

      ASSERT_NYD_RET(t != NIL, su_array_clear(self));

      self->pa_count = 0;
      /* su_PARRAY_SET_ERRPASS(self, su_PARRAY_IS_ERRPASS(t)); No! */

      if(t->pa_count > 0 && !(rv = su_parray_book(self, t->pa_count, TRU1)))
         su_mem_copy(self->pa_data, t->pa_data,
            self->pa_count * sizeof(*self->pa_data));
   }
   NYD_OU;
   return rv;
}

struct su_parray *
su_parray_clear(struct su_parray *self){
   NYD_IN;
   ASSERT(self);

   if(self->pa_data != NIL){
      su_FREE(self->pa_data);
      self->pa_data = NIL;
      self->pa_count = 0;
      su_PARRAY_SET_SIZE(self, 0);
   }
   NYD_OU;
   return self;
}

struct su_parray *
su_parray_swap(struct su_parray *self, struct su_parray *t){
   union {void *vp; u32 i;} u;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(t != NIL, self = su_parray_clear(self));

   u.vp = self->pa_data;
      self->pa_data = t->pa_data;
      t->pa_data = u.vp;
   u.i = self->pa_count;
      self->pa_count = t->pa_count;
      t->pa_count = u.i;
   u.i = self->pa__size_ep;
      self->pa__size_ep = t->pa__size_ep;
      t->pa__size_ep = u.i;
   NYD_OU;
   return self;
}

s32
su_parray_book(struct su_parray *self, uz noof, boole setcount){
   u32 c, nc, i;
   s32 rv;
   NYD_IN;
   ASSERT(self);

   rv = 0;

   if(UNLIKELY(noof == 0))
      goto jleave;
   /* XXX Following overflow statement can be optimized */
   if(UNLIKELY(S(u32,S32_MAX) <= noof) ||
         UNLIKELY(S(u32,S32_MAX) - noof <= (c = self->pa_count)) ||
         UNLIKELY((nc = c + noof) >= S(u32,S32_MAX) / sizeof(*self->pa_data))){
      rv = su_state_err((su_STATE_ERR_OVERFLOW |
               (su_PARRAY_IS_ERRPASS(self) ? su_STATE_ERR_PASS : 0)),
            _("SU parray: book request"));
      goto jleave;
   }

   if(nc > su_PARRAY_SIZE(self)){
      i = nc * sizeof(*self->pa_data);
      i = su_mem_get_usable_size_32(i);
      su_PARRAY_SET_SIZE(self, i / sizeof(*self->pa_data));
      if(UNLIKELY((self->pa_data = su_MEM_REALLOCATE(self->pa_data, i, 1,
             (su_PARRAY_IS_ERRPASS(self) ? su_MEM_ALLOC_NOMEM_OK : 0))
            ) == NIL)){
         self->pa_count = 0;
         su_PARRAY_SET_SIZE(self, 0);
         rv = su_ERR_NOMEM;
         goto jleave;
      }
   }

   if(setcount){
      self->pa_count = nc;
      su_mem_set(&self->pa_data[c], 0, noof * sizeof(*self->pa_data));
   }
jleave:
   NYD_OU;
   return rv;
}

s32
su_parray_book_at(struct su_parray *self, uz off, uz noof){
   u32 c, nc, i;
   s32 rv;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off <= self->pa_count, rv = 0);

   rv = 0;

   if(UNLIKELY(noof == 0))
      goto jleave;
   /* XXX Following overflow statement can be optimized */
   if(UNLIKELY(S(u32,S32_MAX) <= noof) ||
         UNLIKELY(S(u32,S32_MAX) - noof <= (c = self->pa_count)) ||
         UNLIKELY((nc = c + noof) >= S(u32,S32_MAX) / sizeof(*self->pa_data))){
jeoverflow:
      rv = su_state_err((su_STATE_ERR_OVERFLOW |
               (su_PARRAY_IS_ERRPASS(self) ? su_STATE_ERR_PASS : 0)),
            _("SU parray: book_at request"));
      goto jleave;
   }

   if(nc > su_PARRAY_SIZE(self)){
      i = nc * sizeof(*self->pa_data);

      /* Try minimize further resizes xxx nonsense without sizeof(void*).. */
      if(i < 256)
         i = (i << 1) - (i >> 2);
      else{
         uz j;

         j = i >> 3;
         if(UNLIKELY(S(uz,SZ_MAX) - j <= i))
            goto jeoverflow;
         i += j;
      }
      i = su_mem_get_usable_size_32(i);

      su_PARRAY_SET_SIZE(self, i / sizeof(*self->pa_data));
      if(UNLIKELY((self->pa_data = su_MEM_REALLOCATE(self->pa_data, i, 1,
             (su_PARRAY_IS_ERRPASS(self) ? su_MEM_ALLOC_NOMEM_OK : 0))
            ) == NIL)){
         self->pa_count = 0;
         su_PARRAY_SET_SIZE(self, 0);
         rv = su_ERR_NOMEM;
         goto jleave;
      }
   }

   self->pa_count = nc;
   if(c > 0 && c != off)
      su_mem_move(&self->pa_data[off + noof], &self->pa_data[off],
         (c - off) * sizeof(*self->pa_data));
   su_mem_set(&self->pa_data[off], 0, noof * sizeof(*self->pa_data));
jleave:
   NYD_OU;
   return rv;
}

s32
su_parray_prepare_unshift(struct su_parray *self){
   s32 rv;
   NYD_IN;
   ASSERT(self);

   if(self->pa_count == su_PARRAY_SIZE(self))
      rv = su_parray_book_at(self, 0, 1);
   else{
      if(self->pa_count > 0)
         su_mem_move(&self->pa_data[1], self->pa_data,
            self->pa_count * sizeof(*self->pa_data));
      rv = 0;
   }

   if(!rv)
      ++self->pa_count;
   NYD_OU;
   return rv;
}

s32
su_parray_insert(struct su_parray *self, uz off){
   s32 rv;
   u32 c;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off <= self->pa_count, rv = 0);

   if((c = self->pa_count) == su_PARRAY_SIZE(self))
      self = su_parray_book_at(self, off, 1);
   else{
      if(off != c)
         su_mem_move(&self->pa_data[off + 1], &self->pa_data[off],
            (c - off) * sizeof(*self->pa_data));
      rv = 0;
   }

   if(!rv)
      ++self->pa_count;
   NYD_OU;
   return rv;
}

struct su_parray *
su_parray_finish_shift(struct su_parray *self){
   NYD_IN;
   ASSERT(self);

   su_mem_move(self->pa_data, &self->pa_data[1],
      self->pa_count * sizeof(*self->pa_data));
   NYD_OU;
   return self;
}

struct su_parray *
su_parray_finish_remove(struct su_parray *self, uz off){
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off < self->pa_count, );

   su_mem_move(&self->pa_data[off], &self->pa_data[off + 1],
      (self->pa_count - off) * sizeof(*self->pa_data));
   NYD_OU;
   return self;
}

struct su_parray *
su_parray_remove_range(struct su_parray *self, uz off, uz cnt){
   u32 i;
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(off == 0 || off < self->pa_count, );

   i = self->pa_count - off;
   if(cnt > i)
      cnt = i;

   if(cnt > 0 && (self->pa_count -= cnt) != 0){
      i -= cnt;
      su_mem_move(&self->pa_data[off], &self->pa_data[off + count],
         i * sizeof(*self->pa_data));
   }
   NYD_OU;
   return self;
}

struct su_parray *
su_parray_take_data(struct su_parray *self, void **data, uz cnt_a_size){
   NYD_IN;
   ASSERT(self);
   ASSERT_NYD_RET(UCMP(z, cnt_a_size, <=, S32_MAX), );

   if(self->pa_data != NIL)
      su_FREE(self->pa_data);

   self->pa_data = data;
   self->pa_count = cnt_a_size;
   su_PARRAY_SET_SIZE(self, S(u32,cnt_a_size));
   NYD_OU;
   return self;
}

#include "su/code-ou.h"
/* s-it-mode */
