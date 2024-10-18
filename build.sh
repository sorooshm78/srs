# build pjsip
make -C pjproject
make -C pjproject install

# build srs
mkdir -p build  
cmake -S src -B build
make -C build
