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
#include <Persistence.h>

/**-----------------------------------------------------------------------------
 *
 *  Compare two strings, addr is the EEPROM address of a a string,
 *  str is a string in normal memory.
 *  It takes into consideration that the name is limited to a length of 15.
 *
 *  @param addr      address of string in EEPROM memory
 *  @param str       address of string in data memory.
 *
 *  @return   0 -> strings are equal
 *           -1 -> s1 < s2
 *           +1 -> s1 > s2
 *
 *---------------------------------------------------------------------------*/
int16_t persistentStrCmp(uint32_t s1, char *s2) {

	int16_t diff;

	int i = 0;
	char eprC = (char)EEPROM_RD_BYTE(s1);
	char c    = *s2;
	while ((diff = (eprC - c)) == 0 && (c + eprC)) {

		if (i++ == 15) {
			return diff;
		}

		//
		//  Read next characters from both strings
		//
		eprC = (char)EEPROM_RD_BYTE(++s1);
		c    = *++s2;
	}

	return diff;
}

/*--------------------------------------------------------------------------------------------------
 *
 *  Returns true if the persistence store is virgin, i.e. not written in before.
 *
 *------------------------------------------------------------------------------------------------*/
bool isPersistentStorageVirgin() {

   for (int i = 0; i < EEPROM_SIZE; i++) {
      if (EEPROM.read(i) != 0xff)
        return false;
   }

   return true;
}

/**-------------------------------------------------------------------------------------------------
 *
 *  Store EEPROM data persistently
 *
 *  @param addr      Address of the persistent memory where the data is to be stored
 *  @param data      data to be stored
 *  @param size      The size of the data in bytes
 *  @return  > 0 store succeeded. The positive number represents the bytes written
 *           < 0 Error code
 *           -1 - Write error. Read back was not equal to byte value written.
 *
 *------------------------------------------------------------------------------------------------*/
int32_t persistentStore(uint32_t addr, char* data, uint16_t size) {

   for (unsigned long i = 0; i < size; i++) {
     //
     // Only write if data differs from what has already bee stored
     //
     if (EEPROM.read(addr) != data) {
       EEPROM.write((int)addr, *data);

       //
       //  Check if the the byte value written is equal to the value read back
       //

       if ((unsigned char)EEPROM.read((int)addr) != (unsigned char)*data)
    	   return -1;

       //
       //  Successfully written, succeed to next byte
       //
       addr++;
       data++;
     }
   }

   //
   //  Returns the size of the stored data,
   //  which should be equal to size specified
   //
   return size;
}

/*--------------------------------------------------------------------------------------------------
 *
 *  Read persisted data
 *
 *  addr      Address of the persistent memory where the data is stored
 *  data      memory area the persisted data must be copied into
 *  size      The size of the data in bytes
 *
 *------------------------------------------------------------------------------------------------*/

void persistentRead(uint32_t addr, char* data, uint16_t size) {

   for (unsigned long i = 0; i < size; i++) {
     *data = EEPROM.read((int)addr++);
     data++;
   }

}

/**----------------------------------------------------------------------------
 *
 *  Reads an EEPROM memory chunk into the provided data area.
 *
 * @param addr      The address of the header to read.
 * @param dataSize  The size of the EEPROM memory to read.
 * @param data      The buffer to read the EEPROM memory contents into
 *
 * @return          The address of the data buffer passed
 *
 ----------------------------------------------------------------------------*/

char* persistentRead(uint32_t addr, uint16_t dataSize, char* data) {

	uint16_t  i = 0;
	while (i++ < dataSize) {
		*data++ = EEPROM[addr++];
	}

	return data;
}

/**----------------------------------------------------------------------------
 *
 *  Clears a persistent memory area with a specific value.
 *
 * @param addr       The address of the area to be cleared
 * @param clearWith  The char data value with which is needs to be cleared
 * @param size       The size of the memory area to be cleared.
 *
 * @return      >= 0 The size of the memory that has been cleared.
 *               < 0 An error code
 *                -1 Write error
 *
 *---------------------------------------------------------------------------*/
int32_t persistentClear(uint32_t addr, unsigned char clearWith, uint16_t size) {

   uint32_t addrWr = addr;
   for (unsigned long i = 0; i < size; i++) {
     //
     // Only write if data differs from what has already bee stored
     //
     if ((unsigned char)EEPROM.read(addrWr) != clearWith) {
       EEPROM.write((int)addrWr, (unsigned char)clearWith);

       //
       //  Check if the the byte value written is equal to the value read back
       //
       if ((unsigned char)EEPROM.read((int)addrWr) != clearWith)
    	   return -(addrWr - addr);

       //
       //  Successfully written, succeed to next byte
       //
       addrWr++;
     }
   }

   //
   //  Returns the size of the stored data,
   //  which should be equal to size specified
   //
   return size;
}


