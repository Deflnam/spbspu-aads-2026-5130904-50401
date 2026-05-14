#include "graph.hpp"

namespace burukov
{
  void Graph::addVertex(const std::string &vertex_name)
  {
    for (auto it = vertices_.begin(); it != vertices_.end(); ++it)
    {
      if (*it == vertex_name)
      {
        return;
      }
    }
    vertices_.pushFront(vertex_name);
  }

  void Graph::addEdge(const std::string &from, const std::string &to, size_t weight)
  {
    addVertex(from);
    addVertex(to);
    EdgeKey key(from, to);
    if (edges_.contains(key))
    {
      edges_.at(key).pushFront(weight);
    }
    else
    {
      WeightList list;
      list.pushFront(weight);
      edges_.add(key, list);
    }
  }

  void Graph::removeEdge(const std::string &from, const std::string &to, size_t weight)
  {
    EdgeKey key(from, to);
    if (!edges_.contains(key))
    {
      throw std::out_of_range("edge not found");
    }
    WeightList &list = edges_.at(key);
    for (auto it = list.begin(); it != list.end(); ++it)
    {
      if (*it == weight)
      {
        list.erase(it);
        if (list.empty())
        {
          edges_.erase(key);
        }
        return;
      }
    }
    throw std::out_of_range("weight not found");
  }
}
