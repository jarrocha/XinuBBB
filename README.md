## **Hardware Setup**

If you rather work with VMs, check the guide folder. If you get any issues let me know.
The first section does a step-by-step setup of UBoot with BBB, the second section automates the development 
environment by setting up a TFTP server to quickly load images through a network link instead of using an SD card.

1. [Setup for BeagleBone Black](#section1)
2. [Setup a TFTP server for faster image loading](#section2)

**Warning:** the source code provided here is untouched from the original
[Xinu website](https://xinu.cs.purdue.edu/). As it is, it crashes at startup on the BBB while trying to perform an
initial setup for the network interface. After performing the steps on this first section you will have the tools
 necessary to mitigate or fix this crash.

### **1. Setup for BeagleBone Black** <div id="section1"/>
You can use the mainline u-boot for beaglebone.

#### **Cross-Compiler/ToolChain**
* **Note:** Full guide [here](https://elinux.org/Building_for_BeagleBone).
* Get the default cross-compiler
``` Bash
sudo apt-get install gcc-arm-linux-gnueabi
```
* Create a terminal command alias
``` Bash
alias armmake='make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- '
```
#### **Prepare SD-Card**
* SD card needs to be partitioned into Ext3/4/Fat32 filesystem. Use the following images as a guide.
  The size can be bigger than the one shown.
  ![Pic1](/assets/pic1.png)
  ![Pic2](/assets/pic2.png)

#### **Bootoader**
* Get u-boot source
``` Bash
git clone git://git.denx.de/u-boot.git
```
* Clean source
``` Bash
armmake distclean
``` 
* Configure for BeagleBone Black
``` Bash
armmake am335x_evm_config
```
* The U-Boot compilation will generate two files, MLO and uboot.img. Copy them to an SD card.
  The SD card should be able to be mounted in your Linux host system.
``` Bash
armmake
```

* **Note:** The version of mkImage is still compabitle with the latest version of u-boot for loading.
* Copy the MLO and u-boot.img files.
* Also create a *uEnv.txt* file and enter the following commands.
``` Bash
bootfile=xinu.boot 
boot_mmc=fatload mmc 0 0x80200000 ${bootfile}; bootm 
uenvcmd=run boot_mmc
```
* The load location is important, it should point to the header.
* This location was obtained with *iminfo*

#### **Kernel**
* Go to the Xinu BB source, then to */compile*
* Run *make*
* At this location, a *xinu* file will be produced, rename it to *xinu.bin* and copy it to the 
  SDcard partion where the u-boot files are.
  The name does not really matter but it should match the name on the *uEnv.txt* file.
![Pic3](/assets/pic3.png)

#### **Connection**
* Plug the SD card on the BeagleBone and use minicom to connect serially.
* To learn how to connect the BBB with a [serial cable](https://www.dummies.com/computers/beaglebone/how-to-connect-the-beaglebone-black-via-serial-over-usb/).
* Need a [serial cable](https://elinux.org/Beagleboard:BeagleBone_Black_Serial)?
    
``` Bash
sudo minicom -D /dev/ttyUSB0 -c on -b 115200
```
* At the end, you should get the loading screen.
![Pic3](/assets/pic4.png)

### **2. Setup a TFTP server for faster image loading** <div id="section2"/>
I recommend doing the previous work just to make sure that your development environment is ready. After that is done
we can start of the next part which will allow for easier image loading without having to swap the SD card to every 
change done to the OS.

You will need:
* A ethernet port
* A cross-over cable

The ethernet port is needed to communicate the BBB to the TFTP server in your development computer. The cross-over 
cable is required to speak directly to the BBB because they are both end devices and they need to transmitt data between each other. The **alternative** is to have a network switch or a router to handle their communication.

#### **Setup TFTP Server**
First install the TFTP server in the development computer. The command below works for both Ubuntu and Debian, it can also work on other derivatives.
``` Bash
sudo apt-get install tftpd-hpa
```

Run the following command to make sure the TFTP server is up. When you run this command, you
will see the default tftp directory: **/srv/tftp**. Make sure to place the Xinu image there. Just copy the xinu.boot file in there or whichever name you are using.

``` Bash
sudo systemctl status tftpd-hpa
```

You could also use netstat and check for a service on port 69.
``` Bash
netstat -vaun

# if you don't have netstat
sudo apt install net-tools
```
Make sure to check */etc/defaults/tftpd-hpa*, this is the configuration file for **tftpfd-hpa**.

#### **Configure the Server IP**
We need to configure an IP address to the interface which is going to communicate with the BBB.
There are several options and will depend on how you are connecting them, but I recommend to put a static IP address
to this interface. There are several methods to do this but on this guid we are just going to assign a temporary
IP to the interface.

First identify the interface name 
``` Bash
ip addr
```

![Pic5](/assets/pic5.png)

In my case, I'll be using **enx000ec66a8383**. This is a USB Ethernet interface so systemd 
assigns this name. You can see that this interface has an IP already assigned. 

For my setup, I'll assign IP **10.10.10.1/24** for the TFTP server and **10.10.10.10/24** for 
UBoot. To assign this temporary addresss run:

``` Bash
# assigns IP address
sudo ip addr add 10.10.10.1/24 brd + dev enx000ec66a8383

# If you make a mistake, you can remove the address with the command below
# just put switch the "add" argument with "del"
sudo ip addr del 10.10.10.1/24 brd + dev enx000ec66a8383
```

#### **Configure UBoot**
My settings for UBoot could be different from yours. But I'll recommend first loading UBoot by 
itself and then try out what settings works best. Below are the sufficient commands to get
UBoot to load an image from a TFTP server.

``` Bash
U-Boot# setenv ipaddr 10.10.10.10
U-Boot# setenv netmask 255.255.255.0
```

Test if you can ping the TFTP server with
``` Bash
U-Boot# ping 10.10.10
```

![Pic6](/assets/pic6.png)

If you get problems at this point just know that this is network related, it should be an easy 
fix.

Before connecting to the TFTP server we need to setup the server ip variable. After that just request the image with the tftp command.
``` Bash
U-Boot# setenv serverip 10.10.10.1
U-Boot# tftp xinu.boot
```

![Pic7](/assets/pic7.png)

After that just boot up from local memory. You should see Xinu booting up.

``` Bash
U-Boot# bootm
```

Here's the final **uEnv.txt**
``` Bash
ipaddr=10.10.10.10
serverip=10.10.10.1
netmask=255.255.255.0
bootfile=xinu.boot
boot_mmc=tftp ${bootfile}; bootm 
uenvcmd=run boot_mmc
```

Create this file and place it in the SD card, as directed on the first section of the guide.
Now, everytime you make a change to the OS, just compile and copy this final image result to the TFTP directory
location and to a reset on the BBB and that's it.