/**----------------------------------------------------------------------------
 *
 * Returns the EEPROM address of a persistent area.
 *
 * @param name     The name of memory area
 *
 * @return   >0 The memory area EEPROM address
 *            0 Area was not found.
 *
 *---------------------------------------------------------------------------*/
uint32_t getPersistentAreaAddress(char* name) {

  //
  //  For as long as there is initialized EEPROM memory,
  //  search for the area name specified.
  //
  for (uint32_t addr = EPR_START_FREE; addr < EPR_END_FREE; addr += EEPROM_RD_INT(addr)) {
    if (addr == 0xffff) // If uninitialized EEPROM, then end of used EEPROM
	  return 0;

    if (! persistentStrCmp(addr+2*sizeof(uint16_t), name)) {
	  return (uint32_t)addr + PERSISTENT_AREA_PREFIX_SIZE;
	}
  }

  return 0;
}

/**----------------------------------------------------------------------------
 *
 *  Returns the EEPROM address of the area header.
 *
 *  NOTE: This DIFFERS from the address returned by getPersistentAreaAddress(),
 *        which is the address of the data part of an area!!!!
 *
 * @param name  The name of the memory area
 *
 * @return  > 0  The header address of the memory area
 *            0  Error, area with that name was unknown.
 *
 *---------------------------------------------------------------------------*/
uint32_t getPersistentHeaderAddress(char* name) {

  //
  //  For as long as there is initialized EEPROM memory,
  //  search for the area name specified.
  //
  for (uint32_t addr = EPR_START_FREE; addr < EPR_END_FREE; addr += EEPROM_RD_INT(addr)) {
    if (addr == 0xffff) { // If uninitialized EEPROM, then end of used EEPROM
	  return 0;
    }

    if (! persistentStrCmp(addr+2*sizeof(uint16_t), name)) {
	  return (uint32_t)addr;
	}
  }

  return 0;
}

/**----------------------------------------------------------------------------
 *
 *  Returns true if the persistent data area exists, otherwise false.
 *
 * @param name  The name of the desired persistent data area.
 * @return      True if data area exists, otherwise false.
 *
 *---------------------------------------------------------------------------*/
bool hasPersistentArea(char* name) {
	//
	//  If the area exists, then return true.
	//
	if (getPersistentHeaderAddress(name))
		return true;

	//
	// Otherwise return false.
	//
	return false;
}

/**----------------------------------------------------------------------------
 *
 *  Allocates a free header by writing the persistent Area Header:
 *  *next 16 bit offset to the next header area
 *  *data 16 bit offset to the data
 *  *name max 15 bytes + 1 '\0' for the area name
 *
 * @param name  The name of the header, which must be unique
 * @param addr  The address from getNewPersistentHeader(uint16_t size);
 * @param size  The size of the application data to be stored
 * @return      > 0 if allocation succeeded, i.e. the size of the area
 *              < 0 if allocation failed, i.e. the error code
 *              -1 -> Area name already in use (error code is not used here)
 *              -2 -> Passed area address is already in use
 *              -3 -> write error
 *              -4 -> Passed area for reuse, but too small
 *
 *---------------------------------------------------------------------------*/
int16_t newPersistentHeader(char *name, uint32_t addr, uint16_t size) {

  //
  //  Correct the data area address to point at the start of the header
  //
  addr = addr - PERSISTENT_AREA_PREFIX_SIZE;

  //
  // Check that the area is not in use.
  // For that the *data pointer contains 0xffff if it is not in use.
  // If not, then this data area is in use, so return -2
  //
  if (((uint16_t)EEPROM_RD_INT( (uint16_t)(addr + sizeof(uint16_t)) ) & 0xffff) != 0xffff)
	  return -2;

  //
  //  If the area found is not virgin, then check if it is big enough
  uint16_t next = (uint16_t)EEPROM_RD_INT( (uint16_t)(addr + sizeof(uint16_t)) );
  if (next != 0xffff || next < size + PERSISTENT_AREA_PREFIX_SIZE) {
	  return -4;
  }

  //
  //  If next does not contain 0xffff the area is reused
  //
  bool reuse = (next != 0xffff);


  //
  //  Initialize the persistent area prefix header
  //
  struct persistentAreaHeader header;
  //
  // Only touch the next field if this area is not reused.
  // If reused, then leave it alone, as it it part of a linked list
  // with allocated/allocatable memory cells.
  //
  if (!reuse) {
    header.next = PERSISTENT_AREA_PREFIX_SIZE + size;
  }

  //
  //  Get rid of the 0xffff in the data field by assigning it
  //  an offset pointing to the data area.
  //
  header.data = PERSISTENT_AREA_PREFIX_SIZE;

  //
  //  Copy the area name into the header.
  //
  strncpy(header.name, name, PERSISTENT_AREA_NAME_SIZE);

  //
  //  Persist the header, if return value is negative then there was a write error.
  //
  uint32_t rv = persistentStore(addr, (char *)&header, sizeof(header));
  if (rv < 0) {
    return -3;
  }

  //
  //  Return the allocated size
  //
  return size;
}

