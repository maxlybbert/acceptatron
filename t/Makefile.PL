use strict;
use warnings;
use Config;
use File::Spec;
use File::Spec::Functions;

sub find_compiler {
	if ($ENV{CXX}) {
		return $ENV{CXX};
	}
	for my $dir (File::Spec->path()) {
		for my $compiler (qw{c++ cl clang++ g++}) {
			if (-e catfile($dir, $compiler . $Config{_exe})) {
				return $compiler;
			}
		}
	}
	return 'c++';
}

sub compiler_flags {
	my $compiler = shift;
	if ($compiler eq 'cl') {
		return qw{/I.. /EHsc setargv.obj};
	}
	return qw{-std=c++11 -I..};
}

sub compiler_output_flag {
	my $compiler = shift;
	return $compiler eq 'cl' ? '' : '-o';
}

my $compiler = find_compiler();
my @cxxflags = compiler_flags($compiler);
my $cxx_output_flag = compiler_output_flag($compiler);

open my $makefile, '>', 'Makefile' or die "unable to create Makefile: $!";
my @targets = qw{bail_out cmp_ok diag done_testing is isnt like note ok plan
	skip_all todo unlike};
my @binaries = map { $_ . $Config{_exe} } @targets;
my @tests = map { "$_.t" } @targets;

print $makefile <<END_MAKEFILE;
.POSIX: ;
CXX:=$compiler
CXXFLAGS+=@cxxflags
PERL_TESTS:=@tests
BINARIES:=@binaries

check: build
	prove \$(PERL_TESTS)

build: ../acceptatron.h getopt.h \$(BINARIES) \$(PERL_TESTS)

clean:
	-perl -MExtUtils::Command -e rm_f \$(BINARIES)

# gmake has this built in, and nmake has a similar rule with different flags
# (https://msdn.microsoft.com/en-us/library/cx06ysxh.aspx), but it's not
# listed in POSIX
# (http://pubs.opengroup.org/onlinepubs/009695399/utilities/make.html,
# "Default Rules")
.cc$Config{_exe}:
	\$(CXX) \$< \$(CXXFLAGS) $cxx_output_flag \$@

all: build ;
END_MAKEFILE
