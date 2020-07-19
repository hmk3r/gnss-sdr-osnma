/*!
 * \file file_configuration.cc
 * \brief Implementation of the interface ConfigurationInterface that reads the
 * configuration from a file.
 * \author Carlos Aviles, 2010. carlos.avilesr(at)googlemail.com
 *
 * This implementation has a text file as the source for the values of the parameters.
 * The file is in the INI format, containing sections and pairs of names and values.
 * For more information about the INI format, see https://en.wikipedia.org/wiki/INI_file
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * -------------------------------------------------------------------------
 */

#include "file_configuration.h"
#include "gnss_sdr_make_unique.h"
#include <glog/logging.h>
#include <utility>


FileConfiguration::FileConfiguration(std::string filename)
{
    filename_ = std::move(filename);
    init();
}


FileConfiguration::FileConfiguration()
{
    filename_ = "./default_config_file.txt";
    init();
}


void FileConfiguration::init()
{
    converter_ = std::make_unique<StringConverter>();
    overrided_ = std::make_unique<InMemoryConfiguration>();
    ini_reader_ = std::make_unique<INIReader>(filename_);
    error_ = ini_reader_->ParseError();
    if (error_ == 0)
        {
            DLOG(INFO) << "Configuration file " << filename_ << " opened with no errors";
        }
    else if (error_ > 0)
        {
            LOG(WARNING) << "Configuration file " << filename_ << " contains errors in line " << error_;
        }
    else
        {
            LOG(WARNING) << "Unable to open configuration file " << filename_;
        }
}


std::string FileConfiguration::property(std::string property_name, std::string default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    return ini_reader_->Get("GNSS-SDR", property_name, default_value);
}


bool FileConfiguration::property(std::string property_name, bool default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


int64_t FileConfiguration::property(std::string property_name, int64_t default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


uint64_t FileConfiguration::property(std::string property_name, uint64_t default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


int FileConfiguration::property(std::string property_name, int default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


unsigned int FileConfiguration::property(std::string property_name, unsigned int default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


uint16_t FileConfiguration::property(std::string property_name, uint16_t default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


int16_t FileConfiguration::property(std::string property_name, int16_t default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


float FileConfiguration::property(std::string property_name, float default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


double FileConfiguration::property(std::string property_name, double default_value) const
{
    if (overrided_->is_present(property_name))
        {
            return overrided_->property(property_name, default_value);
        }
    const std::string empty;
    return converter_->convert(property(property_name, empty), default_value);
}


void FileConfiguration::set_property(std::string property_name, std::string value)
{
    overrided_->set_property(property_name, value);
}


bool FileConfiguration::is_present(const std::string& property_name) const
{
    return (overrided_->is_present(property_name));
}
