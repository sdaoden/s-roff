/*@ Anything around memory.
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
#ifndef su_MEMORY_H
#define su_MEMORY_H

#include <su/primary.h>

#include <su/code-in.h>
C_DECL_BEGIN

/* Memory allocation routines optionally offer some debug support */
#if (defined su_HAVE_DEBUG || defined su_HAVE_DEVEL) &&\
      !defined su_HAVE_NOMEMDBG
# define su_HAVE_MEMORY_DEBUG
# define su_MEMORY_DEBUG_ARGS     , char const *mdbg_file, int mdbg_line
# define su_MEMORY_DEBUG_ARGSCALL , mdbg_file, mdbg_line
#else
# undef su_HAVE_MEMORY_DEBUG
# define su_MEMORY_DEBUG_ARGS
# define su_MEMORY_DEBUG_ARGSCALL
#endif

/* Tools */
c_decl void *su_memchr(void const *vp, si32 what, uiz len);
c_decl si32 su_memcmp(void const *vpa, void const *vpb, uiz len);
c_decl void *su_memcpy(void *vp, void const *src, uiz len);
c_decl void *su_memmove(void *vp, void const *src, uiz len);
c_decl void *su_memset(void *vp, si32 what, uiz len);

/* Allocation */
c_decl void *su_alloc(uiz size  su_MEMORY_DEBUG_ARGS);
c_decl void *su_realloc(void *vp, uiz size  su_MEMORY_DEBUG_ARGS);
c_decl void *su_calloc(uiz nmemb, uiz size  su_MEMORY_DEBUG_ARGS);
c_decl void su_free(void *vp  su_MEMORY_DEBUG_ARGS);

#ifdef su_HAVE_MEMORY_DEBUG
# define su_alloc(S) (su_alloc)(S, __FILE__, __LINE__)
# define su_realloc(P,S) (su_realloc)(P, S, __FILE__, __LINE__)
# define su_calloc(N,S) (su_calloc)(N, S, __FILE__, __LINE__)
# define su_free(P) (su_free)(P, __FILE__, __LINE__)
#endif

/* "Typed" allocation TODO 2x POSSIBLE MUL OVERFLOW */
#define su_talloc(T,SZ) su_S(T *,su_alloc(sizeof(T) * su_S(su_uiz,SZ)))
#define su_tcalloc(T,SZ) su_S(T *,su_calloc(SZ, sizeof(T))
#define su_trealloc(T,OVPSZ) \
   su_S(T *,su_realloc(OVP, sizeof(T) * su_S(su_uiz,SZ)))

/* Cache "management" */
c_decl void su_memory_reset(void);
#ifdef su_HAVE_MEMORY_DEBUG
c_decl void su_memory_trace(void);
c_decl void su_memory_check(char const *mdbg_file, int mdbg_line);
#endif

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

struct su_mem_johnny, su_mem_mary;

# define su_new(T) new(su_talloc(T, 1), su_S(su_mem_johnny const*,su_NIL)) T
# define su_cnew(T) new(su_tcalloc(T, 1), su_S(su_mem_johnny const*,su_NIL)) T
# define su_new_heap(T,VP) new(VP, su_S(su_mem_johnny*,su_NIL)) T
# define su_del(TP) su_mem_del(TP)
# define su_del_heap(TP) su_mem_del_heap(TP)
# define su_del_private(T,TP) \
   (su_ASSERT((TP) != su_NIL), (TP)->~T(), su_free(TP))
# define su_del_heap_private(T,TP) (su_ASSERT((TP) != su_NIL, (TP)->~T())

inline void *
operator new(size_t sz, void *rfptr, su_mem_johnny const *j){
   UNUSED(sz);
   UNUSED(j);
   return rfptr;
}

template<class T> inline void
su_mem_del(T *tptr){
   ASSERT_RET(tptr != NIL);
   tptr->~T();
   su_free(tptr);
}

template<class T> inline void
su_mem_del_heap(T *tptr){
   ASSERT_RET(tptr != NIL);
   tptr->~T();
}

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_MEMORY_H */
/* s-it-mode */
