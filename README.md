

       /////// ////// //////  //////   /////     /////    ////  //    //
         //   //     //   // //   // //   //    //  //  //   // // //
        //   ////   //////  //////  ///////    /////   //   //   //
       //   //     //  //  // //   //   //    //   // //   //  // //
      //   ////// //   // //   // //   //    //////    ////  //   //

     
               A R D U I N O   E E P R O M   M A N A G E M E N T


                 (C) 2024, C. Hofman - cor.hofman@terrabox.nl

       <README.md> - Library managing Arduino persistent memory usage.
                               10 Sep 2024
                       Released into the public domain
              as GitHub project: TerraboxNL/TerraBox_Persistence
                   under the GNU General public license V3.0
                          
      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <https://www.gnu.org/licenses/>.

Releases
========
1.0.1-beta.1
The first release with support for name based EEPROM memory allocation for Arduino.
Tested on the Arduino Mega2560.


Abstract
========
Manages persistent Arduino memory allocations by supplying the possibility to provide a name for the allocated areas. This makes it easier to manage your memory allocations and overlook what you are doing. Also names are better remembered than addresses.

Persistence
===========
The Arduino platform offers persistent memory to store longer lasting data. Configuration data is a typical example of the application of this type of memory. Where a collection of setting is specified and which must be available the next time your application starts up.

Memory management
=================
The idea is to have a way to manage persistent memory allocation in such a way that the allocations made are recognisable and can coexist without getting harmed. For that it must be possible to go beyond the use of memory addresses, Instead names are used, which are associated with a specific memory structure. For that it must be possible to have an idea of the overall available memory and some form of bookkeeping for allocated memory, where it placed in memory and provide an overview of the areas allocated.

Functions
=========
The functions available are:
- Defined boundaries of allocatable pool of memory.
- Memory allocation
- Freeing memory
- View an allocation using a predefined data structure.
- Providing access using the data structure
- Modifying the data within the data structure
- Store the modified data structure in a persistent way.

Memory pool size
================
The boundaries of the available memory pool are specified within the Persistence.h header file.
Two specific areas are dedicated to the configuration of a TFT screen.
The rest of the memory is available for allocation.
All of this is platform dependent.

Boundaries and sizes:
- EEPROM_SIZE defines the total amount of persistent memory.
- EPR_START_FREE defines the lower address boundary of allocatable memory.
- EPR_END_FREE defines the upper address boundary of allocatable memory.

Allocation structure
====================
Every allocatable chunk of memory has a bit of administrative overhead enabling tracking.
Any allocated memory area contains a header and a data part. 

The header part size has a size expressed PERSISTENT_AREA_PREFIX_SIZE constant. It is in fact the size of the C struct called persistentAreaHeader and specifies the storage overhead for allocatable persistent memory. The header contains an offset to the next header, the name of the allocated memory area and an offset to the data part relative to the address of the header part. Because the link to the next header is an offset and data areas are consecutive this also resembles the size of the allocated memory area including the overhead of the header. Hence next - PERSISTENT_AREA_PREFIX_SIZE gives us the size of the allocated data part. Although there also is an offset to the data part, in practice the data part starts right after the header part, for now. But having a dedicated data offset provides flexibility towards implementation future header modifications.
The name in the header has a maximum length of PERSISTENT_AREA_NAME_SIZE. In fact it is 1 byte smaller since it is a '\0' terminated string.

The data part has the size specified at allocation time. For an application the address of an allocated memory area is always expressed as the starting address of the data part. But there is also support to find the address of the header part independent from finding the data address. So the header address of a named memory area can always be found.

Next the available functions will be explained.

Handling data parts
===================
Handling data is supported by a read, modify, write cycle. Where a read function is used to copy the data to a C struct in RAM, where is can easily be modified using the C struct as the structure template. After modification the address of the struct can be used to persist the data available in the RAM based sC struct.

Memory allocation
=================
There are two functions managing the memory allocation and freeing of allocated memory. Called newPersistentArea() and freePersistentArea()

Example of memory allocation:

``` C++
  #include <Persistence.h>
  :
  :

  struct myDataAreaStruct {
    char myName[16] = "Jan Jansen";
    int  birthYear  = 1999;
    int  birtMonth  = 4;
    int  birthday   = 1;
  };

 myDataAreaStruct* newArea  = (myDataAreaStruct*)newPersistentArea("A Data Area", sizeof(myDataArea));

```

Example of freeing memory:

``` C++
  #include <Persistence.h>
  :
  :


  freePersistentArea("A Data Area");

```

If memory is freed, then the allocated memory is free for use. However the memory size is fixed.
Except when it is the last chunk in the chain, then that chunk is returned to the memory pool. This can be done because it is the last chunk in the chain and memory chunks after it do not need to be preserved.

It would be nice to be able to join two consecutive chunks if both are freed at some point. But this a wish for future development. For now it is assumed that persistent memory use is not that dynamic.

Read, modify, write cycle
=========================

Below is an example hat shows how data is read, modified and stored from and into persistent memory.

``` C++
  #include <Persistence.h>
  :
  :


  //
  //  The structure of the data to be manipulated
  //
  struct myDataAreaStruct {
    char myName[16];
    int  birthYear;
    int  birtMonth;
    int  birthday;
  } myStruct;


  //
  //  Read the data part from persistent memory
  //
  persistentReadArea("A Data Area", sizeof(myDataAreaStruct), (char*) &myStruct);

  //
  //  Modify the data
  //
  strcpy(myStruct.myName, "Jan Jansen");
  mySruct.birthYear   = 1999;
  myStruct.birtMonth  = 4;
  myStruct.birthday   = 1;

  //
  //  Write the data to persistent memory
  //
  persistentWriteArea("A Data Area", sizeof(myDataAreaStruct), (char*) &myStruct);


```