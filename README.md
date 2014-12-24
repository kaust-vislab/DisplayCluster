# DisplayCluster

DisplayCluster is a software environment for interactively driving large-scale tiled displays. 

## Documentation

The DisplayCluster manual is included in the distribution in the doc/ directory, and covers installation and usage.

## Features

DisplayCluster provides the following functionality:
* Interactively view media such as high-resolution imagery and video
* Stream content from remote sources such as laptops / desktops or high-performance remote visualization machines
* [Documentation](http://bluebrain.github.io/DisplayCluster-0.4/index.html)

## Building from Source
OSX Build Pre-requisites

```
brew install qt
brew install poppler --with-qt
brew install mpich2 --without-fortran
brew install gcc --without-multilib
brew install fcgi
brew install boost --with-mpi --without-single
brew install jpeg-turbo
brew install ffmpeg
brew tap kaust-vislab/kvl
brew install tuio
```

Note that the Formulae for `tuio` is not available by default on homebrew, so you would have to use the homebrew-kvl tap.



```
  git clone https://github.com/kaust-vislab/DisplayCluster.git
  cd DisplayCluster
  git checkout osx # checkout osx branch
  mkdir build
  cd build
  cmake .. -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-w -Wno-deprecated-register -Wno-error=shadow -I/opt/X11/include" -DCMAKE_C_FLAGS="-w -Wno-deprecated-register -Wno-error=shadow -I/opt/X11/include" 
  make
```

Or using Buildyard (Not recommended for osx builds):

```
  git clone https://github.com/kaust-vislab/Buildyard.git
  cd Buildyard
  git clone https://github.com/BlueBrain/config.git config.bluebrain
  git clone https://github.com/kaust-vislab/config.local.git config.local
  make DisplayCluster
```

## Original Project

This version of DisplayCluster is a fork of the original project by the Texas Advanced Computing Center, Austin:

https://github.com/TACC/DisplayCluster




