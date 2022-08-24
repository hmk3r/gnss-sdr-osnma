// TODO: docstring
#include "utils.h"

#include <cstdint>
#include <bitset>

int32_t read_bin_string_as_int(const std::string& string, int32_t start_i, int32_t length)
{
    if (length < 0)
        {
            length = string.size() - start_i;
        }
    return std::stoi(string.substr(start_i, length), nullptr, 2);
}

std::vector<uint8_t> bin_string_to_byte_array(const std::string& string)
{
    std::vector<uint8_t> bytes;

    uint8_t padding = (8 - (string.size() % 8)) % 8;
    std::string padded_string = string;

    padded_string.insert(padded_string.end(), padding, '0');
    size_t n_bytes = padded_string.size() / 8;

    for (size_t i = 0; i < n_bytes; ++i)
        {
            std::string byte_bin_str = padded_string.substr(i * 8, 8);
            uint8_t tmp = std::bitset<8>(byte_bin_str).to_ulong() & 0xFF;
            bytes.push_back(tmp);
        }

    return bytes;
}

std::string byte_array_to_bin_str(const std::vector<uint8_t>& byte_array)
{
    std::string bin_str;
    bin_str.reserve(byte_array.size() * 8);

    for (auto const& b: byte_array)
        {
            bin_str += std::bitset<8>(b).to_string();
        }

    return bin_str;
}