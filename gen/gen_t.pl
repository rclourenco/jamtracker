#!/usr/bin/env perl

use strict;
use warnings;

my @label = ("C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-", "--", "--", "--", "--", "--");

my $k = 3547264/128;

printf("uint8_t note_period_tab[2048] = {\n");
printf("\t  0, /* off  */\n");
for(my $i=1; $i<2048; $i++) {
	my $freq = ($k/$i);
 	my $d = $freq;
 	my $p = pitch($d);
 	my $note = $p%12;
 	my $oct = $p/12-1;
	printf("\t%3d%s /* %s%-2d */\n", $p, $i==2047?" ":",", $label[$note],$oct);
}
printf("};\n");

sub pitch {
	my $freq = shift;
	my $x = 81+12*(log($freq/880)/log(2));
	return int($x+0.5);
}
