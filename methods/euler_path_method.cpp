#include <euler_path.hpp>
#include <graph.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <oriented_graph.hpp>
#include <stack>
#include <string>
#include <vector>
#include <weighted_graph.hpp>
#include <weighted_oriented_graph.hpp>

using graph::Graph;
using graph::OrientedGraph;
using graph::WeightedGraph;
using graph::WeightedOrientedGraph;
using std::stack;
using std::vector;

namespace graph {

template <typename GraphType>
int EulerPathMethodHelper(const nlohmann::json& input, nlohmann::json* output);

int EulerPathMethod(const nlohmann::json& input, nlohmann::json* output) {
  std::string graphType = input.at("graph_type");

  if (graphType == "Graph") {
    return EulerPathMethodHelper<Graph>(input, output);
  } else if (graphType == "OrientedGraph") {
    return -1;
  } else if (graphType == "WeightedGraph") {
    std::string weightType = input.at("weight_type");
    if (weightType == "int") {
      return EulerPathMethodHelper<WeightedGraph<int>>(input, output);
    } else {
      return -1;
    }
  } else if (graphType == "WeightedOrientedGraph") {
    std::string weightType = input.at("weight_type");
    if (weightType == "int") {
      return -1;
    } else {
      return -1;
    }
  }

  return -1;
}

/**
 * @brief Метод поиска эйлерова пути.
 *
 * @param input Входные данные в формате JSON.
 * @param output Выходные данные в формате JSON.
 * @return Функция возвращает 0 в случае успеха и отрицательное число
 * если входные данные заданы некорректно.
 *
 * Функция запускает алгоритм поиска эйлерова графа, используя входные данные
 * в JSON формате. Результат также выдаётся в JSON формате.
 */
template <typename GraphType>
int EulerPathMethodHelper(const nlohmann::json& input, nlohmann::json* output) {
  *output = nlohmann::json::parse("{}");
  if (!input.contains("vertices")) {
    (*output)["result"] = nlohmann::json::parse("[]");
    return 0;
  }

  if (!input.contains("edges")) {
    (*output)["result"] = nlohmann::json::parse("[]");
    return 0;
  }
  vector<int> vertices = input.at("vertices");
  vector<nlohmann::json> edges = input.at("edges");
  int n = vertices.size();
  int m = edges.size();
  if (n == 0) {
    (*output)["result"] = nlohmann::json::parse("[]");
    return 0;
  }
  if (m == 0) {
    (*output)["result"] = nlohmann::json::parse("[]");
    return 0;
  }
  vector<vector<int>> g(n, vector<int>(n));

  for (int i = 0; i < m; i++) {
    g[std::find(vertices.begin(), vertices.end(), edges[i]["from"]) -
      vertices.begin()]
     [std::find(vertices.begin(), vertices.end(), edges[i]["to"]) -
      vertices.begin()]++;
    g[std::find(vertices.begin(), vertices.end(), edges[i]["to"]) -
      vertices.begin()]
     [std::find(vertices.begin(), vertices.end(), edges[i]["from"]) -
      vertices.begin()]++;
  }

  vector<int> deg(n);
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) deg[i] += g[i][j];

  int first = 0;
  while (!deg[first]) ++first;

  int v1 = -1, v2 = -1;
  bool bad = false;
  for (int i = 0; i < n; ++i)
    if (deg[i] & 1){
      if (v1 == -1)
        v1 = i;
      else if (v2 == -1)
        v2 = i;
      else
        bad = true;
    }

  if (v1 != -1) ++g[v1][v2], ++g[v2][v1];

  stack<int> st;
  st.push(first);
  vector<int> res;
  while (!st.empty()) {
    int v = st.top();
    int i;
    for (i = 0; i < n; ++i)
      if (g[v][i]) break;
    if (i == n) {
      res.push_back(vertices[v]);
      st.pop();
    } else {
      --g[v][i];
      --g[i][v];
      st.push(i);
    }
  }

  if (v1 != -1)
    for (size_t i = 0; i + 1 < res.size(); ++i)
      if (((res[i] == v1) && (res[i + 1] == v2)) ||
          ((res[i] == v2) && (res[i + 1] == v1))) {
        vector<int> res2;
        for (size_t j = i + 1; j < res.size(); ++j) res2.push_back(res[j]);
        for (size_t j = 1; j <= i; ++j) res2.push_back(res[j]);
        res = res2;
        break;
      }

  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      if (g[i][j]) bad = true;

  if (bad) {
    return 0;
  } else {
    (*output)["result"] = res;
    return 0;
  }
  return 0;
}

}  // namespace graph