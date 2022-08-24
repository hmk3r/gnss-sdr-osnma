/*!
* \file galileo_osnma_worker.h
* \brief Interface of the OSNMA worker block
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


#ifndef GNSS_SDR_GALILEO_OSNMA_WORKER_H
#define GNSS_SDR_GALILEO_OSNMA_WORKER_H

#include "gnss_block_interface.h"
#include "galileo_osnma.h"
#include "osnma_processor.h"
#include <boost/circular_buffer.hpp>  // for boost::circular_buffer
#include <gnuradio/block.h>           // for block
#include <gnuradio/types.h>           // for gr_vector_int
#include <cstddef>                    // for size_t
#include <cstdint>                    // for int32_t
#include <fstream>                    // for std::ofstream
#include <map>                        // for std::map
#include <memory>                     // for std::shared, std:unique_ptr
#include <queue>
#include <string>    // for std::string
#include <typeinfo>  // for typeid
#include <vector>    // for std::vector

/** \addtogroup Security
* \{ */
/** \addtogroup Security_gnuradio_blocks security_gr_blocks
* GNU Radio blocks for enabling GNSS security mechanisms
* \{ */


class galileo_osnma_worker;

using galileo_osnma_worker_sptr = gnss_shared_ptr<galileo_osnma_worker>;

galileo_osnma_worker_sptr galileo_osnma_worker_make(const std::string& keys_dir);

/*!
* \brief This class implements a block that processes Galileo OSNMA data (verifies nav data)
*/
class galileo_osnma_worker : public gr::block
{
public:
   ~galileo_osnma_worker();

private:
   friend galileo_osnma_worker_sptr galileo_osnma_worker_make(const std::string& keys_dir);

   explicit galileo_osnma_worker(const std::string& keys_dir);

   const size_t d_double_type_hash_code = typeid(double).hash_code();
   const size_t d_int_type_hash_code = typeid(int).hash_code();

   void msg_handler_telemetry_to_OSNMA(const pmt::pmt_t& msg);

   std::unique_ptr<OSNMAProcessor> osnma_processor_ptr;
};

/** \} */
/** \} */
#endif  // GNSS_SDR_GALILEO_OSNMA_WORKER_H
