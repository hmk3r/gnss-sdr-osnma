// TODO: docstrings
#include "osnma_processor.h"
#include "utils.h"

#include <pugixml.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <cmath>


namespace fs = boost::filesystem;

OSNMAProcessor::OSNMAProcessor(const std::string &keys_dir)
{
    load_keys(keys_dir);
    for (const auto& x: public_keys)
        {
            std::cout << "[OSNMA Lib] Loaded key with ID " << x.first << " of type " << x.second.algorithm << "\n";
        }
}

void OSNMAProcessor::load_keys(const std::string &keys_dir)
{
    if (!fs::exists(keys_dir) || !fs::is_directory(keys_dir))
        {
            std::cout << "[OSNMA Lib] Error while opening OSNMA keys directory: does not exist/not a directory\n";
            return;
        }


    for(auto const& entry: fs::directory_iterator(keys_dir))
        {
            if (!fs::is_regular_file(entry) || entry.path().extension() != ".xml" )
                {
                    continue;
                }

            OSNMAXmlKeyInfo key_info = get_key_info_from_xml(entry.path().string());
            if(key_info.type == public_key)
                {
                    key_info.filename = entry.path().parent_path().append(key_info.filename).string();
                    load_public_key_from_file(key_info);
                }
            else if (key_info.type == merkle_tree)
                {
                    // TODO: Load merkle tree
                }
        }
}

OSNMAXmlKeyInfo OSNMAProcessor::get_key_info_from_xml(const std::string &filename)
{

    pugi::xml_document doc;
    pugi::xml_parse_result xml_tree = doc.load_file(filename.c_str());

    OSNMAXmlKeyInfo key_info;

    if (!xml_tree)
        {
            std::cout << "[OSNMA Lib] Error while parsing xml file " << filename << "\n";
            std::cout << "[OSNMA Lib] Error description: " << xml_tree.description() << "\n";
            std::cout << "[OSNMA Lib] Error offset: " << xml_tree.offset << "]\n";
            key_info.type = error;
            return key_info;
        }

    pugi::xml_node key_node = doc.child("signalData").child("body").first_child();
    std::string key_node_type = key_node.name();
    if (key_node_type == "MerkleTree")
        {
            // TODO: Merkle Tree XML parsing
            key_info.type = merkle_tree;
        }
    else if (key_node_type == "PublicKey")
        {
            std::cout << key_node_type << "\n";
            key_info.type = public_key;
            key_info.id = key_node.child("PKID").text().as_int();
            key_info.filename = key_node.child("File").text().as_string();
            key_info.algorithm = key_node.child("PKType").text().as_string();
        }
    else
        {
            key_info.type = error;
        }

    return key_info;
}

void OSNMAProcessor::load_public_key_from_file(const OSNMAXmlKeyInfo &key_info)
{
    int err;
    gnutls_datum_t key_pem;
    gnutls_pubkey_t key;

    err = gnutls_load_file(key_info.filename.c_str(), &key_pem);
    if(err)
        {
            std::cout << "[OSNMA Lib] Error while loading OSNMA public key from file " << key_info.filename << "\n";
            return;
        }

    gnutls_pubkey_init(&key);

    err = gnutls_pubkey_import(key, &key_pem, GNUTLS_X509_FMT_PEM);
    if(err)
        {
            gnutls_pubkey_deinit(key);
            std::cout << "[OSNMA Lib] Error while loading OSNMA public key from file " << key_info.filename << "\n";
            return;
        }

    gnutls_sign_algorithm_t sign_algorithm;

    if (key_info.algorithm == "ECDSA P-256/SHA-256")
        {
            sign_algorithm = GNUTLS_SIGN_ECDSA_SHA256;
        }
    else if (key_info.algorithm == "ECDSA P-521/SHA-512")
        {
            sign_algorithm = GNUTLS_SIGN_ECDSA_SHA512;
        }

    PublicKeyInfo public_key_info;
    public_key_info.key = key;
    public_key_info.algorithm = sign_algorithm;

    public_keys[key_info.id] = public_key_info;
}

