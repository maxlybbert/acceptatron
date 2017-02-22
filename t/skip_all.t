#!/bin/env perl
use strict;
use warnings;
use Acceptatron 'get_contents';
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More tests => 2;

open my $OUT, '-|', catfile($FindBin::Bin, "skip_all$Config{_exe}");
if (!defined $OUT) {
	fail('unable to fork child process');
	done_testing();
}

like(get_contents($OUT), qr{^1..0\s*\#\s*Skipped},
	'skip message written to STDOUT');
close $OUT;
is $? >> 8, 0, 'valid exit status';
