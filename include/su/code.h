/*@ Code of the basic infrastructure (POD types, macros etc.) and functions.
 *@ And main documentation entry point, as below.
 *@ - Reacts upon su_HAVE_DEBUG, su_HAVE_DEVEL, and NDEBUG.
 *@ - Some macros require su_FILE to be defined to a literal.
 *@ - Define su_MASTER to inject what is to be injected once; for example,
 *@   it enables su_M*CTA() compile time assertions.
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
#ifndef su_CODE_H
#define su_CODE_H

#include <su/config.h>

/*!@
 * TODO blablabla
 *
 * Some introductional remarks:
 *
 * \list
 * \li
 * Datatype overflow errors and out-of-memory situations are usually detected
 * and result in abortions (via C<LOG_EMERG> logs).
 * Alternatively errors are reported to callers.
 * The actual mode of operation is configurable via @<su_state_set()> and
 * @<su_state_clear()>, and often the default can also be changed by-call or
 * by-object.
 *
 * C++ object creation failure via @<su_MEM_NEW()> etc. will however always
 * cause program abortion due to standard imposed execution flow.
 * This can be worked around by using @<su_MEM_NEW_HEAP()> as appropriate.
 *
 * \li
 * Most collections and string objects work on 32-bit (or even 31-bit) lengths
 * a.k.a. counts a.k.a. sizes.
 * For simplicity of use, and because datatype overflow is a handled case, the
 * user interface very often uses C<su_uz< (i.e., C<size_t>).
 * Other behaviour is explicitly declared with a "big" prefix, as in
 * "biglist", but none such object does yet exist.
 * \end
 */

/* OS {{{ */

#define su_OS_CYGWIN 0
#define su_OS_DARWIN 0
#define su_OS_DRAGONFLY 0
#define su_OS_EMX 0
#define su_OS_FREEBSD 0
#define su_OS_LINUX 0
#define su_OS_MINIX 0
#define su_OS_MSDOS 0
#define su_OS_NETBSD 0
#define su_OS_OPENBSD 0
#define su_OS_SOLARIS 0
#define su_OS_SUNOS 0
#define su_OS_WIN32 0
#define su_OS_WIN64 0

#if 0
#elif defined __CYGWIN__
# undef su_OS_CYGWIN
# define su_OS_CYGWIN 1
#elif defined DARWIN || defined _DARWIN
# undef su_OS_DARWIN
# define su_OS_DARWIN 1
#elif defined __DragonFly__
# undef su_OS_DRAGONFLY
# define su_OS_DRAGONFLY 1
#elif defined __EMX__
# undef su_OS_EMX
# define su_OS_EMX 1
#elif defined __FreeBSD__
# undef su_OS_FREEBSD
# define su_OS_FREEBSD 1
#elif defined __linux__ || defined __linux
# undef su_OS_LINUX
# define su_OS_LINUX 1
#elif defined __minix
# undef su_OS_MINIX
# define su_OS_MINIX 1
#elif defined __MSDOS__
# undef su_OS_MSDOS
# define su_OS_MSDOS 1
#elif defined __NetBSD__
# undef su_OS_NETBSD
# define su_OS_NETBSD 1
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
/* LANG {{{ */

#ifndef __cplusplus
# define su_C_LANG 1
# define su_C_DECL_BEGIN
# define su_C_DECL_END

   /* Casts */
# define su_S(T,I) ((T)(I))
# define su_R(T,I) ((T)(I))
# define su_C(T,I) ((T)(I))

# define su_NIL ((void*)0)
#else
# define su_C_LANG 0
# define su_C_DECL_BEGIN extern "C" {
# define su_C_DECL_END }
# define su_HAVE_NSPC 0
# define su_NSPC_BEGIN(X) //namespace X {
# define su_NSPC_END(X) //}
# define su_NSPC_USE(X) //using namespace X;
# define su_NSPC(X) /*X*/::

   /* Disable copy-construction and assigment of class */
# define su_CLASS_NO_COPY(C) private:C(C const &);C &operator=(C const &);

   /* C++ only allows those at the declaration, not the definition */
# define su_PUB
# define su_PRO
# define su_PRI
# define su_STA
# define su_VIR
# define su_OVW
   /* This is for the declarator only */
# if __cplusplus + 0 < 201103L
#  define su_OVR
# else
#  define su_OVR override
# endif

   /* Casts */
# define su_S(T,I) static_cast<T>(I)
# define su_R(T,I) reinterpret_cast<T>(I)
# define su_C(T,I) const_cast<T>(I)

# define su_NIL (0L)
#endif /* __cplusplus */

/*!@ The \#su_state_err() mechanism can be configured to not cause abortion
 * in case of datatype overflow and out-of-memory situations.
 * Most functions return error conditions to pass them to their caller,
 * but this is impossible for, e.g., C++ copy-constructors and assignment
 * operators.
 * And \SU does not use exceptions.
 * So if those errors could occur and thus be hidden, the prototype is marked
 * with this "keyword" so that callers can decide whether they want to take
 * alternative routes to come to the desired result or not. */
