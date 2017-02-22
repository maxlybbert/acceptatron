package Acceptatron;
require Exporter;
@ISA = qw{Exporter};
@EXPORT_OK = qw{get_contents};

use strict;
use warnings;

sub get_contents {
	my $fh = shift;
	local $/;
	seek $fh, 0, 0;
	return <$fh>;
}

1;
