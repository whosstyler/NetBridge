#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <utility>
#include <map>
#include <spdlog/spdlog.h>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#ifdef _LOG
#define Log( format, ... ) printf( "[ DEBUG ] " format "\n", ##__VA_ARGS__ )
#else:
#define Log( format, ... ) void(0);
#endif 