#define su_SHADOW

/* "su_export myfun()", "class su_export */
#if su_OS_WIN32 || su_OS_WIN64
# define su_EXPORT __declspec((dllexport))
# define su_EXPORT_DATA __declspec((dllexport))
# define su_IMPORT __declspec((dllimport))
# define su_IMPORT_DATA __declspec((dllimport))
#else
# define su_EXPORT /*extern*/
# define su_EXPORT_DATA extern
# define su_IMPORT /*extern*/
# define su_IMPORT_DATA extern
#endif

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
#ifdef su_MASTER
# define su_MCTA(T,M) su_CTA(T, M);
#else
# define su_MCTA(T,M)
#endif

/* LANG }}} */
/* CC {{{ */

#define su_CC_CLANG 0
#define su_PREREQ_CLANG(X,Y) 0
#define su_CC_GCC 0
#define su_PREREQ_GCC(X,Y) 0
#define su_CC_PCC 0
#define su_PREREQ_PCC(X,Y) 0
#define su_CC_SUNPROC 0
#define su_PREREQ_SUNPROC(X,Y) 0
#define su_CC_TINYC 0
#define su_PREREQ_TINYC(X,Y) 0

#ifdef __clang__
# undef su_CC_CLANG
# undef su_PREREQ_CLANG
# define su_CC_CLANG 1
# define su_PREREQ_CLANG(X,Y) \
   (__clang_major__ + 0 > (X) || \
    (__clang_major__ + 0 == (X) && __clang_minor__ + 0 >= (Y)))
# define su_CC_EXTEN __extension__
# define su_CC_PACKED __attribute__((packed))
#elif defined __GNUC__
# undef su_CC_GCC
# undef su_PREREQ_GCC
# define su_CC_GCC 1
# define su_PREREQ_GCC(X,Y) \
   (__GNUC__ + 0 > (X) || (__GNUC__ + 0 == (X) && __GNUC_MINOR__ + 0 >= (Y)))
# define su_CC_EXTEN __extension__
# define su_CC_PACKED __attribute__((packed))
#endif
#elif defined __PCC__
# undef su_CC_PCC
# undef su_PREREQ_PCC
# define su_CC_PCC 1
# define su_PREREQ_PCC(X,Y) \
   (__PCC__ + 0 > (X) || (__PCC__ + 0 == (X) && __PCC_MINOR__ + 0 >= (Y)))
# define su_CC_EXTEN __extension__
# define su_CC_PACKED __attribute__((packed))
#elif defined __SUNPRO_C
# undef su_CC_SUNPROC
# define su_CC_SUNPROC 1
# define su_CC_PACKED TODO: PACKED attribute not supported for SunPro C
#elif defined __TINYC__
# undef su_CC_TINYC
# define su_CC_TINYC 1
# define su_CC_EXTEN /* __extension__ (ignored) */
# define su_CC_PACKED __attribute__((packed))
#elif !defined su_CC_IGNORE_UNKNOWN
# error SU: This compiler is not yet supported.
# error SU: To continue with your CFLAGS etc., define su_CC_IGNORE_UNKNOWN.
# error SU: It may be necessary to define su_CC_PACKED to a statement that
# error SU: enables structure packing; it may not be a #pragma, but a _Pragma
#endif

#ifndef su_CC_EXTEN
# define su_CC_EXTEN
#endif
#ifndef su_CC_PACKED
# define su_CC_PACKED TODO: PACKED attribute not supported for this compiler
#endif

/* Suppress some technical warnings via #pragma's unless developing.
 * XXX Wild guesses: clang(1) 1.7 and (OpenBSD) gcc(1) 4.2.1 don't work */
#ifndef HAVE_DEVEL
# if su_PREREQ_CLANG(3, 4)
/*#  pragma clang diagnostic ignored "-Wformat"*/
#  pragma clang diagnostic ignored "-Wunused-result"
# elif su_PREREQ_GCC(4, 7) || su_CC_PCC || su_CC_TINYC
/*#  pragma GCC diagnostic ignored "-Wformat"*/
#  pragma GCC diagnostic ignored "-Wunused-result"
# endif
#endif

/* Function name */
#if defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
# define su_FUN __func__
#elif su_CC_CLANG || su_PREREQ_GCC(3, 4) || su_CC_PCC || su_CC_TINYC
# define su_FUN __extension__ __FUNCTION__
#else
# define su_FUN su_empty /* Something that is not a literal */
#endif

/* inline keyword */
#if rf_C_LANG
# if su_CC_CLANG || su_CC_GCC || su_CC_PCC || su_CC_TINYC
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
#elif su_CC_CLANG || su_PREREQ_GCC(2, 96) || su_CC_PCC || su_CC_TINYC
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
#define su_IS_POW2(X) ((((X) - 1) & (X)) == 0)

