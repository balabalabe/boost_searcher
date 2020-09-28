#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;
namespace common {

class Util {
public:
  static bool read(const string& input, string* output) {
    ifstream file(input_path.c_str());
    if(!file.is_open()){
       return false;
    }
    
    string line;
    while(getline(file,line)){
       *output += (line+"\n");
    }
    file.close();
    return true;
  }
  
  static void split(const string& input, const string& delimiter, vector<std::string>* output) {
    boost::split(*output, input, boost::is_any_of(delimiter), boost::token_compress_off);
  }
};

}

