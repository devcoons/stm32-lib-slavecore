/*!
	@file   lib_slavecore.c
	@brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------

	MIT License

	Copyright (c) 2022 Ioannis Deligiannis | Devcoons Blog

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/******************************************************************************
* Preprocessor Definitions & Macros
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/

#include "lib_slavecore.h"

#ifdef LIB_SLAVECORE_ENABLED
/******************************************************************************
* Enumerations, structures & Variables
******************************************************************************/

/******************************************************************************
* Declaration | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Static Functions
******************************************************************************/

/******************************************************************************
* Definition  | Public Functions
******************************************************************************/

i_status slavecore_initialize()
{
#if defined(SLAVECORE_FIRMWARE_VALIDATION_VALUE) &&  !defined(SLAVECORE_FIRMWARE_CRC)

	if((*(volatile uint32_t*)SLAVECORE_FIRMWARE_S_ADDRESS) == SLAVECORE_FIRMWARE_VALIDATION_VALUE)
		return I_OK;
	return I_ERROR;

#elif defined(SLAVECORE_FIRMWARE_VALIDATION_VALUE) &&  defined(SLAVECORE_FIRMWARE_CRC)

	if((*(volatile uint32_t*)SLAVECORE_FIRMWARE_S_ADDRESS) == SLAVECORE_FIRMWARE_VALIDATION_VALUE)
	{
		uint16_t flash_crc = 0xFFFF;
		flash_crc = crc16_ccitt(flash_crc, (uint8_t*)(SLAVECORE_FIRMWARE_S_ADDRESS), SLAVECORE_FIRMWARE_ACTUAL_SIZE);

		if(flash_crc == SLAVECORE_FIRMWARE_CRC)
			return I_OK;
	}
	return I_ERROR;

#else
	return I_UNKNOWN;
#endif
}

i_status slavecore_erase()
{
	int erase_max_steps =(SLAVECORE_FIRMWARE_E_ADDRESS - SLAVECORE_FIRMWARE_S_ADDRESS) / FLASH_SECTION_SIZE;
	for(int i=0;i<erase_max_steps;i++)
	{
		if(flash_erase_nb(SLAVECORE_FIRMWARE_S_ADDRESS + (i*FLASH_SECTION_SIZE),1) != I_OK)
			return I_ERROR;
	}
	return I_OK;
}

i_status slavecore_flash()
{
#if defined(SLAVECORE_FIRMWARE_ACTUAL_SIZE) &&  defined(SLAVECORE_FIRMWARE_CRC)
	for(uint32_t i = 0;i<SLAVECORE_FIRMWARE_ACTUAL_SIZE;i+=64)
	{
		if(flash_write(SLAVECORE_FIRMWARE_S_ADDRESS+i, (uint8_t*)&slavecore_firmware[i], 64) != I_OK)
		{
			slavecore_erase();
			return I_ERROR;
		}
	}
	return I_OK;
#else
	return I_UNKNOWN;
#endif
}

/******************************************************************************
* EOF - NO CODE AFTER THIS LINE
******************************************************************************/
#endif
