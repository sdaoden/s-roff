/*@ (TODO ITOA and) ATOI: simple non-restartable integer conversion.
 *
 * Copyright (c) 2017 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef su_ICODEC_H
#define su_ICODEC_H

#include <su/code.h>

#define su_HEADER
#include <su/code-in.h>
C_DECL_BEGIN

enum su_idec_mode{
   su_IDEC_MODE_NONE,
   su_IDEC_MODE_SIGNED_TYPE = 1u<<0,   /* Choose limits, error constants.. */
   /* If a power-of-two is used explicitly, or if base 0 is used and a known
    * standard prefix is seen, enforce interpretation as unsigned.  This only
    * makes a difference in conjunction with su_IDEC_MODE_SIGNED_TYPE */
   su_IDEC_MODE_POW2BASE_UNSIGNED = 1u<<1,
#if 0
   su_IDEC_MODE_SIGN_FORCE_SIGNED_TYPE = 1u<<2,
#endif
   su_IDEC_MODE_LIMIT_8BIT = 1u<<3,    /* Saturate if result exceeds 8-bit */
   su_IDEC_MODE_LIMIT_16BIT = 2u<<3,
   su_IDEC_MODE_LIMIT_32BIT = 3u<<3,
   su__IDEC_MODE_LIMIT_MASK = 3u<<3,
   su_IDEC_MODE_LIMIT_NOERROR = 1u<<5, /* Not an error if limit is excessed! */
   /* These bits are duplicated in the _state result bits! */
   su__IDEC_MODE_MASK = (1u<<6) - 1
};

enum su_idec_state{
   su_IDEC_STATE_NONE,
   /* Malformed input, no usable result has been stored */
   su_IDEC_STATE_EINVAL = 1u<<8,
   /* Bad character according to base, but we have seen some good before,
    * otherwise _EINVAL would have been used */
   su_IDEC_STATE_EBASE = 2u<<8,
   su_IDEC_STATE_EOVERFLOW = 3u<<8,    /* Result too large */
   su_IDEC_STATE_EMASK = 3u<<8,        /* Any errors, that is */
   su_IDEC_STATE_SEEN_MINUS = 1u<<16,  /* Seen - in input? */
   su_IDEC_STATE_CONSUMED = 1u<<17,    /* All the input buffer was consumed */
   su__IDEC_PRIVATE_SHIFT1 = 24u
};
MCTA(su__IDEC_MODE_MASK <= (1u<<8) - 1, "Shared bit range overlaps")

/* Decode clen (or strlen() if UZ_MAX) bytes of cbuf into an integer
 * according to idm, store a/the result in *resp (in _EINVAL case an overflow
 * constant is used, for signed types it depends on parse state w. MIN/MAX),
 * which must point to storage of the correct type, return the result state.
 * If endptr_or_nil is set it will be set to the byte where parsing stopped */
EXPORT enum su_idec_state su_idec(void *resp, char const *cbuf, uz clen,
      u8 base, enum su_idec_mode idm, char const **endptr_or_nil);
#define su_idec_cp(RP,CBP,B,M,CLP) su_idec(RP, CBP, su_UZ_MAX, B, M, CLP)

#define su_idec_u8(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B, (su_IDEC_MODE_LIMIT_8BIT), CLP)
#define su_idec_u8_cp(RP,CBP,B,CLP) su_idec_u8(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_s8(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B,\
      (su_IDEC_MODE_SIGNED_TYPE | su_IDEC_MODE_LIMIT_8BIT), CLP)
#define su_idec_s8_cp(RP,CBP,B,CLP) su_idec_s8(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_u16(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B, (su_IDEC_MODE_LIMIT_16BIT), CLP)
#define su_idec_u16_cp(RP,CBP,B,CLP) su_idec_u16(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_s16(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B,\
      (su_IDEC_MODE_SIGNED_TYPE | su_IDEC_MODE_LIMIT_16BIT), CLP)