/**----------------------------------------------------------------------------
 *
 *  Reads a header into a persistentAreaHeader using the data area address.
 *
 * @param addr   The data address (Not the actual header address!!!!)
 * @param data   The header data.
 * @return
 *
 *---------------------------------------------------------------------------*/
uint32_t persistentReadHeader(
		          uint32_t addr,
				  struct persistentAreaHeader* header) {

	//
	//  Check that the address range is valid
	//
	if (addr < EPR_START_FREE || addr >= EPR_END_FREE)
		return 0;

	//
	//  Read the header into the designated header data buffer
	//
	persistentRead(addr - PERSISTENT_AREA_PREFIX_SIZE,
			      (uint16_t) PERSISTENT_AREA_PREFIX_SIZE,
				  (char*) header);

	//
	//  Return the address of the header read
	//
	return addr;
}

/**----------------------------------------------------------------------------
 *
 *  Reads a header into the provided data area.
 *
 * @param addr      The address of the header to read.
 * @param header    The struct to read the header data into
 *
 * @return      > 0 The EEPROM address of the header read
 *                0 Invalid address.
 *
 ----------------------------------------------------------------------------*/
uint32_t persistentReadHeader(
		       char *name,
		       struct persistentAreaHeader* header) {

	//
	//  Get the header address
	//
	uint32_t addr = getPersistentHeaderAddress(name);

	//
	//  Read the data into the header struct
	//  and return the EEPROM address.
	//
	return persistentReadHeader(addr + PERSISTENT_AREA_PREFIX_SIZE, header);

}


//=============================================================================
//
//  E X T E R N A L   P E R S I S T E N C E   F U N C T I O N S
//
//=============================================================================

/**----------------------------------------------------------------------------
 *
 *  Returns the size in bytes of available persistent memory if available.
 *
 *  @return  the persistent memory size in bytes
 *
 *---------------------------------------------------------------------------*/
uint16_t hasPersistentStorage() {
   return EEPROM_SIZE;
}

/**----------------------------------------------------------------------------
 *
 *  Returns the persistent memory address of the first byte that is allocatable.
 *  Note that with every allocation some extra bytes are consumed due to
 *  bookkeeping of allocated memory size and memory area name.
 *
 *  @return  byte address where allocatable memory starts
 *
 *---------------------------------------------------------------------------*/
uint32_t getFreeStorageAreaStart() {
  return EPR_START_FREE;
}

/**----------------------------------------------------------------------------
 *
 *
 *   Returns the first byte address of not allocatable persistent memory.
 *
 *   @return  first byte address outside the persistent memory.
 *
 *---------------------------------------------------------------------------*/
uint32_t getFreeStorageAreaEnd() {
  return EPR_END_FREE;
}

/**----------------------------------------------------------------------------
 *
 *  Finds the next free persistent area.
 *  It merely find it and returns the data address for it.
 *  It does not write anything in the area found.
 *  So it is ready for use but its header needs to be initialized.
 *  You can use the newPersistentHeader() function to do that.
 *  Or use the function newPersistenceArea() which does it all in one go.
 *
 * @return  > 0 The persistent area data address
 *            0 No allocatable persistent memory left
 *
 *---------------------------------------------------------------------------*/