void OSNMAProcessor::reset_current_subframe(int32_t prn)
{
    prns_next_word_seq_ind[prn] = 0;
    if (prn_current_subframe_map.find(prn) != prn_current_subframe_map.end())
        {
            if(prn_current_subframe_map[prn] != nullptr)
                {
                    delete prn_current_subframe_map[prn];
                    prn_current_subframe_map[prn] = nullptr;
                }
        }

    prn_current_subframe_map[prn] = new OSNMASubframe();
}

void OSNMAProcessor::process_subframe(OSNMASubframe* subframe)
{
    std::string hkroot = subframe->get_hkroot_message();
    int32_t nmas = read_bin_string_as_int(hkroot, LIBOSNMA_NMAS_OFFSET, LIBOSNMA_NMAS_LENGTH);
    // TODO: store nav data

    switch (nmas)
        {
        case 0: // "Reserved/Not in use"
            return;
        case 1: // "Test"
            break;
        case 2: // "Operational"
            break;
        case 3: // "Don't use"
            return;
        default:
            return;
        }

    int32_t cid = read_bin_string_as_int(hkroot, LIBOSNMA_CID_OFFSET, LIBOSNMA_CID_OFFSET);
    int32_t cpks = read_bin_string_as_int(hkroot, LIBOSNMA_CPKS_OFFSET, LIBOSNMA_CPKS_LENGTH);
    int32_t dsm_id = read_bin_string_as_int(hkroot, LIBOSNMA_DSM_ID_OFFSET, LIBOSNMA_DSM_ID_LENGTH);
    int32_t dsm_block_id = read_bin_string_as_int(hkroot, LIBOSNMA_DSM_BLOCK_ID_OFFSET, LIBOSNMA_DSM_BLOCK_ID_LENGTH);
    std::string nma_header = hkroot.substr(LIBOSNMA_NMA_HEADER_OFFSET, LIBOSNMA_NMA_HEADER_LENGTH);
    std::string dsm_block = hkroot.substr(LIBOSNMA_DSM_BLOCK_OFFSET, LIBOSNMA_DSM_BLOCK_LENGTH);

    if (dsm_id >= LIBOSNMA_DSM_ID_PKR_START_ID)
        {
            // TODO: DSM-PKR implementation
        }
    else // DSM-KROOT
        {
            process_dsm_kroot_block(dsm_id, dsm_block_id, dsm_block, nma_header);
        }

}

void OSNMAProcessor::process_dsm_kroot_block(int32_t dsm_id, int32_t block_id, const std::string& dsm_kroot_block, const std::string& nma_header)
{
    if (dsm_id_dsm_message_map.find(dsm_id) == dsm_id_dsm_message_map.end())
        {
            dsm_id_dsm_message_map[dsm_id] = new OSNMA_DSMMessage();
        }

    auto *dsm_message = dsm_id_dsm_message_map[dsm_id];

    if (dsm_message->is_processed)
        {
            return;
        }

    // Ideally nma_header should be assigned based on a consensus algorithm, as to try to prevent DoS attacks
    dsm_message->nma_header = nma_header;
    dsm_message->id_block_map[block_id] = dsm_kroot_block;

    if (block_id == 0)
        {
            int32_t nb_blocks_table_index = read_bin_string_as_int(dsm_kroot_block, LIBOSNMA_DSM_KROOT_NB_BLOCKS_OFFSET, LIBOSNMA_DSM_KROOT_NB_BLOCKS_LENGTH);
            dsm_message->nb_blocks = DSM_KROOT_NB_BLOCKS_TABLE.at(nb_blocks_table_index);

        }

    bool is_message_ready = static_cast<int32_t>(dsm_message->id_block_map.size()) == dsm_message->nb_blocks && dsm_message->nb_blocks != 0;

    if (!is_message_ready)
        {
            // TODO: delete if dsm block is more than x minutes old
            return;
        }

    std::string dsm_kroot_message;
    dsm_kroot_message.reserve(dsm_message->nb_blocks * LIBOSNMA_DSM_BLOCK_LENGTH);

    for (int32_t i = 0; i < dsm_message->nb_blocks; ++i)
        {
            dsm_kroot_message += dsm_message->id_block_map[i];
        }

    int32_t nb_blocks_table_index = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_NB_BLOCKS_OFFSET, LIBOSNMA_DSM_KROOT_NB_BLOCKS_LENGTH);
    int32_t pkid = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_PKID_OFFSET, LIBOSNMA_DSM_KROOT_PKID_LENGTH);
    int32_t cidkr = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_CIDKR_OFFSET, LIBOSNMA_DSM_KROOT_CIDKR_LENGTH);
