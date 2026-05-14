#include "commands.hpp"
#include "../common/vector.hpp"
#include <limits>
#include <algorithm>

namespace burukov
{
  namespace helpers
  {
    void sortStrings(Vector<std::string> &strings)
    {
      for (size_t i = 1; i < strings.getSize(); ++i)
      {
        std::string key = strings[i];
        size_t j = i;
        while (j > 0 && strings[j - 1] > key)
        {
          strings[j] = strings[j - 1];
          --j;
        }
        strings[j] = key;
      }
    }

    void sortWeights(Vector<size_t> &weights)
    {
      for (size_t i = 1; i < weights.getSize(); ++i)
      {
        size_t key = weights[i];
        size_t j = i;
        while (j > 0 && weights[j - 1] > key)
        {
          weights[j] = weights[j - 1];
          --j;
        }
        weights[j] = key;
      }
    }
  }

  void commandGraphs(std::istream &, std::ostream &output_stream, GraphDatabase &database)
  {
    Vector<std::string> names;
    for (auto it = database.begin(); it != database.end(); ++it)
    {
      names.pushBack(it->first);
    }
    if (names.isEmpty())
    {
      output_stream << "0\n";
      return;
    }
    helpers::sortStrings(names);
    for (size_t i = 0; i < names.getSize(); ++i)
    {
      output_stream << names[i] << '\n';
    }
  }

  void commandVertexes(std::istream &input_stream, std::ostream &output_stream, GraphDatabase &database)
  {
    std::string graph_name;
    input_stream >> graph_name;
    if (!database.contains(graph_name))
    {
      throw std::runtime_error("graph not found");
    }
    const Graph &graph = database.at(graph_name);
    Vector<std::string> vertices;
    for (auto it = graph.vertices_.cbegin(); it != graph.vertices_.cend(); ++it)
    {
      vertices.pushBack(*it);
    }
    if (vertices.isEmpty())
    {
      output_stream << "\n";
      return;
    }
    helpers::sortStrings(vertices);
    for (size_t i = 0; i < vertices.getSize(); ++i)
    {
      output_stream << vertices[i] << '\n';
    }
  }

  void commandBind(std::istream &input_stream, std::ostream &, GraphDatabase &database)
  {
    std::string graph_name, from, to;
    size_t weight;
    input_stream >> graph_name >> from >> to >> weight;
    if (!database.contains(graph_name))
    {
      throw std::runtime_error("graph not found");
    }
    database.at(graph_name).addEdge(from, to, weight);
  }

  void commandCut(std::istream &input_stream, std::ostream &, GraphDatabase &database)
  {
    std::string graph_name, from, to;
    size_t weight;
    input_stream >> graph_name >> from >> to >> weight;
    if (!database.contains(graph_name))
    {
      throw std::runtime_error("graph not found");
    }
    database.at(graph_name).removeEdge(from, to, weight);
  }

  void commandCreate(std::istream &input_stream, std::ostream &, GraphDatabase &database)
  {
    std::string graph_name;
    input_stream >> graph_name;
    if (database.contains(graph_name))
    {
      throw std::runtime_error("graph already exists");
    }
    size_t vertex_count;
    input_stream >> vertex_count;
    Graph new_graph;
    for (size_t i = 0; i < vertex_count; ++i)
    {
      std::string vertex;
      input_stream >> vertex;
      new_graph.addVertex(vertex);
    }
    database.add(graph_name, std::move(new_graph));
  }

  void commandOutbound(std::istream &input_stream, std::ostream &output_stream, GraphDatabase &database)
  {
    std::string graph_name, vertex;
    input_stream >> graph_name >> vertex;
    if (!database.contains(graph_name))
    {
      throw std::runtime_error("graph not found");
    }
    const Graph &graph = database.at(graph_name);
    bool vertex_exists = false;
    for (auto it = graph.vertices_.cbegin(); it != graph.vertices_.cend(); ++it)
    {
      if (*it == vertex)
      {
        vertex_exists = true;
        break;
      }
    }
    if (!vertex_exists)
    {
      throw std::runtime_error("vertex not found");
    }

    struct OutgoingInfo
    {
      std::string target;
      Vector<size_t> weights;
    };
    Vector<OutgoingInfo> outgoing_list;
    for (auto it = graph.edges_.cbegin(); it != graph.edges_.cend(); ++it)
    {
      if (it->first.first == vertex)
      {
        bool already_exists = false;
        for (size_t i = 0; i < outgoing_list.getSize(); ++i)
        {
          if (outgoing_list[i].target == it->first.second)
          {
            for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
            {
              outgoing_list[i].weights.pushBack(*weight_it);
            }
            already_exists = true;
            break;
          }
        }
        if (!already_exists)
        {
          OutgoingInfo info;
          info.target = it->first.second;
          for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
          {
            info.weights.pushBack(*weight_it);
          }
          outgoing_list.pushBack(std::move(info));
        }
      }
    }
    if (outgoing_list.isEmpty())
    {
      output_stream << "\n";
      return;
    }
    Vector<std::string> targets;
    for (size_t i = 0; i < outgoing_list.getSize(); ++i)
    {
      targets.pushBack(outgoing_list[i].target);
    }
    helpers::sortStrings(targets);
    for (size_t i = 0; i < targets.getSize(); ++i)
    {
      output_stream << targets[i];
      for (size_t j = 0; j < outgoing_list.getSize(); ++j)
      {
        if (outgoing_list[j].target == targets[i])
        {
          helpers::sortWeights(outgoing_list[j].weights);
          for (size_t w = 0; w < outgoing_list[j].weights.getSize(); ++w)
          {
            output_stream << ' ' << outgoing_list[j].weights[w];
          }
          break;
        }
      }
      output_stream << '\n';
    }
  }

