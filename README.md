# Using doclone 0.8.0

## CONTENTS

 1. Introduction
    1. Description
    2. Supported filesystems
    3. Software required
    4. Building
    5. Supported platforms
    6. Terms of distribution
 2. Using doclone
    1. Local use
    2. Network use
        1. Unicast/Multicast
        2. Chain mode
    3. Command line options
 3. After using doclone
    1. The boot sector
    2. The partitions
 4. Author

Copyright (C)  2013, 2015 Joan Lledó.
Permission is granted to copy, distribute and/or modify this text under
the terms of the Creative Commons Attribution-ShareAlike 3.0 Unported License.
You can read the entire license following this link:
[http://creativecommons.org/licenses/by-sa/3.0/](http://creativecommons.org/licenses/by-sa/3.0/)

-------------------------------------------------------------------------------

## 1	INTRODUCTION

### 1.1 Description
Doclone is a free project developed in C++ for creating and restoring backups of
disks and partitions. It is also possible to work on LAN, by transferring images
or device data between nodes.

### 1.2 Supported filesystems
Doclone uses libblkid for detecting the filesystems, and some external tools for
formatting them. FS labels and uuid's are managed by libparted.

Here is a list of supported file systems, and the required tools for formatting
or mounting them.

Filesystem     |Needed software 
---------------|----------------
ext2           |mke2fs
ext3           |mke2fs
ext4           |mke2fs
fat16          |mkdosfs
fat32          |mkdosfs	
hfs            |mkfs.hfs
hfs+           |mkfs.hfsplus
jfs            |jfs_mkfs
linux-swap     |mkswap
ntfs           |mkntfs
reiserfs       |mkreiserfs
xfs            |mkfs.xfs

### 1.4 Required Software
It is necessary to have the library libparted 3.2 or later installed. The
libraries libe2fs, libuuid, libblkid, libarchive, libxerces-c and liblog4cpp
are also required.

### 1.5 Compliling doclone
As usual, to compile doclone you only need to execute the classic commands:

	./configure
	make
   
And if you wish to install it on your system, execute as root:

	make install

Before compiling, you must have the folowwing development libraries:

* libparted-dev
* e2fslibs-dev
* uuid-dev
* libblkid-dev
* libarchive-dev
* libxerces-c-dev
* liblog4cpp-dev

### 1.6 Supported Platforms
Currently, doclone has been compiled only in GNU/Linux

### 1.7 Terms of Distribution
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

-------------------------------------------------------------------------------

## 2	USING DOCLONE

### 2.1 Local Use
Local use of doclone is limited to create images of a disk or partition, or
restore images created previously on a disk or partition.

### 2.2 Network use
Doclone allows you to work on LAN. By this way, you can restore images created
in another computer. For this purpose, it is possible to work in two modes, the
Unicast/Multicast and the Chain Mode.

### 2.2.1 Unicast/Multicast
In this mode, there is one server and one or more clients. The server must run
before and wait for the clients. Then, the clients can connect with the server,
who sends them its data.

### 2.2.2 Chain Mode
This mode creates a chain of computers where every node send the data to the
next one in the chain. In this mode, the clients must run before and the server
sends a broadcast package to know who is listening. Then, it sorts the chain and sends to each client the IP address of the next one.

### 2.3 Command line options.
Here is more thoroughly detailed how to use doclone.

Synopsis:
```
    doclone FUNCTION [ -d, --device DEVICE ] [ -f, --file FILE ]
          [ -a, --address SERVER-IP-ADDRESS ] [ -n, --nodes NUMBER ]
          [ -i, --interface IP-OF-WORKING-INTERFACE]
          [ -e, --empty ] [ -F, --force]
```

##### ARGUMENTS:
```
-d, --device		The device you are going to use.
						Examples: "/dev/sda", "/dev/sdb2"
                        
-f, --file			The file you are going to create or read.
						Example: "~/backups/hda.doclone"
                        
-a, --address 		IP address you are going to contact to.
						Example: "192.168.1.3"
                        
-n, --nodes			The number of receivers in multicast mode.

-i, --interface		The network interface for network modes.
```


#### OPTIONS:
```
-e, --empty      Saves in the image only the partition table, with no data.
                    It can be used both locally and on LAN.
                    
    				Example:
    				doclone -cd /dev/sda -f ~/sda-empty.doclone -e
  
-F, --force		Restores the image even if the datat doesn't fit in the target
				device.
                    It can be used both locally and on LAN.
                    It only has effect if you are restoring an image.
                    
    				Example:
    				doclone -rd /dev/sda -f ~/sda.doclone -F
```
#### FUNCTIONS:

For local usage:
All of these options must be used with -d and -f. In all cases the order of the
parameters is irrelevant.
```
-c, --create        Create an image of a disk or partition.
					Examples:
					doclone -cd /dev/sda -f ~/backups/sda.doclone
					doclone -cd /dev/sda2 -f ~/backups/sda2.doclone
   
-r, --restore		Restore an image to a disk or partition.
					Examples:
					doclone -rf ~/backups/sda.doclone -d /dev/sda
					doclone -rf ~/backups/sda2.doclone -d /dev/sda2
```

##### Network Usage:

All the network options need the use of -f or -d. Depending on whether what you 
want to send or receive is an image or a device.

Unicast/Multicast:
```
-S, --send			Send the data (the option involves -n)
					Example:
					doclone -Sd /dev/sda -n 1
    				Send the contents of /dev/sda to one receiver.
    				
    				If the -n option is not specified, the number of receivers
    				is set to 1 by default
   
-C, --receive		Receive the data of the server
					Example:
					doclone -Rd /dev/sda -a 192.168.1.150
					Receive information from 192.168.1.150 and save it in
					/dev/sda.

					Example:
					doclone -Rf ~/backups/sda.doclone
					Receive information from 192.168.1.150 and save it in
					~/backups/sda.doclone
					(By this way you can create remote images).
```
   
##### Chain Mode connection:
```
-s, --link-send		Send the data to the network.
    				Example:
    				doclone -sd /dev/sda
    				Send the data of /dev/sda to the chain.
   
-l, --link-receive	Receive the data from the network
					Example:
					doclone -ld /dev/sda
					Receive the data from the chain and restores it in /dev/sda

					Example:
					doclone -lf ~/backups/sda.doclone
					Receive the data from the chain and save it in in an image
					file.
```

-------------------------------------------------------------------------------

## 3	AFTER USING DOCLONE

### 3.1 The boot sector
When you decide to create an image of an entire hard disk, the boot sector is
also saved. This means if you have the GRUB bootloader installed, it will be
cloned as well as all of its options. If the origin machine has no GRUB, the
first 446 bytes of the disk will be copied.

### 3.2 Partitions
It is possible to create an image of a 40GB hard disk in the server, to restore
it in a 20GB hard disk (for example). Since the program only saves your files in 
the image, if your files only take 10GB, the image will be restored without
problems.

Additionally, the partitions size is resized to be proportional to the size they
occupied in the original drive, so if you have a partition that takes the 70% of
a 40GB disk, on the destination the partition will take the 70% of the 20GB
disk, provided your files fit in that space. Otherwise, doclone will return an
error. If the original disk is smaller than the destination, the partition size
is increased proportionally in the destination.

-------------------------------------------------------------------------------

### 4	AUTHOR
doclone has been written by Joan Lledó <jlledom@member.fsf.org>

