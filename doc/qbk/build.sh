#!/bin/bash

./doxy2qbk.pl
bjam
rm -rf ../paxos++/
mkdir ../paxos++/
mkdir ../paxos++/exception/
cp ../../paxos++/client.hpp ../paxos++/
cp ../../paxos++/configuration.hpp ../paxos++/
cp ../../paxos++/server.hpp ../paxos++/
cp ../../paxos++/exception/exception.hpp ../paxos++/exception/
