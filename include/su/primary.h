/*@ Primary: basic infrastructure (POD types etc.) and generic macros.
 *@ - Reacts upon su_HAVE_DEBUG, su_HAVE_DEVEL, and NDEBUG.
 *@ - Some macros require su_FILE to be defined to a literal.
 *@   If that is defined, the su_M*CTA() assertions are injected, too.
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
#ifndef su_PRIMARY_H
#define su_PRIMARY_H

#include <su/config.h>

#include <inttypes.h> /* TODO try to replace with */
#include <limits.h> /* TODO config-time thing! */

/* LANG {{{ */

#ifndef __cplusplus
# define su_C_LANG 1
# define su_C_DECL_BEGIN
# define su_C_DECL_END
# define su_c_decl extern

   /* Casts */
# define su_S(T,I) ((T)(I))
# define su_R(T,I) ((T)(I))
# define su_C(T,I) ((T)(I))

# define su_NIL ((void*)0)
#else
# define su_C_LANG 0
# define su_C_DECL_BEGIN extern "C" {
# define su_C_DECL_END }
# define su_c_decl
# define su_NSPC_BEGIN(X) //namespace X {
# define su_NSPC_END(X) //}
# define su_NSPC_USE(X) //using namespace X;

   /* Disable copy-construction and assigment of class */
# define su_CLASS_NO_COPY(C) private:C(C const &);C &operator=(C const &);

   /* C++ only allows those at the declaration, not the definition */
# define su_public
# define su_protected
# define su_private
# define su_static
# define su_virtual

   /* Casts */
# define su_S(T,I) static_cast<T>(I)
# define su_R(T,I) reinterpret_cast<T>(I)
# define su_C(T,I) const_cast<T>(I)

# define su_NIL (0L)
#endif /* __cplusplus */

/* Compile-Time-Assert FIXME Static_assert C++??
 * Problem is that some compilers warn on unused local typedefs, so add
 * a special local CTA to overcome this */
#if !su_C_LANG && __cplusplus + 0 >= 201103L
# define su_CTA(T,M) static_assert(T, M)
# define su_LCTA(T,M) static_assert(T, M)
#elif defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 201112L
# define su_CTA(T,M) _Static_assert(T, M)
# define su_LCTA(T,M) _Static_assert(T, M)
#else
# define su_CTA(T,M) su__CTA_1(T, su_FILE, __LINE__)
# define su_LCTA(T,M) su__LCTA_1(T, su_FILE, __LINE__)

# define su__CTA_1(T,F,L) su__CTA_2(T, F, L)
# define su__CTA_2(T,F,L) \
   typedef char ASSERTION_failed_in_file_## F ## _at_line_ ## L[(T) ? 1 : -1]
# define su__LCTA_1(T,F,L) su__LCTA_2(T, F, L)
# define su__LCTA_2(T,F,L) \
do{\
   typedef char ASSERT_failed_in_file_## F ## _at_line_ ## L[(T) ? 1 : -1];\
   ASSERT_failed_in_file_## F ## _at_line_ ## L __i_am_unused__;\
   su_UNUSED(__i_am_unused__);\
}while(0)
#endif

#define su_CTAV(T) su_CTA(T, "Unexpected value of constant")
#define su_LCTAV(T) su_LCTA(T, "Unexpected value of constant")
#ifdef su_FILE
# define su_MCTA(T,M) su_CTA(T, M);
#else
# define su_MCTA(T,M)
#endif

/* LANG }}} */
/* OS {{{ */

#define su_OS_DRAGONFLY 0
#define su_OS_EMX 0
#define su_OS_MSDOS 0
#define su_OS_OPENBSD 0
#define su_OS_SOLARIS 0
#define su_OS_SUNOS 0

#ifdef __DragonFly__
# undef su_OS_DRAGONFLY
# define su_OS_DRAGONFLY 1
#elif defined __EMX__
# undef su_OS_EMX
# define su_OS_EMX 1
#elif defined __MSDOS__
# undef su_OS_MSDOS
# define su_OS_MSDOS 1
#elif defined __OpenBSD__
# undef su_OS_OPENBSD
# define su_OS_OPENBSD 1
#elif defined __solaris__ || defined __sun
# if defined __SVR4 || defined __svr4__
#  undef su_OS_SOLARIS
#  define su_OS_SOLARIS 1
# else
#  undef su_OS_SUNOS
#  define su_OS_SUNOS 1
# endif
#endif

