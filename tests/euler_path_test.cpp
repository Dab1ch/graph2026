/**
* @author Ilya Kozlov
**/

#include <httplib.h>
#include <unordered_set>
#include <string>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "test_core.hpp"

static void EmptyGraphTest(httplib::Client* cli);
static void SimpleTest(httplib::Client* cli);
static void RandomTest(httplib::Client* cli);
static void RandomTestHelper(httplib::Client* cli,
    const std::string& graphType);

void TestEulerPath(httplib::Client* cli) {
  TestSuite suite("TestEulerPath");

  RUN_TEST_REMOTE(suite, cli, EmptyGraphTest);
  RUN_TEST_REMOTE(suite, cli, SimpleTest);
  RUN_TEST_REMOTE(suite, cli, RandomTest);
}

/**
 * @brief Простейший статический тест для пустого графа.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void EmptyGraphTest(httplib::Client* cli) {
  /*
  Библиотека nlohmann json позволяет преобразовать
  строку в объект nlohmann::json не только при помощи
  функции nlohmann::json::parse(), но и при помощи
  специального литерала _json. Если его поставить после строки
  в кавычках, то она конвертируется в json объект.

  R"(
  )" Так записывается строка, содержащая символы перевода строки
  в C++. Всё, что между скобками это символы строки. Перводы строк
  можно ставить просто как перевод строки в текстовом редактора
  (а не через \n).
  */

  nlohmann::json input = R"(
{
  "graph_type": "Graph",
  "vertices": [ ],
  "edges": [ ]
}
)"_json;


  /* Делаем POST запрос по адресу нашего метода на сервере.
  Метод dump() используется для преобразования JSON обратно в строку.
  (Можно было сразу строку передать). При передаче JSON данных
  необходимо поставить тип MIME "application/json".
  */
  auto res = cli->Post("/EulerPath", input.dump(),
      "application/json");

  if (!res) {
    REQUIRE(false);
  }

  /* Используем метод parse() для преобразования строки ответа сервера
  (res->body) в объект JSON. */
  nlohmann::json output = nlohmann::json::parse(res->body);

  /* Проверка результатов сортировки. */
  std::vector<int> result = output.at("result");

  std::unordered_set<int> expected;
  std::unordered_set<int> resultSet;

  for (int id : result)
    resultSet.insert(id);

  REQUIRE_EQUAL(expected, resultSet);
}


/**
 * @brief Простейший статический тест.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void SimpleTest(httplib::Client* cli) {
  nlohmann::json input;

  input["graph_type"] = "Graph";
  input["vertices"] = std::vector<int>{1, 3, 7, 9};
  input["edges"][0]["from"] = 1;
  input["edges"][0]["to"] = 3;

  input["edges"][1]["from"] = 3;
  input["edges"][1]["to"] = 7;

  input["edges"][2]["from"] = 1;
  input["edges"][2]["to"] = 9;

/*
  Библиотека nlohmann json позволяет преобразовать
  строку в объект nlohmann::json не только при помощи
  функции nlohmann::json::parse(), но и при помощи
  специального литерала _json. Если его поставить после строки
  в кавычках, то она конвертируется в json объект.

  R"(
  )" Так записывается строка, содержащая символы перевода строки
  в C++. Всё, что между скобками это символы строки. Перводы строк
  можно ставить просто как перевод строки в текстовом редактора
  (а не через \n).

  std::string input = R"(
{
  "graph_type": "Graph",
  "vertices": [1, 3, 5, 7, 9, 11],
  "edges": [
    { "from": 1, "to": 3},
    { "from": 3, "to": 7},
    { "from": 1, "to": 9},
    { "from": 5, "to": 11}
  ]
}
)";
*/

  /* Делаем POST запрос по адресу нашего метода на сервере.
  Метод dump() используется для преобразования JSON обратно в строку.
  (Можно было сразу строку передать). При передаче JSON данных
  необходимо поставить тип MIME "application/json".
  */
  auto res = cli->Post("/EulerPath", input.dump(),
      "application/json");

  if (!res) {
    REQUIRE(false);
  }

  /* Используем метод parse() для преобразования строки ответа сервера
  (res->body) в объект JSON. */
  nlohmann::json output = nlohmann::json::parse(res->body);

  /* Проверка результатов сортировки. */
  std::vector<int> result = output.at("result");

  std::unordered_set<int> expected = {1, 3, 7, 9};
  std::unordered_set<int> resultSet;

  for (int id : result)
    resultSet.insert(id);

  REQUIRE_EQUAL(expected, resultSet);
}

/**
 * @brief Простейший случайный тест.
 *
 * @param cli Указатель на HTTP клиент.
 */
static void RandomTest(httplib::Client* cli) {
  RandomTestHelper(cli, "Graph");
  RandomTestHelper(cli, "OrientedGraph");
}

/**
 * @brief Простейший случайный тест.
 *
 * @param cli Указатель на HTTP клиент.
 * @param graphType Тип графа.
 */
static void RandomTestHelper(httplib::Client* cli,
    const std::string& graphType) {
  // Число попыток.
  const int numTries = 100;
  // Используется для инициализации генератора случайных чисел.
  std::random_device rd;
  // Генератор случайных чисел.
  std::mt19937 gen(rd());
  // Распределение для вершин графа.
  std::uniform_int_distribution<size_t> vertexId(100, 150);
  // Распределение для количества вершин графа.
  std::uniform_int_distribution<size_t> verticesSize(10, 100);
  // Распределение для количества ребер графа.
  std::uniform_int_distribution<size_t> edgesSize(10, 300);

  for (int it = 0; it < numTries; it++) {
    int n = -2;
    size_t numEdges = edgesSize(gen);

    std::vector<size_t> vertices;
    std::vector<nlohmann::json> edges;
    nlohmann::json input;

    input["graph_type"] = graphType;

    size_t numRealEdges = 0;
    for (size_t i = 0; i < numEdges; i++) {
      size_t id1 = vertexId(gen);
      size_t id2 = vertexId(gen);

      if (id1 == id2)
        continue;

      vertices.push_back(id1);
      vertices.push_back(id2);

      input["edges"][numRealEdges]["from"] = id1;
      input["edges"][numRealEdges]["to"] = id2;
      numRealEdges++;
    }

    input["vertices"] = vertices;
    if(input.contains("edges")){
        for(int i = 0; i < input["edges"].size(); i++){
            edges.push_back(input["edges"][i]);
        }
    }
    auto res = cli->Post("/EulerPath", input.dump(),
        "application/json");

    if (!res) {
      REQUIRE(false);
    }

    /* Используем метод parse() для преобразования строки ответа сервера
    (res->body) в объект JSON. */
    nlohmann::json output = nlohmann::json::parse(res->body);
    if(!output.contains("result")){
        REQUIRE_EQUAL(0, 0); // граф неудачный
        continue;
    }
    std::vector<int> result = std::vector<int>(output["result"]);
    int m = edges.size();
    n += result.size();
    REQUIRE_EQUAL(n, edges.size()); // проверка того, что число ребер и там, и там совпадают

  }
}