/*@ Implementation of color.h.
 *@ TODO Adjust color conversion math and get rid of GPL2 copyright->lib-su!
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 2001, 2002, 2003, 2004 Free Software Foundation, Inc.
 *     Written by Gaius Mulley <gaius@glam.ac.uk>
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
#define su_FILE "lib_roff__color"
#define rf_SOURCE
#define rf_SOURCE_LIB_ROFF_COLOR

#include "config.h"
#include "lib.h"

#include "su/cs.h"
#include "su/cstr.h"
#include "su/mem.h"

#include <stdio.h>

#include "color.h"
#include "su/code-in.h"

NSPC_USE(rf)

#if 0
static u8 a_color_default_buf[sizeof color];
PRI STA color *color::s_default;
#endif

CTA(sizeof color::component == sizeof u16);

// Special hexadecimal parser TODO inline atoi, ascii
static boole a_color_atoh(color::component *result, char const *bdat,
      uz blen);

static boole
a_color_atoh(color::component *result, char const *bdat, uz blen){
   char c;
   color::component val;
   NYD2_IN;

   for(val = 0; cs::is_xdigit(c = *bdat++);){
      if(cs::is_digit(c))
         val = val*0x10 + (c - '0');
      else if(cs::is_upper(c))
         val = val*0x10 + (c - 'A') + 10;
      else
         val = val*0x10 + (c - 'a') + 10;

      if(--blen == 0){
         *result = val;
         break;
      }
   }
   NYD2_OU;
   return (blen == 0);
}

#if 0
PRI STA color const &
color::s_default_create(void){ /* XXX _gut <-> rf_library_destroy? */
   NYD2_IN;
   s_default = su_NEW_HEAP(color, a_color_default_buf);
   NYD2_OU;
   return *s_default;
}
#endif

// Static .RODATA inits: assert assignment slots (no .= field inits in C++!)
CTAV(color::scheme_default == 0);
CTAV(color::scheme_cmy == 1);
CTAV(color::scheme_cmyk == 2);
CTAV(color::scheme_rgb == 3);
CTAV(color::scheme_gray == 4);

STA u8 const color::scheme_component_counts[scheme_max] = {
   FIELD_INITI(scheme_default) 0,
   FIELD_INITI(scheme_cmy) 3,
   FIELD_INITI(scheme_cmyk) 4,
   FIELD_INITI(scheme_rgb) 3,
   FIELD_INITI(scheme_gray) 1
};

// Note: the initial letters MUST correspond to the scheme_names[x][y]!
STA char const color::scheme_desc[scheme_max][16] = {
   FIELD_INITI(scheme_default) "default",
   FIELD_INITI(scheme_cmy) "CMY color",
   FIELD_INITI(scheme_cmyk) "CMYK color",
   FIELD_INITI(scheme_rgb) "RGB color",
   FIELD_INITI(scheme_gray) "Gray(scale)\0"
};

// Scheme names are lowercase and case-sensitive according to manual
STA char const color::scheme_names[scheme_max][8] = {
   FIELD_INITI(scheme_default) "default\0",
   FIELD_INITI(scheme_cmy) "cmy",
   FIELD_INITI(scheme_cmyk) "cmyk",
   FIELD_INITI(scheme_rgb) "rgb",
   FIELD_INITI(scheme_gray) "gray"
};

color &
color::assign(color const &t){
   NYD_IN;
   if(this != &t){
      mem::copy(m_comps, t.m_comps, sizeof m_comps);
      m_scheme = t.m_scheme;
   }
   NYD_OU;
   return *this;
}

color &
assign_scheme(scheme s, component const comps[scheme_max]){
   NYD_IN;
   switch(m_scheme = s){
   case scheme_default:
      break;
   case scheme_cmy:
      assign_cmy(comps[0], comps[1], comps[2]);
      break;
   case scheme_cmyk:
      assign_cmyk(comps[0], comps[1], comps[2], comps[3]);
      break;
   case scheme_rgb:
      assign_rgb(comps[0], comps[1], comps[2]);
      break;
   case scheme_gray:
      assign_gray(comps[0]);
      break;
   }
   NYD_OU;
   return *this;
}

boole
color::read_scheme(scheme s, char const *cp){
   u32 cnt, hex_length;
   char const *xp;
   NYD_IN;
   ASSERT_NYD_RET(cp != NULL, FAL0);

   cnt = scheme_component_counts[m_scheme = s];
   hex_length = 2;
   if(*(xp = &cp[1]) == '#'){
      hex_length = 4;
      ++xp;
   }

   for(u32 i = 0; i < cnt; ++i){
      if(!a_color_atoh(&x_comps[i], xp, hex_length)){
         m_scheme = scheme_default;
         cp = NIL;
         break;
      }
      if(hex_length == 2)
         m_comps[i] *= 0x101; // scale up -- 0xff should become 0xffff
      xp += hex_length;
   }
   NYD_OU;
   return (cp != NIL);
}

