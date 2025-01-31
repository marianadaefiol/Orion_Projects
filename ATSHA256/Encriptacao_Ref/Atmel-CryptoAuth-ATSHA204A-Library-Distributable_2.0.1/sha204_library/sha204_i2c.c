/** \file
 *  \brief  Functions for I2C Physical Hardware Independent Layer of ATCA Library
 *  this module implements the API in sha204_protocol_adapter.h
 * 
 * \author Atmel Crypto Products
 * \copyright Copyright (c) 2014 Atmel Corporation. All rights reserved.
 *
 * \atmel_crypto_device_library_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 * 
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel integrated circuit.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \atmel_crypto_device_library_license_stop
 *
 * \todo Develop solution to allow i2c, swi, and atphy to all live together in harmony. 
 *       Right now, you have to choose one
 */

#include <string.h>
#include "sha204_config.h"
#include "sha204_lib_return_codes.h"					// declarations of function return codes
#include "arch/samd21/timer/sha204_timer_utilities.h"	// definitions for delay functions
#include "arch/samd21/i2c/i2c_phys.h"


/** \brief This enumeration lists all packet types sent to a ATCA device.
 *
 * The following byte stream is sent to a ATCA I2C device:
 *    {I2C start} {I2C address} {word address} [{data}] {I2C stop}.
 * Data are only sent after a word address of value #SHA204_I2C_PACKET_FUNCTION_NORMAL.
 */
enum i2c_word_address {
	SHA204_I2C_PACKET_FUNCTION_RESET,  //!< Reset device.
	SHA204_I2C_PACKET_FUNCTION_SLEEP,  //!< Put device into Sleep mode.
	SHA204_I2C_PACKET_FUNCTION_IDLE,   //!< Put device into Idle mode.
	SHA204_I2C_PACKET_FUNCTION_NORMAL  //!< Write / evaluate data that follow this word address byte.
};


/** \brief This enumeration lists flags for I2C read or write addressing. */
enum i2c_read_write_flag {
	I2C_WRITE = (uint8_t) 0x00,  //!< write command flag
	I2C_READ  = (uint8_t) 0x01   //!< read command flag
};


//! I2C address is set when calling #ateccp_init or #ateccp_set_device_id.
static uint8_t device_address;


/** \brief This function initializes the hardware.
 */
void sha204p_init(void)
{
	phy_i2c_master_enable(I2CBUS_0);
	device_address = CRYPTOAUTH_SLAVE_ADDRESS;
}

/** \brief This function disables the hardware.
 */
void sha204p_disable(void)
{
	phy_i2c_master_disable(I2CBUS_0);
}


/** \brief This function sets the I2C address.
 *         Communication functions will use this address.
 *
 *  \param[in] id I2C address
 */
void sha204p_set_device_id(uint8_t i2caddr)
{
	i2c_master_set_slave_address(I2CBUS_0, i2caddr);
}

void sha204p_i2c_set_spd(uint32_t spd_in_khz)
{
	i2c_master_set_i2c_speed(I2CBUS_0, spd_in_khz);
}


/** \brief This I2C function generates a Wake-up pulse and delays.
 * 
 * for some Atmel CA devices, the wakeup pulse is a non-standard I2C pulse width,
 *
 * \return status of the operation
 */
uint8_t sha204p_wakeup(uint8_t *response)
{
	uint8_t i2c_status = I2C_SUCCESS;

	phy_i2c_master_disable(I2CBUS_0);
	i2c_master_set_i2c_speed(I2CBUS_0, 100);
	phy_i2c_master_enable(I2CBUS_0);

	i2c_status = i2c_master_send_wake(CRYPTOAUTH_BUS);

	sha204_delay_ms(3);  // at least 2.5ms

// 	phy_i2c_master_disable(I2CBUS_0);
// 	i2c_master_set_i2c_speed(I2CBUS_0, 400);
// 	phy_i2c_master_enable(I2CBUS_0
	i2c_master_receive_bytes(CRYPTOAUTH_BUS, 4, response);

	return i2c_status;
}

/** \brief This function puts the device into idle state.
 * \return status of the operation
 */
uint8_t sha204p_idle(void)
{
	uint8_t data = SHA204_I2C_PACKET_FUNCTION_IDLE;
	return i2c_master_send_bytes(CRYPTOAUTH_BUS, 1, &data);
}

/** \brief This function puts the device into low-power state.
 *  \return status of the operation
 */
uint8_t sha204p_sleep(void)
{
	uint8_t data = SHA204_I2C_PACKET_FUNCTION_SLEEP;
	return i2c_master_send_bytes(CRYPTOAUTH_BUS, 1, &data);
}


/** \brief This function creates a Start condition and sends the
 * I2C address.
 * \param[in] read #I2C_READ for reading, #I2C_WRITE for writing
 * \return status of the I2C operation
 */
