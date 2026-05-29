#include "list.hpp"

#include <iostream>
#include <string>
#include <limits>
#include <utility>

namespace burukov
{
  using pair_t = std::pair<std::string,List< unsigned long long >>;

  template< class Container >
  void printContainer(
      std::ostream &out,
      const Container &container)
  {
    auto it = container.cbegin();

    if (it != container.cend())
    {
      out << *it;
      ++it;
    }

    for (; it != container.cend(); ++it)
    {
      out << " " << *it;
    }
  }

  void printNames(
      std::ostream &out,
      const List< pair_t > &seqs)
  {
    auto it = seqs.cbegin();

    if (it != seqs.cend())
    {
      out << it->first;
      ++it;
    }

    for (; it != seqs.cend(); ++it)
    {
      out << " " << it->first;
    }
  }

  void readSequences(
      std::istream &in,
      List< pair_t > &seqs)
  {
    std::string name;

    LIter< pair_t > seqTail;
    bool hasTail = false;

    while (in >> name)
    {
      List< unsigned long long > numbers;

      unsigned long long value = 0;

      LIter< unsigned long long > numTail;
      bool numHasTail = false;

      while (in >> value)
      {
        if (!numHasTail)
        {
          numbers.pushFront(value);
          numTail = numbers.begin();
          numHasTail = true;
        }
        else
        {
          numTail = numbers.insertAfter(
              numTail,
              value);
        }
      }

      in.clear();

      pair_t pair;
      pair.first = name;
      pair.second = std::move(numbers);

      if (!hasTail)
      {
        seqs.pushFront(std::move(pair));
        seqTail = seqs.begin();
        hasTail = true;
      }
      else
      {
        seqTail = seqs.insertAfter(
            seqTail,
            std::move(pair));
      }
    }
  }

  size_t getMaxLen(const List< pair_t > &seqs)
  {
    size_t result = 0;

    for (auto it = seqs.cbegin();
        it != seqs.cend();
        ++it)
    {
      if (it->second.size() > result)
      {
        result = it->second.size();
      }
    }

    return result;
  }

  int process(
      std::ostream &out,std::ostream &err,List< pair_t > &seqs)
  {
    size_t maxLen = getMaxLen(seqs);

    if (maxLen == 0)
    {
      out << "0\n";
      return 0;
    }

    List< LIter< unsigned long long > > iters;

    {
      bool first = true;
      LIter< LIter< unsigned long long > > tail;

      for (auto it = seqs.begin();
          it != seqs.end();
          ++it)
      {
        if (first)
        {
          iters.pushFront(it->second.begin());
          tail = iters.begin();
          first = false;
        }
        else
        {
          tail = iters.insertAfter(
              tail,
              it->second.begin());
        }
      }
    }

    List< unsigned long long > sums;

    bool sumTailSet = false;
    LIter< unsigned long long > sumTail;

    bool overflow = false;

    const unsigned long long maxValue =
        std::numeric_limits<
            unsigned long long
        >::max();

    for (size_t column = 0;
        column < maxLen;
        ++column)
    {
      List< unsigned long long > row;

      bool rowTailSet = false;
      LIter< unsigned long long > rowTail;

      unsigned long long sum = 0;

      auto iterIt = iters.begin();

      for (auto seqIt = seqs.begin();
          seqIt != seqs.end();
          ++seqIt, ++iterIt)
      {
        if (column >= seqIt->second.size())
        {
          continue;
        }

        unsigned long long value = *(*iterIt);

        if (!rowTailSet)
        {
          row.pushFront(value);
          rowTail = row.begin();
          rowTailSet = true;
        }
        else
        {
          rowTail = row.insertAfter(
              rowTail,
              value);
        }

        if (!overflow)
        {
          if (sum > maxValue - value)
          {
            overflow = true;
          }
          else
          {
            sum += value;
          }
        }

        ++(*iterIt);
      }

      printContainer(out, row);
      out << "\n";

      if (!overflow)
      {
        if (!sumTailSet)
        {
          sums.pushFront(sum);
          sumTail = sums.begin();
          sumTailSet = true;
        }
        else
        {
          sumTail = sums.insertAfter(
              sumTail,
              sum);
        }
      }
    }

    if (overflow)
    {
      err << "overflow\n";
      return 1;
    }

    printContainer(out, sums);
    out << "\n";

    return 0;
  }
}

int main()
{
  burukov::List< burukov::pair_t > sequences;

  burukov::readSequences(
      std::cin,
      sequences);

  if (sequences.empty())
  {
    std::cout << "0\n";
    return 0;
  }

  burukov::printNames(
      std::cout,
      sequences);

  std::cout << "\n";

  return burukov::process(
      std::cout,
      std::cerr,
      sequences);
}