boole
color::convert_to_scheme(scheme s){
   scheme os;
   NYD_IN;

   if((os = scheme()) != scheme_default && os != s){
      u32 const mv = max_val;
      u32 c0, c1, c2, c3, i;

      c0 = m_comps[0], c1 = m_comps[1], c2 = m_comps[2], c3 = m_comps[3];

      switch(m_scheme = s){
      case scheme_default:
         break;
      case scheme_cmy:
         switch(os){
         case scheme_default:
         case scheme_cmy:
            break;
         case scheme_cmyk:
            i = (mv - c3) / mv;
            set_cyan(c0*i + c3).set_magenta(c1*i + c3).set_yellow(c2*i + c3);
            break;
         case scheme_rgb:
            set_cyan(mv - c0).set_magenta(mv - c1).set_yellow(mv - c2);
            break;
         case scheme_gray:
            c1 = mv - c0;
            set_cyan(c1).set_magenta(c1).set_yellow(c1);
            break;
         }
         break;
      case scheme_cmyk:
         switch(os){
         case scheme_default:
         case scheme_cmyk:
            break;
         case scheme_cmy:
            set_black(c3 = get_min(c0, get_min(c1, c2)));
            if(c3 == mv)
               set_cyan(c3).set_magenta(c3).set_yellow(c3);
            else{
               i = mv - c3;
               set_cyan(((c0 - c3) * mv) / i)
               .set_magenta(((c1 - c3) * mv) / i)
               .set_yellow(((c2 - c3) * mv) / i);
            }
            break;
         case scheme_rgb:
            set_black(c3 = get_min(mv - c0, get_min(mv - c1, mv - c2)));
            if(c3 == mv)
               set_cyan(c3).set_magenta(c3).set_yellow(c3);
            else{
               i = mv - c3;
               set_cyan((mv * (mv - c0 - c3)) / i)
               .set_magenta((mv * (mv - c1 - c3)) / i)
               .set_yellow((mv * (mv - c2 - c3)) / i);
            }
            break;
         case scheme_gray:
            set_cyan(0).set_magenta(0).set_yellow(0).set_black(max_val - c0);
            break;
         }
         break;
      case scheme_rgb:
         switch(os){
         case scheme_default:
         case scheme_rgb:
            break;
         case scheme_cmy:
            set_red(mv - c0).set_green(mv - c1).set_blue(mv - c2);
            break;
         case scheme_cmyk:
            i = (mv - c3) / mv;
            set_red(mv - get_min(mv, c0*i + c3))
            .set_green(mv - get_min(mv, c1*i + c3))
            .set_blue(mv - get_min(mv, c2*i + c3));
            break;
         case scheme_gray:
            set_red(c0).set_green(c0).set_blue(c0);
            break;
         }
         break;
      case scheme_gray:
         // We use "0.222r + 0.707g + 0.071b" (this is the ITU standard)
         // as an approximation for gray
         switch(os){
         case scheme_default:
         case scheme_gray:
            break;
         case scheme_cmy:
            set_gray(mv - (222u*c0 + 707u*c1 + 71u*c2)/1000u);
            break;
         case scheme_cmyk:
            set_gray((mv - (222u*c0 + 707u*c1 + 71u*c2)/1000u) * (mv - c3));
            break;
         case scheme_rgb:
            set_gray((222u*c0 + 707u*c1 + 71u*c2) / 1000u);
            break;
         }
         break;
      }
   }
   NYD_OU;
   return (os != scheme_default);
}

sz
color::cmp(color const &t) const{
   sz rv;
   NYD_IN;

   rv = S(sz,scheme()) - S(sz,t.scheme());

   if(rv == 0){
      uz cnt;

      cnt = scheme_component_counts[scheme()];
      rv = (cnt == 0) ? 0
            : mem::cmp(m_comps, t.m_comps, cnt * sizeof(m_comps[0]));
   }
   NYD_OU;
   return rv;
}

boole
color::to_cstr(cstr &cs) const{ // TODO uses FP calculations!?!? cs.to_fmt()!
   char *bd;
   u32 bl;
   boole rv;
   NYD_IN;

   if(scheme() == scheme_default || !cs.trunc().grow(bl = 30, TRU1)){
      rv = (cs.assign("default") == 0);
      goto jleave;
   }

   bd = cs.data();

   switch(scheme()){
   case scheme_default:
      // FALLTHRY  handled above
   case scheme_rgb:
      bl = snprintf(bd, S(int,bl), "rgb %.2ff %.2ff %.2ff",
         double(red()) / max_val,
         double(green()) / max_val,
         double(blue()) / max_val);
      break;
   case scheme_cmy:
      bl = snprintf(bd, S(int,bl), "cmy %.2ff %.2ff %.2ff",
         double(cyan()) / max_val,
         double(magenta()) / max_val,
         double(yellow()) / max_val);
      break;
   case scheme_cmyk:
      bl = snprintf(bd, S(int,bl), "cmyk %.2ff %.2ff %.2ff %.2ff",
         double(cyan()) / max_val,
         double(magenta()) / max_val,
         double(yellow()) / max_val,
         double(black()) / max_val);
      break;
   case scheme_gray:
      bl = snprintf(bd, S(int,bl), "gray %.2ff",
         double(gray()) / max_val);
      break;
   }
   cs.trunc(bl);

   rv = TRU1;
jleave:
   NYD_OU;
   return rv;
}

static u8 a_color_symbol_default_buf[sizeof color_symbol];
PRI STA color_symbol *color_symbol::s_default;

PRI STA color_symbol const &
color_symbol::s_default_create(void){ /* XXX _gut <-> rf_library_destroy? */
   NYD2_IN;
   s_default = su_NEW_HEAP(color_symbol, a_color_symbol_default_buf);
   NYD2_OU;
   return *s_default;
}

#include "su/code-ou.h"
// s-it-mode
