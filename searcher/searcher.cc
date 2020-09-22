#include <iostream>
#include <fstream>
#include <algorithm>
#include <jsoncpp/json/json.h>

#include "../common/Util.hpp"
#include "searcher.h"

using namespace std;

namespace searcher {

const char* const DICT_PATH = "../jieba_dict/jieba.dict.utf8";    
const char* const HMM_PATH = "../jieba_dict/hmm_model.utf8";    
const char* const USER_DICT_PATH = "../jieba_dict/user.dict.utf8";    
const char* const IDF_PATH = "../jieba_dict/idf.utf8";
const char* const STOP_WORD_PATH = "../jieba_dict/stop_words.utf8";

Index::Index() : jieba(DICT_PATH,
        HMM_PATH,    
        USER_DICT_PATH,    
        IDF_PATH,    
        STOP_WORD_PATH) {

}

const DocInfo* Index::getDocInfo(int64_t docId) {
  if (docId >= (int64_t)forwardIndex.size()) {
    return nullptr;
  }
  return &forwardIndex[docId];
}

const InvertedList* Index::getInverted(const string& key) {
  auto it = invertedIndex.find(key);
  if (it == invertedIndex.end()) {
    return nullptr;
  }
  return &it->second;
}

bool Index::build(const string& inputPath) {
  cout << "build start" << endl;
  ifstream file(inputPath.c_str());
  if (!file.is_open()) {
    cout << "build open inputPath failed! inputPath = " << inputPath << endl;
    return false;
  }
  string line;
  while (getline(file, line)) {
    DocInfo* docInfo = buildForward(line);
    if (docInfo == nullptr) {
      continue;
    }
    buildInverted(*docInfo);
    if (docInfo->docId % 100 == 0) {
      cout << "Index Build docId: " << docInfo->docId << endl;
    }
  }

  file.close();
  cout << "build finish" << endl;
  return true;
}

DocInfo* Index::buildForward(const std::string& line) {
  vector<string> tokens;
  common::Util::split(line, "\3", &tokens);
  if (tokens.size() != 3) {
    return nullptr;
  }
  DocInfo docInfo;
  docInfo.docId = forwardIndex.size();
  docInfo.title = tokens[0];
  docInfo.url = tokens[1];
  docInfo.content = tokens[2];
  forwardIndex.push_back(std::move(docInfo));
  return &forwardIndex.back();
}

void Index::buildInverted(const DocInfo& docInfo) {
  struct WordCnt {
    int titleCnt;  // 标题中出现的次数
    int contentCnt;// 正文中出现的次数

    WordCnt() : titleCnt(0), contentCnt(0) {}
  };
  unordered_map<string, WordCnt> wordCntMap;

  vector<string> titleTokens;
  cutWord(docInfo.title, &titleTokens);
  for (string word : titleTokens) {
    boost::to_lower(word);
    ++wordCntMap[word].titleCnt;
  }
  vector<string> contentTokens;
  cutWord(docInfo.content, &contentTokens);
  for (string word : contentTokens) {
    boost::to_lower(word);
    ++wordCntMap[word].contentCnt;
  }
  for (const auto& wordPair : wordCntMap) {
    Weight weight;
    weight.docId = docInfo.docId;
    weight.weight = wordPair.second.titleCnt * 10 + wordPair.second.contentCnt;
    weight.word = wordPair.first;
    InvertedList& invertedList = invertedIndex[wordPair.first];
    invertedList.push_back(weight);
  }
}

void Index::cutWord(const string& word, vector<string>* tokens) {
  jieba.CutForSearch(word, *tokens);
}

// 以下是 Searcher 模块的实现
bool Searcher::init(const std::string& inputPath) {
  return index->build(inputPath);
}

bool Searcher::search(const std::string query, std::string* output) {
  vector<string> tokens;
  index->cutWord(query, &tokens);
  vector<Weight> allTokenResult;
  for (string word : tokens) {
    boost::to_lower(word);
    const auto* invertedList = index->getInverted(word);
    if (invertedList == nullptr) {
      continue;
    }
    allTokenResult.insert(allTokenResult.end(), invertedList->begin(), invertedList->end());
  }
  std::sort(allTokenResult.begin(), allTokenResult.end(),[](const Weight& w1, const Weight& w2){
        return w1.weight > w2.weight;
      });
  Json::Value results;
  for (const auto& weight : allTokenResult) {
    const auto* docInfo = index->getDocInfo(weight.docId);
    Json::Value result;
    result["title"] = docInfo->title;
    result["url"] = docInfo->url;
    result["desc"] = generateDesc(docInfo->content, weight.word);
    results.append(result);
  }
  Json::FastWriter writer;
  *output = writer.write(results);
  return true;
}

string Searcher::generateDesc(const string& content, const string& word) {
  int64_t pos = content.find(word);
  if (pos == (int64_t)string::npos) {
    return "";
  }
  int64_t beg = pos < 60 ? 0 : pos - 60;
  if (beg + 160 >= (int64_t)content.size()) {
    return content.substr(beg);
  } else {
    string desc = content.substr(beg, 160);
    desc[desc.size() - 1] = '.';
    desc[desc.size() - 2] = '.';
    desc[desc.size() - 3] = '.';
    return desc;
  }
}

}  // end searcher

