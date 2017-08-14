git clone --recursive https://github.com/lexborisov/myhtml.git
cd myhtml/
make -j `nproc`
make install
ldconfig -v
cd ..
rm -r myhtml
