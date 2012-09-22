#!/bin/bash

libtoolize --force
aclocal
autoheader
automake --add-missing --no-force
autoconf
