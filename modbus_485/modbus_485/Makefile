#CROSS = 
CROSS = aarch64-linux-gnu-
CC = $(CROSS)gcc
CXX = $(CROSS)g++
CFLAGS = -Wall -O2 -static

SRCS = rtu_master.cpp

# HEADER_PATH = -I./include/
HEADER_PATH = -I ./include/ -I /home/test/yyz/CV183x_v1.4.0_SDK/cv183x140/middleware/sample/aibox/3rd/
LIB_PATH = -L./lib/

LIBS = -lmodbus

TARGET = master

$(TARGET): $(SRCS)
	$(CXX) $(CFLAGS) -o master $(SRCS) $(LIB_PATH) $(LIBS) $(HEADER_PATH)
	@echo "compile \e[1;32m success\e[0m, target: $(TARGET)"