#include "globalcutpoints.h"
std::vector<std::vector<uint128_t>> globalcutpoints; // global variable to hold the cut points from all mappers
std::mutex globalcutpoints_mutex; // mutex to protect the global cut points vector