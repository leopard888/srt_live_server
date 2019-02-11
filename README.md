# srt_live_server
live server based on srt protocal<br/>
srt_live_server是基于SRT传输协议的直播服务，支持mpegts格式的推流，拉流。<br/>
当前ffmpeg4.1版本以上已经支持srt协议，可以用ffmpeg/ffplay进行测试验证。

## 1. 编译简介
### 1.1 编译libsrt库
编译方法在srt源代码的readme.md中有详细介绍：[srt_code_in_github](https://github.com/Haivision/srt)

### 1.2 编译log4plus
工程项目的日志，使用的是跨平台的log4plus，所以需要编译。<br/>
下载地址：[log4cplus](https://sourceforge.net/projects/log4cplus/)

### 1.3 C++11环境
工程项目使用C++11开发，所以需要安装C++11以上的版本来进行编译。

## 2. srt_live_server
### 2.1 启动方式
<pre>
./srt_live_server
</pre>
启动后，开始监听srt推流端口号，和拉流端口号：<br/>
* 推流端口号: 9090
* 拉流端口号: 9091

<b>固定规则：拉流端口号=推流端口号+1</b>


## 3. 使用简介
### 3.1 客户端请使用ffmpeg/ffplay
* ffmpeg编译时，请加入选项--enable-libsrt
* ffplay编译前需要有libsdl2.0，请下载，并编译[SDL2.0](http://www.libsdl.org/release/SDL2-2.0.9.tar.gz)
* ffplay编译时，请加入--enable-sdl

### 3.2 推流
输入命令行: <br/>
<pre>
ffmpeg -re -i 100.flv -c copy -f mpegts srt://127.0.0.1:9090?streamid=100
</pre>
说明：将本地100.flv的视频文件，不编码，按照时间戳，发送到srt服务器9090端口，streamid为100.

### 3.3 拉流
输入命令行: <br/>
<pre>
ffplay srt://127.0.0.1:9091?streamid=100
</pre>
可以观看对应的streamid=100的视频<br/>
注意：端口号是9091，拉流端口号=推流端口号+1

## 4. Roadmap
### 4.1 还未支持gop缓存，2019.02.30前上线。

## 5. 相关连接
srt协议文档翻译：[srt_protocal](https://github.com/runner365/read_book/blob/master/SRT/srt_protocol.md)
