#include "commands.hpp"
#include "../common/vector.hpp"
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <graph_file>\n";
    return 1;
  }

  std::ifstream input_file(argv[1]);
  if (!input_file.is_open())
  {
    std::cerr << "Cannot open file\n";
    return 1;
  }

  burukov::GraphDatabase database(16);
  std::string graph_name;
  size_t edge_count;
  while (input_file >> graph_name >> edge_count)
  {
    burukov::Graph new_graph;
    for (size_t i = 0; i < edge_count; ++i)
    {
      std::string from, to;
      size_t weight;
      input_file >> from >> to >> weight;
      new_graph.addEdge(from, to, weight);
    }
    database.add(graph_name, std::move(new_graph));
  }
  input_file.close();

  using CommandFunction = void (*)(std::istream &, std::ostream &, burukov::GraphDatabase &);
  burukov::HashTable<std::string, CommandFunction, burukov::SipHash<std::string>, std::equal_to<std::string>> command_table(16);
  command_table.add("graphs",   burukov::commandGraphs);
  command_table.add("vertexes", burukov::commandVertexes);
  command_table.add("outbound", burukov::commandOutbound);
  command_table.add("inbound",  burukov::commandInbound);
  command_table.add("bind",     burukov::commandBind);
  command_table.add("cut",      burukov::commandCut);
  command_table.add("create",   burukov::commandCreate);
  command_table.add("merge",    burukov::commandMerge);
  command_table.add("extract",  burukov::commandExtract);

  std::string command;
  while (std::cin >> command)
  {
    try
    {
      command_table.at(command)(std::cin, std::cout, database);
    }
    catch (const std::exception &)
    {
      std::cout << "<INVALID COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  return 0;
}