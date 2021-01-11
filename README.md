# meta-oses

Yocto custom layer for a Virtual PPG Sensor.

## Quick links

* Git repository web frontend: <https://github.com/giovannipaganini95/meta-oses>
* Issues management (Github Issues): <https://github.com/giovannipaganini95/meta-oses/issues>
* Documentation: <https://github.com/giovannipaganini95/meta-oses/blob/main/README.md>
* Quick Start: <https://github.com/giovannipaganini95/meta-oses/blob/main/README.md#QuickStart>

## Description

This overlay is inteded to add a virtual PPG Sensor, implemented directly as a Linux Kernel Module, and a simple HeartBeat Monitor app, that relies on the first one. It should works on OpenEmbedded/Yocto distributions and layer stacks. In particular, it was entirely devoloped and tested on Yocto, with raspberrypi3 and quemuarm as targets.

The **PPG Kernel Module** provides the standard Linux open, close and read Kernel Module functionalities, while the write is disabled. More information and details about it can be found [here](https://github.com/giovannipaganini95/meta-oses).

The **Heartbeat Monitor Application**, instead, is implemented in C++ and prints to the console the mean bbp rate, with a timing of about 41s. It based on the producer/consumer pattern and it is designed to be modular with respect to the time intervals and buffer dimensions, in order to be easily extended or ported to other devices. More detailed information about the program structure can be found [here](https://github.com/giovannipaganini95/meta-oses).

## Dependencies

This layer depends on:

* URI: git://git.yoctoproject.org/poky
  * branch: dunfell
  * revision: HEAD

## <a name="QuickStart"></a> Quick Start

The following instructions are intended to add this layer to the Raspberry Pi3 Yocto distribution. But with minimum adjustments it can be also used with other distributions.

1. We have first to move to the environment folder and download this repository with the following command:

```
git clone https://github.com/giovannipaganini95/meta-oses
```
    
2. Once the download has been downloaded, we need to set-up console windows and container for building  the distribution:
    
```
source oe-init-build-env build_rpi3
```

3. At this point we need to add the layer to the image configuration:

```
bitbake-layers add-layer ../meta-oses/
```

3. From the "build_rpi3" directory, we need to edit the "conf/local.conf", appending the following lines:

```
IMAGE_INSTALL_append += " myppg myheartbeat"
KERNEL_MODULE_AUTOLOAD += " myppg"
```
    
4. We can now build build the image with the following command: 

```
bitbake core-image-full-cmdline
```

5. Once the build process is completed, we need to copy the fresh new Linux image to a MicroSD in order to boot up our device. It can be done with the following command, taking care of selecting the correct output device (on a native Linux machine it would likely to be something such as "dev/mmcblk0"):

```
sudo dd if=tmp/deploy/images/raspberrypi3/core-image-full-cmdline-raspberrypi3.rpi-sdimg of=<output device> bs=1M
```

6. Now we can boot up our RPI. We can check if the kernel module (called "mypgg") has been loaded correctly: 
```
dmesg | grep 'myppg'
```

7. The hearbeat rate app can be run with the following command: 
```
myheartbeat
```


## Maintainers

* Giovanni Paganini
