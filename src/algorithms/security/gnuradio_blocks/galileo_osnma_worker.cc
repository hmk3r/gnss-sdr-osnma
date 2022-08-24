/*!
* \file galileo_osnma_worker.h
* \brief Implementation of the OSNMA worker block
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

#include "galileo_osnma_worker.h"
#include "gnss_sdr_create_directory.h"
#include "gnss_sdr_filesystem.h"
#include "gnss_sdr_make_unique.h"
#include "galileo_osnma.h"
#include "concurrent_queue.h"
#include "display.h"
#include <glog/logging.h>
#include <gnuradio/io_signature.h>
#include <matio.h>
#include <pmt/pmt.h>
#include <algorithm>  // for std::min
#include <array>
#include <cmath>      // for round
#include <cstdlib>    // for size_t, llabs
#include <exception>  // for exception
#include <iostream>   // for cerr, cout
#include <limits>     // for numeric_limits
#include <utility>    // for move
#include <chrono>
#include <thread>

#if PMT_USES_BOOST_ANY
#include <boost/any.hpp>
namespace wht = boost;
#else
#include <any>
namespace wht = std;
#endif

#if HAS_GENERIC_LAMBDA
#else
#include <boost/bind/bind.hpp>
#endif


galileo_osnma_worker_sptr galileo_osnma_worker_make(const std::string& keys_dir) {
    return galileo_osnma_worker_sptr (new galileo_osnma_worker(keys_dir));
}



galileo_osnma_worker::galileo_osnma_worker(const std::string& keys_dir)
   : gr::block("galileo_osnma_worker",
         gr::io_signature::make(0, 0, 0),
         gr::io_signature::make(0, 0, 0))

{

   // PVT input message port
   this->message_port_register_in(pmt::mp("telemetry_to_OSNMA"));
   this->set_msg_handler(pmt::mp("telemetry_to_OSNMA"),
#if HAS_GENERIC_LAMBDA
       [this](auto &&PH1) { msg_handler_telemetry_to_OSNMA(PH1); });
#else
#if USE_BOOST_BIND_PLACEHOLDERS
       boost::bind(&galileo_osnma_worker::msg_handler_telemetry_to_OSNMA, this, boost::placeholders::_1));
#else
       boost::bind(&galileo_osnma_worker::msg_handler_telemetry_to_OSNMA, this, _1));
#endif
#endif

   // Send authentication messages to pvt
   // TODO: Use authentication messsages to determine if the current position is authentic
   this->message_port_register_out(pmt::mp("OSNMA_to_pvt"));

   osnma_processor_ptr = std::make_unique<OSNMAProcessor>(keys_dir);
}

void galileo_osnma_worker::msg_handler_telemetry_to_OSNMA(const pmt::pmt_t &msg)
{
    const auto gal_osnma = wht::any_cast<std::shared_ptr<Galileo_OSNMA>>(pmt::any_ref(msg));

    std::cout << TEXT_MAGENTA  << "msg_handler_telemetry_to_OSNMA: OSNMA from PRN " << gal_osnma->PRN << TEXT_RESET << "\n";
    osnma_processor_ptr->feed_page(gal_osnma->PRN, gal_osnma->OSNMA_data, gal_osnma->nav_data);
}

// TODO: when destructing, save keys to enable hot-start
galileo_osnma_worker::~galileo_osnma_worker() = default;

