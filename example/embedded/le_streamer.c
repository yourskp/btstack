/*
 * Copyright (C) 2014 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

// *****************************************************************************
/* EXAMPLE_START(le_streamer): LE Peripheral - Stream data over GATT
 *
 * @text All newer operating systems provide GATT Client functionality.
 * This example shows how to get a maximal throughput via BLE:
 * - send whenever possible
 * - use the max ATT MTU 
 * - update the connection paramters to the minimal allowed connection interval
 */
 // *****************************************************************************

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "btstack-config.h"

#include <btstack/run_loop.h>
#include <btstack/sdp_util.h>

#include "debug.h"
#include "btstack_memory.h"
#include "hci.h"
#include "hci_dump.h"

#include "l2cap.h"

#include "le_streamer.h"

#include "att.h"
#include "att_server.h"
#include "le_device_db.h"
#include "gap_le.h"
#include "sm.h"

/* @section Main Application Setup
 *
 * @text Listing MainConfiguration shows main application code.
 * It initializes L2CAP, the Security Manager and configures the ATT Server with the pre-compiled
 * ATT Database generated from $le_streamer.gatt$. Finally, it configures the advertisements 
 * and boots the Bluetooth stack. 
 */
 
/* LISTING_START(MainConfiguration): Init L2CAP, SM, ATT Server, and enable advertisements */
static int  le_notification_enabled;

static void     packet_handler (void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
static int      att_write_callback(uint16_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
static void     streamer(void);

const uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, 0x01, 0x06, 
    // Name
    0x0c, 0x09, 'L', 'E', ' ', 'S', 't', 'r', 'e', 'a', 'm', 'e', 'r', 
};
const uint8_t adv_data_len = sizeof(adv_data);

static void le_streamer_setup(void){
    l2cap_init();
    l2cap_register_packet_handler(packet_handler);

    // setup le device db
    le_device_db_init();

    // setup SM: Display only
    sm_init();

    // setup ATT server
    att_server_init(profile_data, NULL, att_write_callback);    
    att_dump_attributes();

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
    gap_advertisements_enable(1);
}
/* LISTING_END */

/* 
 * @section Packet Handler
 *
 * @text The packet handler is only used to stop the counter after a disconnect
 */

/* LISTING_START(packetHandler): Packet Handler */
static void packet_handler (void * connection, uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
	switch (packet_type) {
		case HCI_EVENT_PACKET:
			switch (packet[0]) {
                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    le_notification_enabled = 0;
                    break;
			}
            break;
	}
    // try sending whenever something happens
    streamer();
}
/* LISTING_END */

/*
 * @section Streamer
 *
 * @text The streamer updates the value of the single Characteristic provided in this example,
 * and sends a notification for this characteristic if enabled, see Listing streamer.
 */

 /* LISTING_START(streamer): Hearbeat Handler */
 #define REPORT_INTERVAL_MS 10000
static int  counter = 'A';
static char test_data[200];
static int  test_data_len;

static uint32_t test_data_sent;
static uint32_t test_data_start;

static uint32_t get_time_ms(void){
#if defined(HAVE_TICK) || defined(HAVE_TIME_MS)
    return embedded_get_time_ms();
#elif defined(HAVE_TIME)
    return posix_get_time_ms();
#else
#error "No idea how to get time"
#endif
}

static void test_reset(void){
    test_data_start = get_time_ms();
    test_data_sent = 0;
}

static void test_track_sent(int bytes_sent){
    test_data_sent += test_data_len;
    // evaluate
    uint32_t now = get_time_ms();
    if (now < test_data_start + REPORT_INTERVAL_MS) return;
    // print speed
    int bytes_per_second = test_data_sent  / (REPORT_INTERVAL_MS / 1000);
    printf("%u bytes sent-> %u.%03u kB/s\n", test_data_sent, bytes_per_second / 1000, bytes_per_second % 1000);
    // restart
    test_reset();
}

static void  streamer(void){
    // check if we can send
    if (!le_notification_enabled) return;
    if (!att_server_can_send()) return;

    // create test data
    int i;
    counter++;
    if (counter > 'Z') counter = 'A';
    for (i=0;i<sizeof(test_data);i++){
        test_data[i] = counter;
    }
    // figure out max data len
    test_data_len = 20;

    // send
    att_server_notify(ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE, (uint8_t*) test_data, test_data_len);

    // track
    test_track_sent(test_data_len);
} 
/* LISTING_END */

/*
 * @section ATT Write
 *
 * @text The only valid ATT write in this example is to the Client Characteristic Configuration, which configures notification
 * and indication. If the ATT handle matches the client configuration handle, the new configuration value is stored and used
 * in the heartbeat handler to decide if a new value should be sent. See Listing attWrite.
 */

/* LISTING_START(attWrite): ATT Write */
static int att_write_callback(uint16_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
    if (att_handle != ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE) return 0;
    le_notification_enabled = READ_BT_16(buffer, 0) == GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION;
    test_reset();
    return 0;
}
/* LISTING_END */

int btstack_main(void);
int btstack_main(void)
{
    le_streamer_setup();

    // turn on!
	hci_power_control(HCI_POWER_ON);
	    
    return 0;
}
/* EXAMPLE_END */