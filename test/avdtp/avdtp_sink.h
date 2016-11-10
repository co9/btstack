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
 * avdtp_sink.h
 * 
 * Audio/Video Distribution Transport Protocol (AVDTP) Sink
 *
 * AVDTP Sink is a device that accepts streamed media data.  
 */

#ifndef __AVDTP_SINK_H
#define __AVDTP_SINK_H

#include <stdint.h>
#include "hci.h"
#include "avdtp.h"

#if defined __cplusplus
extern "C" {
#endif

typedef struct avdtp_stream_endpoint {
    btstack_linked_item_t    item;
    
    uint16_t l2cap_media_cid;
    uint16_t l2cap_reporting_cid;
    uint16_t l2cap_recovery_cid;

    uint8_t  num_l2cap_channels_opened;

    avdtp_stream_endpoint_state_t        avdtp_state;
    avdtp_initiator_stream_config_state_t initiator_config_state;
    avdtp_acceptor_stream_config_state_t  acceptor_config_state;

    uint8_t initiator_transaction_label;
    uint8_t acceptor_transaction_label;
    avdtp_signal_identifier_t unknown_signal_identifier;
    
    // store remote seps
    avdtp_sep_t remote_seps[MAX_NUM_SEPS];
    uint8_t remote_seps_num;
    
    // currently active seid - stream opened
    uint8_t active_seid;
    uint8_t query_seid;

    // currently active remote seid
    uint8_t remote_sep_index;
    
    // register request for L2cap connection release
    uint8_t disconnect;
} avdtp_stream_endpoint_t;

/* API_START */
/**
 * @brief AVDTP Sink service record. 
 * @param service
 * @param service_record_handle
 * @param supported_features 16-bit bitmap, see AVDTP_SINK_SF_* values in avdtp.h
 * @param service_name
 * @param service_provider_name
 */
void a2dp_sink_create_sdp_record(uint8_t * service,  uint32_t service_record_handle, uint16_t supported_features, const char * service_name, const char * service_provider_name);

/**
 * @brief Set up AVDTP Sink device.
 */
void avdtp_sink_init(void);

// returns sep_id
uint8_t avdtp_sink_register_stream_endpoint(avdtp_sep_type_t sep_type, avdtp_media_type_t media_type);

void avdtp_sink_register_media_transport_category(uint8_t seid);
void avdtp_sink_register_reporting_category(uint8_t seid);
void avdtp_sink_register_delay_reporting_category(uint8_t seid);
void avdtp_sink_register_recovery_category(uint8_t seid, uint8_t maximum_recovery_window_size, uint8_t maximum_number_media_packets);
void avdtp_sink_register_header_compression_category(uint8_t seid, uint8_t back_ch, uint8_t media, uint8_t recovery);
void avdtp_sink_register_multiplexing_category(uint8_t seid, uint8_t fragmentation);

void avdtp_sink_register_media_codec_category(uint8_t seid, avdtp_media_type_t media_type, avdtp_media_codec_type_t media_codec_type, const uint8_t * media_codec_info, uint16_t media_codec_info_len);
void avdtp_sink_register_content_protection_category(uint8_t seid, uint8_t cp_type_lsb,  uint8_t cp_type_msb, const uint8_t * cp_type_value, uint8_t cp_type_value_len);

/**
 * @brief Register callback for the AVDTP Sink client. 
 * @param callback
 */
void avdtp_sink_register_packet_handler(btstack_packet_handler_t callback);

/**
 * @brief Connect to device with a bluetooth address. (and perform configuration?)
 * @param bd_addr
 */
void avdtp_sink_connect(bd_addr_t bd_addr);

void avdtp_sink_register_media_handler(void (*callback)(avdtp_stream_endpoint_t * stream_endpoint, uint8_t *packet, uint16_t size));
/**
 * @brief Disconnect from device with connection handle. 
 * @param con_handle
 */
void avdtp_sink_disconnect(uint16_t con_handle);

/* API_END */

#if defined __cplusplus
}
#endif

#endif // __AVDTP_SINK_H