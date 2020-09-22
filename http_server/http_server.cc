#include <string>
#include <vector>
#include <iostream>
#include "httplib.h"
#include "../searcher/searcher.h"

int main() {
  using namespace std;
  using namespace httplib;

  // 1. 初始化搜索模块
  searcher::Searcher searcher;
  bool ret = searcher.init("../data/temp/raw_input");
  if (!ret) {
    cout << "searcher 初始化失败" << endl;
    return 1;
  }

  // 2. 初始化服务器模块
  Server server;
  server.Get("/searcher", [&searcher](const Request& req, Response& resp) {
      bool ret = req.has_param("query");
      if (!ret) {
        resp.set_content("query param miss!", "text/plan");
        return;
      }
      string query = req.get_param_value("query");

      string result;
      searcher.search(query, &result);
      resp.set_content(result, "application/json");
    });

  server.set_base_dir("./background");
  server.listen("0.0.0.0", 10000);

  return 0;
}

