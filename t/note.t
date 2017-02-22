#!/bin/env perl
use strict;
use warnings;
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

for (0..10) {
	open my $OUT, '-|', catfile($FindBin::Bin, "note$Config{_exe}"),
		"hello $_";
	if (!defined($OUT)) {
		fail('unable to fork child process');
		done_testing();
	}
	
	like <$OUT>, qr{^\s*\#\s*hello $_$}, 'diagnostic written';
}
done_testing();