/* OS }}} */
/* CC {{{ */

#define su_CC_CLANG 0
#define su_PREREQ_CLANG(X,Y) 0
#define su_CC_GCC 0
#define su_PREREQ_GCC(X,Y) 0

#ifdef __clang__
# undef su_CC_CLANG
# undef su_PREREQ_CLANG
# define su_CC_CLANG 1
# define su_PREREQ_CLANG(X,Y) \
   (__clang_major__ + 0 > (X) || \
    (__clang_major__ + 0 == (X) && __clang_minor__ + 0 >= (Y)))
# define su_CC_EXTEN __extension__

#elif defined __GNUC__
# undef su_CC_GCC
# undef su_PREREQ_GCC
# define su_CC_GCC 1
# define su_PREREQ_GCC(X,Y) \
   (__GNUC__ + 0 > (X) || (__GNUC__ + 0 == (X) && __GNUC_MINOR__ + 0 >= (Y)))
# define su_CC_EXTEN __extension__
#endif

#ifndef su_CC_EXTEN
# define su_CC_EXTEN
#endif

/* Suppress some technical warnings via #pragma's unless developing.
 * XXX Wild guesses: clang(1) 1.7 and (OpenBSD) gcc(1) 4.2.1 don't work */
#ifndef HAVE_DEVEL
# if su_PREREQ_CLANG(3, 4)
/*#  pragma clang diagnostic ignored "-Wformat"*/
#  pragma clang diagnostic ignored "-Wunused-result"
# elif su_PREREQ_GCC(4, 7)
/*#  pragma GCC diagnostic ignored "-Wformat"*/
#  pragma GCC diagnostic ignored "-Wunused-result"
# endif
#endif

/* Function name */
#if defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
# define su_FUN __func__
#elif su_CC_CLANG || su_PREREQ_GCC(3, 4)
# define su_FUN __extension__ __FUNCTION__
#else
# define su_FUN su_empty /* A non-literal! *//*FIXME no su_empty yet! */
#endif

/* inline keyword */
#if rf_C_LANG
# if su_CC_CLANG || su_CC_GCC
#  if defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
#   define su_INLINE inline
#  else
#   define su_INLINE __inline
# else
#   define su_INLINE static /* TODO __attribute__((unused)) alikes? */
# endif
#else
# define su_INLINE inline
#endif

#if defined __predict_true && defined __predict_false
# define su_LIKELY(X) __predict_true(X)
# define su_UNLIKELY(X) __predict_false(X)
#elif su_CC_CLANG || su_PREREQ_GCC(2, 96)
# define su_LIKELY(X) __builtin_expect(X, 1)
# define su_UNLIKELY(X) __builtin_expect(X, 0)
#else
# define su_LIKELY(X) (X)
# define su_UNLIKELY(X) (X)
#endif

/* CC }}} */
/* SUPPORT MACROS+ {{{ */

/* ABSolutely necessary basic support macros, with side effects */
#define su_ABS(A) ((A) < 0 ? -(A) : (A))
#define su_CLIP(X,A,B) (((X) <= (A)) ? (A) : (((X) >= (B)) ? (B) : (X)))
#define su_MAX(A,B) ((A) < (B) ? (B) : (A))
#define su_MIN(A,B) ((A) < (B) ? (A) : (B))
#define su_ISPOW2(X) ((((X) - 1) & (X)) == 0)

#if !su_C_LANG
template<class T> inline T
su_abs(T const &a){
   return su_ABS(a);
}

template<class T> inline T const &
su_clip(T const &a, T const &min, T const &max){
   return su_CLIP(a, min, max);
}

template<class T> inline T const &
su_max(T const &a, T const &b){
   return su_MAX(a, b);
}

template<class T> inline T const &
su_min(T const &a, T const &b){
   return su_MIN(a, b);
}

template<class T> inline int
su_ispow2(T const &a){
   return su_ISPOW2(a);
}
#endif /* !su_C_LANG */

/* Align something to a size/boundary that cannot cause just any problem.
 * Unfortunately this software uses floating-point... */
#define su_ALIGN(X) (((X) + 2*su__ALIGN_SIZE) & ~((2*su__ALIGN_SIZE) - 1))
#define su_ALIGN_SMALL(X) (((X) + su__ALIGN_SIZE) & ~(su__ALIGN_SIZE - 1))
#define su__ALIGN_SIZE \
   su_MAX(sizeof(uiz_t), su_MAX(sizeof(void*), sizeof(double))) /* XXX si64 */

