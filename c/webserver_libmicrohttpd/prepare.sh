rm -r libmicrohttpd*

wget http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-latest.tar.gz
tar -xvzf libmicrohttpd-latest.tar.gz

cd libmicrohttpd*
./configure
make -j `nproc`
sudo make install

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
sudo /sbin/ldconfig -v

cd ..
rm libmicrohttpd-latest.tar.gz
mv libmicrohttpd-* libmicrohttpd-latest
# rm -r libmicrohttpd*
