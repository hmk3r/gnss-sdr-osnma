// TODO: docstrings

#include "osnma_subframe.h"

OSNMASubframe::OSNMASubframe()
{
    hkroot_message.reserve(LIBOSNMA_HKROOT_MESSAGE_LENGTH);
    mack_message.reserve(LIBOSNMA_MACK_MESSAGE_LENGTH);
}

bool OSNMASubframe::add_page(int32_t word_type, const std::string &osnma_data, const std::string &nav_data)
{
    if (hkroot_message.size() >= LIBOSNMA_HKROOT_MESSAGE_LENGTH && mack_message.size() >= LIBOSNMA_MACK_MESSAGE_LENGTH)
        {
            return true;
        }

    std::string hkroot_field = osnma_data.substr(0, LIBOSNMA_HKROOT_FIELD_LENGTH);
    std::string mack_field = osnma_data.substr(LIBOSNMA_HKROOT_FIELD_LENGTH, LIBOSNMA_MACK_FIELD_LENGTH);

    hkroot_message += hkroot_field;
    mack_message += mack_field;
    nav_data_map[word_type] = nav_data;

    return hkroot_message.size() == LIBOSNMA_HKROOT_MESSAGE_LENGTH &&
           mack_message.size() == LIBOSNMA_MACK_MESSAGE_LENGTH;

}