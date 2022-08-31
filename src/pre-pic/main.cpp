/*@
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2000 - 2003, 2006
 *    Free Software Foundation, Inc.
 *      Written by James Clark (jjc@jclark.com)
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
#include "lib.h"
#include "pic-config.h"

#include "su/cs.h"

#include "file-case.h"

#include "pic.h"

extern int yyparse();

output *out;
char *graphname;		// the picture box name in TeX mode

int flyback_flag;
int zero_length_line_flag = 0;
// Non-zero means we're using a groff driver.
int driver_extension_flag = 1;
int compatible_flag = 0;
int safer_flag = 1;
int command_char = '.';		// the character that introduces lines
				// that should be passed through transparently
static int lf_flag = 1;		// non-zero if we should attempt to understand
				// lines beginning with `.lf'

// Non-zero means a parse error was encountered.
static int had_parse_error = 0;

static void do_file(char const *filename);

class top_input
: public input
{
  file_case *_fcp;
  int bol;
  int eof;
  int push_back[3];
  int start_lineno;

public:
  top_input(file_case *);
  int get();
  int peek();
  int get_location(const char **, int *);
};

top_input::top_input(file_case *fcp) : _fcp(fcp), bol(1), eof(0)
{
  push_back[0] = push_back[1] = push_back[2] = EOF;
  start_lineno = rf_current_lineno();
}

int top_input::get()
{
  if (eof)
    return EOF;
  if (push_back[2] != EOF) {
    int c = push_back[2];
    push_back[2] = EOF;
    return c;
  }
  else if (push_back[1] != EOF) {
    int c = push_back[1];
    push_back[1] = EOF;
    return c;
  }
  else if (push_back[0] != EOF) {
    int c = push_back[0];
    push_back[0] = EOF;
    return c;
  }
  int c = _fcp->get_c();
  while (invalid_input_char(c)) {
    error("invalid input character code %1", int(c));
    c = _fcp->get_c();
    bol = 0;
  }
  if (bol && c == '.') {
    c = _fcp->get_c();
    if (c == 'P') {
      c = _fcp->get_c();
      if (c == 'F' || c == 'E') {
	int d = _fcp->get_c();
	if (d != EOF)
	  _fcp->unget_c(d);
	if (d == EOF || d == ' ' || d == '\n' || compatible_flag) {
	  eof = 1;
	  flyback_flag = c == 'F';
	  return EOF;
	}
	push_back[0] = c;
	push_back[1] = 'P';
	return '.';
      }
      if (c == 'S') {
        c = _fcp->get_c();
        if (c != EOF)
          _fcp->unget_c(c);
	if (c == EOF || c == ' ' || c == '\n' || compatible_flag) {
	  error("nested .PS");
	  eof = 1;
	  return EOF;
	}
	push_back[0] = 'S';
	push_back[1] = 'P';
	return '.';
      }
      if (c != EOF)
        _fcp->unget_c(c);
      push_back[0] = 'P';
      return '.';
    }
    else {
      if (c != EOF)
        _fcp->unget_c(c);
      return '.';
    }
  }
  if (c == '\n') {
    bol = 1;
    rf_current_lineno_inc();
    return '\n';
  }
  bol = 0;
  if (c == EOF) {
    eof = 1;
    error("end of file before .PE or .PF");
    error_with_file_and_line(rf_current_filename(), start_lineno - 1,
			     ".PS was here");
  }
  return c;
}

int top_input::peek()
{
  if (eof)
    return EOF;
  if (push_back[2] != EOF)
    return push_back[2];
  if (push_back[1] != EOF)
    return push_back[1];
  if (push_back[0] != EOF)
    return push_back[0];
  int c = _fcp->get_c();
  while (invalid_input_char(c)) {
    error("invalid input character code %1", int(c));
    c = _fcp->get_c();
    bol = 0;
  }
  if (bol && c == '.') {
    c = _fcp->get_c();
    if (c == 'P') {
      c = _fcp->get_c();
      if (c == 'F' || c == 'E') {
	int d = _fcp->get_c();
	if (d != EOF)
	  _fcp->unget_c(d);
	if (d == EOF || d == ' ' || d == '\n' || compatible_flag) {
	  eof = 1;
	  flyback_flag = c == 'F';
	  return EOF;
	}
	push_back[0] = c;
	push_back[1] = 'P';
	push_back[2] = '.';
	return '.';
      }
      if (c == 'S') {
	c = _fcp->get_c();
	if (c != EOF)
	  _fcp->unget_c(c);
	if (c == EOF || c == ' ' || c == '\n' || compatible_flag) {
	  error("nested .PS");
	  eof = 1;
	  return EOF;
	}
	push_back[0] = 'S';
	push_back[1] = 'P';
	push_back[2] = '.';
	return '.';
      }
      if (c != EOF)
	_fcp->unget_c(c);
      push_back[0] = 'P';
      push_back[1] = '.';
      return '.';
    }
    else {
      if (c != EOF)
	_fcp->unget_c(c);
      push_back[0] = '.';
      return '.';
    }
  }
  if (c != EOF)
    _fcp->unget_c(c);
  if (c == '\n')
    return '\n';
  return c;
}

int top_input::get_location(const char **filenamep, int *linenop)
{
  *filenamep = rf_current_filename();
  *linenop = rf_current_lineno();
  return 1;
}

void do_picture(file_case *fcp)
{
  flyback_flag = 0;
  int c;
  su_FREE(graphname);
  graphname = su_cs_dup("graph"); // default picture name in TeX mode
  while ((c = fcp->get_c()) == ' ')
    ;
  if (c == '<') {
    string filename;
    while ((c = fcp->get_c()) == ' ')
      ;
    while (c != EOF && c != ' ' && c != '\n') {
      filename += char(c);
      c = fcp->get_c();
    }
    if (c == ' ') {
      do {
        c = fcp->get_c();
      } while (c != EOF && c != '\n');
    }
    if (c == '\n')
      rf_current_lineno_inc();
    if (filename.length() == 0)
      error("missing filename after `<'");
    else {
      filename += '\0';
      char *old_filename = su_cs_dup(rf_current_filename());
      int old_lineno = rf_current_lineno();
      // filenames must be permanent
      do_file(su_cs_dup(filename.contents()));
      rf_current_filename_set(old_filename);
      rf_current_lineno_set(old_lineno);
      su_FREE(old_filename);
    }
    out->set_location(rf_current_filename(), rf_current_lineno());
  }
  else {
    out->set_location(rf_current_filename(), rf_current_lineno());
    string start_line;
    while (c != EOF) {
      if (c == '\n') {
	rf_current_lineno_inc();
	break;
      }
      start_line += c;
      c = fcp->get_c();
    }
    if (c == EOF)
      return;
    start_line += '\0';
    double wid, ht;
    switch (sscanf(&start_line[0], "%lf %lf", &wid, &ht)) {
    case 1:
      ht = 0.0;
      break;
    case 2:
      break;
    default:
      ht = wid = 0.0;
      break;
    }
    out->set_desired_width_height(wid, ht);
    out->set_args(start_line.contents());
    lex_init(new top_input(fcp));
    if (yyparse()) {
      had_parse_error = 1;
      lex_error("giving up on this picture");
    }
    parse_cleanup();
    lex_cleanup();

    // skip the rest of the .PF/.PE line
    while ((c = fcp->get_c()) != EOF && c != '\n')
      ;
    if (c == '\n')
      rf_current_lineno_inc();
    out->set_location(rf_current_filename(), rf_current_lineno());
  }
}

static void
do_file(char const *filename)
{
  file_case *fcp;
  if ((fcp = file_case::muxer(filename)) == NULL) {
    assert(strcmp(filename, "-"));
    delete out;
    fatal("can't open `%1': %2", filename, su_err_doc(errno));
  }

  out->set_location(filename, 1);
  rf_current_filename_set(filename);
  enum { START, MIDDLE, HAD_DOT, HAD_P, HAD_PS, HAD_l, HAD_lf } state = START;
  for (;;) {
    int c = fcp->get_c();
    while (invalid_input_char(c)) {
      error("invalid input character code %1", int(c));
      c = fcp->get_c();
    }
    if (c == EOF)
      break;
    switch (state) {
    case START:
      if (c == '.')
	state = HAD_DOT;
      else {
	putchar(c);
	if (c == '\n') {
	  rf_current_lineno_inc();
	  state = START;
	}
	else
	  state = MIDDLE;
      }
      break;
    case MIDDLE:
      putchar(c);
      if (c == '\n') {
	rf_current_lineno_inc();
	state = START;
      }
      break;
    case HAD_DOT:
      if (c == 'P')
	state = HAD_P;
      else if (lf_flag && c == 'l')
	state = HAD_l;
      else {
	putchar('.');
	putchar(c);
	if (c == '\n') {
	  rf_current_lineno_inc();
	  state = START;
	}
	else
	  state = MIDDLE;
      }
      break;
    case HAD_P:
      if (c == 'S')
	state = HAD_PS;
      else  {
	putchar('.');
	putchar('P');
	putchar(c);
	if (c == '\n') {
	  rf_current_lineno_inc();
	  state = START;
	}
	else
	  state = MIDDLE;
      }
      break;
    case HAD_PS:
      if (c == ' ' || c == '\n' || compatible_flag) {
	fcp->unget_c(c);
	do_picture(fcp);
	state = START;
      }
      else {
	fputs(".PS", stdout);
	putchar(c);
	state = MIDDLE;
      }
      break;
    case HAD_l:
      if (c == 'f')
	state = HAD_lf;
      else {
	putchar('.');
	putchar('l');
	putchar(c);
	if (c == '\n') {
	  rf_current_lineno_inc();
	  state = START;
	}
	else
	  state = MIDDLE;
      }
      break;
    case HAD_lf:
      if (c == ' ' || c == '\n' || compatible_flag) {
	string line;
	while (c != EOF) {
	  line += c;
	  if (c == '\n') {
	    rf_current_lineno_inc();
	    break;
	  }
	  c = fcp->get_c();
	}
	line += '\0';
	interpret_lf_args(line.contents());
	printf(".lf%s", line.contents());
	state = START;
      }
      else {
	fputs(".lf", stdout);
	putchar(c);
	state = MIDDLE;
      }
      break;
    default:
      assert(0);
    }
  }
  switch (state) {
  case START:
    break;
  case MIDDLE:
    putchar('\n');
    break;
  case HAD_DOT:
    fputs(".\n", stdout);
    break;
  case HAD_P:
    fputs(".P\n", stdout);
    break;
  case HAD_PS:
    fputs(".PS\n", stdout);
    break;
  case HAD_l:
    fputs(".l\n", stdout);
    break;
  case HAD_lf:
    fputs(".lf\n", stdout);
    break;
  }

  delete fcp;
}

#ifdef FIG_SUPPORT
void do_whole_file(const char *filename)
{
  // Do not set current_filename.
  file_case *fcp;
  if ((fcp = file_case::muxer(filename)) == NULL) {
    assert(strcmp(filename, "-"));
    fatal("can't open `%1': %2", filename, su_err_doc(errno));
  }

  lex_init(new file_input(fcp, filename));
  if (yyparse())
    had_parse_error = 1;
  parse_cleanup();
  lex_cleanup();

  delete fcp;
}
#endif

void usage(FILE *stream)
{
  fprintf(stream, "Synopsis: %s [ -nvCSU ] [ filename ... ]\n",
    rf_current_program());
#ifdef TEX_SUPPORT
  fprintf(stream, "       %s -t [ -cvzCSU ] [ filename ... ]\n",
    rf_current_program());
#endif
#ifdef FIG_SUPPORT
  fprintf(stream, "       %s -f [ -v ] [ filename ]\n", rf_current_program());
#endif
}

int main(int argc, char **argv)
{
  rf_current_program_set(argv[0]);
  static char stderr_buf[BUFSIZ];
  setbuf(stderr, stderr_buf);
  int opt;
#ifdef TEX_SUPPORT
  int tex_flag = 0;
  int tpic_flag = 0;
#endif
#ifdef FIG_SUPPORT
  int whole_file_flag = 0;
  int fig_flag = 0;
#endif
  static const struct option long_options[] = {
    { "help", no_argument, 0, CHAR_MAX + 1 },
    { "version", no_argument, 0, 'v' },
    { NULL, 0, 0, 0 }
  };
  while ((opt = getopt_long(argc, argv, "T:CDSUtcvnxzpf", long_options, NULL))
	 != EOF)
    switch (opt) {
    case 'C':
      compatible_flag = 1;
      break;
    case 'D':
    case 'T':
      break;
    case 'S':
      safer_flag = 1;
      break;
    case 'U':
      safer_flag = 0;
      break;
    case 'f':
#ifdef FIG_SUPPORT
      whole_file_flag++;
      fig_flag++;
#else
      fatal("fig support not included");
#endif
      break;
    case 'n':
      driver_extension_flag = 0;
      break;
    case 'p':
    case 'x':
      warning("-%1 option is obsolete", char(opt));
      break;
    case 't':
#ifdef TEX_SUPPORT
      tex_flag++;
#else
      fatal("TeX support not included");
#endif
      break;
    case 'c':
#ifdef TEX_SUPPORT
      tpic_flag++;
#else
      fatal("TeX support not included");
#endif
      break;
    case 'v':
      {
	puts(L_P_PIC " (" T_ROFF ") v" VERSION);
	exit(0);
	break;
      }
    case 'z':
      // zero length lines will be printed as dots
      zero_length_line_flag++;
      break;
    case CHAR_MAX + 1: // --help
      usage(stdout);
      exit(0);
      break;
    case '?':
      usage(stderr);
      exit(1);
      break;
    default:
      assert(0);
    }
  parse_init();
#ifdef TEX_SUPPORT
  if (tpic_flag) {
    out = make_tpic_output();
    lf_flag = 0;
  }
  else if (tex_flag) {
    out = make_tex_output();
    command_char = '\\';
    lf_flag = 0;
  }
  else
#endif
#ifdef FIG_SUPPORT
  if (fig_flag)
    out = make_fig_output();
  else
#endif
    out = make_troff_output();
#ifdef FIG_SUPPORT
  if (whole_file_flag) {
    if (optind >= argc)
      do_whole_file("-");
    else if (argc - optind > 1) {
      usage(stderr);
      exit(1);
    } else
      do_whole_file(argv[optind]);
  }
  else {
#endif
    if (optind >= argc)
      do_file("-");
    else
      for (int i = optind; i < argc; i++)
	do_file(argv[i]);
#ifdef FIG_SUPPORT
  }
#endif
  delete out;
  if (ferror(stdout) || fflush(stdout) < 0)
    fatal("output error");
  return had_parse_error;
}

// s-it2-mode
