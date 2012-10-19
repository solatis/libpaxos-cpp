#!/bin/bash

bjam
find html/ -type f -exec sed -i 's/\/home\/lmergen\/git\/libpaxos-cpp\///g' {} \;
rm -rf ../paxos++/
mkdir ../paxos++/
cp ../../paxos++/*.hpp ../paxos++/
