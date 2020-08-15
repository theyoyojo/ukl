
## UKL C++ And Other Demo Code

### Requirements
1. Assumes you have built local **gcc/c++** libraries in folder **unikerelLinux/gcc-install**
2. Assumes you have built local **glibc** libraries and files in folder **unikerelLinux/build-glibc/glibc-build**
3. Assumes you have build **Linux** from **ukl-main branch**

### C++, Constructor, Filesystem, and Argc/Environment Variable Apps

The Makefile contains four apps to demonstrate the use of constructors (in both C and C++ code) and the C++ libraries.  

| Makefile target | Description |
| --- | --- |
| c_constructors | Demonstrates using decorated constructor functions in C code |
| c_fs_startup | Shows UKL filesystem access and reading program arguments (argc) and environment variables from C code; includes saving and reading a file and reading the files in the /root directory. |
| hellocpp | Basic C++ application |
| cpp_filesystem | Shows UKL filesystem access from C++ code |



### Usage
Build desired target, then "make run" to see it execute in UKL.  

#### For c_fs_startup:
To pass argc arguments into app, on QEMU commandline add to end of line "-- arg1" (where arg1 represents an argument).  
*E.g.:  COMMANDLINE = -append "console=ttyS0 root=/dev/sda nokaslr net.ifnames=0 biosdevname=0 nopti nosmap mds=off ip=192.168.19.136:::255.255.255.0::eth0:none **-- arg1**"*  

To pass environment variables into app, on QEMU commandline add "key=value".  
*E.g.:  COMMANDLINE = -append "console=ttyS0 root=/dev/sda nokaslr net.ifnames=0 biosdevname=0 nopti nosmap mds=off ip=192.168.19.136:::255.255.255.0::eth0:none **key1=test key2=value** -- arg1"*  

***THIS IS A WORK IN PROGRESS***

### Missing or Incomplete as of August 15, 2020
 - n/a -  