//    std::string reserved_1 = dsm_kroot_message.substr(LIBOSNMA_DSM_KROOT_RESERVED_1_OFFSET, LIBOSNMA_DSM_KROOT_RESERVED_1_LENGTH);
    int32_t hf = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_HF_OFFSET, LIBOSNMA_DSM_KROOT_HF_LENGTH);
    int32_t mf = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_MF_OFFSET, LIBOSNMA_DSM_KROOT_MF_LENGTH);
    int32_t ks_table_index = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_KS_OFFSET, LIBOSNMA_DSM_KROOT_KS_LENGTH);
    int32_t ts_table_index = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_TS_OFFSET, LIBOSNMA_DSM_KROOT_TS_LENGTH);
    int32_t maclt = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_MACLT_OFFSET, LIBOSNMA_DSM_KROOT_MACLT_LENGTH);
//    std::string reserved = dsm_kroot_message.substr(LIBOSNMA_DSM_KROOT_RESERVED_OFFSET, LIBOSNMA_DSM_KROOT_RESERVED_LENGTH);
    int32_t wnk = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_WNK_OFFSET, LIBOSNMA_DSM_KROOT_WNK_LENGTH);
    int32_t towhk = read_bin_string_as_int(dsm_kroot_message, LIBOSNMA_DSM_KROOT_TOWHK_OFFSET, LIBOSNMA_DSM_KROOT_TOWHK_LENGTH);
    std::string alpha = dsm_kroot_message.substr(LIBOSNMA_DSM_KROOT_ALPHA_OFFSET, LIBOSNMA_DSM_KROOT_ALPHA_LENGTH);

    int32_t ks = DSM_KROOT_KS_TABLE.at(ks_table_index);

    if (public_keys.find(pkid) == public_keys.end())
        {
            std::cout << "[OSNMA Lib] No public key with ID " << pkid << "\n";
            return;
        }

    auto public_key_info = public_keys.at(pkid);
    int32_t l_ds;

    if (public_key_info.algorithm == GNUTLS_SIGN_ECDSA_SHA256)
        {
            l_ds = 512;
        }
    else if (public_key_info.algorithm == GNUTLS_SIGN_ECDSA_SHA512)
        {
            l_ds = 1056;
        }
    else {
            return;
        }

    double sig_and_key_length = ks + l_ds;
    int32_t l_dk = LIBOSNMA_DSM_BLOCK_LENGTH * ceil(1 + sig_and_key_length / LIBOSNMA_DSM_BLOCK_LENGTH);
    int32_t pad_length = l_dk - LIBOSNMA_DSM_BLOCK_LENGTH - ks - l_ds;

    std::string root_key_binstr = dsm_kroot_message.substr(LIBOSNMA_DSM_BLOCK_LENGTH, ks);
    std::string sig_binstr = dsm_kroot_message.substr(LIBOSNMA_DSM_BLOCK_LENGTH + ks, l_ds);
    std::string pad_binstr = dsm_kroot_message.substr(LIBOSNMA_DSM_BLOCK_LENGTH + ks + l_ds, pad_length);

    std::cout << "[OSNMA Lib] New DSM-KROOT for chain ID " << cidkr << " received\n";
    std::cout << "[OSNMA Lib] Root key: " << root_key_binstr << "\n";
    std::cout << "[OSNMA Lib] Signature: " << sig_binstr << "\n";
    std::cout << "[OSNMA Lib] Pad: " << pad_binstr << "\n";

    // message to sign
    // NMA Header || CIDKR || ... || KROOT
    std::string m = dsm_message->nma_header; // NMA header
    m += dsm_kroot_message.substr(LIBOSNMA_DSM_KROOT_CIDKR_OFFSET, LIBOSNMA_DSM_BLOCK_CIDKR_TO_KROOT_LENGTH); // Chain info
    m += root_key_binstr; // KROOT

    std::cout << "[OSNMA Lib] M: " + m + "\n";

    std::vector<uint8_t> m_bytes = bin_string_to_byte_array(m);
    std::vector<uint8_t> sig_bytes = bin_string_to_byte_array(sig_binstr);

    // Pad check
    std::vector<uint8_t> t_bytes = bin_string_to_byte_array(m + sig_binstr);
    std::vector<uint8_t> t_digest;
    t_digest.resize(32);
    gnutls_hash_fast(GNUTLS_DIG_SHA256, t_bytes.data(), t_bytes.size(), t_digest.data());

    std::string t_digest_trunc = byte_array_to_bin_str(t_digest).substr(0, pad_length);

    if (t_digest_trunc == pad_binstr)
        {
            std::cout << "[OSNMA Lib] DSM-KROOT Pad matches with the computed value\n";
        }
    else
        {
            std::cout << "[OSNMA Lib] DSM-KROOT Pad does NOT match with the computed value\n";
            std::cout << "[OSNMA Lib] Pad computed: " << t_digest_trunc << "\n";
            std::cout << "[OSNMA Lib] Actual pad: " << pad_binstr << "\n";
            return;
        }

    gnutls_datum_t message;
    gnutls_datum_t signature_r;
    gnutls_datum_t signature_s;
    gnutls_datum_t signature;

    message.data = m_bytes.data();
    message.size = m_bytes.size();

    signature_r.data = sig_bytes.data();
    signature_r.size = sig_bytes.size() / 2;

    signature_s.data = signature_r.data + signature_r.size;
    signature_s.size = signature_r.size;

    int err;
    err = gnutls_encode_rs_value(&signature, &signature_r, &signature_s);

    if (err)
        {
            std::cout << "[OSNMA Lib] Error while encoding signature\n";
            return;
        }

    err = gnutls_pubkey_verify_data2(public_key_info.key, public_key_info.algorithm, 0, &message, &signature);


    if (err < 0)
        {
            if (err == GNUTLS_E_PK_SIG_VERIFY_FAILED)
                {
                    std::cout << "[OSNMA Lib] Invalid KROOT signature\n";
                }
            return;
        }

    std::cout << "[OSNMA Lib] KROOT Signature verified - CID: " << cidkr << " WNK: " << wnk << " TOWHK: " << towhk << "\n";
    dsm_message->is_processed = true;
}

