#ifndef SRT_SERVER_H
#define SRT_SERVER_H

#include "srt/srt.h"
#include "srt_handle.h"
#include "mem/media_packet.h"
#include "logging/logger.h"
#include <thread>
#include <memory>

namespace srt_media {
class srt_server {
public:
    srt_server(int port);
    ~srt_server();

    int init();
    int run();
private:
    void on_work();
    SRTSOCKET accept_connection(SRTSOCKET server_srt_socket);
private:
    int _port;
    SRTSOCKET _rcv_server_socket;
    SRTSOCKET _snd_server_socket;
    int _pollid;
    std::shared_ptr<std::thread> _server_thread;
    std::shared_ptr<srt_handle> _srt_handle_ptr;
};
}
#endif//SRT_SERVER_H