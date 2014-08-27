#!/bin/sh

set -e

cd $TRAVIS_BUILD_DIR

make test
