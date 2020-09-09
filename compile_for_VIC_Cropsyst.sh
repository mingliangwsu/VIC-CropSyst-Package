#!/bin/sh
cd ~/dev/common/weather/build/gcc/Release
rm *.o *.a
make -f Makefile_Kamiak
cd ~/dev/corn/build/gcc/Release
rm *.o *.a
make -f Makefile_Kamiak
cd ~/dev/UED/library/build/gcc/Release/
rm *.o *.a
make -f Makefile_Kamiak
cd ~/dev/CS_suite/build/gcc/Release/
rm *.o *.a
make -f Makefile_Kamiak
cd ~/dev/VIC_CropSyst/build/library/gcc/Release/
rm *.o *.a
make -f Makefile_Kamiak
cd ~/dev/VIC_CropSyst/build/gcc/Release/
rm *.o *.a VIC_CropSyst
make -f Makefile_Kamiak

