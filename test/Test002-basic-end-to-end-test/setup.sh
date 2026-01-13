#!/bin/bash
TESTNAME="Test002-basic-end-to-end-test"
mkdir -p /tmp/$TESTNAME
# start bf_fuse in foreground with matching config
./build/$BUILD_CONFIG/bf_fuse -f --config ./test/$TESTNAME/config.json /tmp/$TESTNAME