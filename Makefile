CC = gcc  
C++ = g++  
LINK = g++  

LIBS = -L/usr/local/lib -pthread -llog4cplus -lsrt

#must add -fPIC option  
CCFLAGS = $(COMPILER_FLAGS) -c -g  -fPIC -Wall
C++FLAGS = $(COMPILER_FLAGS) -c -g -fPIC -Wall -std=c++11
  
TARGET=srt_live_server
  
INCLUDES = -I. -I./sys -I./net -I./logging -I./rapidjson -I./rapidjson/include

C++FILES = ./srt_live_server.cpp ./srtserver/srt_server.cpp ./srtserver/srt_handle.cpp
CFILES = 

OBJFILE = $(CFILES:.c=.o) $(C++FILES:.cpp=.o)  

all:$(TARGET)  
  
$(TARGET): $(OBJFILE)  
	$(LINK) $^ -Wall $(LIBS) -o $@
 
%.o:%.c  
	$(CC) -o $@ $(CCFLAGS) $< $(INCLUDES)  
  
%.o:%.cpp  
	$(C++) -o $@ $(C++FLAGS) $< $(INCLUDES)  
  
  
clean:  
	rm -rf $(TARGET)  
	rm -rf $(OBJFILE)  


