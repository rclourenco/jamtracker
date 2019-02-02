#!/usr/bin/env perl

use strict;
use warnings;

my $clock_pal  = 7093789.2;
my $clock_ntsc = 7159090.5;

#my @periods = (428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226);
my @periods = (1..2024);

print "uint32_t period_phase[2048] = {\n";
print "\t0, /* off */\n";
foreach(@periods)
{
	my $sps1 = $clock_pal/(2*$_);
	#	my $sps2 = $clock_ntsc/(2*$_);
	#
	#	my $ka = 8287.13691588785;
	my $k = 8287.13691588785/22050;


	my $d = $sps1/22050;
	my $phase = int($d*0x10000+0.5);
	#	print "$_ => $sps1, $d\n";

	printf("\t0x%08X, /* %5d => %10.4f, %10.4f %08X */\n", $phase, $_, $sps1, $d, $phase);
}

print "\t0 };\n";

