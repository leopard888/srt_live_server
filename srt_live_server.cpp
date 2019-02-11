#include "srtserver/srt_server.h"
#include "logging/logger.h"

#include <string.h>
#include <memory>
#include <chrono>

const int SRT_PORT = 9090;

using namespace srt_media;

int main(int argn, char** argv) {
    srt_startup();

    InitLog("./conf/log.properties");
    std::shared_ptr<srt_server> srt_server_ptr = std::make_shared<srt_server>(SRT_PORT);
    int ret = srt_server_ptr->init();
    if (ret != 0) {
        return -1;
    }

    ret = srt_server_ptr->run();
    if (ret != 0) {
        return -1;
    }

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    srt_cleanup();
}