/* ASSERT comes from code-{in,ou}.h */

/* Create a bit mask for the bit range LO..HI -- HI can't use highest bit! */
#define su_BITENUM_MASK(LO,HI) (((1u << ((HI) + 1)) - 1) & ~((1u << (LO)) - 1))

/* For injection macros like su_DBG() */
#define su_COMMA ,

/* Debug injections */
#if !defined su_HAVE_DEBUG || defined NDEBUG
# define su_DBG(X)
# define su_NDBG(X) X
# define su_DBGOR(X,Y) Y
#else
# define su_DBG(X) X
# define su_NDBG(X)
# define su_DBGOR(X,Y) X
#endif

/* To avoid files are overall empty */
#define su_EMPTY_FILE() typedef int su_CONCAT(su_notempty_shall_b_, su_FILE);

/* C field init */
#if defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
# define su_FIELD_INITN(N) su_CONCAT(., N) =
# define su_FIELD_INITI(I) [I] =
#else
# define su_FIELD_INITN(N)
# define su_FIELD_INITI(N)
#endif

/* Members in constant array */
#define su_NELEM(A) (sizeof(A) / sizeof((A)[0]))

/* Pointer to size_t */
#define su_PTR2SIZE(X) su_S(su_uiz,su_S(su_uip,X))

/* Pointer comparison */
#define su_PTRCMP(A,C,B) (su_S(su_uip,A) C su_S(su_uip,B))

/* sizeof() for member fields */
#define su_SIZEOF_FIELD(T,F) sizeof(su_S(T *,su_NIL)->F)

/* String stuff.
 * __STDC_VERSION__ is ISO C99, so also use __STDC__, which should work */
#if defined __STDC__ || defined __STDC_VERSION__ || su_C
# define su_STRING(X) #X
# define su_XSTRING(X) su_STRING(X)
# define su_CONCAT(S1,S2) su__CONCAT_1(S1, S2)
# define su__CONCAT_1(S1,S2) S1 ## S2
#else
# define su_STRING(X) "X"
# define su_XSTRING STRING
# define su_CONCAT(S1,S2) S1/* won't work out */S2
#endif

/* Compare (maybe mixed-signed) integers cases to T bits, unsigned,
 * T is one of our homebrew integers, e.g., UICMP(32, su_ABS(n), >, wleft).
 * Note: doesn't sign-extend correctly, that's still up to the caller */
#if su_C_LANG
# define su_UICMP(T,A,C,B) (su_S(su_ ## ui ## T,A) C su_S(su_ ## ui ## T,B))
#else
# define su_UICMP(T,A,C,B) (su_S(su_ ## ui ## T ##,A) C su_S(su_ ## ui ## T,B))
#endif

/* Casts-away (*NOT* cast-away) */
#if su_C_LANG
# define su_UNCONST(P) su_S(void*,su_S(su_uip,void const*,P))
# define su_UNVOLATILE(P) su_S(void*,su_S(su_uip,su_S(void volatile*,P)))
  /* To avoid warnings with modern compilers for "char*i; *(si32_t*)i=;" */
# define su_UNALIGN(T,P) su_S(T,su_S(su_uip,P))
# define su_UNXXX(T,C,P) su_S(T,su_S(su_uip,su_S(C,P)))
#endif

/* Avoid "may be used uninitialized" warnings */
#if (defined su_HAVE_DEBUG || defined su_HAVE_DEVEL) && !defined NDEBUG
# define su_UNINIT(N,V)
#else
# define su_UNINIT(N,V) N = V
#endif

/* Avoid "unused" warnings */
#define su_UNUSED(X) ((void)(X))

/* Variable-type size (with byte array at end) */
#if su_C_LANG && defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
# define su_VFIELD_SIZE(X)
# define su_VSTRUCT_SIZEOF(T,F) sizeof(T)
#else
# define su_VFIELD_SIZE(X) \
   ((X) == 0 ? sizeof(su_uiz) \
    : (su_S(su_siz,X) < 0 ? sizeof(su_uiz) - su_ABS(X) : su_S(su_uiz,X)))
# define su_VSTRUCT_SIZEOF(T,F) (sizeof(T) - su_SIZEOF_FIELD(T, F))
#endif

