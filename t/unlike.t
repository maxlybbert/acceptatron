#!/bin/env perl
use strict;
use warnings;
use Acceptatron 'get_contents';
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

my %op_table = (
	even => sub { return $_[0] % 2 == 0; },
	odd => sub { return $_[0] % 2; }
);

for my $i (0..3) {
	for my $op (keys %op_table) {
		my $msg = "$i is $op";
		open my $OUT, '-|',
			catfile($FindBin::Bin, "unlike$Config{_exe}"),
			'-a', $i, '-o', $op, '-m', $msg;
		if (!defined($OUT)) {
			fail('unable to fork child process');
			done_testing();
		}
		my $response = get_contents($OUT);
		if (!$op_table{$op}->($i)) {
			like $response,	qr{ok\s+\d+\s*-\s*$msg}m, $msg;
			next;
		}
		
		like $response, qr{^not\sok\s+\d+\s*-\s*$msg}m, "$msg 'not ok'";
		like $response, qr{^\s*\#\s*Failed test$}m,
			"$msg 'failed test'";
		like $response, qr{^\s*\#\s*'$i' matches pred.*$}m,
			"$msg pred message";
	}
}
done_testing();

