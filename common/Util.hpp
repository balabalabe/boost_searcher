#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace common {

class Util {
public:
  static bool read(const std::string& input, std::string* output) {
    std::ifstream file(input.c_str(), std::ios::binary);
    if (!file.is_open()) {
      return false;
    }
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    output->resize(length);
    file.read(const_cast<char*>(output->data()), length);
    file.close();
    return true;
  }

  static void split(const std::string& input, const std::string& delimiter, std::vector<std::string>* output) {
    boost::split(*output, input, boost::is_any_of(delimiter), boost::token_compress_off);
  }
};

} 

