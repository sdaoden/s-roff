#!/bin/sh -
#@ Extract documentation from the given sources and place them in $DOCDIR/.


FIXME
   no.  since we will never have s-hdoc boost this one to be what will be
   s-hdoc.  use mdoc syntax, with additions of the form man-XY, as in
   \man-ifdef, \man-endif, \man-define, \man-file, \man-link, \man-include,
   and such.  Like \man-dev[FORMAT]{bla}.
   add optional arguments like that Y (\X[Y]{Z}).
   do not use POD at all that is.
   that also means that we need \p, no .blm (hihi) anymore.

S-roff delayed AGAIN !!!!! because of Efail.

I will become ARRESTED in the lUNATIC aSYLUM if i cannot come back and get this
roff thing going STARTING THEREAFTER!!!!!  I want it!!!!!


#@ XXX May run in EMFILE situations (in Manual::sources()).
#@ TODO - do the POD conversion ourselfs (no external progs, no sed script)
#@ TODO - \#LINK not supported yet; \#{LINK} should be possible!
#@ TODO - escaped left braces keep their escaping reverse solisud
#@ TODO - < and > need to be escaped when placed in POD elements!
#
# Copyright (c) 2013 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# Now start perl(1) without PERL5OPT set to avoid multibyte sequence errors
PERL5OPT= PERL5LIB= exec perl -x "${0}" "${@}"
exit 1
# Thanks to perl(5) and it's -x / #! perl / __END__ mechanism!
# Why can env(1) not be used for such easy things in #!?
#!perl

my $DEBUG = 1;
my $DOCDIR = '.doc';
my $LINKFILE = $DOCDIR . '/links.lst';

##  --  >8  --  8<  --  ##

require 5.008_001;
use diagnostics -verbose;
use strict;
use warnings;

# 0: no arguments, 1: arguments, 2: optional arguments
my %COMMS = (
   author =>1,
   group =>1, group1 =>1, group2 =>1, group3 =>1, group4 =>1,
   link =>1, man =>1, seealso =>1, sources =>1,

   code =>0, end =>0, fun =>1, list =>2, li =>2, quote =>0, sp =>0, syn =>1,
);

my %POD_MAPS = (
   b => 'B', c => 'C', em => 'I'
);

my %FREE_MAPS = (
   em => '--',
   NIL => 'C<NIL>', THIS => 'C<this>'
);

my ($LBRACE, $RBRACE) = ('\{', '}');

sub main_fun{
   panic("USAGE: doc-extract.pl INPUT [:INPUT:]") unless @ARGV;
   unless(-d '.doc'){
      panic("Cannot create .doc/") unless mkdir '.doc'
   }
   while(@ARGV){
      Manual->new(shift @ARGV)
   }
   exit 0
}

sub v{
   return unless $DEBUG;
   print STDOUT '-V  ', shift, "\n";
   while(@_ != 0) {print STDOUT '-V ++ ', shift, "\n"}
}

sub alert{
   print STDERR '!ALERT ', shift, "\n";
   while(@_ != 0) {print STDERR '!ALERT ++  ', shift, "\n"};
}

sub panic{
   print STDERR '!PANIC ', shift, "\n";
   while(@_ != 0) {print STDERR '!PANIC ++  ', shift, "\n"};
   exit 1
}