  void commandInbound(std::istream &input_stream, std::ostream &output_stream, GraphDatabase &database)
  {
    std::string graph_name, vertex;
    input_stream >> graph_name >> vertex;
    if (!database.contains(graph_name))
    {
      throw std::runtime_error("graph not found");
    }
    const Graph &graph = database.at(graph_name);
    bool vertex_exists = false;
    for (auto it = graph.vertices_.cbegin(); it != graph.vertices_.cend(); ++it)
    {
      if (*it == vertex)
      {
        vertex_exists = true;
        break;
      }
    }
    if (!vertex_exists)
    {
      throw std::runtime_error("vertex not found");
    }

    struct IncomingInfo
    {
      std::string source;
      Vector<size_t> weights;
    };
    Vector<IncomingInfo> incoming_list;
    for (auto it = graph.edges_.cbegin(); it != graph.edges_.cend(); ++it)
    {
      if (it->first.second == vertex)
      {
        bool already_exists = false;
        for (size_t i = 0; i < incoming_list.getSize(); ++i)
        {
          if (incoming_list[i].source == it->first.first)
          {
            for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
            {
              incoming_list[i].weights.pushBack(*weight_it);
            }
            already_exists = true;
            break;
          }
        }
        if (!already_exists)
        {
          IncomingInfo info;
          info.source = it->first.first;
          for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
          {
            info.weights.pushBack(*weight_it);
          }
          incoming_list.pushBack(std::move(info));
        }
      }
    }
    if (incoming_list.isEmpty())
    {
      output_stream << "\n";
      return;
    }
    Vector<std::string> sources;
    for (size_t i = 0; i < incoming_list.getSize(); ++i)
    {
      sources.pushBack(incoming_list[i].source);
    }
    helpers::sortStrings(sources);
    for (size_t i = 0; i < sources.getSize(); ++i)
    {
      output_stream << sources[i];
      for (size_t j = 0; j < incoming_list.getSize(); ++j)
      {
        if (incoming_list[j].source == sources[i])
        {
          helpers::sortWeights(incoming_list[j].weights);
          for (size_t w = 0; w < incoming_list[j].weights.getSize(); ++w)
          {
            output_stream << ' ' << incoming_list[j].weights[w];
          }
          break;
        }
      }
      output_stream << '\n';
    }
  }

  void commandMerge(std::istream &input_stream, std::ostream &, GraphDatabase &database)
  {
    std::string new_name, graph1, graph2;
    input_stream >> new_name >> graph1 >> graph2;
    if (database.contains(new_name) || !database.contains(graph1) || !database.contains(graph2))
    {
      throw std::runtime_error("invalid merge");
    }
    Graph merged;
    const Graph &source1 = database.at(graph1);
    const Graph &source2 = database.at(graph2);
    for (auto it = source1.vertices_.cbegin(); it != source1.vertices_.cend(); ++it)
    {
      merged.addVertex(*it);
    }
    for (auto it = source2.vertices_.cbegin(); it != source2.vertices_.cend(); ++it)
    {
      merged.addVertex(*it);
    }
    for (auto it = source1.edges_.cbegin(); it != source1.edges_.cend(); ++it)
    {
      for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
      {
        merged.addEdge(it->first.first, it->first.second, *weight_it);
      }
    }
    for (auto it = source2.edges_.cbegin(); it != source2.edges_.cend(); ++it)
    {
      for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
      {
        merged.addEdge(it->first.first, it->first.second, *weight_it);
      }
    }
    database.add(new_name, std::move(merged));
  }

  void commandExtract(std::istream &input_stream, std::ostream &, GraphDatabase &database)
  {
    std::string new_name, old_name;
    size_t keep_count;
    input_stream >> new_name >> old_name >> keep_count;
    if (database.contains(new_name) || !database.contains(old_name))
    {
      throw std::runtime_error("invalid extract");
    }
    const Graph &source = database.at(old_name);
    List<std::string> vertices_to_keep;
    for (size_t i = 0; i < keep_count; ++i)
    {
      std::string vertex;
      input_stream >> vertex;
      vertices_to_keep.pushFront(vertex);
    }
    for (auto it = vertices_to_keep.cbegin(); it != vertices_to_keep.cend(); ++it)
    {
      bool found = false;
      for (auto vertex_it = source.vertices_.cbegin(); vertex_it != source.vertices_.cend(); ++vertex_it)
      {
        if (*vertex_it == *it)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        throw std::runtime_error("vertex not found");
      }
    }
    Graph extracted;
    for (auto it = vertices_to_keep.cbegin(); it != vertices_to_keep.cend(); ++it)
    {
      extracted.addVertex(*it);
    }
    for (auto it = source.edges_.cbegin(); it != source.edges_.cend(); ++it)
    {
      bool from_ok = false;
      bool to_ok = false;
      for (auto keep_it = vertices_to_keep.cbegin(); keep_it != vertices_to_keep.cend(); ++keep_it)
      {
        if (*keep_it == it->first.first)
        {
          from_ok = true;
        }
        if (*keep_it == it->first.second)
        {
          to_ok = true;
        }
      }
      if (from_ok && to_ok)
      {
        for (auto weight_it = it->second.cbegin(); weight_it != it->second.cend(); ++weight_it)
        {
          extracted.addEdge(it->first.first, it->first.second, *weight_it);
        }
      }
    }
    database.add(new_name, std::move(extracted));
  }
}
