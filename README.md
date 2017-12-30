# keysight_34470a_demo

Sample c++ program to communicate with the keysight 34470a digital multi-meter relying on USBTMC driver.

Compile and Run the program:
-Check where your device lives, usually 1 on Ubuntu, 0 on arm based SBCs (tinkerboard/rpi)
  1) ls -la /dev/usbtmc*
  2) g++ keysight_34470a_demo.cpp -Wall -o keysight_34470a_demo
  3) ./keysight_34470a_demo * (Where * is the integer found from #1, usually 0 or 1)

USBTMC Driver:
-This program relies on the usbtmc driver being installed on the system
-This driver creates a file at /dev/usbtmc* where * is an integer from 0-16 when the meter is plugged in.
-Ubuntu 16.04 (4.10.0-42-generic) the usbtmc driver was installed by default.
-Raspbian had this installed by default.
-For the tinkerboard running Armbian_5.35_Tinkerboard_Ubuntu_xenial_default_4.4.102_desktop (4.4.102-rockchip) 
  the driver WAS NOT installed.

Installing USBTMC on Tinkerboard Instructions:
-Get the latest linux headers for your image
  1) sudo apt-get update
  2) sudo apt-get install linux-headers-rockchip
-Compile build scripts
  1) cd /usr/src/linux-headers-4.4.102-rockchip/
  2) sudo make scripts ARCH=arm
-Download driver sources from git
  1) cd ~
  2) git clone https://github.com/dpenkler/linux-usbtmc.git linux-usbtmc
-Make and Install Driver
  1) cd linux-usbtmc
  2) sudo make ARCH=arm
  3) sudo make install ARCH=arm
-Reboot
  1) sudo reboot
-Plug In Meter and check for /dev/usbtmc*
  1) ls -la /dev/usbtmc*
  2) For all following commands replace * with integer from ls command above.
-Load Kernel Module (Driver)
  1) sudo insmod /home/<username>/linux-usbtmc/usbtmc.ko;
-Change Permissions on /dev/usbtmc*
  1) sudo chmod a+rw /dev/usbtmc*
-Test Communication
  1) echo "*idn?" > /dev/usbtmc0; cat /dev/usbtmc0;