/* Align something to a size/boundary that cannot cause just any problem.
 * Unfortunately this software uses floating-point... */
#define su_ALIGN(X) (((X) + 2*su__ALIGN_SIZE) & ~((2*su__ALIGN_SIZE) - 1))
#define su_ALIGN_SMALL(X) (((X) + su__ALIGN_SIZE) & ~(su__ALIGN_SIZE - 1))
#define su__ALIGN_SIZE \
   su_MAX(sizeof(su_uz), su_MAX(sizeof(void*), sizeof(su_u64))) /* XXX FP? */

/* ASSERT comes from code-{in,ou}.h (support function below) */

/* Create a bit mask for the bit range LO..HI -- HI can't use highest bit! */
#define su_BITENUM_MASK(LO,HI) (((1u << ((HI) + 1)) - 1) & ~((1u << (LO)) - 1))

/* For injection macros like su_DBG(), NDBG, DBGOR, 64, 32, 6432 */
#define su_COMMA ,

/* Debug injections */
#if defined NDEBUG || !defined su_HAVE_DEBUG
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

/* NYD comes from code-{in,ou}.h (support function below) */

/* Pointer to size_t */
#define su_PTR2UZ(X) su_S(su_uz,su_S(su_up,X))

/* Pointer comparison */
#define su_PTRCMP(A,C,B) (su_S(su_up,A) C su_S(su_up,B))

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
 * T is one of our homebrew integers, e.g., UCMP(32, su_ABS(n), >, wleft).
 * Note: doesn't sign-extend correctly, that's still up to the caller */
#if su_C_LANG
# define su_UCMP(T,A,C,B) (su_S(su_ ## u ## T,A) C su_S(su_ ## u ## T,B))
#else
# define su_UCMP(T,A,C,B) (su_S(su_ ## u ## T ##,A) C su_S(su_ ## u ## T,B))
#endif

/* Casts-away (*NOT* cast-away) */
#if su_C_LANG
# define su_UNCONST(P) su_S(void*,su_S(su_up,void const*,P))
# define su_UNVOLATILE(P) su_S(void*,su_S(su_up,su_S(void volatile*,P)))
  /* To avoid warnings with modern compilers for "char*i; *(s32_t*)i=;" */
# define su_UNALIGN(T,P) su_S(T,su_S(su_up,P))
# define su_UNXXX(T,C,P) su_S(T,su_S(su_up,su_S(C,P)))
#endif

/* Avoid "may be used uninitialized" warnings */
#if !defined NDEBUG && (defined su_HAVE_DEBUG || defined su_HAVE_DEVEL)
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
   ((X) == 0 ? sizeof(su_uz) \
    : (su_S(su_sz,X) < 0 ? sizeof(su_uz) - su_ABS(X) : su_S(su_uz,X)))
# define su_VSTRUCT_SIZEOF(T,F) (sizeof(T) - su_SIZEOF_FIELD(T, F))
#endif

/* SUPPORT MACROS+ }}} */

/* We are ready to start using our own style */
#include <inttypes.h> /* TODO try to replace with */
#include <limits.h> /* TODO config-time thing! */

#define su_HEADER
#include <su/code-in.h>

/* POD TYPE SUPPORT TODO maybe configure-time, from a su/config.h?! {{{ */
C_DECL_BEGIN

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
# define su_U8_MAX UINT8_MAX
# define su_S8_MIN INT8_MIN
# define su_S8_MAX INT8_MAX
typedef uint8_t su_u8;
typedef int8_t su_s8;
#elif UCHAR_MAX != 255
# error UCHAR_MAX must be 255
#else
# define su_U8_MAX UCHAR_MAX
# define su_S8_MIN CHAR_MIN
# define su_S8_MAX CHAR_MAX
typedef unsigned char su_u8;
typedef signed char su_s8;
#endif

#if !defined PRIu8 || !defined PRId8
# undef PRIu8
# undef PRId8
# define PRIu8 "hhu"
# define PRId8 "hhd"
#endif

#ifdef UINT16_MAX
# define su_U16_MAX UINT16_MAX
# define su_S16_MIN INT16_MIN
# define su_S16_MAX INT16_MAX
typedef uint16_t su_u16;
typedef int16_t su_s16;
#elif USHRT_MAX != 0xFFFFu
# error USHRT_MAX must be 0xFFFF
#else
# define su_U16_MAX USHRT_MAX
# define su_S16_MIN SHRT_MIN
# define su_S16_MAX SHRT_MAX
typedef unsigned short su_u16;
typedef signed short su_s16;
#endif

