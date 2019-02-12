#include "ts_gop_cache.h"

#include <assert.h>

namespace srt_media {

#define TS_PACKET_HEADER_SIZE 4

ts_gop_cache::ts_gop_cache() {
    memset(_pat_p, 0, MPEG_TS_SIZE);
    memset(_pmt_p, 0, MPEG_TS_SIZE);
}

ts_gop_cache::~ts_gop_cache() {

}

void ts_gop_cache::save_gop_cache(char* data_p, int data_len, std::string streamid) {
    if ((data_p == nullptr) || (data_p[0] != 0x47) || (data_len == 0)) {
        assert(0);
        return;
    }

    if ((data_len % MPEG_TS_SIZE) != 0) {
        assert(0);
        return;
    }
    
    for (int index = 0; index < (data_len / MPEG_TS_SIZE); index++) {
        char* ts_data_p = data_p + index * MPEG_TS_SIZE;
        
        if (ts_data_p[0] != 0x47) {
            assert(0);
            return;
        }

        //pid:xxxo oooo oooo oooo
        int pid = ((int(ts_data_p[1] & 0x1f)) << 8) | ts_data_p[2];

        if (pid == 0) {// PAT 
            char* payload_p = ts_data_p + TS_PACKET_HEADER_SIZE;
            if (payload_p[0] != 0x00) {
                assert(0);
                return;
            }
        }
    }
    return;
}

int ts_gop_cache::send_gop_cache(std::string streamid, SRTSOCKET dst_srtsocket) {
    int gop_total = 0;

    return gop_total;
}
}