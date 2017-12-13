/*@ Implementation of device.h: device::intoproc and device::process_file().
 *@ TODO The error messages should give command names etc.
 *@ TODO Change copyright to ISC?
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2001 - 2006, 2008 Free Software Foundation, Inc.
 *    Written by James Clark (jjc@jclark.com)
 *    Major rewrite 2001 by Bernd Warken (bwarken@mayn.de)
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
#define su_FILE "lib_roff__device_intoproc"
#define rf_SOURCE
#define rf_SOURCE_LIB_ROFF_DEVICE_INTOPROC

#include "config.h"
#include "lib.h"

#include "su/array.h"
#include "su/cs.h"
#include "su/cstr.h"
#include "su/icodec.h"

#include "file-case.h"

#include "device.h"
#include "su/code-in.h"

NSPC_USE(rf)

// TODO In groff this files former incarnation had a lengthy comment on
// TODO "stupid drawing positioning", see top of
// TODO  $ git show s-roff:src/libs/libdriver/input.cpp

// Instanceless logic encapsulator for the intermediate output format
class device::intoproc{
   CLASS_NO_COPY(intoproc)

   enum skipaction{
      skipaction_none,
      skipaction_warn,
      skipaction_error
   };

   static context *s_ctx;
   static file_case *s_input;
   static postproc *s_post;
   // Number of pages processed so far (including current page); this is
   // _not_ the page number in the printout (can be set with `p').
   static uz s_npages;

   // Retrieve an argument suitable for the color commands m and DF
   static color::component get_color_arg(void);

   // Get a fixed number of integer arguments for D commands.
   // A line skip is done.  Fatal if wrong number of arguments.
   // Too many arguments on the line raise a warning.
   // store: assigned the result and returned
   // number: the number of arguments to be retrieved
   static array<s32> &get_D_fixed_args(array<s32> &store, uz number);

   // Get a fixed number of integer arguments for D commands and optionally
   // ignore a dummy integer argument if the requested number is odd.
   // A line skip is done.
   // Error if the number of arguments differs from the scheme above.
   // The troff program adds a dummy argument to some commands to get an even
   // number of arguments.
   // store: assigned the result and returned
   // number: the number of arguments to be retrieved
   static array<s32> &get_D_fixed_args_odd_dummy(array<s32> &store, uz number);

   // Get a variable even number of integer arguments for D commands.
   // Get as many integer arguments as possible from the rest of the current
   // line.
   // - The arguments are separated by an arbitrary sequence of space or
   //    tab characters.
   // - A comment, a newline, or EOF indicates the end of processing.
   // - Error on non-digit characters different from these.
   // - A final line skip is performed (except for EOF).
   // store: assigned the result and returned
   static array<s32> &get_D_variable_args(array<s32> &store);

   // Retrieve extended arg for `x X' command.
   // - Skip leading spaces and tabs, error on EOL or newline.
   // - Return everything before the next NL or EOF ('#' is not a comment);
   //    as long as the following line starts with '+' this is returned
   //    as well, with the '+' replaced by a newline.
   // - Final line skip is always performed.
   // store: assigned the result and returned
   static cstr &get_extended_arg(cstr &store);

   // Get integer argument.
   // Skip leading spaces and tabs, collect an optional '-' and all following
   // decimal digits (at least one) up to the next non-digit, which is
   // restored onto the input queue.
   // Fatal error on all other situations
   static s32 get_integer_arg(void);

   // Parse the rest of the input line as a list of integer arguments.
   // Get as many integer arguments as possible from the rest of the current
   // line, even none.
   // - The arguments are separated by an arbitrary sequence of space or
   //    tab characters.
   // - A comment, a newline, or EOF indicates the end of processing.
   // - Error on non-digit characters different from these.
   // - No line skip is performed.
   // store: assigned the result and returned
   static array<s32> &get_possibly_integer_args(array<s32> &store);

   // Read in next string arg.
   // - Skip leading spaces and tabs; error on EOL or newline.
   // - Return all following characters before the next space, tab,
   //    newline, or EOF character (in-word '#' is not a comment character).
   // - The terminating space, tab, newline, or EOF character is restored
   //    onto the input queue, so no line skip.
   // store: assigned the result and returned
   static cstr &get_string_arg(cstr &store);

   // Return first character of next argument.
   // Skip space and tab characters; error on newline or EOF.
   // Returns first character different from these (including '#')
   static s32 next_arg_begin(void);

   // Find the first character of the next command.
   // Skip spaces, tabs, comments (introduced by #), and newlines.
   // Returns the first character different from these (including EOF)
   static s32 next_command(void);

   // Move graphical pointer to end of drawn figure.
   // This is used by the D commands that draw open geometrical figures.
   // The algorithm simply sums up all horizontal displacements (arguments
   // with even number) for the horizontal component.   Similarly, the
   // vertical component is the sum of the odd arguments.
   // args: the arguments of a former drawing command
   static void position_to_end_of_args(array<s32> const &args);

   // Call draw method of printer class.
   // subcmd: letter of actual D subcommand
   // args: array of integer arguments of actual D subcommand
   static void send_draw(s32 subcmd, array<s32> const &args);

   // Go to next line within the input queue.
   // Skip the rest of the current line, including the newline character.
   // The global variable rf_current_lineno is adjusted.
   // No errors are raised
   static void skip_line(void);

   // Skip line, but take given action if arguments are left on actual line.
   // Spaces, tabs, and a comment are allowed before newline or EOF.
   // All other characters raise the requested action to be taken
   static void skip_line_action(enum skipaction sa=skipaction_none);

   // Skip line in `x' commands.
   // Decide whether in case of an additional argument a fatal error is
   // raised (the documented classical behavior), only a warning is
   // issued, or the line is just skipped (former groff behavior)
   static void skip_line_x(void) {skip_line_action(skipaction_warn);}

   // Skip line in `D' commands.
   // Decide whether in case of an additional argument a fatal error is
   // raised (the documented classical behavior), only a warning is
   // issued, or the line is just skipped (former groff behavior)
   static void skip_line_D(void) {skip_line_action(skipaction_warn);}

   // Go to the end of the current line.
   // Skip the rest of the current line, excluding the newline character.
   // The global variable rf_current_lineno is not changed.
   // No errors are raised
   static void skip_to_end_of_line(void);

   // Process the commands m and DF, but not Df.
   // store: the color object to be set
   static void parse_color_command(color_symbol &store);

   // Parse the subcommands of graphical command D.
   // - Error on lacking or wrong arguments.
   // - Warning on too many arguments.
   // - Line is always skipped.
   static void parse_D_command(void);

   // Parse the subcommands of device control command x.
   // - Error on duplicate prologue commands.
   // - Error on wrong or lacking arguments.
   // - Warning on too many arguments.
   // - Line is always skipped.
   // Return: boole: TRU1 if parsing should be stopped (`x stop')
   static boole parse_x_command(void);

public:
   // Backs device::process_file()
   static void process_file(char const *filename);
};

device::context::context(void)
      : m_fontno(-1), m_size(0), m_hpos(-1), m_vpos(-1), m_height(0),
         m_slant(0), m_col(), m_fill(){
   NYD2_IN;
   NYD2_OU;
}

device::context::~context(void){
   NYD2_IN;
   NYD2_OU;
}

PRI STA device::context *device::intoproc::s_ctx;
PRI STA device::postproc *device::intoproc::s_post;
PRI STA file_case *device::intoproc::s_input;
PRI STA uz device::intoproc::s_npages;

PRI STA color::component
device::intoproc::get_color_arg(void){
   s32 x;
   NYD_IN;

   x = get_integer_arg();
   if(x < 0 || x > color::max_val){
      error(_("Color component argument out of range, using 0"));
      x = 0;
   }
   NYD_OU;
   return S(color::component,x);
}

PRI STA array<s32> &
device::intoproc::get_D_fixed_args(array<s32> &store, uz number){
   NYD_IN;
   if(number == 0) // XXX maximum check?
      fatal(_("Requested number of arguments must be > 0"));

   for((void)store.trunc().book(number, FAL0); number != 0; --number)
      store.push_fast(get_integer_arg());

   skip_line_D();
   NYD_OU;
   return store;
}

PRI STA array<s32> &
device::intoproc::get_D_fixed_args_odd_dummy(array<s32> &store, uz number){
   NYD_IN;
   if(number == 0) // XXX maximum check?
      fatal(_("Requested number of arguments must be > 0"));

   boole isodd = (number & 1);

   for((void)store.trunc().book(number, FAL0); number != 0; --number)
      store.push_fast(get_integer_arg());

   if(isodd){
      array<s32> cai;

      if(get_possibly_integer_args(cai).count() > 1)
         error(_("Too many arguments"));
   }

   skip_line_D();
   NYD_OU;
   return store;
}

PRI STA array<s32> &
device::intoproc::get_D_variable_args(array<s32> &store){
   NYD_IN;
   if(get_possibly_integer_args(store).is_empty())
      error(_("No arguments found"));
   else if(store.count() & 1)
      error(_("Even number of arguments expected"));

   skip_line_D();
   NYD_OU;
   return store;
}

PRI STA cstr &
device::intoproc::get_extended_arg(cstr &store){
   NYD_IN;
   store.truncate();

   for(s32 c = next_arg_begin(); c != EOF; c = s_input->get_c()){
      if(c != '\n'){
         store.push(c);
         continue;
      }

      rf_current_lineno_inc();
      c = s_input->get_c();
      if(c == '+')
         store.push('\n');
      else{
         s_input->unget_c(c); // Do not consume first character of next line
         break;
      }
   }
   NYD_OU;
   return store;
}

PRI STA s32
device::intoproc::get_integer_arg(void){
   s32 c;
   NYD_IN;
   cstr s;

   c = next_arg_begin();
   if(c == '-'){
      s.push(c);
      c = s_input->get_c();
   }

   boole any = FAL0;

   while(cs::is_digit(c)){
      any = TRU1;
      s.push(c);
      c = s_input->get_c();
   }
   s_input->unget_c(c);

   if(UNLIKELY(!any)){
      error(_("Integer argument expected"));
      c = 0;
   }else if((idec::convert(*&c, s.cp(), 10, NIL) &
            (idec::state_emask | idec::state_consumed)) !=
         idec::state_consumed){
      error(_("Invalid integer argument, using 0"));
      c = 0;
   }
   NYD_OU;
   return c;
}

PRI STA array<s32> &
device::intoproc::get_possibly_integer_args(array<s32> &store){
   NYD_IN;
   cstr cs;

   store.trunc();

   for(s32 c = ' ';; cs.trunc()){
      while(c == ' ' || c == '\t')
         c = s_input->get_c();

      if(c == '-'){
         s32 c2;

         c2 = s_input->get_c();
         if(cs::is_digit(c2)){
            cs.push(c2);
            c = c2;
         }else
            s_input->unget_c(c2);
      }

      while(cs::is_digit(c)){
         cs.push(c);
         c = s_input->get_c();
      }

      if(!cs.is_empty()){
         s32 n;

         if((idec::convert(*&n, cs.cp(), 10, NIL) &
                  (idec::state_emask | idec::state_consumed)) !=
               idec::state_consumed){
            error(_("Invalid integer argument, using 0"));
            n = 0;
         }
         store.push(n);
      }

      // Terminate on comment, EOL or EOF, whereas SPACE and HT indicate
      // continuation.  Anything else is error
      if(c == '#'){
         skip_to_end_of_line();
         break;
      }else if(c == '\n'){
         s_input->unget_c(c);
         break;
      }else if(c == EOF)
         break;
      else if(c != ' ' && c != '\t')
         error(_("Integer argument expected"));
   }
   NYD_OU;
   return store;
}

PRI STA cstr &
device::intoproc::get_string_arg(cstr &store){
   NYD_IN;
   store.trunc();

   s32 c = next_arg_begin();
   while(c != ' ' && c != '\t' && c != '\n' && c != EOF){
      store.push(c);
      c = s_input->get_c();
   }
   s_input->unget_c(c);

   NYD_OU;
   return store;
}

PRI STA s32
device::intoproc::next_arg_begin(void){
   s32 c;
   NYD_IN;

   for(;;){
      c = s_input->get_c();
      if(c == ' ' || c == '\t')
         ;
      else if(c == '\n' || c == EOF)
         error(_("Missing argument"));
      else
         break;
   }
   NYD_OU;
   return c;
}

PRI STA s32
device::intoproc::next_command(void){
   s32 c;
   NYD_IN;

   for(;;){
      c = s_input->get_c();
      if(c == ' ' || c == '\t')
         ;
      else if(c == '\n')
         rf_current_lineno_inc();
      else if(c == '#')
         skip_line();
      else
         break;
   }
   NYD_OU;
   return c;
}

PRI STA void
device::intoproc::position_to_end_of_args(array<s32> const &args){
   NYD_IN;
   uz h, v, i;
   s32 const *sip;

   sip = args.data();
   h = s_ctx->hpos();
   v = s_ctx->vpos();

   for(i = args.count(); i-- > 0; sip += 2){
      h += sip[0];
      if(i-- == 0)
         break;
      v += sip[1];
   }

   // XXX join error messages via format string
   if(UCMP(z, h, >, max::s32)){
      error(_("Horizontal displacement exceeds datatype, using limit"));
      h = max::s32;
   }
   s_ctx->ref_hpos() = S(s32,h);
   if(UCMP(z, v, >, max::s32)){
      error(_("Vertical displacement exceeds datatype, using limit"));
      v = max::s32;
   }
   s_ctx->ref_vpos() = S(s32,v);
   NYD_OU;
}

PRI STA void
device::intoproc::send_draw(s32 subcmd, array<s32> const &args){
   NYD_IN;
   s_post->draw(subcmd, args.data(), args.count(), *s_ctx);
   NYD_OU;
}

PRI STA void
device::intoproc::skip_line(void){
   NYD_IN;
   for(;;){
      s32 c;

      if((c = s_input->get_c()) == '\n'){
         rf_current_lineno_inc();
         break;
      }else if(c == EOF)
         break;
   }
   NYD_OU;
}

PRI STA void
device::intoproc::skip_line_action(enum skipaction sa){
   NYD_IN;
   s32 c;

   while((c = s_input->get_c()) == ' ' || c == '\t')
      ;

   switch(c){
   case EOF:
      break;
   case '\n':
      rf_current_lineno_inc();
      break;
   default:
      if(sa != skipaction_none){
         rf_current_lineno_dec();
         (sa == skipaction_warn ? warning : error)(
            _("Too many arguments on current line"));
         rf_current_lineno_inc();
      }
      // FALLTHRU
   case '#':
      skip_line();
      break;
   }
   NYD_OU;
}

PRI STA void
device::intoproc::skip_to_end_of_line(void){
   NYD_IN;
   for(;;){
      s32 c;

      if((c = s_input->get_c()) == '\n'){
         s_input->unget_c(c);
         break;
      }else if(c == EOF)
         break;
   }
   NYD_OU;
}

PRI STA void
device::intoproc::parse_color_command(color_symbol &store){
   NYD_IN;
   color::component comps[color::scheme_max];
   color::scheme s;

   switch(s32 subcmd = next_arg_begin()){
   case 'c': // DFc or mc: CMY
      s = color::scheme_cmy;
      break;
   case 'd': // DFd or md: set default color
      s = color::scheme_default;
      break;
   case 'g': // DFg or mg: gray
      s = color::scheme_gray;
      break;
   case 'k': // DFk or mk: CMYK
      s = color::scheme_cmyk;
      break;
   case 'r': // DFr or mr: RGB
      s = color::scheme_rgb;
      break;
   default:
      error(_("Invalid color scheme: %1"), subcmd);
      goto jleave;
   }

   for(u8 m = color::scheme_component_counts[s], i = 0; i < m; ++i)
      comps[i] = get_color_arg();

   store.assign_scheme(s, comps);
jleave:
   NYD_OU;
}

PRI STA void
device::intoproc::parse_D_command(void){
   NYD_IN;
   array<s32> intargs;

   switch(s32 subcmd = next_arg_begin()){
   case '~': // D~: draw B-spline
      // Actually, this isn't available for some postprocessors
      // FALLTHRU
   default: // Unknown options are passed to device
      send_draw(subcmd, get_D_variable_args(intargs));
      position_to_end_of_args(intargs);
      break;
   case 'a': // Da: draw arc
      send_draw(subcmd, get_D_fixed_args(intargs, 4));
      position_to_end_of_args(intargs);
      break;
   case 'C': // DC: draw solid circle
      send_draw(subcmd, get_D_fixed_args_odd_dummy(intargs, 1));
      // Move to right end
      s_ctx->ref_hpos() += intargs.at(0);
      break;
   case 'c': // Dc: draw circle line
      send_draw(subcmd, get_D_fixed_args(intargs, 1);
      // Move to right end
      s_ctx->ref_hpos() += intargs.at(0);
      break;
   case 'E': // DE: draw solid ellipse
   case 'e': // De: draw ellipse line
      send_draw(subcmd, get_D_fixed_args(intargs, 2));
      // Move to right end
      s_ctx->ref_hpos() += intargs.at(0);
      break;
   case 'F': // DF: set fill color, several formats
      parse_color_command(s_ctx->ref_fill_color());
      s_post->change_fill_color(s_ctx);
      // No positioning (setting-only command)
      skip_line_x();
      break;
   case 'f':{ // Df: set fill gray; obsoleted by DFg
      s32 iarg;

      if((iarg = get_integer_arg()) >= 0 && iarg <= 1000){
         // Transform Df style color into DF style color.
         // Df color: 0-1000, 0 is white
         // DF color: 0-65536, 0 is black
         // Df is obsoleted by command DFg, but kept for compatibility
         iarg = S(s32,S(u32,1000 - Df_gray) * (S(u32,color::max_val) + 1)
               / 1000);
         s_ctx->ref_fill_color().assign_gray(iarg);
      }else
         s_ctx->ref_fill_color().assign(s_ctx->outline_color());
      s_post->change_fill_color(s_ctx);
      skip_line_x();
   }  break;
   case 'l': // Dl: draw line
      send_draw(subcmd, get_D_fixed_args(intargs, 2));
      position_to_end_of_args(intargs);
      break;
   case 'P': // DP: draw solid closed polygon
   case 'p': // Dp: draw closed polygon line
      send_draw(subcmd, get_D_variable_args(intargs));
      position_to_end_of_args(intargs);
      break;
   case 't': // Dt: set line thickness
      send_draw(subcmd, get_D_fixed_args_odd_dummy(intargs, 1));
      position_to_end_of_args(intargs);
      break;
   }
   NYD_OU;
}

PRI STA boole
device::intoproc::parse_x_command(void){
   boole rv;
   NYD_IN;
   cstr csarg;

   rv = FAL0;

   switch(get_string_arg(csarg).cp()[0]){
   case 'F': // x Filename: set filename for errors
      if(!get_extended_arg(csarg).is_empty())
         rf_current_source_filename_set(csarg.cp());
      else
         warning(_("`x F': empty argument"));
      break;
   case 'f': // x font: load font to given (non-negative) font number
      s_post->font_mount(get_integer_arg(), get_string_arg(csarg));
      skip_line_x();
      break;
   case 'H': // x Height: set character height
      if((s_ctx->ref_height() = get_integer_arg()) == s_ctx->size())
         s_ctx->ref_height() = 0;
      skip_line_x();
      break;
   case 'i': // x init: initialize device
      error(_("Duplicate `x init' command"));
      skip_line_x();
      break;
   case 'p': // x pause: pause device
      skip_line_x();
      break;
   case 'r': // x res: set resolution
      error(_("Duplicate `x res' command"));
      skip_line_x();
      break;
   case 'S': // x Slant: set slant
      s_ctx->ref_slant() = get_integer_arg();
      skip_line_x();
      break;
   case 's': // x stop: stop device
      rv = TRU1;
      skip_line_x();
      break;
   case 'T': // x Typesetter: set typesetter
      error(_("Duplicate `x T' command"));
      skip_line();
      break;
   case 't': // x trailer: generate trailer info
      skip_line_x();
      break;
   case 'u': // x underline: from .cu
      s_post->special(get_string_arg(csarg).cp(), s_ctx, 'u');
      skip_line_x();
      break;
   case 'X': // x X: send uninterpretedly to device
      get_extended_arg(csarg); // Includes line skip
      if(s_npages == 0)
         error(_("`x X' command invalid before first `p' command"));
      else if(!csarg.is_empty() && !csarg.cmp("devtag:"))
         s_post->devtag(csarg.cp(), s_ctx);
      else
         s_post->special(csarg.cp(), s_ctx);
      break;
   default: // ignore unknown x commands, but warn
      warning(_("Unknown command: `x %1'"), csarg.cp());
      skip_line();
   }
   NYD_OU;
   return rv;
}

STA void
device::intoproc::process_file(char const *filename){
   NYD_IN;
   cstr csarg;
   context ctx;
   s32 iarg;
   boole stopped;

   // s_post is initialized after the prologue.
   s_ctx = &ctx;
   if((s_input = file_case::muxer(filename)) == NIL){
      error(_("Cannot open file: %1"), filename);
      goto jleave;
   }

   s_npages = 0;
   rf_current_lineno_set(1);
   rf_current_filename_set(filename);

   stopped = FAL0;

   // Prologue (first 3 commands) //

   // 1st command `x T'
   if((iarg = next_command()) == EOF)
      goto jleave;
   if(iarg != 'x' || get_string_arg(csarg).cmp('T'))
      fatal(_("The first command must be `x T'"));

   get_string_arg(csarg);
   if(s_post == NIL){ // Initialized after prologue FIXME ALWAYS!!
      rf_current_device_set(csarg.to_cp_dup());
      if(!font::load_desc())
         fatal(_("Could not load DESC file, cannot continue"));
   }else{
      ASSERT(rf_current_device() != NIL);
      if(csarg.cmp(rf_current_device())))
         fatal(_("All files must use the same device"));
   }
   skip_line_x();

   s_ctx->ref_size() = 10 * font::sizescale;

   // 2nd command `x res', "res" can be abbreviated
   if((iarg = next_command()) != 'x' || get_string_arg(csarg).cp()[0] != 'r')
      fatal(_("The second command must be `x res'"));
   if((iarg = get_integer_arg()) != font::res)
      fatal(_("Resolution does not match"));
   if((iarg = get_integer_arg()) != font::hor)
      fatal(_("Minimum horizontal motion does not match"));
   if((iarg = get_integer_arg()) != font::vert)
      fatal(_("Minimum vertical motion does not match"));
   skip_line_x();

   // 3rd command `x init'
   if((iarg = next_command()) != 'x' || get_string_arg(csarg).cp()[0] != 'i')
      fatal(_("The third command must be `x init'"));
   skip_line_x();

   // Body //

   if(s_post == NIL) // FIXME ALWAYS!!
      s_post = postproc::new_instance();

   while(!stopped){
      if((iarg = next_command()) == EOF)
         break;
      // Spaces, tabs, comments, and newlines are skipped here
      switch(iarg){
      case 'C': // Print named special character
         if(s_npages == 0)
            goto jfatal;
         s_post->set_special_char(get_string_arg(csarg).cp(), s_ctx);
         break;
      case 'c': // Print ascii char without moving
         if(s_npages == 0)
            goto jfatal;
         if((iarg = next_arg_begin()) == '\n' || iarg == EOF)
            error(_("Missing argument to `c' command"));
         else if(UCMP(32, iarg, >, max::u8))
            error(_("`c': invalid argument"));
         else
            s_post->set_ascii_char(S(uc,iarg), s_ctx);
         break;
      case 'D': // Drawing commands
         if(s_npages == 0)
            goto jfatal;
         parse_D_command();
         break;
      case 'F': // Obsolete, replaced by `x F'
         rf_current_source_filename_set(get_extended_arg(csarg).cp());
         break;
      case 'f': // Set font to number
         s_ctx->ref_fontno() = get_integer_arg();
         break;
      case 'H': // Absolute horizontal positioning
         s_ctx->ref_hpos() = get_integer_arg();
         break;
      case 'h': // Relative horizontal move
         s_ctx->ref_hpos() += get_integer_arg();
         break;
      case 'm': // Set glyph color
         parse_color_command(s_ctx->ref_outline_color());
         s_post->change_color(s_ctx);
         break;
      case 'N': // Print char with given int code
         if(s_npages == 0)
            goto jfatal;
         s_post->set_numbered_char(get_integer_arg(), s_ctx);
         break;
      case 'n': // Print end of line
         if(s_npages == 0)
            goto jfatal;
         s_post->end_of_line();
         // Ignore two arguments (historically)
         get_integer_arg();
         get_integer_arg();
         break;
      case 'p': // Start new page with given number
         if(s_npages > 0)
            s_post->page_end(s_ctx->vpos());
         ++s_npages;
         s_post->page_begin(get_integer_arg());
         s_ctx->ref_vpos() = 0;
         break;
      case 's': // Set point size
         if((s_ctx->ref_size() = get_integer_arg()) == s_ctx->height())
            s_ctx->ref_height() = 0;
         break;
      case 't': // Print a text word
         if(s_npages == 0)
            goto jfatal;
         if(!get_string_arg(csarg).is_empty())
            for(char const *cp = csarg.cp(); *cp != '\0'; ++cp)
               s_ctx->ref_hpos() += s_post->set_ascii_char(S(uc,*cp), s_ctx);
         break;
      case 'u': // Print spaced word
         if(s_npages == 0)
            goto jfatal;
         iarg = get_integer_arg(); // Additional kerning
         if(!get_string_arg(csarg).is_empty())
            for(char const *cp = csarg.cp(); *cp != '\0'; ++cp)
               s_ctx->ref_hpos() += s_post->set_ascii_char(S(uc,*cp), s_ctx) +
                     iarg;
         break;
      case 'V': // Absolute vertical positioning
         s_ctx->ref_vpos() = get_integer_arg();
         break;
      case 'v': // Relative vertical move
         s_ctx->ref_vpos() += get_integer_arg();
         break;
      case 'w': // Inform about paddable space xxx
         break;
      case 'x': // Device controlling commands
         stopped = parse_x_command();
         break;
      case '#': // Comment, ignore up to end of line
         skip_line();
         break;
#if 0
      case '{': // Start a new device ctx (a copy)
         // This was a list{instance,ptr}, then a stack.
         break;
      case '}': // Pop previous env from stack
         break;
#endif
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
         // ddc: obsolete jump and print command
         // Expect 2 digits and a character
         if(s_npages == 0)
jfatal:
            fatal(_("`%1': command invalid before first `p' command"), iarg);

         (void)csarg.trunc().book(2);
         csarg.ref_at(0) = S(char,iarg);
         iarg = next_arg_begin();
         if(!cs::is_digit(iarg)){
            error(_("Digit expected, using 0"));
            iarg = 0;
         }
         csarg.ref_at(1) = S(char,iarg);
         if((idec::convert(*&iarg, csarg.cp(), 10, NIL) &
                  (idec::state_emask | idec::state_consumed)) !=
               idec::state_consumed){
            error(_("Could not convert 2 digits, using 0"));
            iarg = 0;
         }
         s_ctx->ref_hpos() += iarg;

         if((iarg = next_arg_begin()) == '\n' || iarg == EOF ||
               UCMP(32, iarg, >, max::u8))
            error(_("Valid character argument expected"));
         else
            s_post->set_ascii_char(S(uc,iarg), s_ctx);
         break;
      default:
         warning(_("Unrecognized command: %1"), S(uc,iarg));
         skip_line();
         break;
      }
   }

   // End of file reached
   if(s_npages > 0)
      s_post->page_end(s_ctx->vpos());

   // Have we seen `x stop'?
   if(!stopped)
      warning(_("No final `x stop' command"));

   if(char const *errcp = s_post->processing_completed())
      fatal(_("Post-processor error: %1"), errcp);

   s_input->close(); // XXX return check

   su_DEL(s_post); DBG( s_post = NIL );
   su_DEL(s_input); DBG( s_input = NIL );
   DBG( s_ctx = NIL );
jleave:
   NYD_OU;
}

STA void
device::process_file(char const *filename){
   NYD2_IN;
   // We must fail for memory allocations.  Documented in header.
   state::clear(state::err_type_mask);

   intoproc::process_file(filename);
   NYD2_OU;
}

#include "su/code-ou.h"
// s-it-mode