uint32_t findNewPersistentArea(uint16_t dataSize) {

	//
	//  Calculate the size of the entire memory cell going to be allocated,
	//  which includes a header for bookkeeping purposes
	//
	uint16_t size = PERSISTENT_AREA_PREFIX_SIZE + dataSize;

	//
	//  For as long as there is allocatable EEPROM memory,
	//
	struct persistentAreaHeader header;
	for (uint32_t addr = EPR_START_FREE; addr < EPR_END_FREE; addr += header.next) {

		//
		//  Read in the header
		//
		persistentReadHeader(addr + PERSISTENT_AREA_PREFIX_SIZE, &header);


		//
		//  Check that the data field contains 0xffff,
		//  If not then the cell is occupied.
		//  So skip to the next cell.
		//

		if ( header.data != 0xffff) {
			continue;
		}

		//
		//  We have a potential candidate...
		//  So read the next field of the header.
		//  which contains the total size of the cell
		//  which includes the data + the header (i.e. prefix) size.
		//

		//
		//  If the next contains 0xffff, then this is the end of the linked list.
		//  So that memory can be allocated, provided the requested size does
		//  not get past the END of the allocatable persistent memory space.
		//
		if (header.next == 0xffff) {
			if ( (header.next + PERSISTENT_AREA_PREFIX_SIZE + dataSize) < EPR_END_FREE )  {
		      return addr + PERSISTENT_AREA_PREFIX_SIZE; // Uasable, return it.
			}
		}
		else {
		  //
		  //  If it contains something else as 0xffff, then this is a freed,
		  //  hence allocatable, memory cell.
		  //  For that the total size of the cell needs to be big enough.
		  //  That is it needs to be greater or equal the requested dataSize
		  //  plus the size of the header (i.e. prefix).
		  //
		  if (header.next >= (PERSISTENT_AREA_PREFIX_SIZE + dataSize) ) {

		    return addr + PERSISTENT_AREA_PREFIX_SIZE;  // Useable, so return it.

		  }
		}

		//
		//  If we are here, then we need to go to the next cell.
		//

	}

	//
	//  Otherwise return -, which indicates no free EEPROM mmory was found.
	//
	return 0;
}

/**----------------------------------------------------------------------------
 *
 *  Searches for the next free persistent memory address.
 *
 * @param size  Size in bytes of the area to allocate
 *
 * @return      >  0 The data address of the free memory.
 *              <= 0 Error getting the new memory
 *                 0 No free memory available.
 *                -1 -> Area name is already in use.
 *                -2 -> Passed area address is already in use
 *                -3 -> write error
 *                -4 -> Passed area for reuse, but too small
 *
 ----------------------------------------------------------------------------*/
uint32_t newPersistentArea(char* name, uint16_t dataSize) {

	//
	// Check if the area name does not already exist.
	// If so return the proper error code.
	//
	uint32_t existAddr = getPersistentHeaderAddress(name);

	if (existAddr) {
	  return -1;
	}

	//
	//  Find a new area which fits the requested dataSize
	//
	uint32_t addr = findNewPersistentArea(dataSize);

	//
	//  If nothing found, then return
	//
	if (addr <= 0) {
	  return 0;
	}

	//
	//  For as long as there is initialized EEPROM memory,
	//
	int16_t size = newPersistentHeader(name, addr, dataSize);
	if (size == dataSize) {
	  return addr;
	}

	//
	//  Otherwise return 0, which indicates no free EEPROM memory was found.
	//
	return 0;
}

/**---------------------------------------------------------------------------
 *
 * Read the data for the named area from the corresponding EEPROM area.
 *
 * @param name      Name of the area
 * @param dataSize  The size of the data area in bytes
 * @param data      The persisted area data
 *
 * @return          A return code.
 *                   1  Success
 *                   0  Not used
 *                  -1  The area with the specified name was not found
 *                  -2  The requested data size differs from the area
 *                      its actual data size.
 *
 *---------------------------------------------------------------------------*/
int16_t persistentReadArea(char* name, uint16_t dataSize, char* data) {

  //
  //  Read in the area header & return the EEPROM address of the data area.
  //
  struct persistentAreaHeader header;
  uint32_t addr = persistentReadHeader(name, &header);

  //
  // If addres 0 is returned, then the area doesn't exist.
  //
  if (addr == 0) {
	  return -1;
  }

  //
  // Check if the requested data size corresponds to the stored data size
  //
  uint16_t areaDataSize = header.next - header.data;
  if (areaDataSize != dataSize) {
	  return -2;
  }

  //
  // For an existing area, read its contents and return it
  //
  persistentRead(addr, dataSize, data);
  return 1;
}

/**----------------------------------------------------------------------------
 *
 *  Writes data from a data buffer to persistent memory.
 *  If succesfully written the size returned is equal to the dataSize.
 *  If tha is not the case, then an error occurred while writing and
 *  the persistent memory is then corrupted.
 *
 * @param addr      The EEPROM address of the persistent memory
 * @param dataSize  The size of the data to be written
 * @param data      The address of the data buffer
 * @return          > 0 The amount of bytes that were successfully written.
 *                    0 Nothing is written, because it was the wrong data area.
 *                  < 0 The number of bytes not written after a write error.
 *
 *---------------------------------------------------------------------------*/
