#ifndef TS_GOP_CACHE_H
#define TS_GOP_CACHE_H
#include "mem/media_packet.h"
#include "srt/srt.h"
#include <list>
#include <unordered_map>
#include <string.h>

namespace srt_media {
#define MPEG_TS_SIZE 188

class ts_gop_cache {
public:
    ts_gop_cache();
    ~ts_gop_cache();

    void save_gop_cache(char* data_p, int data_len, std::string streamid);
    int send_gop_cache(std::string streamid, SRTSOCKET dst_srtsocket);

private:
    char _pat_p[MPEG_TS_SIZE];
    char _pmt_p[MPEG_TS_SIZE];
    std::unordered_map<SRTSOCKET, std::list<Media_Packet>> _packet_map;
};

}
#endif //TS_GOP_CACHE_H