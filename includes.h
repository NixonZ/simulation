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
#include <algorithm>
#include <iomanip>
#define float double
#define precision 7
#define INF 3 * std::numeric_limits<float>::max()
#define endl "\n"
#define TimeReset(t) (t - int(t)) + int(t) % 1440
#define random (float)rand() / RAND_MAX;
#define rnd  distribution(generator)
#define customer std::array<int,2> /*!< Customer data type 0 - Priority, 1- Customer ID*/
#define customer_data std::tuple<customer, float, int, int, float, float>
#define tandem_data std::tuple<customer, float, int,std::vector<std::tuple<int,int,int>>, float, float>
#define graph_data std::tuple<customer, float, int, float, float>
using C_type = std::function<int(float t)>; /*!< Function type float -> int*/
using event_type = std::function<float(float t)>; /*!< Function type float -> float*/
using priority_type = std::function<int()>; /*!< Function type void -> int*/
using event_type_list = std::vector<event_type>; /*!< Vector of Function type float -> float*/
#endif