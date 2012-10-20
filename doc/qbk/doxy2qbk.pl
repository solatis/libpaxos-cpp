#!/usr/bin/perl -w

use strict;

system("doxygen reference.dox");
chdir("xml");
system("xsltproc combine.xslt index.xml > all.xml");
chdir("..");
system("xsltproc reference.xsl xml/all.xml > reference.qbk");
system("rm -rf xml");