{package Manual;
   our ($self_cnt, $self_id1, $link_fh) = (0, undef);

   sub _new{
      my $class = shift;
      my $self = {
         ID => ++$self_cnt,
         IFILE => undef,   # Input file path
         IFH => undef,     # .. fd
         OFILE => undef,   # Output file basename
         OBASE => undef,   # .. prepared for L<>
         OFH => undef,     # .. fd
         SEEALSO => [],
         SOURCES => '',    # \sources
         AUTHORS => undef, # \author (only on "mainpage")
         #
         BLOCKSTACK => [], # Stack of open commands which require \end
         LISTS => [],      # List tracking
         NEEDSEP => 0,     # Seen POD thing, need newline seps around it
         CODE => 0,        # \code open
         QUOTE => 0        # \quote open
      };
      $self_id1 = $self if $self->{ID} == 1;
      bless $self, $class
   }

   sub new{
      my $self = _new(shift);
      $self->{IFILE} = shift;

      ::panic("Cannot read '$self->{IFILE}'")
         unless open $self->{IFH}, '<', $self->{IFILE};
      print ".. '$self->{IFILE}'\n";

      while(readline $self->{IFH}){
         next unless $_ =~ /^\s*\/\*!\@/;
         $self->parse()
      }
      $self->finalize();
      close $self->{IFH};

      close $link_fh if $self->{ID} == 1 && $link_fh;
      $self
   }

   sub _next_line{
      my $self = $_[0];
      my $cl = undef;
      # Handle backslash escaped newlines XXX non-even number of backslashes
      for(;;){
         my $l = readline $self->{IFH};
         last unless $l;
         chomp $l;
         # Don't care!  If the line contains */, we're finished!
         if(0 <= index $l, '*/'){
            $cl = undef;
            last
         }
         # After a lot of distress with U+00A0 (not matched by \s), test it!
         ::v("NBSP U+00A0 in line <$l>") if $l =~ /\xA0/;

         # During \code, special handling is used
         # XXX We should NOT touch any leading whitespace if there is no
         # XXX asterisk around at all, since this seems to be a carefully
         # XXX hand-crafted textfile, then
         if($self->{CODE}){
            $l =~ /^\s*(?:\*\s?)?(.*)$/;
            $cl = $1;
            last
         }

         my $cont = ($l =~ s/^(.*)\\$/$1/);
         $l =~ s/^\s*(?:\*\s*)?(.*)$/$1/;
         $cl = $cl ? $cl . $l : $l;
         last unless $cont
      }
      $cl
   }

   sub parse{
      my $self = shift;
      while(defined(my $l = $self->_next_line())){
         # \code blocks get special treatment for leading WS, and we have to
         # take care for the 'non-command' \end
         if($self->{CODE}){
            if($l =~ /^\s*(?:\*\s*)?\\end\s*$/){
               pop @{$self->{BLOCKSTACK}};
               $self->{CODE} = 0;
               next
            }
            $l =~ s/^(\s*)(.*)$/'   ' . ("   " x length $1) . $2/e
         }elsif($l =~ /^\\([\w]+)\s?(?:${LBRACE}(.*)${RBRACE})?$/ &&
               exists $COMMS{$1}){
            my ($c, $r) = ($1, $2);
            $c = $self->comm($c, $r);
            next unless $c < 0;
            last
         }else{
            $l = $self->text_expand($l)
         }

         $self->force_ofile() unless $self->{OFILE};
         print {$self->{OFH}} "\n" if $self->{NEEDSEP} > 0;
         print {$self->{OFH}} $l, "\n";
         $self->{NEEDSEP} = -length $l
      }
      if(@{$self->{BLOCKSTACK}}){
         do{
            ::alert('Block command open: ' . pop @{$self->{BLOCKSTACK}})
         }while(@{$self->{BLOCKSTACK}});
         ::panic('Block commands remain open')
      }
      $self
   }

   sub finalize{
      my $self = shift;
      $self->sources() if length $self->{SOURCES};

      # Did we have worked any content for this one?
      if($self->{OFILE}){
         # .. then handle \seealso entries etc.
         # Sort it, and before we unshift the mainpage so that it'll be first
         @{$self->{SEEALSO}} = sort @{$self->{SEEALSO}};

         my ($pre, $post) = ('', "\n");
         if($self->{ID} == 1){
            print {$self->{OFH}} "\n\n=head1 API REFERENCES\n\n=over 3\n\n";
            $pre = "\n\n=item ";
            $post = "\n\n";
         }else{
            print {$self->{OFH}} "\n\n=head1 SEE ALSO\n\n";
            unshift(@{$self->{SEEALSO}}, $self_id1->{OBASE});
         }
         while(defined (my $e = shift @{$self->{SEEALSO}})){
            $e =~ /^([^(]+)/;
            print {$self->{OFH}} $pre . 'L<' . $e . '|' . ($1 ? $1 : $e) .
               (@{$self->{SEEALSO}} ? ">, " : "> " . $post);
         }

         if($self->{ID} == 1){
            print {$self->{OFH}} "\n\n=back\n";

            # And the mainpage may also have \author's
            if($self->{AUTHORS}){
               print {$self->{OFH}} "\n=head1 AUTHORS\n\n";
               while(defined (my $a = shift @{$self->{AUTHORS}})){
                  print {$self->{OFH}} $a .
                     (@{$self->{AUTHORS}} ? ",\n" : "\n")
               }
            }
         }
         close $self->{OFH}
      }
      $self
   }

   sub comm{ # {{{
      my ($self, $c, $l) = @_;
      ::panic("Unknown command: '$c'") unless exists $COMMS{$c};
      if($COMMS{$c}){
         ::panic("$c: requires argument(s)") unless($COMMS{$c} == 2 || $l);
      }else{
         ::alert("$c: does not take argument(s)") if $l;
      }

      # \man is special since it can be used to create multiple output files
      # for one input file
      if($c eq 'man'){
         my $xself = undef;

         if($self->{OFH}){
            $xself = _new(__PACKAGE__);
            $xself->{IFILE} = $self->{IFILE};
            $xself->{IFH} = $self->{IFH};
            my $i = $self;
            $self = $xself;
            $xself = $i
         }

         $l =~ s/(?:.*\/)?(.+)\s*/$1/;
         $self->{OFILE} = $l;
         $l =~ /(.+)\.(.+)/;
         $self->{OBASE} = "$1($2)";
         $self->{OPOD} = $DOCDIR . '/' . $1 . '.pod';
         ::panic("Cannot write '$self->{OPOD}'")
            unless open $self->{OFH}, '>', $self->{OPOD};
         ::v("Created output file '$self->{OPOD}'");

         if($xself){
            $self->parse()->finalize();
            push @{$self_id1->{SEEALSO}}, $self->{OBASE};
            $self = $xself;
            return -1
         }
         return 0
      }

      # No \man command first, create the name of this manual page
      $self->force_ofile unless $self->{OFILE};

      # Care must be taken to surround POD commands with empty lines.
      # Since the PODs are not shipped, don't care about beauty
      my ($out, $needsep) = ('', 1);
      if($self->{NEEDSEP}){
         $self->{NEEDSEP} = 0;
         $out = "\n"
      }

      # And the comms
      if($c eq 'code'){
         push @{$self->{BLOCKSTACK}}, 'code';
         $self->{CODE} = 1;
         $needsep = 0
      }elsif($c eq 'end'){
         ::panic('No commands to \end') unless @{$self->{BLOCKSTACK}};
         $c = pop @{$self->{BLOCKSTACK}};
         if($c eq 'code'){
            # Verbatim mode is specially handled, never reached...
            $self->{CODE} = 0
         }elsif($c eq 'list'){
            pop @{$self->{LISTS}};
            $out .= '=back'
         }elsif($c eq 'quote') { # XXX should be nothing in between, like /code
            $self->{QUOTE} = 0;
            $out .= '=back'
         }
      }elsif($c eq 'fun'){
         if($l =~ /^\s*(?:#(\s*)define\s*)(.+)$/){
            $l = '#' . $1 . 'define B<' . $2 . '>'
         }elsif($l =~ /^((?:\w+ )?\w+)\s+(\w+)\(([^)]+)\);?$/){
            $l = $1 . ' B<' . $2 . '>(' . $3 . ');'
         }elsif($l =~ /^typedef\s+(.+?)\s+(\w+);?$/){
            $l = 'typedef ' . $1 . ' B<' . $2 . '>;'
         }elsif($l =~ /^struct\s+(\w+)\s*{?$/){
            $l = 'struct B<' . $1 . '>'
         }elsif($l =~ /^enum\s+(\w+)\s*{?$/){
            $l = 'enum B<' . $1 . '>'
         }else{
            ::panic("Bad usage of \\fun: $l")
         } # } } }
         $out .= "=for html <div class=\"decl\">\n\n" .
               "=over 3\n\n=item $l\n\n=back\n\n" .
               "=for html </div>"
      }elsif($c eq 'list'){
         push @{$self->{BLOCKSTACK}}, 'list';
         if(!$l || !length $l){
            $l = 'bullet'
         }elsif($l !~ /^(bullet|desc|number)$/){
            ::panic("For \\list{X}, X must be 'bullet', 'desc' or 'number'")
         }
         push @{$self->{LISTS}}, $l;
         $out .= '=over'
      }elsif($c eq 'li'){
         ::panic("'\\li' without a '\\list'") unless @{$self->{LISTS}};
         # XXX weeell, we could auto-deduct bullet and number, AND warn.. ?
         # TODO if no argument is given we yet do not provide default values
         # TODO for numbered lists, and use the very same bullet item in all
         # TODO levels; we should track item counts etc..
         if(length $l){
            $l = ' ' . $self->text_expand($l);
         }else{
            $l = $self->{LISTS}->[@{$self->{LISTS}} - 1];
            if($l eq 'bullet') {$l = ' *'}
         }
         $out .= "=item$l"
      }elsif($c eq 'quote'){
         ::panic("Already in a '\\quote'") if $self->{QUOTE};
         push @{$self->{BLOCKSTACK}}, 'quote';
         $self->{QUOTE} = 1;
         $out .= '=over'
      }elsif($c eq 'sp'){
         $out .= "=begin html\n\n<p class=\"sp\"><br /></p>\n\n=end html"
      }elsif($c eq 'syn'){
         $c = $l;
         $c =~ s/&/&amp;/g;
         $c =~ s/</&lt;/g;
         $c =~ s/>/&gt;/g;
         $c =~ s/^(\s+)(.*)$/('&nbsp;&nbsp;&nbsp;' x length $1) . $2/e;
         $out .= "=begin html\n\n<p class=\"syn\">$c</p>\n\n=end html\n\n" .
            "=begin :man\n\nB<< $l >>\n\n=end :man"
      }
      # []
      elsif($c eq 'author'){
         # \author's are always propagated to the mainpage
         $self_id1->{AUTHORS} = [] unless $self_id1->{AUTHORS};
         push @{$self_id1->{AUTHORS}}, $l
      }elsif($c =~ /^group(\d)*$/){
         $c = $1 ? $1 : '1';
         $l =~ /(?:"([^"]+)")|(\w+)\s*(.*)/;
         my $n = $1 ? $1 : $2;
         # If there is descriptive text, check it for the usual "X1[,X2] -- Y"
         # notation, and if so, enwrap each X in B<>
         if($3){
            my $v = $3;
            if($v =~ /(.*?)\s*--\s*(.*)/){
               my ($keys, $desc) = ($1, $2);
               $keys =~ s/(?:([\w-]+)((?:,\s*)?))/B<$1>$2/g;
               $v = $keys . ' -- ' . $desc;
               $n .= "\n\n"
            }else{
               $n .= ' '
            }
            $v = $self->text_expand($v);
            $n .= $v
         }
         $out .= "=head$c $n"
      }elsif($c eq 'link'){
         force_link_file() unless $link_fh;
         ::panic("Cannot write '$LINKFILE'\n")
            unless print $link_fh $self->{OFILE}, ' ', $l, "\n";
         $needsep = 0
      }elsif($c eq 'seealso'){
         push @{$self->{SEEALSO}}, $l;
         $needsep = 0
      }elsif($c eq 'sources'){
         $self->{SOURCES} .= $l . ' ';
         $needsep = 0
      }

      print {$self->{OFH}} $out, "\n" if length $out;
      $self->{NEEDSEP} = $needsep;
      0
   } # }}}

   sub text_expand{ # {{{
      our ($self, $t) = @_;

      sub recur{
         my ($lvl, $resref, $tailref) = @_;
         ++$lvl;
jnext:
         if($$tailref =~ /(.*?)\\([\w]+)${LBRACE}(.*)/){
            my ($m1, $m2, $m3, $i, $res);

            ($m1, $m2, $m3) = ($1, $2, $3);
            $m1 =~ s/\\${RBRACE}/${RBRACE}/g if $lvl > 0;
            $$resref .= $m1;
            $m1 = '';
            $$tailref = $m3;
jftail:
            if($$tailref =~ /(.*?)(?<!\\)${RBRACE}(.*)/){
               my ($mm1, $mm2) = ($1, $2);

               if($mm1 =~ /\\([\w]+)${LBRACE}/){
                  $res = '';
                  recur($lvl, \$res, $tailref);
                  $m1 .= $res;
                  goto jftail
               }

               $$tailref = $mm2;
               $mm1 =~ s/\\$RBRACE/$RBRACE/g;
               $m1 .= $mm1;

               if(exists $POD_MAPS{$m2}){
                  $m1 = $POD_MAPS{$m2} . '<' . $m1 . '>'
               }else{
                  $m1 = 'C<' . $m1 . '>'
               }
               $$resref .= $m1;
               goto jnext if length $$tailref && $lvl == 0
            }else{
               ::alert("Command not closed") # TODO
            }
         }else{
            $$resref .= $$tailref
         }
      }
      my $res = '';
      recur(-1, \$res, \$t);
      $t = $res;
      # TODO allow escaping of \ before trigger word
      for($res = ''; $t && $t =~ /(.*?)(?:\\([\w]+))(\s+.*)?/;){
         $res .= $1;
         $t = $3;
         if(exists $FREE_MAPS{$2}){
            $res .= $FREE_MAPS{$2}
         }else{
            ::alert("Unknown expansion request: $2");
            $res .= '$$' . $2
         }
      }
      $res .= $t if $t;
      $res
   } # }}}

   sub force_ofile{
      my $self = $_[0];
      my $f = $self->{IFILE};
      $f =~ s/(.+)(?:\..+)/$1.3/;
      $self->comm('man', $f)
   }

   sub sources{
      my $self = $_[0];
      foreach my $s (glob $self->{SOURCES}){
         my $m = Manual->new($s);
         push @{$self->{SEEALSO}}, $m->{OBASE} if $m->{OFILE}
      }
   }

   sub force_link_file{
      ::panic("Cannot open '$LINKFILE'")
         unless open $link_fh, '>>', $LINKFILE
   }
}

{package main; main_fun()}

# vim:set ft=perl:s-it-mode
