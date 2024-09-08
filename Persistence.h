/*-------------------------------------------------------------------------------------------------


       /////// ////// //////  //////   /////     /////    ////  //    //
         //   //     //   // //   // //   //    //  //  //   // // //
        //   ////   //////  //////  ///////    /////   //   //   //
       //   //     //  //  // //   //   //    //   // //   //  // //
      //   ////// //   // //   // //   //    //////    ////  //   //

     
               A R D U I N O   E E P R O M   M A N A G E M E N T


                 (C) 2024, C. Hofman - cor.hofman@terrabox.nl

    <Persistence.cpp> - Library managing Arduino persistent memory usage.
                               16 Aug 2024
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

 *---------------------------------------------------------------------------*
 *
 *  C H A N G E  L O G :
 *  ==========================================================================
 *  P0001 - Initial release 
 *  ==========================================================================
 *
 *--------------------------------------------------------------------------*/
#include <Arduino.h>
#include <EEPROM.h>

#ifndef PERSISTENCE_h
#define PERSISTENCE_h

/*================================================================================================*/
#if defined(TEENSYDUINO)

    //  --------------- Teensy -----------------

    #if defined(__AVR_ATmega32U4__)
        #define BOARD "Teensy 2.0"
    #elif defined(__AVR_AT90USB1286__)
        #define BOARD "Teensy++ 2.0"
    #elif defined(__MK20DX128__)
        #define BOARD "Teensy 3.0"
    #elif defined(__MK20DX256__)
        #define BOARD "Teensy 3.2" // and Teensy 3.1 (obsolete)
    #elif defined(__MKL26Z64__)
        #define BOARD "Teensy LC"
    #elif defined(__MK64FX512__)
        #define BOARD "Teensy 3.5"
    #elif defined(__MK66FX1M0__)
        #define BOARD "Teensy 3.6"
    #elif defined(__IMXRT1062__) && defined(ARDUINO_TEENSY40)
        #define BOARD "Teensy 4.0"
    #elif defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41)
        #define BOARD "Teensy 4.1"
    #else
       #error "Unknown board"
    #endif

#else // --------------- Arduino ------------------

    #if   defined(ARDUINO_AVR_ADK)
        #define BOARD "Mega Adk"
    #elif defined(ARDUINO_AVR_BT)    // Bluetooth
        #define BOARD "Bt"
    #elif defined(ARDUINO_AVR_DUEMILANOVE)
        #define BOARD "Duemilanove"
    #elif defined(ARDUINO_AVR_ESPLORA)
        #define BOARD "Esplora"
    #elif defined(ARDUINO_AVR_ETHERNET)
        #define BOARD "Ethernet"
    #elif defined(ARDUINO_AVR_FIO)
        #define BOARD "Fio"
    #elif defined(ARDUINO_AVR_GEMMA)
        #define BOARD "Gemma"
    #elif defined(ARDUINO_AVR_LEONARDO)
        #define BOARD "Leonardo"
    #elif defined(ARDUINO_AVR_LILYPAD)
        #define BOARD "Lilypad"
    #elif defined(ARDUINO_AVR_LILYPAD_USB)
        #define BOARD "Lilypad Usb"
    #elif defined(ARDUINO_AVR_MEGA)
        #define BOARD "Mega"
    #elif defined(ARDUINO_AVR_MEGA2560)
        #define BOARD "Mega 2560"
    #elif defined(ARDUINO_AVR_MICRO)
        #define BOARD "Micro"
    #elif defined(ARDUINO_AVR_MINI)
        #define BOARD "Mini"
    #elif defined(ARDUINO_AVR_NANO)
        #define BOARD "Nano"
    #elif defined(ARDUINO_AVR_NG)
        #define BOARD "NG"
    #elif defined(ARDUINO_AVR_PRO)
        #define BOARD "Pro"
    #elif defined(ARDUINO_AVR_ROBOT_CONTROL)
        #define BOARD "Robot Ctrl"
    #elif defined(ARDUINO_AVR_ROBOT_MOTOR)
        #define BOARD "Robot Motor"
    #elif defined(ARDUINO_AVR_UNO)
        #define BOARD "Uno"
    #elif defined(ARDUINO_AVR_YUN)
        #define BOARD "Yun"

    // These boards must be installed separately:
    #elif defined(ARDUINO_SAM_DUE)
        #define BOARD "Due"
    #elif defined(ARDUINO_SAMD_ZERO)
        #define BOARD "Zero"
    #elif defined(ARDUINO_ARC32_TOOLS)
        #define BOARD "101"
    #else
       #error "Unknown board"
    #endif

#endif

#define EEPROM_SIZE    EEPROM.length()   // The number of bytes available in EEPROM

#ifdef ARDUINO_AVR_MEGA2560

/*================================================================================================*/


  #define EEPROM_RD_BYTE(addr) ((unsigned char)(EEPROM.read(addr)))
  #define EEPROM_RD_INT(addr)  ((unsigned int)((EEPROM.read(addr+1) << 8) + EEPROM.read(addr)))
  #define EEPROM_RD_LONG(addr) ((unsigned long)((EEPROM.RD_INT(addr+2) << 16) + EEPROM.RD_INT(addr)))

  #define EEPROM_WR_BYTE(addr, v)   EEPROM.write(addr, (unsigned char)(v & 0xff))
  #define EEPROM_WR_INT(addr, v)  \
                                    EEPROM_WR_BYTE(addr+1,   (unsigned char) ((v & 0xff00) >> 8) ); \
                                    EEPROM_WR_BYTE(addr,     (unsigned char) ((v & 0x00ff)     ) )
  #define EEPROM_WR_LONG(addr, v) \
                                    EEPROM_WR_INT(addr+2,    (unsigned int) ((v & 0xffff0000) >> 16) ); \
                                    EEPROM_WR_INT(addr,      (unsigned int) ((v & 0x0000ffff)      ) )

