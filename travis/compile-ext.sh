#!/bin/sh -x

set -e

cd $TRAVIS_BUILD_DIR

$HOME/bin/phpize

./configure --with-php-config=$HOME/bin/php-config --enable-ustring

make -j2 --quiet