#if !defined PRIu16 || !defined PRId16
# undef PRIu16
# undef PRId16
# if su_U16_MAX == UINT_MAX
#  define PRIu16 "u"
#  define PRId16 "d"
# else
#  define PRIu16 "hu"
#  define PRId16 "hd"
# endif
#endif

#ifdef UINT32_MAX
# define su_U32_MAX UINT32_MAX
# define su_S32_MIN INT32_MIN
# define su_S32_MAX INT32_MAX
typedef uint32_t su_u32;
typedef int32_t su_s32;
#elif ULONG_MAX == 0xFFFFFFFFu
# define su_U32_MAX ULONG_MAX
# define su_S32_MIN LONG_MIN
# define su_S32_MAX LONG_MAX
typedef unsigned long int su_u32;
typedef signed long int su_s32;
#elif UINT_MAX != 0xFFFFFFFFu
# error UINT_MAX must be 0xFFFFFFFF
#else
# define su_U32_MAX UINT_MAX
# define su_S32_MIN INT_MIN
# define su_S32_MAX INT_MAX
typedef unsigned int su_u32;
typedef signed int su_s32;
#endif

#if !defined PRIu32 || !defined PRId32
# undef PRIu32
# undef PRId32
# if su_U32_MAX == ULONG_MAX
#  define PRIu32 "lu"
#  define PRId32 "ld"
# else
#  define PRIu32 "u"
#  define PRId32 "d"
# endif
#endif

#ifdef UINT64_MAX
# define su_U64_MAX UINT64_MAX
# define su_S64_MIN INT64_MIN
# define su_S64_MAX INT64_MAX
# define su_S64_C(C) INT64_C(C)
# define su_U64_C(C) UINT64_C(C)
typedef uint64_t su_u64;
typedef int64_t su_s64;
#elif ULONG_MAX <= 0xFFFFFFFFu
# if !defined ULLONG_MAX
#  error We need a 64 bit integer
# else
#  define su_U64_MAX ULLONG_MAX
#  define su_S64_MIN LLONG_MIN
#  define su_S64_MAX LLONG_MAX
#  define su_S64_C(C) su_CONCAT(C, ll)
#  define su_U64_C(C) su_CONCAT(C, ull)
su_CC_EXTEN typedef unsigned long long su_u64;
su_CC_EXTEN typedef signed long long su_s64;
# endif
#else
# define su_U64_MAX ULONG_MAX
# define su_S64_MIN LONG_MIN
# define su_S64_MAX LONG_MAX
# define su_S64_C(C) su_CONCAT(C, l)
# define su_U64_C(C) su_CONCAT(C, ul)
typedef unsigned long su_u64;
typedef signed long su_s64;
#endif

#if !defined PRIu64 || !defined PRId64 || !defined PRIX64 || !defined PRIo64
# undef PRIu64
# undef PRId64
# undef PRIX64
# undef PRIo64
# if defined ULLONG_MAX && su_U64_MAX == ULLONG_MAX
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

/* (So that we can use UCMP() for size_t comparison, too) */
typedef size_t su_uz;
typedef ssize_t su_sz;

#undef PRIuZ
#undef PRIdZ
#if defined __STDC_VERSION__ && __STDC_VERSION__ + 0 >= 199901L
# define PRIuZ "zu"
# define PRIdZ "zd"
# define su_UZ_MAX SIZE_MAX
#elif defined SIZE_MAX
   /* UnixWare has size_t as unsigned as required but uses a signed limit
    * constant (which is thus false!) */
# if SIZE_MAX == U64_MAX || SIZE_MAX == S64_MAX
#  define PRIuZ PRIu64
#  define PRIdZ PRId64
MCTA(sizeof(size_t) == sizeof(u64),
   "Format string mismatch, compile with ISO C99 compiler (-std=c99)!")
# elif SIZE_MAX == U32_MAX || SIZE_MAX == S32_MAX
#  define PRIuZ PRIu32
#  define PRIdZ PRId32
MCTA(sizeof(size_t) == sizeof(u32),
   "Format string mismatch, compile with ISO C99 compiler (-std=c99)!")
# else
#  error SIZE_MAX is neither U64_MAX nor U32_MAX (please report this)
# endif
# define su_UZ_MAX SIZE_MAX
#endif
#ifndef PRIuZ
# define PRIuZ "lu"
# define PRIdZ "ld"
MCTA(sizeof(size_t) == sizeof(unsigned long),
   "Format string mismatch, compile with ISO C99 compiler (-std=c99)!")
# define su_UZ_MAX ULONG_MAX
#endif
/* The signed equivalence is not really compliant to the standard */
#if UZ_MAX == U32_MAX || UZ_MAX == S32_MAX
# define su_SZ_MIN su_S32_MIN
# define su_SZ_MAX su_S32_MAX
# define su_UZ_BITS 32
# define su_64BIT 0
# define su_64(X)
# define su_32(X) X
# define su_6432(X,Y) Y
#elif su_UZ_MAX == su_U64_MAX
# define su_SZ_MIN su_S64_MIN
# define su_SZ_MAX su_S64_MAX
# define su_UZ_BITS 64
# define su_64BIT 1
# define su_64(X) X
# define su_32(X)
# define su_6432(X,Y) X
#else
# error I cannot handle this maximum value of size_t
#endif

