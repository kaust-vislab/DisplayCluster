#!/bin/bash


module load supercollider/3.5.7
module load kvl-remote
module load boost/1.49.0-mpich-3.1
module load ffmpeg/0.10.2
module load turbojpeg/1.2.1
module load mpich-x86_64
module load python/2.7.3
module load qt/4.8.1
module load fcgi/2.4.1
module load TUIO/1.4


cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/software/DisplayCluster/sc-dev
make -j24
make install 