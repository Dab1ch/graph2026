/**
 * @file include/euler_path.hpp
 * @author Ilya Kozlov
 *
 * Алгоритм обхода графа в ширину.
 */

#ifndef INCLUDE_EULER_PATH_HPP_
#define INCLUDE_EULER_PATH_HPP_

#include <unordered_set>
#include <queue>

namespace graph {

/**
 * @brief Вспомогательна функция для обхода графа в ширину.
 *
 * @param graph Граф для обхода в ширину.
 * @param predicate Предикат, принимающий аргумент типа size_t.
 * @param id Начальная вершина.
 * @param used Множество посещённых вершин.
 *
 * Функция обходит в ширину компоненту связности графа, начиная с вершины id
 * и при заходе в каждую вершину вызывает предикат, которому передаёт id
 * текущей вершины в качестве аргумента.
 *
 */
template<typename GraphType, typename Predicate>
void EulerPathHelper(const GraphType& graph, Predicate predicate,
    size_t id, std::unordered_set<size_t>* used) {

  std::queue<size_t> ids;

  ids.push(id);

  used->insert(id);

  while (!ids.empty()) {
    size_t current = ids.front();
    ids.pop();

    predicate(current);

    for (size_t neighbour : graph.Edges(current)) {
      if (used->find(neighbour) == used->end())
        ids.push(neighbour);

      used->insert(neighbour);
    }
  }
}


/**
 * @brief Обход графа в ширину.
 *
 * @param graph Граф для обхода в ширину.
 * @param predicate Предикат, принимающий аргумент типа size_t.
 *
 * Функция обходит граф в ширину и при заходе в каждую вершину вызывает
 * предикат, которому передаёт id текущей вершины в качестве аргумента.
 */
template<typename GraphType, typename Predicate>
void EulerPath(const GraphType& graph, Predicate predicate) {
  std::unordered_set<size_t> used;

  for (size_t id : graph.Vertices()) {
    if (used.find(id) == used.end()) {
      EulerPathHelper(graph, predicate, id, &used);
    }
  }
}

}  // namespace graph

#endif  // INCLUDE_BREADTH_FIRST_TRAVERSAL_HPP_