#include "searcher.h"
#include <iostream>

int main() {
  using namespace std;

  // 调用 Searcher 类的相关操作
  searcher::Searcher searcher;
  bool ret = searcher.init("../data/temp/raw_input");
  if (!ret) {
    cout << "searcher init failed" << endl;
    return 1;
  }

  // 如果初始化成功, 就可以让用户输入一些查询词, 进一步的调用 search 函数来进行搜索. 
  while (true) {
    cout << "searcher> " << flush;
    string query;
    cin >> query;
    if (!cin.good()) {
      cout << "goodbye!" << endl;
      break;
    }
    string output;
    searcher.search(query, &output);
    cout << "output: " << endl;
    cout << output << endl;
  }
  return 0;
}

