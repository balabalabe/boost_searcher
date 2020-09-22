#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "../common/Util.hpp"

using namespace std;
string g_input_path = "../data/input";
string g_output_path = "../data/temp/raw_input";

struct DocInfo {
  string title;
  string url;
  string content;
};

bool enumFile(const string& input, vector<string>* output) {
  namespace fs = boost::filesystem;

  fs::path rootPath(input);
  if (!fs::exists(rootPath)) {
    cout << "input path not exists!" << endl;
    return false;
  }

  fs::recursive_directory_iterator endIter; 
  for (fs::recursive_directory_iterator it(rootPath); it != endIter; ++it) {
    if (!fs::is_regular_file(*it)) {
      continue;
    }
    if (it->path().extension() != ".html") {
      continue;
    }
    output->push_back(it->path().string());
  }

  return true;
}

bool parseTitle(const string& html, string* title) {
  size_t beg = html.find("<title>");
  if (beg == string::npos) {
    cout << "parseTitle failed! no beg!" << endl;
    return false;
  }
  size_t end = html.find("</title>");
  if (end == string::npos) {
    cout << "parseTitle failed! no end!" << endl;
    return false;
  }
  beg += string("<title>").size();
  *title = html.substr(beg, end - beg);
  return true;
}

bool parseUrl(const string& path, string* url) {
  string head = "https://www.boost.org/doc/libs/1_53_0/doc/";
  string tail = path.substr(g_input_path.size());
  *url = head + tail;
  return true;
}

bool parseContent(const string& html, string* content) {
  bool isContent = true;
  for (auto c : html) {
    if (isContent) {
      if (c == '<') {
        isContent = false;
      } else {
        if (c == '\n') {
          c = ' ';
        }
        content->push_back(c);
      }
    } else {
      if (c == '>') {
        isContent = true;
      }
    }
  }
  return true;
}

bool parseFile(const string& input, DocInfo* docInfo) {
  string html;
  bool ret = common::Util::read(input, &html);
  if (!ret) {
    cout << "read file failed! " << input << endl;
    return false;
  }
  ret = parseTitle(html, &docInfo->title);
  if (!ret) {
    cout << "parseTitle failed! " << input << endl;
    return false;
  }
  ret = parseUrl(input, &docInfo->url);
  if (!ret) {
    cout << "parseUrl failed! " << input << endl;
    return false;
  }
  ret = parseContent(html, &docInfo->content);
  if (!ret) {
    cout << "parseContent failed! " << input << endl;
    return false;
  }
  return true;
}

void writeOutput(const DocInfo& docInfo, ofstream& outputFile) {
  outputFile << docInfo.title << "\3" << docInfo.url << "\3" << docInfo.content << endl;
}

int main() {
  vector<string> fileList;
  if (!enumFile(g_input_path, &fileList)) {
    cout << "enumFile failed" << endl;
    return 1;
  }
  ofstream outputFile(g_output_path);
  if (!outputFile.is_open()) {
    cout << "open output file failed" << endl;
    return 1;
  }
  for (const auto& f : fileList) {
    cout << "parse file: " << f << endl;
    DocInfo docInfo;
    if (!parseFile(f, &docInfo)) {
      cout << "parseFile failed: " << f << endl;
      continue;
    }
    writeOutput(docInfo, outputFile);
  }
  outputFile.close();
  
  return 0;
}