uint8_t sha204p_send_slave_address(i2c_bus_t bus, uint8_t read)
{
	uint8_t sla = device_address | read;
	uint8_t ret_code = phy_i2c_master_send_start(bus);
	if (ret_code != I2C_SUCCESS)
		return ret_code;

	ret_code = i2c_master_send_bytes(bus, 1, &sla);

	if (ret_code != I2C_SUCCESS)
		(void) phy_i2c_master_send_stop(bus);

	return ret_code;
}

/** \brief This function sends a command to the device.
 * \param[in] count number of bytes to send
 * \param[in] command pointer to command buffer
 * \return status of the operation
 */
uint8_t sha204p_send_command(uint8_t count, uint8_t *command)
{
	uint8_t ret_code;	
	uint8_t cmd_byte = 0x03;
	uint8_t cmd[256];
	
	if (count > 0)
	{
		cmd[0] = cmd_byte;
		memcpy(&cmd[1], command, count );
		
		ret_code = i2c_master_send_bytes(CRYPTOAUTH_BUS, count+1, cmd);
	}
	else
	{
		ret_code = i2c_master_send_bytes(CRYPTOAUTH_BUS, count, command);
	}

	return ret_code;
}

/** \brief This function receives a response from the device.
 *
 * \param[in] size size of rx buffer
 * \param[out] response pointer to rx buffer
 * \return status of the operation
 */
uint8_t sha204p_receive_response(uint8_t size, uint8_t *response)
{
	uint8_t i2c_status = I2C_SUCCESS;

	// Receive the bytes.
	i2c_status = i2c_master_receive_bytes(CRYPTOAUTH_BUS, size, response);

	if (i2c_status != I2C_SUCCESS)
		return SHA204_COMM_FAIL;
	else
		return SHA204_SUCCESS;
}


/** \brief This function resets the I/O buffer of the device.
 * \return status of the operation
 */
uint8_t sha204p_reset_io(void)
{
	uint8_t data = SHA204_I2C_PACKET_FUNCTION_RESET;
	return i2c_master_send_bytes(CRYPTOAUTH_BUS, 1, &data);
}

/** \brief This function resynchronizes communication.
 *
 * Parameters are not used for I2C.\n
 * Re-synchronizing communication is done in a maximum of three steps
 * listed below. This function implements the first step. Since
 * steps 2 and 3 (sending a Wake-up token and reading the response)
 * are the same for I2C and SWI, they are
 * implemented in the communication layer (#ateccc_resync).
  <ol>
     <li>
       To ensure an IO channel reset, the system should send
       the standard I2C software reset sequence, as follows:
       <ul>
         <li>a Start condition</li>
         <li>nine cycles of SCL, with SDA held high</li>
         <li>another Start condition</li>
         <li>a Stop condition</li>
       </ul>
       It should then be possible to send a read sequence and
       if synchronization has completed properly the ATATECC will
       acknowledge the device address. The chip may return data or
       may leave the bus floating (which the system will interpret
       as a data value of 0xFF) during the data periods.\n
       If the chip does acknowledge the device address, the system
       should reset the internal address counter to force the
       ATATECC to ignore any partial input command that may have
       been sent. This can be accomplished by sending a write
       sequence to word address 0x00 (Reset), followed by a
       Stop condition.
     </li>
     <li>
       If the chip does NOT respond to the device address with an ACK,
       then it may be asleep. In this case, the system should send a
       complete Wake token and wait t_whi after the rising edge. The
       system may then send another read sequence and if synchronization
       has completed the chip will acknowledge the device address.
     </li>
     <li>
       If the chip still does not respond to the device address with
       an acknowledge, then it may be busy executing a command. The
       system should wait the longest TEXEC and then send the
       read sequence, which will be acknowledged by the chip.
     </li>
  </ol>
 * \param[in] size size of rx buffer
 * \param[out] response pointer to response buffer
 * \return status of the operation
 */
uint8_t sha204p_resync(uint8_t size, uint8_t *response)
{
	uint8_t ret_code = phy_i2c_master_send_start(CRYPTOAUTH_BUS);

	// Do not evaluate the return code that most likely indicates error,
	// since nine_clocks is unlikely to be acknowledged.
//	(void) i2c_master_send_bytes(CRYPTOAUTH_BUS, 1, &nine_clocks);
	(void) i2c_master_send_reset(CRYPTOAUTH_BUS);

	// Send another Start. The function sends also one byte,
	// the I2C address of the device, because I2C specification
	// does not allow sending a Stop right after a Start condition.
	ret_code = sha204p_send_slave_address(CRYPTOAUTH_BUS, I2C_READ);

	// Send only a Stop if the above call succeeded.
	// Otherwise the above function has sent it already.
	if (ret_code == I2C_SUCCESS)
		ret_code = phy_i2c_master_send_stop(CRYPTOAUTH_BUS);

	// Return error status if we failed to re-sync.
	if (ret_code != I2C_SUCCESS)
		return SHA204_COMM_FAIL;

	// Try to send a Reset IO command if re-sync succeeded.
	return sha204p_reset_io();
}

/** @} */
