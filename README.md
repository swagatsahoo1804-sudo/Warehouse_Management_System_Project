1. Project Overview

Features for Inventory Management Inventory codes, product description, units per load,
quantity, and price are all stored in the Warehouse Management System.
The system maintains the warehouse inventory data within the Linux kernel using a custom
character device driver. A user-space C++ program communicates with the driver through the
/dev/warehouse device.
The project demonstrates practical concepts of:
* Linux Kernel Module development
* Character Device Drivers
* System calls
* User-space and kernel-space communication
* C programming
* C++ programming
* File operations
* Mutex-based synchronization
* Makefile and Linux kbuild
* Device file management
* Testing and debugging

2. Problem Statement

Conventional small business inventory management becomes complex and problematic when
relying on manually monitoring stock levels. It can result in inaccurate stock numbers, poor
updating and trouble retrieving the inventory data.
The purpose of the project is to build a basic Linux-based warehouse management system
which enables access to the inventory data via a custom character device driver.
Add, remove, check stock, and see product info. We can use the system to add new stock to the
stockroom and remove stock from the stockroom.

3. Objectives

The main objectives of the project are:
* To create a Linux character device driver.
* To create a /dev/warehouse device interface.
* For the purpose of storing and managing warehouse inventory information.
* To allow a C++ application to communicate with the kernel driver.
* To implement stock-in and stock-out operations.
* For providing inventory info to the user.
* To demonstrate kernel-space and user-space interaction.
* To access inventory safely by employing synchronization mechanisms.
* To drive, driver compilation automation with a Makefile.

4. Features

Inventory Management :
* Add stock to an existing product.
* Remove stock from an existing product.
* Monitor the inventory level.
* View warehouse inventory.
* Search for products using product IDs.
* Display product name, quantity, and price.

Linux Driver Features :
* Character device implementation.
* Dynamic device number allocation.
* Creation of a device class.
* Creation of /dev/warehouse.
* Read and write operations.
* Kernel-space inventory storage.
* Mutex-based synchronization.
* User-space and kernel-space data transfer.

Application Features :
* C++ user-space application.
* Menu-based warehouse interface.
* Communication with /dev/warehouse.
* User-friendly operation selection.
* Display of driver responses.

5. System Architecture

The general information flow of the project is:

    USER 
     |
warehouse_app.cpp
     |
open / read / write
     |
 /dev/warehouse
     |
 warehouse_driver
     |
 Inventory Data
     |
  Response
     |
warehouse_app.cpp
     |
    USER

Communication Flow :
 
    User
     ↓
 C++ Application
     ↓
 /dev/warehouse
     ↓
Kernel Character Driver
     ↓
Inventory Processing
     ↓
Driver Response
     ↓
 C++ Application
     ↓
    User


5. Linux Character Device Driver

The project uses the following Linux character device driver: warehouse
The driver creates the device: /dev/warehouse
This device is the link between the C++ application and the kernel.

Major Driver Components :
The driver contains:
* Module initialization
* Module cleanup
* Character device registration
* Device class creation
* Device creation
* File operations
* Read operation
* Write operation
* Inventory processing
* Mutex synchronization
* User/kernel data transfer
6. Device Operations

The driver supports standard character device operations.

Open :
The application opens the warehouse device

Write :
Notes sent to the driver from user space

Read :
The app is able to read the driver's reply

Close :
At the end of the operation, the application quits your device

7. User-Space Application

The user-space application is implemented in: application/warehouse_app.cpp
It offers an interface for executing warehouse activities.
The Application presents a Menu Driven interface to the WMS.
The app uses the Linux driver not inventory data.

The basic flow is:

    C++ Application
          |
     /dev/warehouse
          |
     Linux Driver
          |
      Inventory
          |
    C++ Application

8. Warehouse Operations

Stock In :
It raises the number of a new product.
Example:
STOCK_IN P101 10
If P101 initially contains 20 units:
20 + 10 = 30
The driver returns:
STOCK_IN SUCCESS: P101 quantity = 30

Stock Out :
The Stock Out operation decreases the quantity of a product.
Example:
STOCK_OUT P101 5
The driver verifies that sufficient stock is available before updating the quantity.

Check Stock :
The system can check the current quantity of a product.
Example:
CHECK P101
The driver returns the current product information.

View Inventory :
The application can display available warehouse products and their current quantities.

Search Product :
Users can search for a product using its product ID.

9. Makefile

The Makefile uses:
obj-m += warehouse_driver.o
KDIR:= /lib/modules/$(shell uname -r)/build
PWD:= $(shell pwd)
all:
make -C (KDIR)M=(PWD) modules
clean:
make -C (KDIR)M=(PWD) clean

