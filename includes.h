#ifndef __INCLUDES_H_
#define __INCLUDES_H_

#include <set>
#include <vector>
#include <fstream>
#include <queue>
#include <tuple>
#include <assert.h>
#include <functional>
#include <time.h>
#include <cmath>
#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <random>
#include <chrono>
#define INF 3 * std::numeric_limits<float>::max()
#define endl "\n"
#define T(t) (t - int(t)) + int(t) % 1440
#define random (float)rand() / RAND_MAX;
#define rnd  distribution(generator)
#define customer std::array<int,2>
#define customer_data std::tuple<customer, float, int, int, float, float>
#define tandem_data std::tuple<customer, float, int,std::vector<std::tuple<int,int,int>>, float, float>
#define graph_data std::tuple<customer, float, int, float, float>
#endif