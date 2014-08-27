#!/bin/sh

set -e

cd ~/krakjoe/ustring
~/php/bin/phpize

./configure --with-php-config=~/php/bin/php-config --enable-ustring
make -j2 --quiet
