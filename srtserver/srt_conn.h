#ifndef SRT_CONN_H
#define SRT_CONN_H
#include "srt/srt.h"
#include <thread>
#include <memory>

namespace srt_media {
#define PULL_SRT_MODE 0x01
#define PUSH_SRT_MODE 0x02

class srt_conn {
public:
    srt_conn(SRTSOCKET conn_fd, int mode):_conn_fd(conn_fd),
        _mode(mode) {

    }

    ~srt_conn() {
        close();
    }

    void close() {
        if (_conn_fd == SRT_INVALID_SOCK) {
            return;
        }
        srt_close(_conn_fd);
        _conn_fd = SRT_INVALID_SOCK;
    }

    SRTSOCKET get_conn() {
        return _conn_fd;
    }
    int get_mode() {
        return _mode;
    }
private:
    SRTSOCKET _conn_fd;
    int _mode;
};
}

#endif //SRT_CONN_H