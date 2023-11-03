mkdir concorde_build
cd concorde_build
../concorde/configure
make
cp concorde.a ../lib
cd ..
rm -r concorde_build
make -f Makefile CONF=Release
cp dist/Release/GNU-Linux/coco CoCo
 

