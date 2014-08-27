#!/usr/bin/env sh

git clone https://github.com/php/php-src
cd php-src
git clone https://github.com/krakjoe/ustring ext/ustring
./buildconf --force
./configure --disable-all --enable-ustring --enable-maintainer-zts
make -j2 --quiet

