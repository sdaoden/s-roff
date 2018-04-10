/*@ Anything around memory.
 *@ The allocation interface is macro-based for the sake of debugging.
 *@ It interacts with su_STATE_ERR_NOMEM and su_STATE_ERR_OVERFLOW,
 *@ but also allows per-call failure ignorance.
 *@ All interfaces provide shortcuts via code-in.h.
 *
 * Copyright (c) 2001 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef su_MEM_H
#define su_MEM_H

#include <su/code.h>

#define su_HEADER
#include <su/code-in.h>
C_DECL_BEGIN

/* Memory allocation routines optionally offer some debug support */
#if (defined su_HAVE_DEBUG || defined su_HAVE_DEVEL) &&\
      !defined su_HAVE_NOMEMDEBUG
# define su_HAVE_MEM_DEBUG
# define su_MEM_DEBUG_ARGS_DECL , char const *mdbg_file, int mdbg_line
# define su_MEM_DEBUG_ARGS_INJ , __FILE__, __LINE__
# define su_MEM_DEBUG_ARGS_USE , mdbg_file, mdbg_line
#else
# undef su_HAVE_MEM_DEBUG
# define su_MEM_DEBUG_ARGS_DECL
# define su_MEM_DEBUG_ARGS_INJ
# define su_MEM_DEBUG_ARGS_USE
#endif

enum su_mem_alloc_flags{
   su_MEM_ALLOC_NONE,
   su_MEM_ALLOC_CLEAR = 1u<<1,      /* Memory shall be zeroed (not realloc) */
   su_MEM_ALLOC_OVERFLOW_OK = 1u<<2, /* Overflow error shall return NIL */
   su_MEM_ALLOC_NOMEM_OK = 1u<<3,   /* Out-of-memory shall return NIL */
   su_MEM_ALLOC_MUSTFAIL = 1u<<4    /* NIL return is a no-go */
};

enum{
   /* Minimum real size of an allocation: it does not make real sense to ask
    * for less; also see su_mem_get_usable_size() */
   su_MEM_ALLOC_MIN = ALIGN_SMALL(1) /* XXX fake */
};

/*
 * Tools
 */

EXPORT sz su_mem_cmp(void const *vpa, void const *vpb, uz len);
EXPORT void *su_mem_copy(void *vp, void const *src, uz len);
EXPORT void *su_mem_find(void const *vp, s32 what, uz len);
EXPORT void *su_mem_move(void *vp, void const *src, uz len);
EXPORT void *su_mem_set(void *vp, s32 what, uz len);

/*
 * Allocation interface
 */

/* These are rather internal, but due to maf maybe handy sometimes.
 * Normally to be used through the macros below */
EXPORT void *su_mem_allocate(uz sz, uz no, enum su_mem_alloc_flags maf
      su_MEM_DEBUG_ARGS_DECL);
EXPORT void *su_mem_reallocate(void *ovp, uz sz, uz no,
      enum su_mem_alloc_flags maf su_MEM_DEBUG_ARGS_DECL);
EXPORT void *su_mem_free(void *ovp  su_MEM_DEBUG_ARGS_DECL);

#define su_MEM_ALLOC(SZ) su_MEM_ALLOCATE(SZ, 1, su_MEM_ALLOC_NONE)
#define su_MEM_ALLOC_N(SZ,NO) su_MEM_ALLOCATE(SZ, NO, su_MEM_ALLOC_NONE)
#define su_MEM_CALLOC(SZ) su_MEM_ALLOCATE(SZ, 1, su_MEM_ALLOC_CLEAR)
#define su_MEM_CALLOC_N(SZ,NO) su_MEM_ALLOCATE(SZ, NO, su_MEM_ALLOC_CLEAR)
#define su_MEM_REALLOC(OVP,SZ) su_MEM_REALLOCATE(OVP, SZ, 1, su_MEM_ALLOC_NONE)
#define su_MEM_REALLOC_N(OVP,SZ,NO) \
   su_MEM_REALLOCATE(OVP, SZ, NO, su_MEM_ALLOC_NONE)