Makefile Explanation :
* obj-m specifies the kernel module object.
* KDIR specifies the Linux kernel build directory.
* PWD identifies the current driver directory.
* all builds the kernel module.
* clean removes generated build files.
The resulting module is: warehouse_driver.ko

10. Prerequisites

The project requires:
● Ubuntu Linux
● Linux kernel headers
● GCC
● G++
● Make
● Kernel build tools
● Root/sudo access for driver operations

11. Building the Kernel Driver

Navigate to the driver directory: cd ~/warehouse-management/driver
Run: make
Compilation should provide the following kernel module: warehouse_driver.ko.
Check the generated module using ls

Cleaning the Driver :
To remove generated compilation files: make clean

Loading the Driver :
To load the driver into the Linux kernel use: sudo insmod warehouse_driver.ko
Verify that the module is loaded: lsmod | grep warehouse
Expected output contains: warehouse_driver

Checking the Device :
Once the driver is loaded, see if the character device exists: ls -l /dev/warehouse
The project creates: /dev/warehouse
This is the plugin between the C++ application and kernel driver.

12. Compiling and Running the C++ Application

Navigate to the application directory: cd ~/warehouse-management/application
Compile the application using: g++ warehouseapp.cpp -o warehouseapp
The executable will be: warehouse_app

Running the Application :
Run: sudo./warehouse_app
A successful initial test produced:
Warehouse device opened successfully.
Data from driver: STOCK_IN P101 20
Warehouse device closed.

Testing the Driver :
You can also give the drive a test directly using commands from Linux.
Issue a Stock-IN command: echo "STOCK_IN P101 10" | sudo tee /dev/warehouse
Driver Response: sudo cat /dev/warehouse

This confirms that:
1. The driver is loaded.
2. /dev/warehouse exists.
3. The device accepts input.
4. The driver processes the command.
5. Inventory quantity is updated.
6. The driver returns a response.

13. Synchronization

The driver uses a Linux mutex: DEFINEMUTEX(warehousemutex);
The mutex prevents one inventory task from accessing the inventory while another one is using
it.
This avoids inconsistent data from being recorded when the different operations use the
common inventory.

14. User Space and Kernel Space

This project shows how the space of user and kernel space.

User Space :
It is a C++ user space application: warehouse_app.cpp
It sends requests to the device.

Kernel Space :
This character driver has to be loaded into kernel: warehouse_driver.c
It processes requests and manages inventory.

15. UML and Design Diagrams

Component Diagram :
    User
     |
warehouse_app.cpp
     |
/dev/warehouse
     |
warehouse_driver.c
     |
Inventory Structure/Data

16. Development Environment

The project was developed in:
Operating System : Ubuntu Linux
Environment : Oracle VirtualBox
Language : C / C++
Kernel Module : Linux Character Driver
Compiler : GCC / G++
Build System : Linux kbuild + Makefile
Device : /dev/warehouse

17. Version Control

The project source code and documentation can be kept in the Git.

Repository structure :
warehouse-management/
├── application/
├── data/
├── docs/
├── driver/
└── tests/

18. Achievements

The project successfully demonstrates:
● Development of a Linux character device driver.
● Creation of the /dev/warehouse device.
● Communication between user space and kernel space.
● A menu-driven C++ application.
● Switch-case based operation selection.
● Inventory management through the kernel driver.
● Stock update operations.
● Device read/write operations.
● Mutex-based synchronization.
● Linux kernel module compilation using kbuild.
● Successful loading and testing of the kernel module.
The project successfully performed real communication with the Linux character device and
produced the expected inventory update response.

19. Limitations

The current version has some limitations:
* Inventory data is stored in kernel memory.
* Data may be lost when the driver is unloaded or the system is restarted.
* The number of products is limited.
* The system does not use a persistent database.
* The application is intended primarily for a Linux environment.
* Authentication and user access control are not implemented.
* Advanced warehouse features such as suppliers, orders, and reports are not included.

20. Future Enhancements

The project can be extended with:
* Persistent database storage
* User authentication.
* Admin and employee roles.
* Supplier management.
* Purchase and sales management.
* Low-stock notifications.
* Product categories.
* Barcode/QR code support.
* Warehouse analytics.
* Graphical user interface.
* Web-based dashboard.
* Automatic backup and recovery.
* Multi-warehouse support.

21. Troubleshooting

Driver does not compile :
Check the current kernel version: uname -r
Verify that the kernel build directory exists: ls /lib/modules/$(uname -r)/build
Then run:
make clean
make

`/dev/warehouse` does not exist :
First check whether the module is loaded: lsmod | grep warehouse
If it is not loaded: sudo insmod warehouse_driver.ko
Then check: ls -l /dev/warehouse

Permission denied :
Operations involving the kernel device may require root privileges.
Use: sudo ./warehouse_app or sudo cat /dev/warehouse
