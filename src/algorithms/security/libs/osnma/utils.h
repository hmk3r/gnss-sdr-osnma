// TODO: docstrings
#ifndef GNSS_SDR_OSNMA_UTILS_H
#define GNSS_SDR_OSNMA_UTILS_H

#include <string>
#include <cstdint>
#include <vector>

int32_t read_bin_string_as_int(const std::string& string, int32_t start_i = 0, int32_t length = -1);
std::vector<uint8_t> bin_string_to_byte_array(const std::string& string);
std::string byte_array_to_bin_str(const std::vector<uint8_t>& array);

#endif // GNSS_SDR_OSNMA_UTILS_H
