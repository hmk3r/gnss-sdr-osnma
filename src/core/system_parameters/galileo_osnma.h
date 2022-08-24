/*!
* \file galileo_osnma.h
* \brief  Interface of a Galileo OSNMA message model storage
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


#ifndef GNSS_SDR_GALILEO_OSNMA_H
#define GNSS_SDR_GALILEO_OSNMA_H

#include <cstdint>
#include <string>

/** \addtogroup Core
* \{ */
/** \addtogroup System_Parameters
* \{ */


/*!
* \brief This class is a storage for data used in Galileo OSNMA as per
* Galileo Open Service Navigation Message Authentication (OSNMA) User ICD For The Test Phase v1.0
*
* See https://www.gsc-europa.eu/sites/default/files/sites/all/files/Galileo_OSNMA_User_ICD_for_Test_Phase_v1.0.pdf
*/
class Galileo_OSNMA
{
public:
   /*!
    * Default constructor
    */
    Galileo_OSNMA() = default;

    int32_t PRN;
    std::string OSNMA_data;
    std::string nav_data;
};


/** \} */
/** \} */
#endif  // GNSS_SDR_GALILEO_OSNMA_H