#ifdef UINTPTR_MAX
typedef uintptr_t su_up;
typedef intptr_t su_sp;
#else
# ifdef SIZE_MAX
typedef su_uz su_up;
typedef su_sz su_sp;
# else
typedef su_ul su_up;
typedef su_sl su_sp;
# endif
#endif

enum {su_FAL0, su_TRU1, su_TRUM1 = -1};
typedef su_s8 su_boole;

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

// Define in-namespace wrappers for C types.  code-in/ou do not define short
// names for POD when used from within C++
typedef su_ul ul;
typedef su_ui ui;
typedef su_us us;
typedef su_uc uc;

typedef su_sl sl;
typedef su_si si;
typedef su_ss ss;
typedef su_sc sc;

typedef su_u8 u8;
typedef su_s8 s8;
typedef su_u16 u16;
typedef su_s16 s16;
typedef su_u32 u32;
typedef su_s32 s32;
typedef su_u64 u64;
typedef su_s64 s64;

typedef su_uz uz;
typedef su_sz sz;

typedef su_up up;
typedef su_sp sp;

enum {FAL0 = su_FAL0, TRU1 = su_TRU1, TRUM1 = su_TRUM1};
typedef su_boole boole;

NSPC_END(su)
#endif /* !C_LANG */
/* POD TYPE SUPPORT }}} */
/* BASIC TYPE TRAITS {{{ */
C_DECL_BEGIN

struct su_toolbox;
// plus PTF typedefs

typedef void *(*su_new_fun)(void);
typedef void *(*su_clone_fun)(void const *t);
typedef void (*su_delete_fun)(void *self);
// It should not be assumed self has been updated, use return value instead
typedef void *(*su_assign_fun)(void *self, void const *t);
typedef su_sz (*su_compare_fun)(void const *a, void const *b);
typedef su_uz (*su_hash_fun)(void const *self);

// This needs to be binary compatible with su::{toolbox,type_toolbox<T>}!
struct su_toolbox{
   su_new_fun tb_clone;
   su_delete_fun tb_delete;
   su_assign_fun tb_assign;
   su_compare_fun tb_compare;
   su_hash_fun tb_hash;
};
#define su_TOOLBOX_I9R(CLONE,DELETE,ASSIGN,COMPARE,HASH) {\
   su_FIELD_INITN(tb_clone) (su_new_fun)CLONE,\
   su_FIELD_INITN(tb_delete) (su_delete_fun)DELETE,\
   su_FIELD_INITN(tb_assign) (su_assign_fun)ASSIGN,\
   su_FIELD_INITN(tb_compare) (su_compare_fun)COMPARE,\
   su_FIELD_INITN(tb_hash) (su_hash_fun)HASH\
}

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

template<class T> class type_traits;
template<class T> struct type_toolbox;
// Plus C wrapper typedef

// External forward, defined in a-t-t.h.
template<class T> class auto_type_toolbox;

typedef su_toolbox toolbox;

template<class T>
class type_traits{
public:
   typedef T type;
   typedef T *ptr;
   typedef T const const_type;
   typedef T const *const_ptr;

   typedef NSPC(su)type_toolbox<type> toolbox;
   typedef NSPC(su)auto_type_toolbox<type> auto_type_toolbox;

   // Non-pointer types are by default own-guessed, pointer based ones not
   static boole const ownguess = TRU1;
   // Ditto, associative collections, keys.
   static boole const ownguess_key = TRU1;

   static void *to_vptr(const_ptr t) {return C(void*,S(void const*,t));}
   static void const *to_const_vptr(const_ptr t) {return t;}

   static ptr to_ptr(void const *t) {return C(ptr,S(const_ptr,t));}
   static const_ptr to_const_ptr(void const *t) {return S(const_ptr,t);}
};

// Some specializations
template<class T>
class type_traits<const T>{ // (ugly, but required for some node based colls..)
public:
   typedef T type;
   typedef T *ptr;
   typedef T const const_type;
   typedef T const *const_ptr;
   typedef NSPC(su)type_toolbox<type> toolbox;
   typedef NSPC(su)auto_type_toolbox<type> auto_type_toolbox;

   static boole const ownguess = FAL0;
   static boole const ownguess_key = TRU1;

   static void *to_vptr(const_ptr t) {return C(ptr,t);}
   static void const *to_const_vptr(const_ptr t) {return t;}
   static ptr to_ptr(void const *t) {return C(ptr,S(const_ptr,t));}
   static const_ptr to_const_ptr(void const *t) {return S(const_ptr,t);}
};

