DisplayCluster is a software environment for interactively driving large-scale tiled displays. The software allows users to interactively view media such as high-resolution imagery and video, as well as stream content from remote sources such as laptops / desktops or high-performance remote visualization machines. Many users can simultaneously interact with DisplayCluster with devices such as joysticks or touch-enabled devices such as the iPhone / iPad / iTouch or Android devices. Additionally, a Python scripting interface is provided to automate interaction with DisplayCluster.


```
#!bash


[msrinivasan@madhu-00 Buildyard]$ module load kvl-remote
[msrinivasan@madhu-00 Buildyard]$ module load boost/1.50.0
[msrinivasan@madhu-00 Buildyard]$ module load ffmpeg/0.10.2
[msrinivasan@madhu-00 Buildyard]$ module load turbojpeg/1.2.1
[msrinivasan@madhu-00 Buildyard]$ module load openmpi-x86_64
[msrinivasan@madhu-00 Buildyard]$ module load PythonQt/2.1-qt-4.6
[msrinivasan@madhu-00 Buildyard]$ module load python/2.7.3
=======
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
