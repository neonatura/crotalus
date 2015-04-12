#!/bin/bash
mkdir -p release/crotalus
cd release
../configure --libdir=/h/src/crotalus/release/crotalus --bindir=/h/src/crotalus/release/crotalus --sbindir=/h/src/crotalus/release/crotalus --docdir=/h/src/crotalus/release/crotalus --with-libshare=/h/src/sharelib/build
make
make install
tar -cpf crotalus-`arch`.tar crotalus
gzip -f crotalus-`arch`.tar
