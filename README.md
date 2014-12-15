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
brew install boost
brew install jpeg-turbo
brew install ffmpeg
```

```
  git clone https://github.com/kaust-vislab/DisplayCluster.git
  cd DisplayCluster
  git checkout osx # checkout osx branch
  mkdir build
  cd build
  ccmake ..
  make
```

Or using Buildyard:

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




