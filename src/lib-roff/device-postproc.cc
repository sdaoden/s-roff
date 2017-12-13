/*@ Implementation of device.h: device::postproc.
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2001 - 2006 Free Software Foundation, Inc.
 *    Written by James Clark (jjc@jclark.com)
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
#define su_FILE "lib_roff__device_postproc"
#define rf_SOURCE
#define rf_SOURCE_LIB_ROFF_DEVICE_POSTPROC

#include "config.h"
#include "lib.h"

#include "su/array.h"
#include "su/cstr.h"

#include "device.h"
#include "su/code-in.h"

NSPC_USE(rf)

PRI font *
device::postproc::font_find(cstr const &name){
   font **fpp, *rv;
   NYD_IN;

   if((fpp = m_font_array.data()) != NIL) // XXX array::view
      for(font **fpp_max = &fpp[m_font_array.count()]; fpp != fpp_max; ++fpp)
         if(!name.cmp((rv = *fpp)->name()))
            goto jleave;

   if((rv = font_make(name)) == NIL)
      fatal(_("Failed to load font"));
   (void)m_font_array.push(rv);
jleave:
   NYD_OU;
   return rv;
}

VIR
device::postproc::~postproc(void){
   font **fpp;
   NYD_IN;

   if((fpp = m_font_array.data()) != NIL) // XXX array::view
      for(font **fpp_max = &fpp[m_font_array.count()]; fpp != fpp_max; ++fpp)
         su_DEL(*fpp);
   NYD_OU;
}

VIR char const *
device::postproc::processing_completed(void){
   char const *rv;
   NYD_IN;

   // TODO postproc: I/O errors should always be handled along the way, not
   // TODO only in here (introduce this->stop() or so).  Also
   if(ferror(stdout))
      rv = _("output error");
   // FIXME Discard EPIPE error <-> SIGPIPE!  Not on Win?!  Where is
   // FIXME handler, for the others?
   else if(fflush(stdout) == EOF && err::no_via_errno() != err::e_pipe)
      rv = _("final output flush failed");
   else
      rv = NIL;
   NYD_OU;
   return rv;
}

void
device::postproc::font_mount(uz off, cstr const &name){
   u32 cnt;
   NYD_IN;

   if(off >= (cnt = m_font_mounts.count()))
      (void)m_font_mounts.book_at(cnt, off - cnt + 1);

   m_font_mounts.at(off) = font_find(name);
   NYD_OU;
}

PRO VIR font *
device::postproc::font_make(cstr const &name){
   font *rv;
   NYD2_IN;

   rv = font::load_font(name);
   NYD2_OU;
   return rv;
}

VIR void
device::postproc::special(char *, device::context const *, char){
}

VIR void
device::postproc::devtag(char *, device::context const *, char){
}




void device::postproc::draw(int, int *, int, device::context const *)
{
}

void device::postproc::change_color(device::context const *)
{
}

void device::postproc::change_fill_color(device::context const *)
{
}

void device::postproc::set_ascii_char(unsigned char c, device::context const *dcp,
                int *widthp)
{
   char  buf[2];
   int      w;
   font *f;

   buf[0] = c;
   buf[1] = '\0';

   glyph *g = set_char_and_width(buf, dcp, &w, &f);
   set_char(g, f, dcp, w, 0);
   if (widthp) {
      *widthp = w;
   }
}

void device::postproc::set_special_char(const char *nm, device::context const *dcp,
                   int *widthp)
{
   font *f;
   int w;
   glyph *g = set_char_and_width(nm, dcp, &w, &f);
   if (g != UNDEFINED_GLYPH) {
      set_char(g, f, dcp, w, nm);
      if (widthp)
         *widthp = w;
   }
}

glyph *device::postproc::set_char_and_width(const char *nm, device::context const *dcp,
                int *widthp, font **f)
{
   glyph *g = name_to_glyph(nm);
   int fn = dcp->fontno();
   if (fn < 0 || fn >= nfonts) {
      error("bad font position `%1'", fn);
      return UNDEFINED_GLYPH;
   }
   *f = font_table[fn];
   if (*f == 0) {
      error("no font mounted at `%1'", fn);
      return UNDEFINED_GLYPH;
   }
   if (!(*f)->contains(g)) {
      if (nm[0] != '\0' && nm[1] == '\0')
         error("font `%1' does not contain ascii character `%2'",
         (*f)->get_name(),
         nm[0]);
      else
         error("font `%1' does not contain special character `%2'",
         (*f)->get_name(),
         nm);
      return UNDEFINED_GLYPH;
   }
   int w = (*f)->get_width(g, dcp->size());
   if (widthp)
      *widthp = w;
   return g;
}

void device::postproc::set_numbered_char(int num, device::context const *dcp, int *widthp)
{
   glyph *g = number_to_glyph(num);
   int fn = dcp->fontno();
   if (fn < 0 || fn >= nfonts) {
      error("bad font position `%1'", fn);
      return;
   }
   font *f = font_table[fn];
   if (f == 0) {
      error("no font mounted at `%1'", fn);
      return;
   }
   if (!f->contains(g)) {
      error("font `%1' does not contain numbered character %2",
      f->get_name(),
      num);
      return;
   }
   int w = f->get_width(g, dcp->size());
   if (widthp)
      *widthp = w;
   set_char(g, f, dcp, w, 0);
}






VIR void
device::postproc::end_of_line(){
   NYD_IN;
   NYD_OU;
}

#include "su/code-ou.h"
// s-it-mode