#define su_MEM_TALLOC(T,SZ) su_S(T *,su_MEM_ALLOC_N(sizeof(T), su_S(su_uz,SZ)))
#define su_MEM_TCALLOC(T,SZ) \
   su_S(T *,su_MEM_CALLOC_N(sizeof(T), su_S(su_uz,SZ))
#define su_MEM_TREALLOC(T,OVP,SZ) \
   su_S(T *,su_MEM_REALLOC_N(OVP, sizeof(T), su_S(su_uz,SZ)))

#define su_MEM_ALLOCATE(SZ,NO,F) \
   su_mem_allocate(SZ, NO, F  su_MEM_DEBUG_ARGS_INJ)
#define su_MEM_REALLOCATE(OVP,SZ,NO,F) \
   su_mem_reallocate(OVP, SZ, NO, F  su_MEM_DEBUG_ARGS_INJ)

#define su_MEM_FREE(OVP) su_mem_free(OVP  su_MEM_DEBUG_ARGS_INJ)

/* Cache "support" */
#define su_mem_get_usable_size(SZ) su_ALIGN_SMALL(SZ) /* XXX fake */
#define su_mem_get_usable_size_32(SZ) ((su_u32)su_ALIGN_SMALL(SZ)) /*XXX fake*/

/* Cache "management".  Note: "normal" logs via su_LOG_DEBUG */
EXPORT void su_mem_reset(void);
#ifdef su_HAVE_MEM_DEBUG
EXPORT void su_mem_trace(void);
EXPORT void su_mem_check(char const *mdbg_file, int mdbg_line);
#endif

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

// Instanceless static encapsulator.
class mem{
public:
   struct johnny, mary;

   /// Tools

   static sz cmp(void const *vpa, void const *vpb, uz len){
      return su_mem_cmp(vpa, vpb, len);
   }
   static void *copy(void *vp, void const *src, uz len){
      return su_mem_copy(vp, src, len);
   }
   static void *find(void const *vp, s32 what, uz len){
      return su_mem_find(vp, what, len);
   }
   static void *move(void *vp, void const *src, uz len){
      return su_mem_move(vp, src, len);
   }
   static void *set(void *vp, s32 what, uz len){
      return su_mem_set(vp, what, len);
   }

   /// Allocation interface (mostly a direct take over from C)

# define su_MEM_NEW(T) \
   new(su_MEM_ALLOCATE(sizeof(T), 1, su_MEM_ALLOC_MUSTFAIL,\
      su_S(su_NSPC(su)mem::johnny const*,su_NIL)) T
# define su_MEM_CNEW(T) \
   new(su_MEM_ALLOCATE(sizeof(T), 1,\
         su_MEM_ALLOC_CLEAR | su_MEM_ALLOC_MUSTFAIL),\
      su_S(su_NSPC(su)mem::johnny const*,su_NIL)) T
# define su_MEM_NEW_HEAP(T,VP) new(VP, su_S(su_NSPC(su)mem::johnny*,su_NIL)) T
# define su_MEM_DEL(TP) (su_NSPC(su)mem::del__heap(TP), su_MEM_FREE(TP))
# define su_MEM_DEL_HEAP(TP) su_NSPC(su)mem::del__heap(TP)
# define su_MEM_DEL_PRIVATE(T,TP) \
   (su_ASSERT((TP) != su_NIL), (TP)->~T(), su_MEM_FREE(TP))
# define su_MEM_DEL_HEAP_PRIVATE(T,TP) (su_ASSERT((TP) != su_NIL), (TP)->~T())

   template<class T>
   static void del__heap(T *tptr){
      ASSERT_RET(tptr != NIL, );
      tptr->~T();
   }
};

inline void *operator new(size_t sz, void *vp, NSPC(su)mem::johnny const *j){
   UNUSED(sz);
   UNUSED(j);
   return vp;
}

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_MEM_H */
/* s-it-mode */
