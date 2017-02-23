#!/bin/env perl
use strict;
use warnings;
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

for (0..3) {
	open my $OUT, '-|', catfile($FindBin::Bin, "diag$Config{_exe}"),
		qq{"hello $_"};
	if (!defined($OUT)) {
		fail('unable to fork child process');
		done_testing();
	}
	
	like <$OUT>, qr{^\s*\#\s*"hello $_".*$}, 'diagnostic written';
}
done_testing();

