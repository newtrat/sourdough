#!/bin/bash

cd .. && ./autogen.sh && ./configure && make && cd datagrump
./run-contest sudafed