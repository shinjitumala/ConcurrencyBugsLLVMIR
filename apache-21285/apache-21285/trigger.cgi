#!/usr/bin/perl

$var = $ENV{'QUERY_STRING'};

print "Expires: Mon, 26 Jul 2012 05:00:00 GMT\n";
print "Cache-Control: max-age=3600\n\n";

print "<html>";
print "<head>";
print "<title>CGI Program</title>";
print "</head>";
print "<body>";
print "<h2>OK</h2>";
print "</body>";
print "</html>";
print "$var is passed\n"