#define su_idec_s16_cp(RP,CBP,B,CLP) su_idec_s16(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_u32(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B, (su_IDEC_MODE_LIMIT_32BIT), CLP)
#define su_idec_u32_cp(RP,CBP,B,CLP) su_idec_u32(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_s32(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B,\
      (su_IDEC_MODE_SIGNED_TYPE | su_IDEC_MODE_LIMIT_32BIT), CLP)
#define su_idec_s32_cp(RP,CBP,B,CLP) su_idec_s32(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_u64(RP,CBP,CL,B,CLP) su_idec(RP, CBP, CL, B, 0, CLP)
#define su_idec_u64_cp(RP,CBP,B,CLP) su_idec_u64(RP,CBP,su_UZ_MAX,B,CLP)

#define su_idec_s64(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B, (su_IDEC_MODE_SIGNED_TYPE), CLP)
#define su_idec_s64_cp(RP,CBP,B,CLP) su_idec_s64(RP,CBP,su_UZ_MAX,B,CLP)

#if UZ_BITS == 32
# define su_idec_uz(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B, (su_IDEC_MODE_LIMIT_32BIT), CLP)

# define su_idec_sz(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B,\
      (su_IDEC_MODE_SIGNED_TYPE | su_IDEC_MODE_LIMIT_32BIT), CLP)
#else
# define su_idec_uz(RP,CBP,CL,B,CLP) su_idec(RP, CBP, CL, B, 0, CLP)
# define su_idec_sz(RP,CBP,CL,B,CLP) \
   su_idec(RP, CBP, CL, B, (su_IDEC_MODE_SIGNED_TYPE), CLP)
#endif
#define su_idec_uz_cp(RP,CBP,B,CLP) su_idec_uz(RP,CBP,su_UZ_MAX,B,CLP)
#define su_idec_sz_cp(RP,CBP,B,CLP) su_idec_sz(RP,CBP,su_UZ_MAX,B,CLP)

C_DECL_END
#if !C_LANG
NSPC_BEGIN(su)

// Instanceless static encapsulator.
class idec{
public:
   enum mode{
      mode_none = su_IDEC_MODE_NONE,
      mode_signed_type = su_IDEC_MODE_SIGNED_TYPE,
      mode_pow2base_unsigned = su_IDEC_MODE_POW2BASE_UNSIGNED,
      mode_limit_8bit = su_IDEC_MODE_LIMIT_8BIT,
      mode_limit_16bit = su_IDEC_MODE_LIMIT_16BIT,
      mode_limit_32bit = su_IDEC_MODE_LIMIT_32BIT,
      mode_limit_noerror = su_IDEC_MODE_LIMIT_NOERROR,
   };

   enum state{
      state_none = su_IDEC_STATE_NONE,
      state_einval = su_IDEC_STATE_EINVAL,
      state_ebase = su_IDEC_STATE_EBASE,
      state_eoverflow = su_IDEC_STATE_EOVERFLOW,
      state_emask = su_IDEC_STATE_EMASK,
      state_seen_minus = su_IDEC_STATE_SEEN_MINUS,
      state_consumed = su_IDEC_STATE_CONSUMED,
   };

   static enum state convert(void *resp, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec(resp, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(void *resp, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_cp(resp, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(u8 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u8(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(u8 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u8_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(s8 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s8(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(s8 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s8_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(u16 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u16(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(u16 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u16_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(s16 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s16(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(s16 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s16_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(u32 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u32(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(u32 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u32_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(s32 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s32(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(s32 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s32_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(u64 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u64(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(u64 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_u64_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(s64 &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s64(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(s64 &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_s64_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(uz &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_uz(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(uz &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_uz_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }

   static enum state convert(sz &resr, char const *cbuf, uz clen,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_sz(&resr, cbuf, clen, base, S(enum mode,m),
            endptr_or_nil);
   }
   static enum state convert(sz &resr, char const *cbuf,
         u8 base, enum mode m, char const **endptr_or_nil=NIL){
      return S(enum state,su_idec_sz_cp(&resr, cbuf, base, S(enum mode,m),
            endptr_or_nil);
   }
};

NSPC_END(su)
#endif /* !C_LANG */
#include <su/code-ou.h>
#endif /* su_ICODEC_H */
/* s-it-mode */
