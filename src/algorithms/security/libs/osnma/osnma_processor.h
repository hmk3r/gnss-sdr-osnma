// TODO: Docstrings
/*!
* \file osnma_processor.h
* \brief  OSNMA Lib
* \author Lyubomir Kyorovski, 2022. lkyorovski(at)ethz.ch
*
* -----------------------------------------------------------------------------
*
* GNSS-SDR is a Global Navigation Satellite System software-defined receiver.
* This file is part of GNSS-SDR.
*
* Copyright (C) 2010-2022  (see AUTHORS file for a list of contributors)
* SPDX-License-Identifier: GPL-3.0-or-later
*
* -----------------------------------------------------------------------------
 */

#ifndef GNSS_SDR_OSNMA_PROCESSOR_H
#define GNSS_SDR_OSNMA_PROCESSOR_H


#include "osnma_subframe.h"
#include "osnma_dsm_message.h"

#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <unordered_set>

// TODO: Add OpenSSL Fallback
//#if USE_OPENSSL_FALLBACK
//#include <openssl/crypto.h>
//#include <openssl/x509.h>
//#include <openssl/pem.h>
//#include <openssl/ssl.h>
//#include <openssl/err.h>
//#else

#include <gnutls/gnutls.h>
#include <gnutls/compat.h>
#include <gnutls/crypto.h>
#include <gnutls/x509.h>
#include <gnutls/abstract.h>
//#endif

#define LIBOSNMA_OSNMA_DATA_LENGTH 40
#define LIBOSNMA_NAV_DATA_LENGTH 128

// Nav data fields
#define LIBOSNMA_WORD_TYPE_OFFSET 0
#define LIBOSNMA_WORD_TYPE_LENGTH 6

//NMA Header fields
#define LIBOSNMA_NMA_HEADER_OFFSET 0
#define LIBOSNMA_NMA_HEADER_LENGTH 8
#define LIBOSNMA_NMAS_OFFSET 0
#define LIBOSNMA_NMAS_LENGTH 2
#define LIBOSNMA_CID_OFFSET (LIBOSNMA_NMAS_OFFSET + LIBOSNMA_NMAS_LENGTH)
#define LIBOSNMA_CID_LENGTH 2
#define LIBOSNMA_CPKS_OFFSET (LIBOSNMA_CID_OFFSET + LIBOSNMA_CID_LENGTH)
#define LIBOSNMA_CPKS_LENGTH 3
#define LIBOSNMA_NMAH_RESERVED_OFFSET (LIBOSNMA_CPKS_OFFSET + LIBOSNMA_CPKS_LENGTH)
#define LIBOSNMA_NMAH_RESERVED_LENGTH 1

// DSM Header fields
#define LIBOSNMA_DSM_ID_OFFSET (LIBOSNMA_NMAH_RESERVED_OFFSET + LIBOSNMA_NMAH_RESERVED_LENGTH)
#define LIBOSNMA_DSM_ID_LENGTH 4
#define LIBOSNMA_DSM_BLOCK_ID_OFFSET (LIBOSNMA_DSM_ID_OFFSET + LIBOSNMA_DSM_ID_LENGTH)
#define LIBOSNMA_DSM_BLOCK_ID_LENGTH 4
#define LIBOSNMA_DSM_BLOCK_OFFSET (LIBOSNMA_DSM_BLOCK_ID_OFFSET + LIBOSNMA_DSM_BLOCK_ID_LENGTH)
#define LIBOSNMA_DSM_BLOCK_LENGTH 104

