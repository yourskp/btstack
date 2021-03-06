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
//
//  HFP Hands-Free (HF) unit and Audio-Gateway Commons
//
// *****************************************************************************


#ifndef btstack_hfp_h
#define btstack_hfp_h

#include "hci.h"
#include "sdp_query_rfcomm.h"

#if defined __cplusplus
extern "C" {
#endif


/* HF Supported Features: 
0: EC and/or NR function
1: Three-way calling
2: CLI presentation capability
3: Voice recognition activation
4: Remote volume control
5: Enhanced call status
6: Enhanced call control
7: Codec negotiation
8: HF Indicators
9: eSCO S4 (and T2) Settings Supported
10-31: Reserved for future definition
*/
#define HFP_HFSF_THREE_WAY_CALLING  1
#define HFP_HFSF_EC_NR_FUNCTION     0
#define HFP_HFSF_CODEC_NEGOTIATION  7
#define HFP_HFSF_HF_INDICATORS      8
#define HFP_HFSF_ESCO               9

/* AG Supported Features:
0: Three-way calling
1: EC and/or NR function
2: Voice recognition function
3: In-band ring tone capability
4: Attach a number to a voice tag
5: Ability to reject a call
6: Enhanced call status
7: Enhanced call control
8: Extended Error Result Codes
9: Codec negotiation
10: HF Indicators
11: eSCO S4 (and T2) Settings Supported
12-31: Reserved for future definition
*/
#define HFP_AGSF_THREE_WAY_CALLING  0
#define HFP_AGSF_EC_NR_FUNCTION     1
#define HFP_AGSF_IN_BAND_RING_TONE  3
#define HFP_AGSF_CODEC_NEGOTIATION  9
#define HFP_AGSF_HF_INDICATORS      10
#define HFP_AGSF_ESCO               11

#define HFP_DEFAULT_HF_SUPPORTED_FEATURES 0x0000
#define HFP_DEFAULT_AG_SUPPORTED_FEATURES 0x0009

#define HFP_MAX_NUM_CODECS 20
#define HFP_MAX_NUM_AG_INDICATORS 20
#define HFP_MAX_NUM_HF_INDICATORS 20
#define HFP_MAX_INDICATOR_DESC_SIZE 20 

#define HFP_SUPPORTED_FEATURES "+BRSF"
#define HFP_AVAILABLE_CODECS "+BAC"
#define HFP_INDICATOR "+CIND"
#define HFP_ENABLE_STATUS_UPDATE_FOR_AG_INDICATORS "+CMER"
#define HFP_UPDATE_ENABLE_STATUS_FOR_INDIVIDUAL_AG_INDICATORS "+BIA" // +BIA:<enabled>,,<enabled>,,,<enabled>
#define HFP_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES "+CHLD"
#define HFP_GENERIC_STATUS_INDICATOR "+BIND"
#define HFP_TRANSFER_AG_INDICATOR_STATUS "+CIEV" // +CIEV: <index>,<value>
#define HFP_QUERY_OPERATOR_SELECTION "+COPS"     // +COPS: <mode>,0,<opearator>
#define HFP_ENABLE_EXTENDED_AUDIO_GATEWAY_ERROR "+CMEE"
#define HFP_EXTENDED_AUDIO_GATEWAY_ERROR "+CME ERROR"
#define HFP_TRIGGER_CODEC_CONNECTION_SETUP "+BCC"
#define HFP_CONFIRM_COMMON_CODEC "+BCS"
#define HFP_CALL_ANSWERED "ATA"
#define HFP_HANG_UP_CALL "+CHUP"

#define HFP_OK "OK"
#define HFP_ERROR "ERROR"

// Codecs 
#define HFP_CODEC_CVSD 0x01
#define HFP_CODEC_MSBC 0x02

typedef enum {
    HFP_CMD_NONE = 0,
    HFP_CMD_ERROR,
    HFP_CMD_UNKNOWN,
    HFP_CMD_OK,
    HFP_CMD_SUPPORTED_FEATURES,
    HFP_CMD_AVAILABLE_CODECS,
    
    HFP_CMD_RETRIEVE_AG_INDICATORS,
    HFP_CMD_RETRIEVE_AG_INDICATORS_STATUS, 
    
    HFP_CMD_ENABLE_INDICATOR_STATUS_UPDATE,
    HFP_CMD_ENABLE_INDIVIDUAL_AG_INDICATOR_STATUS_UPDATE,
    HFP_CMD_SUPPORT_CALL_HOLD_AND_MULTIPARTY_SERVICES,

    HFP_CMD_LIST_GENERIC_STATUS_INDICATORS,
    HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS,
    HFP_CMD_RETRIEVE_GENERIC_STATUS_INDICATORS_STATE,
    
    HFP_CMD_TRANSFER_AG_INDICATOR_STATUS,

    HFP_CMD_QUERY_OPERATOR_SELECTION_NAME,
    HFP_CMD_QUERY_OPERATOR_SELECTION_NAME_FORMAT,

    HFP_CMD_ENABLE_EXTENDED_AUDIO_GATEWAY_ERROR,
    HFP_CMD_EXTENDED_AUDIO_GATEWAY_ERROR,
    HFP_CMD_TRIGGER_CODEC_CONNECTION_SETUP,
    HFP_CMD_AG_SEND_COMMON_CODEC,
    HFP_CMD_AG_SUGGESTED_CODEC,
    HFP_CMD_HF_CONFIRMED_CODEC,
    HFP_CMD_CALL_ANSWERED,
    HFP_CMD_HANG_UP_CALL
} hfp_command_t;

typedef enum {
    HFP_CME_ERROR_AG_FAILURE = 0, 
    HFP_CME_ERROR_NO_CONNECTION_TO_PHONE,
    HFP_CME_ERROR_2,
    HFP_CME_ERROR_OPERATION_NOT_ALLOWED, 
    HFP_CME_ERROR_OPERATION_NOT_SUPPORTED,
    HFP_CME_ERROR_PH_SIM_PIN_REQUIRED,
    HFP_CME_ERROR_6,
    HFP_CME_ERROR_7,
    HFP_CME_ERROR_8,
    HFP_CME_ERROR_9,
    HFP_CME_ERROR_SIM_NOT_INSERTED,
    HFP_CME_ERROR_SIM_PIN_REQUIRED,
    HFP_CME_ERROR_SIM_PUK_REQUIRED,
    HFP_CME_ERROR_SIM_FAILURE,
    HFP_CME_ERROR_SIM_BUSY,
    HFP_CME_ERROR_15,
    HFP_CME_ERROR_INCORRECT_PASSWORD, 
    HFP_CME_ERROR_SIM_PIN2_REQUIRED,
    HFP_CME_ERROR_SIM_PUK2_REQUIRED,
    HFP_CME_ERROR_19,
    HFP_CME_ERROR_MEMORY_FULL,
    HFP_CME_ERROR_INVALID_INDEX,
    HFP_CME_ERROR_22,
    HFP_CME_ERROR_MEMORY_FAILURE,
    HFP_CME_ERROR_TEXT_STRING_TOO_LONG,
    HFP_CME_ERROR_INVALID_CHARACTERS_IN_TEXT_STRING,
    HFP_CME_ERROR_DIAL_STRING_TOO_LONG,
    HFP_CME_ERROR_INVALID_CHARACTERS_IN_DIAL_STRING,
    HFP_CME_ERROR_28,
    HFP_CME_ERROR_29,
    HFP_CME_ERROR_NO_NETWORK_SERVICE,
    HFP_CME_ERROR_NETWORK_TIMEOUT,
    HFP_CME_ERROR_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY
} hfp_cme_error_t;

typedef enum {
    HFP_CALL_STATUS_NO_HELD_OR_ACTIVE_CALLS = 0,
    HFP_CALL_STATUS_ACTIVE_OR_HELD_CALL_IS_PRESENT
} hfp_call_status_t;

typedef enum {  
    HFP_CALLSETUP_STATUS_NO_CALL_SETUP_IN_PROGRESS = 0, 
    HFP_CALLSETUP_STATUS_INCOMING_CALL_SETUP_IN_PROGRESS,
    HFP_CALLSETUP_STATUS_OUTGOING_CALL_SETUP_IN_DIALING_STATE,
    HFP_CALLSETUP_STATUS_OUTGOING_CALL_SETUP_IN_ALERTING_STATE
} hfp_callsetup_status_t;

typedef enum {
    HFP_HELDCALL_STATUS_NO_CALLS_HELD = 0,
    HFP_HELDCALL_STATUS_CALL_ON_HOLD_OR_SWAPPED,
    HFP_HELDCALL_STATUS_CALL_ON_HOLD_AND_NO_ACTIVE_CALLS 
} hfp_callheld_status_t;

typedef enum {
    HFP_PARSER_CMD_HEADER = 0,
    HFP_PARSER_CMD_SEQUENCE,
    HFP_PARSER_SECOND_ITEM,
    HFP_PARSER_THIRD_ITEM
} hfp_parser_state_t;


typedef enum {
    HFP_IDLE = 0, //0
    HFP_SDP_QUERY_RFCOMM_CHANNEL,
    HFP_W4_SDP_QUERY_COMPLETE,
    HFP_W4_RFCOMM_CONNECTED,
    
    HFP_EXCHANGE_SUPPORTED_FEATURES,
    HFP_W4_EXCHANGE_SUPPORTED_FEATURES, // 5
    
    HFP_NOTIFY_ON_CODECS,
    HFP_W4_NOTIFY_ON_CODECS,
    
    HFP_RETRIEVE_INDICATORS,
    HFP_W4_RETRIEVE_INDICATORS,
    
    HFP_RETRIEVE_INDICATORS_STATUS, // 10
    HFP_W4_RETRIEVE_INDICATORS_STATUS,
    
    HFP_ENABLE_INDICATORS_STATUS_UPDATE,
    HFP_W4_ENABLE_INDICATORS_STATUS_UPDATE,
    
    HFP_RETRIEVE_CAN_HOLD_CALL,
    HFP_W4_RETRIEVE_CAN_HOLD_CALL, // 15
    
    HFP_LIST_GENERIC_STATUS_INDICATORS,
    HFP_W4_LIST_GENERIC_STATUS_INDICATORS,
    
    HFP_RETRIEVE_GENERIC_STATUS_INDICATORS,
    HFP_W4_RETRIEVE_GENERIC_STATUS_INDICATORS,
    
    HFP_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS, 
    HFP_W4_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS,
    
    HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED, 
    
    HFP_W2_CONNECT_SCO,
    HFP_W4_SCO_CONNECTED,
    
    HFP_AUDIO_CONNECTION_ESTABLISHED, 
    
    HFP_W2_DISCONNECT_SCO,
    HFP_W4_SCO_DISCONNECTED, 

    HFP_W2_DISCONNECT_RFCOMM,
    HFP_W4_RFCOMM_DISCONNECTED, 
    HFP_W4_RFCOMM_DISCONNECTED_AND_RESTART, 
    HFP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN
} hfp_state_t;

typedef enum {
    HFP_CODECS_IDLE,
    HFP_CODECS_RECEIVED_LIST,
    HFP_CODECS_RECEIVED_TRIGGER_CODEC_EXCHANGE,
    HFP_CODECS_W4_AG_COMMON_CODEC,
    HFP_CODECS_AG_SENT_COMMON_CODEC,
    HFP_CODECS_AG_RESEND_COMMON_CODEC,
    HFP_CODECS_EXCHANGED,
    HFP_CODECS_ERROR
} hfp_codecs_state_t;

typedef enum {
    HFP_CALL_IDLE,
    HFP_CALL_TRIGGER_AUDIO_CONNECTION,
    HFP_CALL_W4_AUDIO_CONNECTION,
    HFP_CALL_RING,
    HFP_CALL_W4_ANSWER,

    HFP_CALL_TRANSFER_CALL_STATUS,
    HFP_CALL_TRANSFER_CALLSETUP_STATUS,
    HFP_CALL_ACTIVE
} hfp_call_state_t;

typedef enum{
    HFP_NONE_SM,
    HFP_SLC_SM,
    HFP_SLC_QUERIES_SM,
    HFP_CODECS_CONNECTION_SM,
    HFP_AUDIO_CONNECTION_SM,
    HFP_CALL_SM
} hfp_state_machine_t;

typedef void (*hfp_callback_t)(uint8_t * event, uint16_t event_size);

typedef struct{
    uint16_t uuid;
    uint8_t state; // enabled
} hfp_generic_status_indicator_t;

typedef struct{
    uint8_t index;
    char name[HFP_MAX_INDICATOR_DESC_SIZE];
    uint8_t min_range;
    uint8_t max_range;
    uint8_t status;
    uint8_t mandatory;
    uint8_t enabled;
    uint8_t status_changed;
} hfp_ag_indicator_t;

typedef struct{
    char name[3];
} hfp_call_service_t;


typedef struct{
    uint8_t mode;
    uint8_t format;
    char name[17]; // enabled
} hfp_network_opearator_t;


typedef struct hfp_connection {
    linked_item_t    item;
    
    bd_addr_t remote_addr;
    uint16_t con_handle;
    uint16_t sco_handle;
    uint16_t rfcomm_channel_nr;
    uint16_t rfcomm_cid;
    
    hfp_state_machine_t state_machine;

    hfp_state_t state;
    hfp_call_state_t call_state;
    hfp_codecs_state_t codecs_state;
    
    // needed for reestablishing connection
    uint16_t service_uuid;

    // used during service level connection establishment
    hfp_command_t command;
    hfp_parser_state_t parser_state;
    int      parser_item_index;
    uint8_t  line_buffer[HFP_MAX_INDICATOR_DESC_SIZE];
    int      line_size;
    
    uint32_t remote_supported_features;

    // TODO: rename into hf_codecs_nr
    int      remote_codecs_nr;
    uint16_t remote_codecs[HFP_MAX_INDICATOR_DESC_SIZE];
    int      ag_indicators_nr;
    hfp_ag_indicator_t ag_indicators[HFP_MAX_INDICATOR_DESC_SIZE];
    int      remote_call_services_nr;
    hfp_call_service_t remote_call_services[HFP_MAX_INDICATOR_DESC_SIZE];
    
    // TODO: use bitmap.
    int      generic_status_indicators_nr;
    hfp_generic_status_indicator_t generic_status_indicators[HFP_MAX_INDICATOR_DESC_SIZE];
    uint8_t  enable_status_update_for_ag_indicators;

    uint32_t ag_indicators_status_update_bitmap;
    hfp_network_opearator_t network_operator;
    
    // Retrieved during service level connection establishment, not used yet
    uint8_t  negotiated_codec;

    // TODO: put these bit flags in a bitmap
    uint8_t ok_pending;
    // uint8_t send_ok;
    uint8_t send_error;

    uint8_t keep_separator;
    
    uint8_t change_status_update_for_individual_ag_indicators; 
    uint8_t operator_name_changed;      

    uint8_t enable_extended_audio_gateway_error_report;
    uint8_t extended_audio_gateway_error;

    // establish codecs connection
    uint8_t suggested_codec;
    uint8_t codec_confirmed;

    uint8_t establish_audio_connection; 
    uint8_t release_audio_connection; 

    uint8_t run_call_state_machine;
    uint8_t use_in_band_ring_tone;
    uint8_t terminate_call;
} hfp_connection_t;

// UTILS_START : TODO move to utils
int send_str_over_rfcomm(uint16_t cid, char * command);
int join(char * buffer, int buffer_size, uint8_t * values, int values_nr);
int join_bitmap(char * buffer, int buffer_size, uint32_t values, int values_nr);
int get_bit(uint16_t bitmap, int position);
int store_bit(uint32_t bitmap, int position, uint8_t value);
// UTILS_END

void hfp_create_sdp_record(uint8_t * service, uint16_t service_uuid, int rfcomm_channel_nr, const char * name);
void hfp_handle_hci_event(hfp_callback_t callback, uint8_t packet_type, uint8_t *packet, uint16_t size);
void hfp_emit_event(hfp_callback_t callback, uint8_t event_subtype, uint8_t value);

hfp_connection_t * get_hfp_connection_context_for_rfcomm_cid(uint16_t cid);
hfp_connection_t * get_hfp_connection_context_for_bd_addr(bd_addr_t bd_addr);
hfp_connection_t * get_hfp_connection_context_for_sco_handle(uint16_t handle);

int get_hfp_generic_status_indicators_nr(void);
hfp_generic_status_indicator_t * get_hfp_generic_status_indicators(void);
void set_hfp_generic_status_indicators(hfp_generic_status_indicator_t * indicators, int indicator_nr);

linked_list_t * hfp_get_connections(void);
void hfp_parse(hfp_connection_t * context, uint8_t byte, int isHandsFree);

void hfp_init(uint16_t rfcomm_channel_nr);
void hfp_establish_service_level_connection(bd_addr_t bd_addr, uint16_t service_uuid);
void hfp_release_service_level_connection(hfp_connection_t * connection);
void hfp_reset_context_flags(hfp_connection_t * context);

void hfp_release_audio_connection(hfp_connection_t * context);

const char * hfp_hf_feature(int index);
const char * hfp_ag_feature(int index);

#if defined __cplusplus
}
#endif

#endif