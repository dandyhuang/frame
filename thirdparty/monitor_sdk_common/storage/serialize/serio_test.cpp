#include <float.h>
#include <math.h>
#include <time.h>
#include <complex>
#include <limits>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include "thirdparty/monitor_sdk_common/storage/serialize/serio.h"

using namespace std;
using namespace common;

void SaveVector() {
  int i = 10;
  double d = 12;
  std::vector<int> vector = {1, 2, 3, 4};
  Serio::save("test-vector-data.bin", vector,i,d);
}

void LoadVector() {
  std::vector<int> vector;
  int i = 0;
  double d = 0;
  Serio::load("test-vector-data.bin", vector, i, d);
  std::cout << "i = " << i << ",d = " << d << std::endl;
  for (size_t j = 0; j < vector.size(); j++) {
    std::cout << "vec is " << vector[j] << std::endl;
  }
}

void SaveMap() {
  std::unordered_map<std::string, std::string> map;
  map.insert(std::pair<std::string, std::string>("apple", "aa"));
  map.insert(std::pair<std::string, std::string>("banana", "ab"));
  map.insert(std::pair<std::string, std::string>("pea", "ac"));
  Serio::save("test-map-data.bin", map);
}

void LoadMap() {
  std::unordered_map<std::string, std::string> map;
  Serio::load("test-map-data.bin", map);
  for (auto iter = map.begin(); iter != map.end(); ++iter) {
    std::cout << "map is " << iter->first << " , value = " << iter->second
              << std::endl;
  }
}

int main() {
  std::cout << "test vec :" << std::endl;
  SaveVector();
  LoadVector();
  std::cout << "test map :" << std::endl;
  SaveMap();
  LoadMap();
  return 0;
}
