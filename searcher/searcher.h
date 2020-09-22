#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "cppjieba/Jieba.hpp"

using std::string;
using std::vector;
using std::unordered_map;

namespace searcher {

struct DocInfo {
  int64_t docId;
  string title;
  string url;
  string content;
};

struct Weight {
  int64_t docId;
  int weight;
  string word;
};

typedef vector<Weight> InvertedList;

class Index {
private:
  vector<DocInfo> forwardIndex;
  unordered_map<string, InvertedList> invertedIndex;

public:
  const DocInfo* getDocInfo(int64_t docId);
  const InvertedList* getInverted(const string& key);
  bool build(const string& inputPath);

  Index();

  void cutWord(const string& word, vector<string>* tokens);

private:
  DocInfo* buildForward(const std::string& line);
  void buildInverted(const DocInfo& docInfo);

  cppjieba::Jieba jieba; 
};


class Searcher {
private:
  Index* index;

public:
  Searcher() : index(new Index()) {  }
  bool init(const std::string& inputPath);
  bool search(const std::string query, std::string* result);

private:
  static string generateDesc(const string& content, const string& word);
};


}

