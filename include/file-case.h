/*@ file_case: input file encapsulator.
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
#ifndef rf_FILE_CASE_H
#define rf_FILE_CASE_H

#include <lib.h>

#include <stdio.h>

#define rf_HEADER
#include <su/code-in.h>
NSPC_BEGIN(rf)

class file_case;

class EXPORT file_case{
   CLASS_NO_COPY(file_case)
public:
   // Flags for ctor / muxer()
   enum flags{
      fc_none,
      fc_dont_close = 1u<<0,  // Don't close the file, if any
      fc_pipe = 1u<<1,        // _file is not seekable
      fc_const_path = 1u<<2,  // Don't dup path, and don't a_delete it
      fc_take_path = 1u<<3,   // Don't dup path, but a_delete it
      fc_have_stdio = 1u<<4,  // .file() may be used
      fc__freebit = 5u,
      fc_mask = (1u<<fc__freebit) - 1
   };

   // Flags only for muxer()
   enum mux_flags{
      mux_need_seek = 1u<<(fc__freebit+0),   // File must be seekable
      mux_need_binary = 1u<<(fc__freebit+1), // Need binary I/O
      mux_unpack = 1u<<(fc__freebit+2),      // Do check for FILE{.gz,.bz2..}
      mux_no_unpack = 1u<<(fc__freebit+3),   // Do NOT auto-check
      mux_need_stdio = 1u<<(fc__freebit+4),  // Only then may .file() be used
      mux__freebit = fc__freebit + 5,
      mux_mask = ~fc_mask,
      mux_default = fc_none,
      // Defines the global default strategy for dealing with packed files in
      // case none of the above has been given explicitly by a callee
      mux__unpack_default = mux_unpack
   };

   enum seek_whence{
      seek_set,
      seek_cur,
      seek_end
   };

private:
   char const *m_path;
   FILE *m_file;     // fc_have_stdio
   void *m_layer;    // E.g., gzFile
   u32 m_flags;
   u8 m__pad[4];

public:
   file_case(FILE *fp, char const *path, u32 flags=fc_none)
         : m_path(path), m_file(fp), m_layer(NIL), m_flags(flags){
      ASSERT(!(flags & (fc_const_path | fc_take_path)) ||
         !(flags & fc_const_path) != !(flags & fc_take_path));
      ASSERT(!(flags & ~fc_mask));
   }

   ~file_case(void){
      if(m_file != NIL || m_layer != NIL) // xxx S(up,a) | S(up,b)
         close();
   }

   boole close(void);

   char const *path(void) const {return m_path;}

   FILE *file(void) const{ // TODO get rid of that
      ASSERT(_flags & fc_have_stdio);
      return m_file;
   }

   boole is_pipe(void) const {return ((m_flags & fc_pipe) != 0);}

   boole is_eof(void) const;
   s32 get_c(void);
   s32 unget_c(int c);
   char *get_line(char *buf, uz buf_size);
   uz get_buf(void *buf, uz buf_size);
   void rewind(void);
   boole seek(s64 offset, seek_whence whence=seek_set);

   // Factory muxer; note that fc_take_path will be honoured even on failure
   // If path is NIL or "-" we'll go for stdin
   static file_case *muxer(char const *path=NIL, u32 flags=mux_default);
};

NSPC_END(rf)
#include <su/code-ou.h>
#endif // rf_FILE_CASE_H
// s-it-mode
