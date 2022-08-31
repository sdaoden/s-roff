/*@
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 2000, 2001, 2003 - 2005 Free Software Foundation, Inc.
 *      Written by Gaius Mulley (gaius@glam.ac.uk).
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

#include "config.h"
#include "html-config.h"

#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include "su/cs.h"

#include "errarg.h"
#include "error.h"
#include "lib.h"
#include "nonposix.h"
#include "posix.h"
#include "stringclass.h"

#include "pushback.h"
#include "pre-html.h"

#define ERROR(X) \
  (void)(fprintf(stderr, "%s:%d error %s\n", __FILE__, __LINE__, X) &&\
   (fflush(stderr)) && localexit(1))

pushBackBuffer::pushBackBuffer (char *filename)
{
  charStack = (char *)malloc(MAXPUSHBACKSTACK);
  if (charStack == 0) {
    sys_fatal("malloc");
  }
  stackPtr = 0;   /* index to push back stack        */
  debug    = 0;
  verbose  = 0;
  eofFound = false;
  lineNo   = 1;
  if (su_cs_cmp(filename, "")) {
    if ((stdIn = dup(0)) == -1)
      sys_fatal("cannot dup(2)licate standard input");
    close(0);
    if (open(filename, O_RDONLY) != 0) {
      sys_fatal("when trying to open file");
    } else {
      fileName = filename;
    }
  }
}

pushBackBuffer::~pushBackBuffer ()
{
  if (charStack != 0) {
    free(charStack);
  }
  close(0);
  /* restore stdin in file descriptor 0 */
  if (dup(stdIn) == -1) /* FIXME use dup2() if possible, not close(0)+dup(2)! */
    sys_fatal("cannot restore dup(2)licated standard input");
  close(stdIn);
}

/*
 *  localexit - wraps exit with a return code to aid the ERROR macro.
 */

int localexit (int i)
{
  exit(i);
  return( 1 );
}

/*
 *  getPB - returns a character, possibly a pushed back character.
 */

char pushBackBuffer::getPB (void)
{
  if (stackPtr>0) {
    stackPtr--;
    return( charStack[stackPtr] );
  } else {
    char ch;

    if (read(0, &ch, 1) == 1) {
      if (verbose) {
	printf("%c", ch);
      }
      if (ch == '\n') {
	lineNo++;
      }
      return( ch );
    } else {
      eofFound = true;
      return( eof );
    }
  }
}

/*
 *  putPB - pushes a character onto the push back stack.
 *          The same character is returned.
 */

char pushBackBuffer::putPB (char ch)
{
  if (stackPtr<MAXPUSHBACKSTACK) {
    charStack[stackPtr] = ch ;
    stackPtr++;
  } else {
    ERROR("max push back stack exceeded, increase MAXPUSHBACKSTACK constant");
  }
  return( ch );
}

/*
 *  isWhite - returns true if a white character is found. This character is NOT consumed.
 */

static int isWhite (char ch)
{
  return( (ch==' ') || (ch == '\t') || (ch == '\n') );
}

/*
 *  skipToNewline - skips characters until a newline is seen.
 */

void pushBackBuffer::skipToNewline (void)
{
  while ((putPB(getPB()) != '\n') && (! eofFound)) {
    getPB();
  }
}

/*
 *  skipUntilToken - skips until a token is seen
 */

void pushBackBuffer::skipUntilToken (void)
{
  char ch;

  while ((isWhite(putPB(getPB())) || (putPB(getPB()) == '#')) && (! eofFound)) {
    ch = getPB();
    if (ch == '#') {
      skipToNewline();
    }
  }
}

/*
 *  isString - returns true if the string, s, matches the pushed back string.
 *             if true is returned then this string is consumed, otherwise it is
 *             left alone.
 */

int pushBackBuffer::isString (const char *s)
{
  int length=su_cs_len(s);
  int i=0;

  while ((i<length) && (putPB(getPB())==s[i])) {
    if (getPB() != s[i]) {
      ERROR("assert failed");
    }
    i++;
  }
  if (i==length) {
    return true;
  } else {
    i--;
    while (i>=0) {
      if (putPB(s[i]) != s[i]) {
	ERROR("assert failed");
      }
      i--;
    }
  }
  return false;
}

/*
 *  isDigit - returns true if the character, ch, is a digit.
 */

static int isDigit (char ch)
{
  return( ((ch>='0') && (ch<='9')) );
}

/*
 *  isHexDigit - returns true if the character, ch, is a hex digit.
 */

#if 0
static int isHexDigit (char ch)
{
  return( (isDigit(ch)) || ((ch>='a') && (ch<='f')) );
}
#endif

/*
 *  readInt - returns an integer from the input stream.
 */

int pushBackBuffer::readInt (void)
{
  int  c =0;
  int  i =0;
  int  s =1;
  char ch=getPB();

  while (isWhite(ch)) {
    ch=getPB();
  }
  // now read integer

  if (ch == '-') {
    s = -1;
    ch = getPB();
  }
  while (isDigit(ch)) {
    i *= 10;
    if ((ch>='0') && (ch<='9')) {
      i += (int)(ch-'0');
    }
    ch = getPB();
    c++;
  }
  if (ch != putPB(ch)) {
    ERROR("assert failed");
  }
  return( i*s );
}

/*
 *  convertToFloat - converts integers, a and b into a.b
 */

static double convertToFloat (int a, int b)
{
  int c=10;
  double f;

  while (b>c) {
    c *= 10;
  }
  f = ((double)a) + (((double)b)/((double)c));
  return( f );
}

/*
 *  readNumber - returns a float representing the word just read.
 */

double pushBackBuffer::readNumber (void)
{
  int i;
  char ch;

  i = readInt();
  if ((ch = getPB()) == '.') {
    return convertToFloat(i, readInt());
  }
  putPB(ch);
  return (double)i;
}

/*
 *  readString - reads a string terminated by white space
 *               and returns a malloced area of memory containing
 *               a copy of the characters.
 */

char *pushBackBuffer::readString (void)
{
  char  buffer[MAXPUSHBACKSTACK];
  char *str = 0;
  int   i=0;
  char ch=getPB();

  while (isWhite(ch)) {
    ch=getPB();
  }
  while ((i < MAXPUSHBACKSTACK) && (! isWhite(ch)) && (! eofFound)) {
    buffer[i] = ch;
    i++;
    ch = getPB();
  }
  if (i < MAXPUSHBACKSTACK) {
    uz len;

    buffer[i] = '\0';
    len = su_cs_len(buffer) +1;
    str = su_TALLOC(char, len);
    su_mem_copy(str, buffer, len);
  }
  return str;
}

// s-it2-mode
