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
		for my $op (qw{< <= == != > >=}) {
			my $msg = "$i $op $j";
			open my $OUT, '-|',
				catfile($FindBin::Bin, "todo$Config{_exe}"),
				'-a', $i, '-b', $j, '-o', $op, '-m', $msg;
			if (!defined($OUT)) {
				fail('unable to fork child process');
				done_testing();
			}
			my $response = get_contents($OUT);
			like $response,	qr{^.[^#]*\#\s*TODO.*$}m, $msg;
		}
	}
}
done_testing();