template<class T>
class type_traits<T *>{
public:
   typedef T type;
   typedef T *ptr;
   typedef T const const_type;
   typedef T const *const_ptr;
   typedef NSPC(su)type_toolbox<type> toolbox;
   typedef NSPC(su)auto_type_toolbox<type> auto_type_toolbox;

   static boole const ownguess = FAL0;
   static boole const ownguess_key = TRU1;

   static void *to_vptr(const_ptr t) {return C(ptr,t);}
   static void const *to_const_vptr(const_ptr t) {return t;}
   static ptr to_ptr(void const *t) {return C(ptr,S(const_ptr,t));}
   static const_ptr to_const_ptr(void const *t) {return S(const_ptr,t);}
};

template<>
class type_traits<void *>{
public:
   typedef void *type;
   typedef void *ptr;
   typedef void const *const_type;
   typedef void const *const_ptr;
   typedef NSPC(su)toolbox toolbox;
   typedef NSPC(su)auto_type_toolbox<void *> auto_type_toolbox;

   static boole const ownguess = FAL0;
   static boole const ownguess_key = FAL0;

   static void *to_vptr(const_ptr t) {return C(ptr,t);}
   static void const *to_const_vptr(const_ptr t) {return t;}
   static ptr to_ptr(void const *t) {return C(ptr,S(const_ptr,t));}
   static const_ptr to_const_ptr(void const *t) {return S(const_ptr,t);}
};

template<>
class type_traits<void const *>{
public:
   typedef void const *type;
   typedef void const *ptr;
   typedef void const *const_type;
   typedef void const *const_ptr;
   typedef NSPC(su)toolbox toolbox;
   typedef NSPC(su)auto_type_toolbox<void const *> auto_type_toolbox;

   static boole const ownguess = FAL0;
   static boole const ownguess_key = FAL0;

   static void *to_vptr(const_ptr t) {return C(void*,t);}
   static void const *to_const_vptr(const_ptr t) {return t;}
   static ptr to_ptr(void const *t) {return C(void*,t);}
   static const_ptr to_const_ptr(void const *t) {return t;}
};

template<>
class type_traits<char *>{
public:
   typedef char *type;
   typedef char *ptr;
   typedef char const *const_type;
   typedef char const *const_ptr;
   typedef NSPC(su)type_toolbox<type> toolbox;
   typedef NSPC(su)auto_type_toolbox<type> auto_type_toolbox;

   static boole const ownguess = FAL0;
   static boole const ownguess_key = TRU1;

   static void *to_vptr(const_ptr t) {return C(ptr,t);}
   static void const *to_const_vptr(const_ptr t) {return t;}
   static ptr to_ptr(void const *t) {return C(ptr,S(const_ptr,t));}
   static const_ptr to_const_ptr(void const *t) {return S(const_ptr,t);}
};

template<>
class type_traits<char const *>{
public:
   typedef char const *type;
   typedef char const *ptr;
   typedef char const *const_type;
   typedef char const *const_ptr;
   typedef NSPC(su)type_toolbox<type> toolbox;
   typedef NSPC(su)auto_type_toolbox<type> auto_type_toolbox;

   static boole const ownguess = FAL0;
   static boole const ownguess_key = TRU1;

   static void *to_vptr(const_ptr t) {return C(char*,t);}
   static void const *to_const_vptr(const_ptr t) {return t;}
   static ptr to_ptr(void const *t) {return C(char*,S(const_ptr,t));}
   static const_ptr to_const_ptr(void const *t) {return S(const_ptr,t);}
};

// This needs to be binary compatible with toolbox (and su_toolbox)!
template<class T>
struct type_toolbox{
   typedef NSPC(su)type_traits<T> type_traits;

   typename type_traits::ptr (*ttb_clone)(typename type_traits::const_ptr t);
   void (*ttb_delete)(typename type_traits::ptr self);
   typename type_traits::ptr (*ttb_assign)(typename type_traits::ptr self,
         typename type_traits::const_ptr t);
   sz (*ttb_compare)(typename type_traits::const_ptr self,
         typename type_traits::const_ptr t);
   uz (*ttb_hash)(typename type_traits::const_ptr self);
};
#define su_TYPE_TOOLBOX_I9R(CLONE,DELETE,ASSIGN,COMPARE,HASH) {\
   su_FIELD_INITN(ttb_clone) CLONE,\
   su_FIELD_INITN(ttb_delete) DELETE,\
   su_FIELD_INITN(ttb_assign) ASSIGN,\
   su_FIELD_INITN(ttb_compare) COMPARE,\
   su_FIELD_INITN(ttb_hash) HASH\
}

