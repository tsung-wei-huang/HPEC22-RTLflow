use strict;
use warnings;
 
my $filename = './result_jacobi_stencil_14';
open(my $fh, '<:encoding(UTF-8)', $filename)
  or die "Could not open file '$filename' $!";
 

my $f1 = 'repeat_1.csv';
my $f2 = 'repeat_5.csv';
my $f3 = 'repeat_10.csv';
my $f4 = 'repeat_100.csv';

open(my $fh1, '>', $f1) or die "Could not open file '$f1' $!";
open(my $fh2, '>', $f2) or die "Could not open file '$f2' $!";
open(my $fh3, '>', $f3) or die "Could not open file '$f3' $!";
open(my $fh4, '>', $f4) or die "Could not open file '$f4' $!";

my $r1 = <$fh>;
$r1 = <$fh>;
$r1 = <$fh>;

my $count = 0;
while (my $row = <$fh>) {
  if($count % 5 == 0) {
    print $fh1 "$row";
  }
  elsif($count % 5 == 1) {
    print $fh2 "$row";
  }
  elsif($count % 5 == 2) {
    print $fh3 "$row";
  }
  elsif($count % 5 == 3) {
    print $fh4 "$row";
  }
  $count += 1;
}


