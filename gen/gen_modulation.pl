#!/usr/bin/env perl

use warnings;
use strict;

my $PI = 3.14159265359;

print "int16_t mod_sine[64]={\n";
for(my $i=0;$i<64;$i++) {
	printf ("\t%d%s\n", sin($i*2*$PI/64.0)*64, $i!=63?",":"");
}
print "};\n";

print "\n";

my $m = 128/63;


print "int16_t mod_saw[64]={\n";
for(my $i=0;$i<64;$i++) {
	my $v = 64 - $m*$i;
	printf ("\t%d%s\n", $v, $i!=63?",":"");
}
print "};\n";

print "\n";

print "int16_t mod_square[64]={\n";
for(my $i=0;$i<64;$i++) {
	my $v = $i < 32 ? 64 :-64;
	printf ("\t%d%s\n", $v, $i!=63?",":"");
}
print "};\n";

print "\n";

print "int16_t mod_rnd[64]={\n";
for(my $i=0;$i<64;$i++) {
	my $v = 64-int(rand(129));
	printf ("\t%d%s\n", $v, $i!=63?",":"");
}
print "};\n";
