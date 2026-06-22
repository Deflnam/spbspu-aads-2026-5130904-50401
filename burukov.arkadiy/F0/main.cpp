#include <iostream>
#include <string>
#include <limits>
#include "AVLTree.hpp"
#include "commands.hpp"

int main()
{
  using CommandFunc = void (*)(std::istream&, std::ostream&, burukov::LibraryManager&);
  burukov::AVLTree< std::string, CommandFunc > commands;
  std::string addTitle = "add-title";
  std::string addCopy = "add-copy";
  std::string lend = "lend";
  std::string ret = "return";
  std::string demandTitle = "demand-model-title";
  std::string demandGenre = "demand-model-genre";
  std::string sliceTitle = "slice-title";
  std::string sliceGenre = "slice-genre";
  std::string recommend = "recommend";
  std::string deadStock = "dead-stock";
  std::string demandBalance = "demand-balance";
  commands.push(addTitle, burukov::parsingAddTitle);
  commands.push(addCopy, burukov::parsingAddCopy);
  commands.push(lend, burukov::parsingLend);
  commands.push(ret, burukov::parsingReturn);
  commands.push(demandTitle, burukov::parsingDemandTitle);
  commands.push(demandGenre, burukov::parsingDemandGenre);
  commands.push(sliceTitle, burukov::parsingSliceTitle);
  commands.push(sliceGenre, burukov::parsingSliceGenre);
  commands.push(recommend, burukov::parsingRecommend);
  commands.push(deadStock, burukov::parsingDeadStock);
  commands.push(demandBalance, burukov::parsingDemandBalance);
  burukov::LibraryManager lib;
  std::string cmd;
  while (std::cin >> cmd)
  {
    try
    {
      if (commands.hasKey(cmd))
      {
        commands.at(cmd)(std::cin, std::cout, lib);
      }
      else
      {
        throw std::runtime_error("unknown command");
      }
    }
    catch (const std::exception&)
    {
      std::cout << "<INVALID COMMAND>\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits< std::streamsize >::max(), '\n');
    }
  }
  return 0;
}
