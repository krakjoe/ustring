#!/usr/bin/env sh

set -e

git clone --depth 1 https://github.com/php/php-src ~/php-src
cd ~/php-src
ln -s ~/krakjoe/ustring ext/ustring

./buildconf --force
./configure --disable-all --enable-ustring --enable-maintainer-zts
make -j2 --quiet

