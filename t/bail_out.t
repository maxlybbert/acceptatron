#!/bin/env perl
use strict;
use warnings;
use Acceptatron 'get_contents';
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More tests => 2;

open my $OUT, '-|', catfile($FindBin::Bin, "bail_out$Config{_exe}");
if (!defined $OUT) {
	fail('unable to fork child process');
	done_testing();
}

like(get_contents($OUT), qr{^Bail\s*out!}, 'message written to STDOUT');
close $OUT;
is $? >> 8, 255, 'valid exit status';

