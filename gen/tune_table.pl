#!/usr/bin/env perl

use warnings;
use strict;

my $ratio = 2**(1/12);

my $k = 2**(1/96);

print "Ratio: ",$ratio,"\n";
print "k: ",$k,"\n";

my @table = (
	$k**(0), 
	$k**(1),
	$k**(2),
	$k**(3),
	$k**(4),
	$k**(5),
	$k**(6),
	$k**(7),
	$k**(-8), #8
	$k**(-7), #9
	$k**(-6), #A
	$k**(-5), #B
	$k**(-4), #C
	$k**(-3), #D
	$k**(-2), #E
	$k**(-1), #F
);

for(@table) {
	print "===> $_\n";
}

print "Arpeggio table\n";

my @arp_table = map {$ratio**$_ } (0..15);
for(@arp_table) {
	print "$_\n";
}
