#!/usr/bin/env sh

set -e

mkdir -p ~/php
git clone --depth 1 https://github.com/php/php-src ~/php/src

cd ~/php/src
./buildconf --force
./configure --prefix=~/php --disable-all --enable-maintainer-zts
make -j2 --quiet install

