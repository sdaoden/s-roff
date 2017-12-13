/*@ Output device / TYPESETTER / driver or however you call it.
 *@ device::process_file(), and class device::intoproc, the parser for the
 *@ intermediate roff output format, see XYroff-intofor(5), which initiates
 *@ the printout for the given device.
 *@ class device::postproc, the postprocessing superclass of and for a device:
 *@ Each device only needs to implement a subclass and the undefined superclass
 *@ function postproc::create(), then the subclass is used transparently by
 *@ device::process_file().
 *@ TODO Change copyright to ISC?
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2001 - 2004, 2006 Free Software Foundation, Inc.
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
#ifndef rf_DEVICE_H
#define rf_DEVICE_H

#include <lib.h>

#include <su/array.h>
#include <su/cstr.h>

#include <color.h>
#include <font.h>

#define rf_HEADER
#include <su/code-in.h>
NSPC_BEGIN(rf)

class device;

// Instanceless logic encapsulator.
class EXPORT device{
   CLASS_NO_COPY(device)

   // Internalized workhorse to compute the intermediate roff output
   class intoproc;
public:
   class context;
   class postproc;

   // class context is the carrier workhorse that passes state in between
   // the internal intermediate output format processor intoproc and the
   // actual per-device postproc subclass
   // TODO device::context: some should be unsigned a.k.a. u32 for sure!
   class EXPORT context{
      CLASS_NO_COPY(context)

      s32 m_fontno;
      s32 m_size;
      s32 m_hpos;
      s32 m_vpos;
      s32 m_height;
      s32 m_slant;
      color_symbol m_col;
      color_symbol m_fill;

   public:
      context(void);
      ~context(void);

      s32 fontno(void) const {return m_fontno;}
      s32 &ref_fontno(void) {return m_size;}

      s32 size(void) const {return m_size;}
      s32 &ref_size(void) {return m_size;}

      s32 hpos(void) const {return m_hpos;}
      s32 &ref_hpos(void) {return m_hpos;}

      s32 vpos(void) const {return m_vpos;}
      s32 &ref_vpos(void) {return m_vpos;}

      s32 height(void) const {return m_height;}
      s32 &ref_height(void) {return m_height;}

      s32 slant(void) const {return m_slant;}
      s32 &ref_slant(void) {return m_slant;}

      color_symbol const &outline_color(void) const {return m_col;}
      color_symbol &ref_outline_color(void) {return m_col;}

      color_symbol const &fill_color(void) const {return m_fill;}
      color_symbol &ref_fill_color(void) {return m_fill;}
   };

   // Abstract per-device postprocessor superclass
   // TODO I/O error is checked via processing_completed(), but should happen
   // TODO for each I/O operation.  No STDIO should be used at all!
   class EXPORT postproc{
      CLASS_NO_COPY(postproc)

      array<font*> m_font_array;    // Allocation list
      array<font*> m_font_mounts;   // Same data, but at mounted offsets

   public:
      postproc(void) : m_font_array(), m_font_mounts() {}
      virtual ~postproc(void);

      // Called once processing is completed, right before destruction.
      // Return: NIL upon success, a(n) (translated) error message to indicate
      //    which hard error occurred otherwise.
      //    The base implementation checks for I/O error on stdout.
      //    It also performs a fflush(3) (if possible)
      virtual char const *processing_completed(void);

      // For iterating purposes TODO font_array::view
      array<font*> const &font_array(void) const {return m_font_array;}

      // Return: NIL if fontno is invalid
      font *font_from_index(uz fontno){
         return (fontno < m_font_mounts.count())
            ? m_font_mounts.at(fontno) : NIL;
      }

      // Tries to mount font name into position off.  Any error is fatal.
      // The second is the internal backing workhorse, the default calls
      // font::load_font() (return NIL for error)
      void font_mount(uz off, cstr const &name);
   protected:
      virtual font *font_make(cstr const &name);
   public:

      virtual void special(char const *, context const *, char='p');
      virtual void devtag(char const *, context const *, char='p');

      //
      virtual void page_begin(int) = 0;
      virtual void page_end(int) = 0;


//FIXME: does not take int* as third, but returns horizontal moved width!
//FIXME: true for all int*=0 here (error? -1)
      void set_ascii_char(uc, context const *, int *=0);

      void set_special_char(char const *, context const *, int *=0);
      virtual void set_numbered_char(int, context const *, int *=0);
      glyph *set_char_and_width(char const *, context const *,
                  int *, font **);
   protected:
      virtual void set_char(glyph *, font *, context const *, int,
            char const *) = 0;
   public:


      virtual void draw(s32 subcmd, s32 const *data, uz count,
                     context const &dcr);
      // perform change of line color (text, outline) in the print-out
      virtual void change_color(context const *);
      // perform change of fill color in the print-out
      virtual void change_fill_color(context const *);

      // Default does nothing.
      virtual void end_of_line(void);

      // Create a post-processor instance for the device that will be used to
      // process a single input file.
      // This is to be implemented in the actual sub-, for the superclass!
      static postproc *new_instance(void);

   private:
      font *font_find(cstr const &name);


   };

   // Open and parse filename ("-" denotes standard input), which contains
   // data in XYroff-intofor(5) intermediate output format.
   // The processed data is fed into a device::postproc::new_instance().
   // The SU state machine is configured to abort for failures.
   static void process_file(char const *filename);
};

NSPC_END(rf)
#include <su/code-ou.h>
#endif // rf_DEVICE_H
// s-it-mode
