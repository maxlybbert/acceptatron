#!/bin/env perl
use strict;
use warnings;
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

for (0..10) {
	open my $OUT, '-|', catfile($FindBin::Bin, "plan$Config{_exe}"),
		'-p', $_;
	if (!defined($OUT)) {
		fail('unable to fork child process');
		done_testing();
	}
	
	my $tap = <$OUT>;
	like $tap, qr{^TAP(?:\s+version\s+\d+)?$}, 'TAP header written';
	my $plan = <$OUT>;
	like $plan, qr{^1..$_$}, 'plan written';
}
done_testing();

