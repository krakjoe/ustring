#!/bin/sh

set -e

cd $HOME/krakjoe/ustring
$HOME/bin/phpize

./configure --with-php-config=$HOME/bin/php-config --enable-ustring
make -j2 --quiet
