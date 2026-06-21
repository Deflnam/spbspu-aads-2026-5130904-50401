#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <list.hpp>
#include "hash_table.hpp"
#include "siphash.hpp"
#include <string>
#include <utility>
#include <stdexcept>

namespace burukov
{
  class Graph
  {
  public:
    using EdgeKey = std::pair<std::string, std::string>;
    using WeightList = List<size_t>;

    Graph() = default;
    explicit Graph(size_t bucket_hint) : edges_(bucket_hint) {}

    void addVertex(const std::string &vertex_name);
    void addEdge(const std::string &from, const std::string &to, size_t weight);
    void removeEdge(const std::string &from, const std::string &to, size_t weight);

    List<std::string> vertices_;
    HashTable<EdgeKey, WeightList, PairHash, std::equal_to<EdgeKey>> edges_;
  };
}

#endif
