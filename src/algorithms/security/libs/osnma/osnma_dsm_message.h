#ifndef GNSS_SDR_OSNMA_DSM_MESSAGE_H
#define GNSS_SDR_OSNMA_DSM_MESSAGE_H

#include <unordered_map>
#include <string>


class OSNMA_DSMMessage
{
public:
    OSNMA_DSMMessage() = default;

    int32_t nb_blocks;
    std::string nma_header;
    std::unordered_map<int32_t, std::string> id_block_map;
    bool is_processed;
};
#endif // GNSS_SDR_OSNMA_DSM_MESSAGE_H
