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

For the scripts below, the variables `<INSTALL_DIR>` and `<CONFIG_DIR>` should be replaced with the paths to the install directory and the configuration directory for DisplayCluster.


```
  module load mpi/mpich-x86_64

  git clone -b CentOS7 https://github.com/kaust-vislab/DisplayCluster.git
  cd DisplayCluster
  mkdir build
  cd build
  cmake .. -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
  make -j72
  make install
```

## Modules File for DisplayCluster

This file can also be found [here](examples/modulefiles/DisplayCluster/0.4).

```
  #%Module
  #
  # Modulefile for Display Cluster (touch) 
  #
  # Madhu Srinivasan, madhu.srinivasan@kaust.edu.sa
  #
  proc ModulesHelp { } {
    puts stderr " "
    puts stderr "\t This module installs DisplayCluster v0.4 "
    puts stderr " "
  }

  module-whatis   "installs DisplayCluster v0.4"

  module load mpi/mpich-x86_64

  prepend-path DISPLAYCLUSTER_CONFIG_DIR <CONFIG_DIR>
  prepend-path DISPLAYCLUSTER_DIR <INSTALL_DIR>
  set package_root <INSTALL_DIR>

  prepend-path PATH ${package_root}/bin
  prepend-path LIBRARY_PATH ${package_root}/lib
  prepend-path LD_LIBRARY_PATH ${package_root}/lib
  prepend-path CMAKE_LIBRARY_PATH ${package_root}/lib
  prepend-path CPLUS_INCLUDE_PATH ${package_root}/include
  prepend-path CMAKE_INCLUDE_PATH ${package_root}/include

```

## Packaging DesktopStreamer for osx Distribution

TBD

## Original Project

This version of DisplayCluster is a fork of the original project by the Texas Advanced Computing Center, Austin:

https://github.com/TACC/DisplayCluster