// abc,clip,max,min,pow2 -- the C macros are in SUPPORT MACROS+
template<class T> inline T get_abs(T const &a) {return su_ABS(a);}
template<class T>
inline T const &get_clip(T const &a, T const &min, T const &max){
   return su_CLIP(a, min, max);
}
template<class T>
inline T const &get_max(T const &a, T const &b) {return su_MAX(a, b);}
template<class T>
inline T const &get_min(T const &a, T const &b) {return su_MIN(a, b);}
template<class T> inline int is_pow2(T const &a) {return su_IS_POW2(a);}

NSPC_END(su)
#endif /* !C_LANG */
/* BASIC TYPE TRAITS }}} */
/* BASIC C/C++ INTERFACE (SYMBOLS) {{{ */
C_DECL_BEGIN

/* Byte order of host.  su_ENDIAN one of those two, comes from config.h */
#define su_ENDIAN_LITTLE 1234
#define su_ENDIAN_BIG 4321

/* Log priorities, for simplicity of use without _LEVEL or _LVL prefix */
enum su_log_level{
   su_LOG_EMERG,  /* System is unusable (abort()s the program) */
   su_LOG_ALERT,  /* Action must be taken immediately */
   su_LOG_CRIT,   /* Criticial conditions */
   su_LOG_ERR,    /* Error conditions */
   su_LOG_WARN,   /* Warning conditions */
   su_LOG_NOTICE, /* Normal but significant condition */
   su_LOG_INFO,   /* Informational */
   su_LOG_DEBUG   /* Debug-level message */
};

enum su_state_flags{
   /* enum su_log_level is first "member" */
   su__STATE_LOG_MASK = 0xFFu,

   su_STATE_DEBUG = 1u<<8,
   su_STATE_MEMORY_DEBUG = 1u<<9,
   su_STATE_VERBOSE = 1u<<10,
   su__STATE_D_V = su_STATE_DEBUG | su_STATE_VERBOSE,

   /* By default out-of-memory situations, or container and string etc.
    * insertions etc. which cause count/offset result in LOG_EMERGs.
    * This default can be changed by setting the corresponding su_state*()
    * bit, the functions will return the corresponding error code, then.
    * ERR_PASS may be used only as an argument to state_err(), when LOG_EMERGs
    * are to be avoided at all cost (only LOG_DEBUG logs happen then).
    * Likewise, ERR_NOPASS, to be used when state_err() must not return.
    * Ditto, ERR_NOERRNO, to be used when su_err_no() shall not be set */
   su_STATE_ERR_NOMEM = 1u<<26,
   su_STATE_ERR_OVERFLOW = 1u<<27,
   su_STATE_ERR_TYPE_MASK = su_STATE_ERR_NOMEM | su_STATE_ERR_OVERFLOW,

   su_STATE_ERR_PASS = 1u<<28,
   su_STATE_ERR_NOPASS = 1u<<29,
   su_STATE_ERR_NOERRNO = 1u<<30,
   su__STATE_ERR_MASK = su_STATE_ERR_TYPE_MASK |
         su_STATE_ERR_PASS | su_STATE_ERR_NOPASS | su_STATE_ERR_NOERRNO,

   su__STATE_USER_MASK = ~(su__STATE_LOG_MASK |
         su_STATE_ERR_PASS | su_STATE_ERR_NOPASS | su_STATE_ERR_NOERRNO)
};
MCTA(su_LOG_DEBUG <= su__STATE_LOG_MASK, "Bit ranges may not overlap");

/* The byte order mark (U+FEFF) in host byte order */
EXPORT_DATA u16 const su_bom;

/* The empty string */
EXPORT_DATA char const su_empty[1];

/* (Not yet) Internal enum su_state_flags bit carrier */
EXPORT_DATA uz su__state;

/* Set to the name of the program to create a common log message prefix */
EXPORT_DATA char const *su_program;

/* Interaction with the SU library "enum su_state_flags" machine.
 * The last to be called once one of the STATE_ERR* conditions occurred,
 * it returns (if it returns) the corresponding enum su_err_number */
#define su_state_has(F) \
   ((su__state & (su_S(su_uz,F) & su__STATE_USER_MASK)) != 0)
#define su_state_set(F) \
   (su__state |= su_S(su_uz,F) & su__STATE_USER_MASK, (void)NIL)
#define su_state_clear(F) \
   (su__state &= ~(su_S(su_uz,F) & su__STATE_USER_MASK), (void)NIL)
EXPORT s32 su_state_err(uz state, char const *msg_or_nil);

/* Return string(s) describing C error number eno */
EXPORT char const *su_err_doc(s32 eno);
EXPORT char const *su_err_name(s32 eno);
#define su_err_no() errno /* TODO */
#define su_err_set_no(V) do{su_err_no = V;}while(0) /* TODO */
#define su_err_no_via_errno() su_err_no /* TODO */
#define su_err_set_errno(V) do{su_err_no = V;}while(0) /* TODO */

/* Try to map an error name to an error number.
 * Returns the fallback error as a negative value if none found */
