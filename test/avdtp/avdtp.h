/*
 * Copyright (C) 2016 BlueKitchen GmbH
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

/*
 * avdtp.h
 * 
 * Audio/Video Distribution Transport Protocol
 *
 * This protocol defines A/V stream negotiation, establishment, and transmission 
 * procedures. Also specified are the message formats that are exchanged between 
 * such devices to transport their A/V streams in A/V distribution applications.
 *
 * Media packets are unidirectional, they travel downstream from AVDTP Source to AVDTP Sink. 
 */

#ifndef __AVDTP_H
#define __AVDTP_H

#include <stdint.h>
#include "hci.h"

#if defined __cplusplus
extern "C" {
#endif

#define MAX_NUM_SEPS 10

// protocols
#define PSM_AVCTP 0x0017
#define PSM_AVDTP 0x0019

// service classes
#define AUDIO_SOURCE_GROUP          0x110A
#define AUDIO_SINK_GROUP            0x110B
#define AV_REMOTE_CONTROL_TARGET    0X110C
#define ADVANCED_AUDIO_DISTRIBUTION 0X110D
#define AV_REMOTE_CONTROL           0X110E
#define AV_REMOTE_CONTROL_CONTROLER 0X110F

#define MAX_CSRC_NUM 15

// Supported Features
#define AVDTP_SOURCE_SF_Player      0x0001
#define AVDTP_SOURCE_SF_Microphone  0x0002
#define AVDTP_SOURCE_SF_Tuner       0x0004
#define AVDTP_SOURCE_SF_Mixer       0x0008

#define AVDTP_SINK_SF_Headphone     0x0001
#define AVDTP_SINK_SF_Speaker       0x0002
#define AVDTP_SINK_SF_Recorder      0x0004
#define AVDTP_SINK_SF_Amplifier     0x0008

// ACP to INT, Signal Response Header Error Codes
#define BAD_HEADER_FORMAT 0x01

// ACP to INT, Signal Response Payload Format Error Codes
#define BAD_LENGTH 0x11
#define BAD_ACP_SEID 0x12
#define SEP_IN_USE 0x13
#define SEP_NOT_IN_USE 0x14
#define BAD_SERV_CATEGORY 0x17
#define BAD_PAYLOAD_FORMAT 0x18
#define NOT_SUPPORTED_COMMAND 0x19
#define INVALID_CAPABILITIES 0x1A

// ACP to INT, Signal Response Transport Service Capabilities Error Codes
#define BAD_RECOVERY_TYPE 0x22
#define BAD_MEDIA_TRANSPORT_FORMAT 0x23
#define BAD_RECOVERY_FORMAT 0x25
#define BAD_ROHC_FORMAT 0x26
#define BAD_CP_FORMAT 0x27
#define BAD_MULTIPLEXING_FORMAT 0x28
#define UNSUPPORTED_CONFIGURAION 0x29

// ACP to INT, Procedure Error Codes
#define BAD_STATE 0x31
// Signal Identifier fields
typedef enum {
    AVDTP_SI_DISCOVER = 0x01,
    AVDTP_SI_GET_CAPABILITIES,
    AVDTP_SI_SET_CONFIGURATION,
    AVDTP_SI_GET_CONFIGURATION,
    AVDTP_SI_RECONFIGURE, //5
    AVDTP_SI_OPEN,
    AVDTP_SI_START,
    AVDTP_SI_CLOSE,
    AVDTP_SI_SUSPEND,
    AVDTP_SI_ABORT, //10
    AVDTP_SI_SECURITY_CONTROL,
    AVDTP_SI_GET_ALL_CAPABILITIES, //12
    AVDTP_SI_DELAYREPORT
} avdtp_signal_identifier_t;

typedef enum {
    AVDTP_SINGLE_PACKET= 0,
    AVDTP_START_PACKET    ,
    AVDTP_CONTINUE_PACKET ,
    AVDTP_END_PACKET
} avdtp_packet_type_t;

typedef enum {
    AVDTP_CMD_MSG = 0,
    AVDTP_GENERAL_REJECT_MSG   ,
    AVDTP_RESPONSE_ACCEPT_MSG ,
    AVDTP_RESPONSE_REJECT_MSG
} avdtp_message_type_t;

typedef enum{
    AVDTP_AUDIO = 0,
    AVDTP_VIDEO,
    AVDTP_MULTIMEDIA 
} avdtp_media_type_t;

typedef enum{
    AVDTP_CODEC_SBC             = 0x00,
    AVDTP_CODEC_MPEG_1_2_AUDIO  = 0x01, 
    AVDTP_CODEC_MPEG_2_4_AAC    = 0x02,
    AVDTP_CODEC_ATRAC_FAMILY    = 0x04,
    AVDTP_CODEC_NON_A2DP        = 0xFF
} avdtp_media_codec_type_t;

typedef enum{
    AVDTP_SOURCE = 0,
    AVDTP_SINK
} avdtp_sep_type_t;

typedef enum {
    AVDTP_MEDIA_TRANSPORT = 0X01,
    AVDTP_REPORTING,
    AVDTP_RECOVERY,
    AVDTP_CONTENT_PROTECTION,
    AVDTP_HEADER_COMPRESSION,
    AVDTP_MULTIPLEXING,
    AVDTP_MEDIA_CODEC,
    AVDTP_DELAY_REPORTING
} avdtp_service_category_t;

typedef struct {
    uint8_t recovery_type;                  // 0x01 = RFC2733
    uint8_t maximum_recovery_window_size;   // 0x01 to 0x18, for a Transport Packet
    uint8_t maximum_number_media_packets;   // 0x01 to 0x18, The maximum number of media packets a specific parity code covers
} avdtp_recovery_capabilities_t;            

typedef struct {
    avdtp_media_type_t       media_type;                     
    avdtp_media_codec_type_t media_codec_type; 
    uint16_t  media_codec_information_len;
    const uint8_t * media_codec_information;
} adtvp_media_codec_capabilities_t;

typedef struct {
    uint8_t cp_type_lsb;
    uint8_t cp_type_msb;
    uint16_t cp_type_value_len;
    const uint8_t * cp_type_value;
} adtvp_content_protection_t;

typedef struct{
    uint8_t back_ch;  // byte0 - bit 8; 0=Not Available/Not Used; 1=Available/In Use
    uint8_t media;    // byte0 - bit 7
    uint8_t recovery; // byte0 - bit 6
} avdtp_header_compression_capabilities_t;

typedef struct{
    uint8_t fragmentation; // byte0 - bit 8, Allow Adaptation Layer Fragmentation, 0 no, 1 yes
    // Request/indicate value of the Transport Session Identifier for a media, reporting, or recovery transport sessions, respectively
    uint8_t transport_identifiers_num;
    uint8_t transport_session_identifiers[3];   // byte1, upper 5bits, 0x01 to 0x1E
    // Request/indicate value for TCID for a media, reporting, or transport session
    uint8_t tcid[3];         // byte2 0x01 to 0x1E 
} avdtp_multiplexing_mode_capabilities_t;

typedef struct{
    avdtp_recovery_capabilities_t recovery;
    adtvp_media_codec_capabilities_t media_codec;
    adtvp_content_protection_t content_protection;
    avdtp_header_compression_capabilities_t header_compression;
    avdtp_multiplexing_mode_capabilities_t multiplexing_mode;
} avdtp_capabilities_t;

typedef struct {
    uint8_t seid;           // 0x01 – 0x3E, 6bit
    uint8_t in_use;         // 1 bit, 0 - not in use, 1 - in use
    avdtp_media_type_t media_type;     // 4 bit
    avdtp_sep_type_t   type;       // 1 bit, 0 - SRC, 1 - SNK

    uint16_t registered_service_categories;
    avdtp_capabilities_t capabilities;
} avdtp_sep_t;

typedef enum{
    AVDTP_SBC_48000 = 1,
    AVDTP_SBC_44100 = 2,
    AVDTP_SBC_32000 = 4,
    AVDTP_SBC_16000 = 8
} avdtp_sbc_sampling_frequency_t;

typedef enum{
    AVDTP_SBC_JOINT_STEREO  = 1,
    AVDTP_SBC_STEREO        = 2,
    AVDTP_SBC_DUAL_CHANNEL  = 4,
    AVDTP_SBC_MONO          = 8
} avdtp_sbc_channel_mode_t;

typedef enum{
    AVDTP_SBC_BLOCK_LENGTH_16 = 1,
    AVDTP_SBC_BLOCK_LENGTH_12 = 2,
    AVDTP_SBC_BLOCK_LENGTH_8  = 4,
    AVDTP_SBC_BLOCK_LENGTH_4  = 8
} avdtp_sbc_block_length_t;

typedef enum{
    AVDTP_SBC_SUBBANDS_8 = 1,
    AVDTP_SBC_SUBBANDS_4 = 2
} avdtp_sbc_subbands_t;

typedef enum{
    AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS = 1,
    AVDTP_SBC_ALLOCATION_METHOD_SNR      = 2
} avdtp_sbc_allocation_method_t;

typedef struct {
    uint8_t fragmentation;
    uint8_t starting_packet; // of fragmented SBC frame
    uint8_t last_packet;     // of fragmented SBC frame
    uint8_t num_frames;
} avdtp_sbc_codec_header_t;

typedef struct {
    uint8_t transaction_label;
    avdtp_packet_type_t packet_type;
    avdtp_message_type_t message_type;
    uint8_t signal_identifier;
} avdtp_signaling_packet_header_t;

typedef struct {
    uint8_t version;
    uint8_t padding;
    uint8_t extension;
    uint8_t csrc_count;
    uint8_t marker;
    uint8_t payload_type;

    uint16_t sequence_number;
    uint32_t timestamp;
    uint32_t synchronization_source;

    uint32_t csrc_list[MAX_CSRC_NUM];
} avdtp_media_packet_header_t;

typedef enum {
    AVDTP_IDLE,
    AVDTP_CONFIGURATION_SUBSTATEMACHINE,
    AVDTP_CONFIGURED,
    AVDTP_W2_ANSWER_OPEN_STREAM,
    AVDTP_W4_L2CAP_FOR_MEDIA_CONNECTED,
    AVDTP_OPEN, // 5
    AVDTP_W2_ANSWER_START_SINGLE_STREAM,
    AVDTP_W4_STREAMING_CONNECTION_OPEN,
    AVDTP_STREAMING, // 8
    AVDTP_CLOSING,
    AVDTP_ABORTING,
    AVDTP_W4_L2CAP_FOR_MEDIA_DISCONNECTED
} avdtp_stream_endpoint_state_t;

typedef enum {
    AVDTP_INITIATOR_STREAM_CONFIG_IDLE,
    AVDTP_INITIATOR_W2_DISCOVER_SEPS,
    AVDTP_INITIATOR_W4_SEPS_DISCOVERED,
    AVDTP_INITIATOR_W2_GET_CAPABILITIES,
    AVDTP_INITIATOR_W4_CAPABILITIES,
    AVDTP_INITIATOR_W2_GET_ALL_CAPABILITIES,
    AVDTP_INITIATOR_W4_ALL_CAPABILITIES,
    AVDTP_INITIATOR_W2_SET_CONFIGURATION,
    AVDTP_INITIATOR_W4_CONFIGURATION_SET,
    AVDTP_INITIATOR_W2_GET_CONFIGURATION,
    AVDTP_INITIATOR_W4_CONFIGURATION_RECEIVED,
    AVDTP_INITIATOR_STREAM_CONFIG_DONE
} avdtp_initiator_stream_config_state_t;

typedef enum {
    AVDTP_ACCEPTOR_STREAM_CONFIG_IDLE,
    AVDTP_ACCEPTOR_W2_ANSWER_DISCOVER_SEPS,
    AVDTP_ACCEPTOR_W2_ANSWER_GET_CAPABILITIES,
    AVDTP_ACCEPTOR_W2_ANSWER_GET_ALL_CAPABILITIES,
    AVDTP_ACCEPTOR_W2_ANSWER_SET_CONFIGURATION,
    AVDTP_ACCEPTOR_W2_REJECT_UNKNOWN_CMD,
    AVDTP_ACCEPTOR_STREAM_CONFIG_DONE
} avdtp_acceptor_stream_config_state_t;

typedef enum {
    AVDTP_BASIC_SERVICE_MODE,
    AVDTP_MULTIPLEXING_SERVICE_MODE
} avdtp_service_mode_t;

#if defined __cplusplus
}
#endif

#endif // __AVDTP_H