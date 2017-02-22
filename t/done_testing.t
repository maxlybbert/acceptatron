#!/bin/env perl
use strict;
use warnings;
use Acceptatron 'get_contents';
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

my $OUT;
my $cmd = catfile($FindBin::Bin, "done_testing$Config{_exe}");
for my $tests (0..3) {
	for my $failures (0..$tests) {
		open $OUT, '-|', $cmd, '-t', $tests, '-f', $failures;
		if (!defined($OUT)) {
			fail('unable to fork child process');
			done_testing();
		}
		my $payload = get_contents($OUT);
		close $OUT;
		is $? >> 8, $failures, 'status code gives number of failures';

		like $payload, qr{^1..$tests$}m, 'number of tests written';
		open $OUT, '-|', $cmd, '-t', $tests, '-f', $failures, '-p';
		$payload = get_contents($OUT);
		like $payload, qr{^1..$tests$}m,
			'number of tests written with plan';
		my $plan_lines = scalar(grep qr{^1..\d+}, $payload);
		is $plan_lines, 1, 'no duplicate plan lines';
	}
}

open $OUT, '-|', $cmd, qw{-t300 -f300};
get_contents($OUT);
close $OUT;
is $? >> 8, 254, 'status code is clamped to 254';

done_testing();