/* SUPPORT MACROS+ }}} */
/* TYPES TODO maybe configure-time, from a su/config.h?! {{{ */
su_C_DECL_BEGIN

/* First some shorter aliases for "normal" integers */
typedef unsigned long su_ul;
typedef unsigned int su_ui;
typedef unsigned short su_us;
typedef unsigned char su_uc;

typedef signed long su_sl;
typedef signed int su_si;
typedef signed short su_ss;
typedef signed char su_sc;

#ifdef UINT8_MAX
# define su_UI8_MAX UINT8_MAX
# define su_SI8_MIN INT8_MIN
# define su_SI8_MAX INT8_MAX
typedef uint8_t su_ui8;
typedef int8_t su_si8;
#elif UCHAR_MAX != 255
# error UCHAR_MAX must be 255
#else
# define su_UI8_MAX UCHAR_MAX
# define su_SI8_MIN CHAR_MIN
# define su_SI8_MAX CHAR_MAX
typedef unsigned char su_ui8;
typedef signed char su_si8;
#endif

#if !defined PRIu8 || !defined PRId8
# undef PRIu8
# undef PRId8
# define PRIu8 "hhu"
# define PRId8 "hhd"
#endif

enum {su_FAL0, su_TRU1, su_TRUM1 = -1};
typedef su_si8 su_boole;

#ifdef UINT16_MAX
# define su_UI16_MAX UINT16_MAX
# define su_SI16_MIN INT16_MIN
# define su_SI16_MAX INT16_MAX
typedef uint16_t su_ui16;
typedef int16_t su_si16;
#elif USHRT_MAX != 0xFFFFu
# error USHRT_MAX must be 0xFFFF
#else
# define su_UI16_MAX USHRT_MAX
# define su_SI16_MIN SHRT_MIN
# define su_SI16_MAX SHRT_MAX
typedef unsigned short su_ui16;
typedef signed short su_si16;
#endif

#if !defined PRIu16 || !defined PRId16
# undef PRIu16
# undef PRId16
# if su_UI16_MAX == UINT_MAX
#  define PRIu16 "u"
#  define PRId16 "d"
# else
#  define PRIu16 "hu"
#  define PRId16 "hd"
# endif
#endif

#ifdef UINT32_MAX
# define su_UI32_MAX UINT32_MAX
# define su_SI32_MIN INT32_MIN
# define su_SI32_MAX INT32_MAX
typedef uint32_t su_ui32;
typedef int32_t su_si32;
#elif ULONG_MAX == 0xFFFFFFFFu
# define su_UI32_MAX ULONG_MAX
# define su_SI32_MIN LONG_MIN
# define su_SI32_MAX LONG_MAX
typedef unsigned long int su_ui32;
typedef signed long int su_si32;
#elif UINT_MAX != 0xFFFFFFFFu
# error UINT_MAX must be 0xFFFFFFFF
#else
# define su_UI32_MAX UINT_MAX
# define su_SI32_MIN INT_MIN
# define su_SI32_MAX INT_MAX
typedef unsigned int su_ui32;
typedef signed int su_si32;
#endif

#if !defined PRIu32 || !defined PRId32
# undef PRIu32
# undef PRId32
# if su_UI32_MAX == ULONG_MAX
#  define PRIu32 "lu"
#  define PRId32 "ld"
# else
#  define PRIu32 "u"
#  define PRId32 "d"
# endif
#endif

#ifdef UINT64_MAX
# define su_UI64_MAX UINT64_MAX
# define su_SI64_MIN INT64_MIN
# define su_SI64_MAX INT64_MAX
typedef uint64_t su_ui64;
typedef int64_t su_si64;
#elif ULONG_MAX <= 0xFFFFFFFFu
# if !defined ULLONG_MAX || (ULLONG_MAX >> 31) < 0xFFFFFFFFu
#  error We need a 64 bit integer
# else
#  define su_UI64_MAX ULLONG_MAX
#  define su_SI64_MIN LLONG_MIN
#  define su_SI64_MAX LLONG_MAX
su_CC_EXTEN typedef unsigned long long su_ui64;
su_CC_EXTEN typedef signed long long su_si64;
# endif
#else
# define su_UI64_MAX ULONG_MAX
# define su_SI64_MIN LONG_MIN
# define su_SI64_MAX LONG_MAX
typedef unsigned long su_ui64;
typedef signed long su_si64;
#endif