// DSM-KROOT header fields, block 0
#define LIBOSNMA_DSM_KROOT_NB_BLOCKS_OFFSET 0
#define LIBOSNMA_DSM_KROOT_NB_BLOCKS_LENGTH 4
#define LIBOSNMA_DSM_KROOT_PKID_OFFSET (LIBOSNMA_DSM_KROOT_NB_BLOCKS_OFFSET + LIBOSNMA_DSM_KROOT_NB_BLOCKS_LENGTH)
#define LIBOSNMA_DSM_KROOT_PKID_LENGTH 4
#define LIBOSNMA_DSM_KROOT_CIDKR_OFFSET (LIBOSNMA_DSM_KROOT_PKID_OFFSET + LIBOSNMA_DSM_KROOT_PKID_LENGTH)
#define LIBOSNMA_DSM_KROOT_CIDKR_LENGTH 2
#define LIBOSNMA_DSM_KROOT_RESERVED_1_OFFSET (LIBOSNMA_DSM_KROOT_CIDKR_OFFSET + LIBOSNMA_DSM_KROOT_CIDKR_LENGTH)
#define LIBOSNMA_DSM_KROOT_RESERVED_1_LENGTH 2
#define LIBOSNMA_DSM_KROOT_HF_OFFSET (LIBOSNMA_DSM_KROOT_RESERVED_1_OFFSET + LIBOSNMA_DSM_KROOT_RESERVED_1_LENGTH)
#define LIBOSNMA_DSM_KROOT_HF_LENGTH 2
#define LIBOSNMA_DSM_KROOT_MF_OFFSET (LIBOSNMA_DSM_KROOT_HF_OFFSET + LIBOSNMA_DSM_KROOT_HF_LENGTH)
#define LIBOSNMA_DSM_KROOT_MF_LENGTH 2
#define LIBOSNMA_DSM_KROOT_KS_OFFSET (LIBOSNMA_DSM_KROOT_MF_OFFSET + LIBOSNMA_DSM_KROOT_MF_LENGTH)
#define LIBOSNMA_DSM_KROOT_KS_LENGTH 4
#define LIBOSNMA_DSM_KROOT_TS_OFFSET (LIBOSNMA_DSM_KROOT_KS_OFFSET + LIBOSNMA_DSM_KROOT_KS_LENGTH)
#define LIBOSNMA_DSM_KROOT_TS_LENGTH 4
#define LIBOSNMA_DSM_KROOT_MACLT_OFFSET (LIBOSNMA_DSM_KROOT_TS_OFFSET + LIBOSNMA_DSM_KROOT_TS_LENGTH)
#define LIBOSNMA_DSM_KROOT_MACLT_LENGTH 8
#define LIBOSNMA_DSM_KROOT_RESERVED_OFFSET (LIBOSNMA_DSM_KROOT_MACLT_OFFSET + LIBOSNMA_DSM_KROOT_MACLT_LENGTH)
#define LIBOSNMA_DSM_KROOT_RESERVED_LENGTH 4
#define LIBOSNMA_DSM_KROOT_WNK_OFFSET (LIBOSNMA_DSM_KROOT_RESERVED_OFFSET + LIBOSNMA_DSM_KROOT_RESERVED_LENGTH)
#define LIBOSNMA_DSM_KROOT_WNK_LENGTH 12
#define LIBOSNMA_DSM_KROOT_TOWHK_OFFSET (LIBOSNMA_DSM_KROOT_WNK_OFFSET + LIBOSNMA_DSM_KROOT_WNK_LENGTH)
#define LIBOSNMA_DSM_KROOT_TOWHK_LENGTH 8
#define LIBOSNMA_DSM_KROOT_ALPHA_OFFSET (LIBOSNMA_DSM_KROOT_TOWHK_OFFSET + LIBOSNMA_DSM_KROOT_TOWHK_LENGTH)
#define LIBOSNMA_DSM_KROOT_ALPHA_LENGTH 48
#define LIBOSNMA_DSM_BLOCK_CIDKR_TO_KROOT_LENGTH (LIBOSNMA_DSM_BLOCK_LENGTH - LIBOSNMA_DSM_KROOT_CIDKR_OFFSET)


// MACK message fields, offset must be calculated dynamically
#define LIBOSNMA_MACK_MACSEQ_LENGTH 12
#define LIBOSNMA_MACK_RESERVED_2_LENGTH 4
#define LIBOSNMA_MACK_TAG_INFO_LENGTH 16
#define LIBOSNMA_MACK_TAG_INFO_PRN_D_LENGTH 8
#define LIBOSNMA_MACK_TAG_INFO_ADKD_LENGTH 4
#define LIBOSNMA_MACK_TAG_INFO_RESERVED_2_LENGTH 4

// Constants
#define LIBOSNMA_DSM_ID_PKR_START_ID 12


enum KeyType
{
    merkle_tree,
    public_key,
    error
};

class OSNMAXmlKeyInfo
{
public:
    OSNMAXmlKeyInfo() = default;
    KeyType type;
    int32_t id;
    std::string filename;
    std::string algorithm;
};

class PublicKeyInfo
{
public:
    PublicKeyInfo() = default;
    gnutls_pubkey_t key;
    gnutls_sign_algorithm_t algorithm;

};

const std::vector<std::unordered_set<int>> GALILEO_E1_INAV_WORD_SEQ
    {
        {2},
        {4},
        {6},
        {7, 9},
        {8, 10},
        {17, 18, 0},
        {19, 20, 0},
        {16, 0},
        {0},
        {0},
        {1},
        {3},
        {5},
        {0},
        {16,0}
    };

const std::unordered_map<int32_t, int32_t> DSM_KROOT_NB_BLOCKS_TABLE
    {
        {0, -1},
        {1, 7},
        {2, 8},
        {3, 9},
        {4, 10},
        {5, 11},
        {6, 12},
        {7, 13},
        {8, 14},
        {9, -1},
        {10, -1},
        {11, -1},
        {12, -1},
        {13, -1},
        {14, -1},
        {15, -1}
    };

const std::unordered_map<int32_t, int32_t> DSM_KROOT_KS_TABLE
    {
        {0, 96},
        {1, 104},
        {2, 112},
        {3, 120},
        {4, 128},
        {5, 160},
        {6, 192},
        {7, 224},
        {8, 256},
        {9, -1},
        {10, -1},
        {11, -1},
        {12, -1},
        {13, -1},
        {14, -1},
        {15, -1}
    };

class OSNMAProcessor
{
public:
    explicit OSNMAProcessor(const std::string& keys_dir);
    ~OSNMAProcessor();

    void feed_page(int32_t prn, const std::string& osnma_data, const std::string& nav_data);
    void get_verifications();
private:

    std::unordered_map<int32_t, PublicKeyInfo> public_keys;
    std::unordered_map<int32_t, uint8_t> prns_next_word_seq_ind;
    std::unordered_map<int32_t, OSNMASubframe*> prn_current_subframe_map;
    std::unordered_map<int32_t, OSNMA_DSMMessage*> dsm_id_dsm_message_map;

    void reset_current_subframe(int32_t prn);
    void process_subframe(OSNMASubframe* subframe);
    void process_dsm_kroot_block(int32_t dsm_id, int32_t block_id, const std::string& dsm_kroot_block, const std::string& nma_header);

    void load_keys(const std::string& keys_dir);
    OSNMAXmlKeyInfo get_key_info_from_xml(const std::string& filename);
    void load_public_key_from_file(const OSNMAXmlKeyInfo& key_info);

};

#endif // GNSS_SDR_OSNMA_PROCESSOR_H