/*================================================================================================*/

#else      // Otherwise platform unknown

/*================================================================================================*/
  #define EEPROM_SIZE    0      // For MEGA 2560.

#endif


//
//   EEPROM  m e m o r y  m a p 
//
//          +---------------+- 0
//          |               |
//          |     Fixed     |
//          |      size     |
//          |      data     |
//          |               |
//          +---------------+- EEPROM_START_FREE
//          |       |       |
//          |       V grows |
//          :       :       :
//          :       :       :
//          :       :       :
//          |       A grows |
//          |       |       |
//          +---------------+- EEPROM_END_FREE
//          |               |
//          |    Variable   |
//          :      size     :
//          |      data     |
//          |               |
//          +---------------+- EEPROM_SIZE
//
//
//
//
//

#define EPR16_TFT_X_W          0      // Contains 16 bit screen width
#define EPR16_TFT_Y_H          2      // Contains 16 bit screen height
#define EPR8_TFT_CALIBRATED    4      // Contains 0 if not calibrated yet, non-zero otherwise
#define EPR8_CELL_S            5      // Cell size used to calibrate
#define EPR16_TFT_CALIBR_X_S   6      // Calibration data size in bytes for the X-axis 
#define EPR16_TFT_CALIBR_Y_S   8      // Calibration data size in bytes for the Y axis

#define EPR_START_FREE         10     // Current free EEPROM

//
//  The following are are EEPROM memory areas that depend on the variable length data
//  Each of those memory areas are stored in a fixed order.
//  Since their size is dynamically specified, these macro's only provide sensible addresses
//  if the length of those areas is stored in their assigned EEPROM address.
//
#define ADR_TFT_CALIBR_X          ((uint32_t)(EEPROM_SIZE - \
                                                   (EEPROM_RD_INT(EPR16_TFT_CALIBR_X_S) * sizeof(uint16_t)) ) )
#define ADR_TFT_CALIBR_Y          ((uint32_t)((ADR_TFT_CALIBR_X) - \
                                                   (EEPROM_RD_INT(EPR16_TFT_CALIBR_Y_S) * sizeof(uint16_t)) ) )


#define EPR_END_FREE ADR_TFT_CALIBR_Y  // The width calibration data is the first chuck of variable data

extern uint16_t hasPersistentStorage();
extern uint32_t getFreeStorageAreaStart();
extern uint32_t getFreeStorageAreaEnd();

extern bool     isPersistentStorageVirgin();
extern int32_t  persistentStore(uint32_t addr, char* data, uint16_t size);
extern void     persistentRead( uint32_t addr, char* data, uint16_t size);
extern void     persistentDump( uint32_t addr, uint16_t size);
extern void     persistentDumpRAM(uint32_t addr, uint16_t size);

//
//  External functions for EEPROM memory allocation
//
extern int16_t  dumpHeader           (char* name); // Makes a memory dump of the header
extern int16_t  persistentReadArea   (char* name, uint16_t dataSize, char* data);
extern int16_t  persistentWriteArea  (char* name, uint16_t dataSize, char* data);
extern uint32_t newPersistentArea    (char* name, uint16_t dataSize); // Allocates new header address
extern int16_t  freePersistentArea   (char* name);    // frees an allocated header
extern void     listPersistentAreas  ();              // Lists the data areas
extern bool     hasPersistentArea    (char* name);    // True if data area exists
extern void     dumpDataArea         (uint32_t addr); // Dump the data of a data area


//
// Just for test purposes
//
extern uint32_t getPersistentHeaderAddress(char* name);
extern uint32_t persistentReadHeader(
		          uint32_t addr,
				  struct persistentAreaHeader* header);
//
//  Struct specifying the data area header
//
#define PERSISTENT_AREA_NAME_SIZE    16   // Size of the name in the area header
#define PERSISTENT_AREA_PREFIX_SIZE  (sizeof(persistentAreaHeader))  // Size of area header



struct persistentAreaHeader {
	//
	//  Size of the data area including its header area
	//  and expressed in bytes.
	//  The expression "&areaHeader + areaHeader.next"
	//  points to the next area header.
	//
	//  If *next contains 0xffff or data contains 0xffff
	//  then this indicates free EEPROM memory.
	//
	//  Note that *next only contains 0xffff if it EEPROM
	//  memory that was not written/used before. If the memory
	//  area is freed, it will still contain the size of the
	//  freed area. The pointer to the data of a freed area
	//  is reset to 0xffff however.
	//
	uint16_t next;     // offset to the next area

	//
	//  Offset to the data of the area. This typically points right
	//  after the address containing the terminating '\0' of the name.
	//
	uint16_t data;    // Offset of data calculated from &bext

	//
	//  The first character of the '\0\ terminates area name
	//
	char     name[PERSISTENT_AREA_NAME_SIZE];      // Array with area name
};

#endif
