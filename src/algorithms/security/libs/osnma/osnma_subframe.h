// TODO: Docstrings
/*!
* \file osnma_subframe.h
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

#ifndef GNSS_SDR_OSNMA_SUBFRAME_H
#define GNSS_SDR_OSNMA_SUBFRAME_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

#define LIBOSNMA_PAGES_IN_SUBFRAME 15
#define LIBOSNMA_HKROOT_FIELD_LENGTH 8
#define LIBOSNMA_MACK_FIELD_LENGTH 32
#define LIBOSNMA_HKROOT_MESSAGE_LENGTH LIBOSNMA_PAGES_IN_SUBFRAME * LIBOSNMA_HKROOT_FIELD_LENGTH
#define LIBOSNMA_MACK_MESSAGE_LENGTH LIBOSNMA_PAGES_IN_SUBFRAME * LIBOSNMA_MACK_FIELD_LENGTH

class OSNMASubframe
{
public:
    OSNMASubframe();

    bool add_page(int32_t word_type, const std::string& osnma_data, const std::string& nav_data);

    std::string get_hkroot_message()
    {
        return hkroot_message;
    };

    std::string get_mack_message()
    {
        return mack_message;
    }

    std::unordered_map<int32_t, std::string> get_nav_data_map()
    {
        return nav_data_map;
    }

private:
    std::string hkroot_message;
    std::string mack_message;
    std::unordered_map<int32_t, std::string> nav_data_map;
};
#endif // GNSS_SDR_OSNMA_SUBFRAME_H