int16_t persistentWriteArea(char *name, uint16_t dataSize, char* data) {

	//
	//  Read in the area header.
	//
	struct persistentAreaHeader header;
	uint32_t addr = persistentReadHeader(name, &header);

	if (!addr)
		return 0;
	//
	//  calculate data addresses
	//
	uint32_t start  = addr;
	uint32_t end    = start + dataSize;

	//
	//  If the dataSize is unequal to the available memory,
	//  then return the -1 error code
	//
	if (dataSize != (end - start)) {
        return 0;
	}

	//
	//  Write the data buffer too EEPROM
	//
	uint32_t addrWr = start;
	while (addrWr < end) {

		//
		//  If data is unequal to value to be written,
		//  then we have to modify the EEPROM
		//
	    if (EEPROM.read(addrWr) != *data) {

	      //
	      //  Write the byte to EEPROM
	      //
	      EEPROM.write((int)addrWr, *data);

		  //
		  //  Read the value in the byte just written back
		  //  It should be equal to the data buffer byte
		  //  If not we have a write error.
		  //
		  char c = EEPROM.read(addrWr);
		  if (c != *data)
			return (addrWr - start) - dataSize;  // bytes not written
	    }

		data++;    // Next data buffer byte.
		addrWr++;  // Next EEPROM address to write to.
	}

	return addrWr - start; // Return the number of bytes successfully written

}

/**----------------------------------------------------------------------------
 *
 *  Frees the allocated persistent memory area
 *
 * @param name  Name of the memory area to free
 *
 * @return      >0     if the memory was freed successfully
 *               1     memory is freed
 *               2     Is already freed
 *               0     Unused
 *              -1     Area name was not found
 *              <0     if the memory was not freed (completely)
 *                     rc & 0xC000 -> 0x8001 - 0xBFFF is write error in header
 *                     rc & 0xC000 -> 0xC000 - 0xFFFE is write error in data
 *
 *---------------------------------------------------------------------------*/
int16_t freePersistentArea(char* name) {

	uint32_t addr = getPersistentHeaderAddress(name);
	uint32_t addrNext = 0;

	if (addr == 0) {
		return -1;
	}

	//
	// Read in the persistentAreaHeader
	//
	struct persistentAreaHeader header;
	persistentReadHeader(name, &header);

	//
	// Check if the area has already been freed.
	//
	if ((uint16_t)header.data == 0xffff) {
		return 2;
	}

	//
	// Clear the data and the name part of the header.
	// Optionally clear the next if the data field next of the next
	// block contains 0xffff. This means we free the last area in the chain.
	// The last block can be completely erased without consequences.
	// With a next alloc it can be allocated with a different size without
	// any side effects.
	// Otherwise we leave the next for what it is, since it is part of
	// the chain and has a next block.
	//
	struct persistentAreaHeader nextHeader;

	//
	// Read in the next header
	//
	addrNext = addr + header.next;
	persistentRead((uint32_t)(addrNext),
			        PERSISTENT_AREA_PREFIX_SIZE,
					(char*)&nextHeader);

	//
	//  Clear the data field, indicating that there is no data in use.
	//
	uint32_t addrData = addr + header.data;
	header.data = 0xffff;           // Always clear the data field.

	//
	//  And clear the area name with '\0'. This prevents a false match
	//  if we are searching for a header with a specific name.
	//
	for (int i = 0; i < PERSISTENT_AREA_NAME_SIZE; i++)  // Clear the entire name
	  header.name[i] = (char)0xFF;  // Clear each byte of the name[] array

	//
	// If the next header contains 0xffff in the next field,
	// then this is untouched persistent memory.
	// The implication of that is that "header" is the last
	// allocated area of persistent memory.
	// Therefore we can clear the header.next of the area to be freed
	// indicating that this is now virgin memory.
	//
	if (nextHeader.next == 0xffff) // Is header the last header in the chain?
		header.next = 0xffff;      // Yes, then we can clear the next field.

	//
	//  Store the modified header, thereby persisting it in memory.
	//
	int32_t clearedBytes = persistentStore(addr, (char*)&header, (uint16_t)sizeof(header));
	if (clearedBytes != sizeof(header)) {
    	return -clearedBytes;
	}

	//
	//  Optionally clear the data area
	//  Modify this to "#if 1" -> clears the entire data area
	//
#if 1
	uint16_t dataSize = addrNext - addrData;
	clearedBytes = persistentClear(addrData, 0xff, dataSize);
    if (clearedBytes != dataSize)
    	return (clearedBytes | 0xC000); // highest two bits set.
#endif

	return 1;
}



