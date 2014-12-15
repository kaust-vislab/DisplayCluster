# DisplayCluster

DisplayCluster is a software environment for interactively driving large-scale tiled displays.

## Documentation

The DisplayCluster manual is included in the distribution in the doc/ directory, and covers installation and usage.

## Features

DisplayCluster provides the following functionality:
* Interactively view media such as high-resolution imagery and video
* Stream content from remote sources such as laptops / desktops or high-performance remote visualization machines
* [Documentation](http://bluebrain.github.io/DisplayCluster-0.4/index.html)

## Building from Source using Buildyard

```
  module load boost/1.55.0-mpich-3.1
  module load ffmpeg/0.10.2
  module load turbojpeg/1.2.1
  module load mpich-x86_64
  module load python/2.7.3
  module load qt/4.8.1
  module load fcgi/2.4.1
  module load TUIO/1.4

  git clone https://github.com/kaust-vislab/DisplayCluster.git
  cd DisplayCluster
  mkdir build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/your/custom/install/dir
```

## Original Project

This version of DisplayCluster is a fork of the original project by the Texas Advanced Computing Center, Austin:

https://github.com/TACC/DisplayCluster