void OSNMAProcessor::get_verifications()
{

}

void OSNMAProcessor::feed_page(int32_t prn, const std::string &osnma_data, const std::string &nav_data)
{
    if (osnma_data.size() != LIBOSNMA_OSNMA_DATA_LENGTH || nav_data.size() != LIBOSNMA_NAV_DATA_LENGTH)
        {
            reset_current_subframe(prn);
            return;
        }

    if (prns_next_word_seq_ind.find(prn) == prns_next_word_seq_ind.end())
        {
            reset_current_subframe(prn);
        }

    int32_t word_type = read_bin_string_as_int(nav_data, LIBOSNMA_WORD_TYPE_OFFSET, LIBOSNMA_WORD_TYPE_LENGTH);

    int32_t next_word_seq_ind = prns_next_word_seq_ind[prn];
    const auto& expected_word_types = GALILEO_E1_INAV_WORD_SEQ[next_word_seq_ind];

    bool word_sequence_valid = expected_word_types.find(word_type) != expected_word_types.end();


    if (!word_sequence_valid)
        {
            reset_current_subframe(prn);
            return;
        }

    auto *const subframe = prn_current_subframe_map[prn];

    next_word_seq_ind = (next_word_seq_ind + 1) % GALILEO_E1_INAV_WORD_SEQ.size();

    prns_next_word_seq_ind[prn] = next_word_seq_ind;

    bool subframe_ready = subframe->add_page(word_type, osnma_data, nav_data);

    if (subframe_ready)
        {
            process_subframe(subframe);
            reset_current_subframe(prn);
        }
}

OSNMAProcessor::~OSNMAProcessor()
{
    for (auto const& pair: prn_current_subframe_map)
        {
            if (pair.second == nullptr)
                {
                    continue;
                }
            delete pair.second;
        }

    for (auto const& pair: dsm_id_dsm_message_map)
        {
            if (pair.second == nullptr)
                {
                    continue;
                }
            delete pair.second;
        }

    for (auto const& pair: public_keys)
        {
            gnutls_pubkey_deinit(pair.second.key);
        }
}

