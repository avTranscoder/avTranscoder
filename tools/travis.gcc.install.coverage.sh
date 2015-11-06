#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# install latest LCOV (1.9 was failing for me)
wget http://ftp.de.debian.org/debian/pool/main/l/lcov/lcov_1.11.orig.tar.gz
tar xf lcov_1.11.orig.tar.gz
sudo make -C lcov-1.11/ install

# install lcov to coveralls conversion + upload tool
gem install coveralls-lcov

# init coverage to 0 (optional)
lcov --directory ${AVTRANSCODER_BUILD} --zerocounters

# install nosetests plugins
sudo pip install coverage
