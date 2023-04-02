#define NOMINMAX
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
#include <memory>
#include <chrono>
#include <functional>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#ifdef _LOG
#define Log( format, ... ) printf( "[ DEBUG ] " format "\n", ##__VA_ARGS__ )
#else:
#define Log( format, ... ) void(0);
#endif 

#ifndef NOMINMAX
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#endif



