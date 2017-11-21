/*@ color and color_symbol objects. FIXME get rid of latter!
 *@ TODO Add named_color or so which has a name, have a list of builtin colors,
 *@ TODO generate them à la nail's okeys, errors etc.
 *@ TODO color_sym is roff only, _that_ color could be su, then.
 *@ TODO We could get rid of a lot of .defcolor macros in tmac, only needed to
 *@ TODO override device-specific non-standard settings, then!?
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef rf_COLOR_H
#define rf_COLOR_H

#include <lib.h>

#include <su/cstr.h>

#include <symbol.h>

#define rf_HEADER
#include <su/code-in.h>
NSPC_BEGIN(rf)

class color;
class color_symbol;

class EXPORT color{
public:
   enum scheme{
      scheme_default,   // This is 0.
      scheme_cmy,
      scheme_cmyk,
      scheme_rgb,
      scheme_gray,
      scheme_max = scheme_gray // The largest value, inclusive
   };

   enum{
      max_val = 0xFFFFu
   };
   typedef u16 component;

   static u8 const scheme_component_counts[scheme_max];
   static char const scheme_desc[scheme_max][16]; // Initial letters EQ name!
   static char const scheme_names[scheme_max][8]; // Lowercase!
protected:
   component m_comps[4];
   u8 m_scheme;
   u8 m__pad[su_6432(7, 3)];

public:
   color(void) : m_comps(), m_scheme(scheme_default) {}
   color(color const &t) : m_comps(), m_scheme() {assign(t);}
   ~color(void) {}

   color &assign(color const &t);
   color &operator=(color const &t) {return assign(t);}

   color &reset(void){
      m_scheme = scheme_default;
      return *this;
   }

   scheme scheme(void) const {return S(scheme,m_scheme);}

   // TODO 32-bit accessors.  Offer some calculation functions instead?!
   u32 cyan(void) const{
      ASSERT_RET(scheme() == scheme_cmy || scheme() == scheme_cmyk, 0);
      return m_comps[0];
   }
   color &set_cyan(u32 c){
      ASSERT_RET(scheme() == scheme_cmy || scheme() == scheme_cmyk, *this);
      m_comps[0] = get_min(c, max_val);
      return *this;
   }

   u32 magenta(void) const{
      ASSERT_RET(scheme() == scheme_cmy || scheme() == scheme_cmyk, 0);
      return m_comps[1];
   }
   color &set_magenta(u32 c){
      ASSERT_RET(scheme() == scheme_cmy || scheme() == scheme_cmyk, *this);
      m_comps[1] = get_min(c, max_val);
      return *this;
   }

   u32 yellow(void) const{
      ASSERT_RET(scheme() == scheme_cmy || scheme() == scheme_cmyk, 0);
      return m_comps[2];
   }
   color &set_yellow(u32 c){
      ASSERT_RET(scheme() == scheme_cmy || scheme() == scheme_cmyk, *this);
      m_comps[2] = get_min(c, max_val);
      return *this;
   }

   u32 black(void) const{
      ASSERT_RET(scheme() == scheme_cmyk, 0);
      return m_comps[3];
   }
   color &set_black(u32 c){
      ASSERT_RET(scheme() == scheme_cmyk, *this);
      m_comps[3] = get_min(c, max_val);
      return *this;
   }

   u32 red(void) const{
      ASSERT_RET(scheme() == scheme_rgb, 0);
      return m_comps[0];
   }
   color &set_red(u32 c){
      ASSERT_RET(scheme() == scheme_rgb, *this);
      m_comps[0] = get_min(c, max_val);
      return *this;
   }

   u32 green(void) const{
      ASSERT_RET(scheme() == scheme_rgb, 0);
      return m_comps[1];
   }
   color &set_green(u32 c){
      ASSERT_RET(scheme() == scheme_rgb, *this);
      m_comps[1] = get_min(c, max_val);
      return *this;
   }

   u32 blue(void) const{
      ASSERT_RET(scheme() == scheme_rgb, 0);
      return m_comps[2];
   }
   color &set_blue(u32 c){
      ASSERT_RET(scheme() == scheme_rgb, *this);
      m_comps[2] = get_min(c, max_val);
      return *this;
   }

   u32 gray(void) const{
      ASSERT_RET(scheme() == scheme_gray, 0);
      return m_comps[0];
   }
   color &set_gray(u32 c){
      ASSERT_RET(scheme() == scheme_gray, *this);
      m_comps[0] = get_min(c, max_val);
      return *this;
   }

   // Set from given color component values, according to s
   color &assign_scheme(scheme s, component const comps[scheme_max]);

   color &assign_default(void) {return reset();}

   color &assign_cmy(u32 c, u32 m, u32 y){
      m_scheme = scheme_cmy;
      return set_cyan(c).set_magenta(m).set_yellow(y);
   }

   color &assign_cmyk(u32 c, u32 m, u32 y, u32 k){
      m_scheme = scheme_cmyk;
      return set_cyan(c).set_magenta(m).set_yellow(y).set_black(k);
   }

   color &assign_rgb(u32 r, u32 g, u32 b){
      m_scheme = scheme_rgb;
      return set_red(r).set_green(g).set_blue(b);
   }

   color &assign_gray(u32 g){
      m_scheme = scheme_gray;
      return set_gray(g);
   }

   // Set from a roff color string
   boole read_scheme(scheme s, char const *cp);
   boole read_cmy(char const *cp) {return read_scheme(scheme_cmy, cp);}
   boole read_cmyk(char const *cp) {return read_scheme(scheme_cmyk, cp);}
   boole read_rgb(char const *cp) {return read_scheme(scheme_rgb, cp);}
   boole read_gray(char const *cp) {return read_scheme(scheme_gray, cp);}

   // Convert to scheme; return FAL0 only if current is scheme_default
   boole convert_to_scheme(scheme s);

   // But rather: is_equal only
   sz cmp(color const &t) const;
   boole operator==(color const &t) const {return (cmp(t) == 0);}
   boole operator!=(color const &t) const {return (cmp(t) != 0);}

   // FAL0 only on out-of-memory; cs is empty, then
   boole to_cstr(cstr &cs) const;

#if 0
   static color const &get_default(void){
      return (s_default == NIL) ? s_default_create() : *s_default;
   }
private:
   static color *s_default; // Move to top when enabled
   static color const &s_default_create(void);
#endif
};

class EXPORT color_symbol : public color{
   static color_symbol *s_default;
public:
   symbol nm; /* TODO why do symbols use public members? */

   color_symbol(symbol const &s=symbol::get_default()) : color(), nm(s) {}
   color_symbol(color_symbol const &t) : color(t), nm(t.nm) {}
   ~color_symbol(void) {} // TODO not virtual, never deleted

   // But rather: is_equal only
   sz cmp(color_symbol const &t) const{ // TODO need symbol::cmp()
      return (nm == t.nm ? color::cmp(t) : 1);
   }
   boole operator==(color_symbol const &t) const {return (cmp(t) == 0);}
   boole operator!=(color_symbol const &t) const {return (cmp(t) != 0);}

   static color_symbol const &get_default(void){
      return (s_default == NIL) ? s_default_create() : *s_default;
   }

private:
   static color_symbol const &s_default_create(void);
};

NSPC_END(rf)
#include <su/code-ou.h>
#endif // rf_COLOR_H
// s-it-mode
