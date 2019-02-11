#ifndef MEDIA_PACKET_H
#define MEDIA_PACKET_H
#include <string.h>

namespace srt_media {
#define DATA_MAX_LEN (188*7)

class Media_Packet {
public:
    Media_Packet(char* pData, int iLen) {
        memcpy(_pData, pData, iLen);
        _size = iLen;
    }
    ~Media_Packet() {
        
    }

    char* get_data() {
        return _pData;
    }

    int get_size() {
        return _size;
    }
private:
    char _pData[DATA_MAX_LEN];
    int _size;
};
}
#endif//MEDIA_PACKET_H