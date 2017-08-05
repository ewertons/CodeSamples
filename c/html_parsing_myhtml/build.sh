echo "This only clones and builds myhtml library"

git clone --recursive https://github.com/lexborisov/myhtml.git
cd myhtml/
make -j `nproc`
cd examples/myhtml
