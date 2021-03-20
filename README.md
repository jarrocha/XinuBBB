## **Hardware Setup**

If you rather work with VMs, check the guide folder. If you get any issues let me know.

### **Setup for BeagleBone Black**
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