#if !defined PRIu64 || !defined PRId64 || !defined PRIX64 || !defined PRIo64
# undef PRIu64
# undef PRId64
# undef PRIX64
# undef PRIo64
# if defined ULLONG_MAX && su_UI64_MAX == ULLONG_MAX
#  define PRIu64 "llu"
#  define PRId64 "lld"
#  define PRIX64 "llX"
#  define PRIo64 "llo"
# else
#  define PRIu64 "lu"
#  define PRId64 "ld"
#  define PRIX64 "lX"
#  define PRIo64 "lo"
# endif
#endif

/* (So that we can use UICMP() for size_t comparison, too) */
typedef size_t su_uiz;
typedef ssize_t su_siz;

#undef PRIuZ
#undef PRIdZ
#if defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
# define PRIuZ "zu"
# define PRIdZ "zd"
# define su_UIZ_MAX SIZE_MAX
#elif defined SIZE_MAX
   /* UnixWare has size_t as unsigned as required but uses a signed limit
    * constant (which is thus false!) */
# if SIZE_MAX == su_UI64_MAX || SIZE_MAX == su_SI64_MAX
#  define PRIuZ PRIu64
#  define PRIdZ PRId64
su_MCTA(sizeof(size_t) == sizeof(su_ui64),
    "Format string mismatch, compile with ISO C99 compiler (-std=c99)!")
# elif SIZE_MAX == su_UI32_MAX || SIZE_MAX == su_SI32_MAX
#  define PRIuZ PRIu32
#  define PRIdZ PRId32
su_MCTA(sizeof(size_t) == sizeof(su_ui32),
    "Format string mismatch, compile with ISO C99 compiler (-std=c99)!")
# else
#  error SIZE_MAX is neither su_UI64_MAX nor su_UI32_MAX (please report this)
# endif
# define su_UIZ_MAX SIZE_MAX
#endif
#ifndef PRIuZ
# define PRIuZ "lu"
# define PRIdZ "ld"
su_MCTA(sizeof(size_t) == sizeof(unsigned long),
    "Format string mismatch, compile with ISO C99 compiler (-std=c99)!")
# define su_UIZ_MAX ULONG_MAX
#endif

#ifdef UINTPTR_MAX
typedef uintptr_t su_uip;
typedef intptr_t su_sip;
#else
# ifdef SIZE_MAX
typedef su_uiz su_uip;
typedef su_siz su_sip;
# else
typedef su_ul su_uip;
typedef su_sl su_sip;
# endif
#endif

su_C_DECL_END
/* TYPES }}} */
/* SUPPORT FUNS+ {{{ */
su_C_DECL_BEGIN

enum su_log_level{
   /* No _EMERG: use su_panic(), then */
   su_LOGL_ALERT,    /* Action must be taken immediately */
   su_LOGL_CRIT,     /* Criticial conditions */
   su_LOGL_ERR,      /* Error conditions */
   su_LOGL_WARNING,  /* Warning conditions */
   su_LOGL_NOTICE,   /* Normal but significant condition */
   su_LOGL_INFO,     /* Informational */
   su_LOGL_DEBUG     /* Debug-level message */
};

enum su_state_flags{
   su__SF_LOGL_MASK = 0xFFu,
   su_SF_DEBUG = 1u<<8,
   su_SF_MEMORY_DEBUG = 1u<<9,

   su_SF_VERBOSE = 1u<<10

   su_SF_D_V = su_SF_DEBUG | su_SF_VERBOSE
};

su_c_decl uiz su_state; /* enum su_state_flags */

/* Log functions of various sort.
 * The vp is a &va_list */
su_c_decl void su_log(enum su_log_level lvl, char const *fmt, ...);
su_c_decl void su_vlog(enum su_log_level lvl, char const *fmt, void *vp);

su_c_decl void su_alert(char const *fmt, ...);
su_c_decl void su_err(char const *fmt, ...);
/* Also logs if su_state&su_SF_D_V */
su_c_decl void su_debug(char const *fmt, ...);

/* Log and abort */
su_c_decl void su_panic(char const *fmt, ...);

/* perror(3), but uses su_log */
su_c_decl void su_perr(char const *cp, si32 errno_or_0);

su_C_DECL_END
/* SUPPORT FUNS+ }}} */

#endif /* su_PRIMARY_H */
/* s-it-mode */
