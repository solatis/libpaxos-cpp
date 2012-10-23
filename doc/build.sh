#!/bin/bash

./doxy2qbk.pl
bjam
find html/ -name '*.html' -exec sed -i 's/<head>/<head>\n<meta name="keywords" value="paxos,c++">/g' {} \;
