#!/bin/env perl
use strict;
use warnings;
use Acceptatron 'get_contents';
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

for my $i (0..3) {
	for my $j (0..3) {
		my $msg = "$i == $j";
		open my $OUT, '-|', catfile($FindBin::Bin, "is$Config{_exe}"),
			'-a', $i, '-b', $j, '-m', $msg;
		if (!defined($OUT)) {
			fail('unable to fork child process');
			done_testing();
		}
		my $response = get_contents($OUT);
		if ($i == $j) {
			like $response,	qr{ok\s+\d+\s*-\s*$msg}m, $msg;
			next;
		}
		like $response, qr{^not\sok\s+\d+\s*-\s*$msg}m, "$msg 'not ok'";
		like $response, qr{^\s*\#\s*Failed test.*$}m,
			"$msg 'failed test'";
		like $response, qr{^\s*\#\s*got:\s*'$i'.*$}m, "$msg got";
		like $response, qr{^\s*\#\s*expected:\s*'$j'.*$}m,
			"$msg expected";
	}
}
done_testing();

