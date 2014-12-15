# DisplayCluster

DisplayCluster is a software environment for interactively driving large-scale tiled displays. 

## Documentation

The DisplayCluster manual is included in the distribution in the doc/ directory, and covers installation and usage.

## Features

DisplayCluster provides the following functionality:
* Interactively view media such as high-resolution imagery and video
* Stream content from remote sources such as laptops / desktops or high-performance remote visualization machines
* [Documentation](http://bluebrain.github.io/DisplayCluster-0.2/index.html)

## Building from Source

```
  git clone https://github.com/BlueBrain/DisplayCluster.git
  mkdir DisplayCluster/build
  cd DisplayCluster/build
  cmake ..
  make
```

Or using Buildyard:

```
  git clone https://github.com/Eyescale/Buildyard.git
  cd Buildyard
  git clone https://github.com/BlueBrain/config.git config.bluebrain
  make DisplayCluster
```

## Original Project

This version of DisplayCluster is a fork of the original project by the Texas Advanced Computing Center, Austin:

https://github.com/TACC/DisplayCluster




