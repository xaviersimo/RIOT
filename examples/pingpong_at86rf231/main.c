/*
 * Copyright (C) 2014 Triagnosys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       pingpong at86rf231
 *
 * @author      Xavier Perez <xaviersimo@triagnosys.com>
 *
 * @}
 */

//#include <stdio.h>
#include "board.h"
#include "vtimer.h"
#include "uart.h"
#include "gpio.h"
#include "thread.h"
#include "transceiver.h"
#include "at86rf231.h"

#define MSEC (1000)
#define SEC (1000 * MSEC)




int main(void)
{


	/*Init program*/
	printf("# ********************************************* \n");
	printf("# ************ pingpong AT86RF231 ************* \n");
	printf("# ********************************************* \n");



	transceiver_init(TRANSCEIVER_AT86RF231);
	transceiver_start();

	//at86rf231_initialize(NULL); //Use this function to config directly the radio modul withou use general transceiver functions


	/*Chanel*/
	//unsigned int chan = 6;
	//at86rf231_set_channel(chan);
	uint16_t channel = at86rf231_get_channel();
	printf("channel is: %d\n", channel);

	/*adress*/
	radio_address_t addr = 0x0001;
	at86rf231_set_address(addr);
	radio_address_t address = at86rf231_get_address();
	printf("Source address is: %x \n", address);

	/*PAN*/
	uint16_t set_pan = 0x0001, newpan;
	newpan = at86rf231_set_pan(set_pan);
	uint16_t pan = at86rf231_get_pan();
	printf("source PAN is: %x \n", pan);



	/*send a packet*/
	uint8_t payload[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

	/*Config MAC header */
	ieee802154_frame_t frame;
	frame.fcf.frame_type = IEEE_802154_DATA_FRAME;
	frame.fcf.sec_enb = 0;
	frame.fcf.frame_pend = 0;
	frame.fcf.ack_req = 1;
	frame.dest_pan_id = pan;
	frame.dest_addr[0] = 0x02;
	frame.dest_addr[1] = 0x00;
	frame.fcf.src_addr_m = 0x02;
	frame.fcf.dest_addr_m = 0x02;
	frame.payload = payload;
	frame.payload_len = sizeof(payload);
	printf("frame created, payload length: %d\n", frame.payload_len);

	at86rf231_packet_t packet;
	packet.frame = frame;
	printf("packet created\n");

	for (int i=0; i<5; i++)
	{
	at86rf231_send(&packet);
	}

while(1)
{
//	at86rf231_rx_handler();




}



	return 0;
}
