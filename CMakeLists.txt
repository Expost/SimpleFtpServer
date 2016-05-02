cmake_minimum_required(VERSION 3.5)
project(SimpleFtpServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp CTransfer.cpp CTransfer.h CFtpTransfer.cpp CFtpTransfer.h Common.h CFtpCommand.cpp CFtpCommand.h CClient.cpp CClient.h CManageClient.cpp CManageClient.h json/json-forwards.h json/json.h jsoncpp.cpp Common.cpp CPasvDataTransfer.cpp CPasvDataTransfer.h CThread.cpp CThread.h)
add_executable(SimpleFtpServer ${SOURCE_FILES})

target_link_libraries(SimpleFtpServer -lpthread)