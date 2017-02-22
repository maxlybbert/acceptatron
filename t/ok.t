#!/bin/env perl
use strict;
use warnings;
use Acceptatron 'get_contents';
use Config;
use File::Spec::Functions;
use FindBin '$Bin';
use Test::More;

my %op_table = (
	'<' => sub { return $_[0] < $_[1]; },
	'<=' => sub { return $_[0] <= $_[1]; },
	'==' => sub { return $_[0] == $_[1]; },
	'!=' => sub { return $_[0] != $_[1]; },
	'>' => sub { return $_[0] > $_[1]; },
	'>=' => sub { return $_[0] >= $_[1]; },
);

for my $i (0..3) {
	for my $j (0..3) {
		for my $op (keys %op_table) {
			my $msg = "$i $op $j";
			open my $OUT, '-|',
				catfile($FindBin::Bin, "ok$Config{_exe}"),
				'-a', $i, '-b', $j, '-o', $op, '-m', $msg;
			if (!defined($OUT)) {
				fail('unable to fork child process');
				done_testing();
			}
			my $response = get_contents($OUT);
			if ($op_table{$op}->($i, $j)) {
				like $response,	qr{ok\s+\d+\s*-\s*$msg}m, $msg;
				next;
			}
			like $response,	qr{not ok\s+\d+\s*-\s*$msg}m, $msg;
		}
	}
}
done_testing();

