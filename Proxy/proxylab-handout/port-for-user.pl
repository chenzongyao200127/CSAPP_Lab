#! /usr/bin/perl -w
use strict;
use Digest::MD5;

# Script: port-for-user.pl
# Purpose: Generate a unique, even port number for a given user name with low collision probability.
# Usage: ./port-for-user.pl [optional user name]

# Maximum and minimum port numbers.
my $maxport = 65536;
my $minport = 1024;

# Subroutine to compute an even port number based on a hash of the given name.
sub hashname {
    # Retrieve the name passed as an argument.
    my $name = shift;
    my $port;

    # Generate MD5 hash of the name.
    my $hash = Digest::MD5::md5_hex($name);

    # Take only the last 8 hex digits (last 32 bits) of the hash.
    $hash = substr($hash, -8);

    # Convert the hex string to an integer.
    $hash = hex($hash);

    # Calculate a port number within the specified range and ensure it's even.
    $port = $hash % ($maxport - $minport) + $minport;
    $port = $port & 0xfffffffe;

    # Print the user name and the calculated port number.
    print "$name: $port\n";
}

# Main program logic.
# If no command line argument is provided, use the current user's name.
# Otherwise, use the provided argument(s).
if($#ARGV == -1) {
    # Get the user name from the user ID.
    my ($username) = getpwuid($<);

    # Generate a port number for the current user.
    hashname($username);
} else {
    # Generate a port number for each provided argument.
    foreach(@ARGV) {
        hashname($_);
    }
}