EXPORT s32 su_err_from_name(char const *name);

/* Log functions of various sort.
 * Regardless of the level these also log if STATE_DEBUG|STATE_VERBOSE.
 * The vp is a &va_list */
#define su_log_level_get() (su__state & su__STATE_LOG_MASK)
#define su_log_level_set(F) \
   (su__state = (su__state & su__STATE_USER_MASK) |\
         (su_S(su_uz,F) & su__STATE_LOG_MASK), (void)NIL)
EXPORT void su_log(enum su_log_level lvl, char const *fmt, ...);
EXPORT void su_vlog(enum su_log_level lvl, char const *fmt, void *vp);

/* Like perror(3) */
EXPORT void su_perr(char const *msg, s32 eno_or_0);

/* With a false crash this only logs */
EXPORT void su_assert(char const *expr, char const *file, s32 line,
      char const *fun, boole crash);

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

// All instanceless static encapsulators.
// P.S.: endian is not in POD section above since it introduces .RODATA
class endian;
class err;
class log;
class state;

class endian{
public:
   enum type{
      little = su_ENDIAN_LITTLE,
      big = su_ENDIAN_BIG
   };

#if su_ENDIAN == su_ENDIAN_BIG
   static type const host = big;
   static type const nothost = little;
#else
   static type const host = little;
   static type const nothost = big;
#endif

   // The byte order mark (U+FEFF) in host byte order
   static u16 const bom;
};

class err{
public:
   static s32 no(void) {return su_err_no();}
   static void set_no(s32 nerr) {su_err_set_no(nerr);}
   static char const *doc(s32 eno) {return su_err_doc(eno);}
   static char const *name(s32 eno) {return su_err_name(eno);}
   static s32 from_name(char const *name) {return su_err_from_name(name);}

   static s32 no_via_errno(void) {return su_err_no_via_errno();}
   static void set_errno(s32 nerr); {su_err_set_errno(nerr);}
};

class EXPORT log{
public:
   // Log priorities, for simplicity of use without _LEVEL or _LVL prefix
   enum level{
      emerg = su_LOG_EMERG,   // System is unusable (abort()s the program)
      alert = su_LOG_ALERT,   // Action must be taken immediately
      crit = su_LOG_CRIT,     // Criticial conditions
      err = su_LOG_ERR,       // Error conditions
      warn = su_LOG_WARN,     // Warning conditions
      notice = su_LOG_NOTICE, // Normal but significant condition
      info = su_LOG_INFO,     // Informational
      debug = su_LOG_DEBUG    // Debug-level message
   };

   // Log functions of various sort.
   // Regardless of the level these also log if state_debug|state_verbose.
   // The vp is a &va_list
   static level level_get(void) {return S(level,su_log_level_get());}
   static void level_set(level lvl) {su_log_level_set(lvl);}
   static void log(level lvl, char const *fmt, ...);
   static void vlog(level lvl, char const *fmt, void *vp){
      su_vlog(S(enum su_log_level,lvl), fmt, vp);
   }

   // Like perror(3)
   static void perr(char const *msg, s32 eno_or_0) {su_perr(msg, eno_or_0);}
};

class state{
public:
   enum flags{
      debug = su_STATE_DEBUG,
      memory_debug = su_STATE_MEMORY_DEBUG,
      verbose = su_STATE_VERBOSE,

      // By default out-of-memory situations, or container and string etc.
      // insertions etc. which cause count/offset overflow cause panics.
      // This default can be changed by setting the corresponding state bit,
      // the functions will return the corresponding error code, then.
      // err_pass may be used only as an argument to state_err(), when
      // log::emerg's are to be avoided at all cost (only log::debug logs
      // happen then).
      // Likewise, err_nopass, to be used when state::err() must not return.
      // Ditto, err_noerrno, to be used when err::no() shall not be set
      err_nomem = su_STATE_ERR_NOMEM,
      err_overflow = su_STATE_ERR_OVERFLOW,
      err_type_mask = su_STATE_ERR_TYPE_MASK,

      err_pass = su_STATE_ERR_PASS,
      err_nopass = su_STATE_ERR_NOPASS,
      err_noerrno = su_STATE_ERR_NOERRNO
   };

   // Interaction with the SU library "enum state" machine.
   // The last to be called once one of the state_err* conditions occurred
   // it returns (if it returns) the corresponding enum su_err_number
   static boole has(uz state) {return su_state_has(state);}
   static void set(uz state) {su_state_set(state);}
   static void clear(uz state) {su_state_clear(state);}
   static s32 err(uz state, char const *msg_or_nil=NIL){
      return su_state_err(state, msg_or_nil);
   }
};

NSPC_END(su)
#endif /* !C_LANG */
/* BASIC C/C++ INTERFACE (SYMBOLS) }}} */

#include <su/code-ou.h>
#endif /* su_CODE_H */
/* s-it-mode */
