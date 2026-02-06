#ifndef GLOBALCUTPOINTS_H_
#define GLOBALCUTPOINTS_H_
#include <vector>
#include<mutex>
#include<inttypes.h>

#if defined(__SIZEOF_INT128__)
	typedef __uint128_t uint128_t;
#else
	#error "Compiler does not support 128-bit integers." // define uint128_t as needed
#endif

extern std::vector<std::vector<uint128_t>> globalcutpoints;
extern std::mutex globalcutpoints_mutex;
#endif
