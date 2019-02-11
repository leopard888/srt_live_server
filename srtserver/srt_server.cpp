#include "srt_server.h"
#include "srt_conn.h"
#include "net/netpub.h"
#include "logging/logger.h"

namespace srt_media {

srt_server::srt_server(int port):_port(port),
    _rcv_server_socket(-1),
    _snd_server_socket(-1),
    _pollid(-1) {

}

srt_server::~srt_server() {

}

int srt_server::init() {
    if ((_rcv_server_socket != -1) || (_snd_server_socket != -1)) {
        return -1;
    }

    _rcv_server_socket = srt_create_socket();
    sockaddr_in sa = create_addr_inet("", _port);
    sockaddr* psa = (sockaddr*)&sa;

    int ret = srt_bind(_rcv_server_socket, psa, sizeof(sa));
    if ( ret == SRT_ERROR )
    {
        srt_close(_rcv_server_socket);
        ErrorLogf("srt bind error: %d", ret);
        return -1;
    }

    ret = srt_listen(_rcv_server_socket, 5);
    if (ret == SRT_ERROR)
    {
        srt_close(_rcv_server_socket);
        ErrorLogf("srt listen error: %d", ret);
        return -2;
    }

    _snd_server_socket = srt_create_socket();
    sa = create_addr_inet("", _port+1);
    psa = (sockaddr*)&sa;

    ret = srt_bind(_snd_server_socket, psa, sizeof(sa));
    if ( ret == SRT_ERROR )
    {
        srt_close(_snd_server_socket);
        ErrorLogf("srt bind error: %d", ret);
        return -1;
    }

    ret = srt_listen(_snd_server_socket, 5);
    if (ret == SRT_ERROR)
    {
        srt_close(_snd_server_socket);
        ErrorLogf("srt listen error: %d", ret);
        return -2;
    }

    InfoLogf("srt server listen rcv_port=%d, snd_port=%d, rcv_server_fd=%d, snd_server_fd=%d", 
        _port, _port+1, _rcv_server_socket, _snd_server_socket);
    
    _srt_handle_ptr = std::make_shared<srt_handle>();
    ret = _srt_handle_ptr->start();
    if (ret != 0) {
        srt_close(_rcv_server_socket);
        srt_close(_snd_server_socket);
        ErrorLogf("srt handle start error: %d", ret);
        return -3;
    }

    return 0;
}

int srt_server::run() {
    _pollid = srt_epoll_create();
    if (_pollid < -1) {
        ErrorLogf("srt server srt_epoll_create error, port=%d", _port);
        return -1;
    }
    int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
    int ret = srt_epoll_add_usock(_pollid, _rcv_server_socket, &events);
    if (ret < 0) {
        ErrorLogf("srt server run add epoll error:%d", ret);
        return ret;
    }

    ret = srt_epoll_add_usock(_pollid, _snd_server_socket, &events);
    if (ret < 0) {
        ErrorLogf("srt server run add epoll error:%d", ret);
        return ret;
    }
    InfoLogf("SRT server is running rcv port=%d, send port=%d", _port, _port+1);
    _server_thread = std::make_shared<std::thread>(&srt_server::on_work, this);
    _server_thread->join();
    return 0;
}

SRTSOCKET srt_server::accept_connection(SRTSOCKET server_srt_socket) {
    sockaddr_in scl;
    int sclen = sizeof(scl);

    SRTSOCKET conn = srt_accept(server_srt_socket, (sockaddr*)&scl, &sclen);
    
    return conn;
}

void srt_server::on_work() {
    InfoLogf("srt server is running...port=%d", _port);
    while(true) {
        SRTSOCKET read_fds = -1;
        SRTSOCKET write_fds = -1;
        int rfd_num = 1;
        int wfd_num = 1;
        int ret = srt_epoll_wait(_pollid, &read_fds, &rfd_num, &write_fds, &wfd_num, -1,
                        nullptr, nullptr, nullptr, nullptr);
        if (ret < 0) {
            InfoLogf("epoll is timeout, port=%d", _port);
            continue;
        }
        
        SRT_SOCKSTATUS status;

        if (rfd_num > 0) {
            status = srt_getsockstate(read_fds);
        } else if (wfd_num > 0) {
            status = srt_getsockstate(write_fds);
        }
        InfoLogf("epoll get: ret=%d, rfd_num=%d, wfd_num=%d, status=%d", ret, rfd_num, wfd_num, status);
        switch(status) {
            case SRTS_LISTENING:
            {
                SRTSOCKET conn_fd = -1;
                
                if (rfd_num > 0) {
                    conn_fd = accept_connection(read_fds);
                    if (conn_fd == -1) {
                        continue;
                    }
                    if (read_fds == _rcv_server_socket) {
                        std::shared_ptr<srt_conn> srt_conn_ptr = std::make_shared<srt_conn>(conn_fd, PUSH_SRT_MODE);
                        InfoLogf("accept new read connection: socket=%d, read_fds=%d", conn_fd, read_fds);

                        int conn_event = SRT_EPOLL_IN |SRT_EPOLL_OUT| SRT_EPOLL_ERR;
                        _srt_handle_ptr->add_newconn(srt_conn_ptr, conn_event);
                    } else if (read_fds == _snd_server_socket) {
                        std::shared_ptr<srt_conn> srt_conn_ptr = std::make_shared<srt_conn>(conn_fd, PULL_SRT_MODE);
                        InfoLogf("accept new write connection: socket=%d, read_fds=%d", conn_fd, read_fds);

                        int conn_event = SRT_EPOLL_OUT | SRT_EPOLL_ERR;
                        _srt_handle_ptr->add_newconn(srt_conn_ptr, conn_event);
                    }

                } else if (wfd_num > 0) {
                    conn_fd = accept_connection(write_fds);
                    if (conn_fd == -1) {
                        continue;
                    }
                    if (write_fds == _rcv_server_socket) {
                        std::shared_ptr<srt_conn> srt_conn_ptr = std::make_shared<srt_conn>(conn_fd, PUSH_SRT_MODE);
                        InfoLogf("accept new read connection: socket=%d, write_fds=%d", conn_fd, write_fds);

                        int conn_event = SRT_EPOLL_IN |SRT_EPOLL_OUT| SRT_EPOLL_ERR;
                        _srt_handle_ptr->add_newconn(srt_conn_ptr, conn_event);
                    } else if (write_fds == _snd_server_socket) {
                        std::shared_ptr<srt_conn> srt_conn_ptr = std::make_shared<srt_conn>(conn_fd, PULL_SRT_MODE);
                        InfoLogf("accept new write connection: socket=%d, write_fds=%d", conn_fd, write_fds);

                        int conn_event = SRT_EPOLL_OUT | SRT_EPOLL_ERR;
                        _srt_handle_ptr->add_newconn(srt_conn_ptr, conn_event);
                    }
                }
                
                break;
            }
            case SRTS_CONNECTED:
            {
                InfoLogf("srt connected: socket=%d", read_fds);
                break;
            }
            case SRTS_BROKEN:
            {
                srt_epoll_remove_usock(_pollid, read_fds);
                srt_close(read_fds);
                WarnLogf("srt close: socket=%d", read_fds);
                break;
            }
            default:
                WarnLogf("unkown status:%d, read_fd=%d, write_fd=%d", status, read_fds, write_fds);
        }
    }
}

}