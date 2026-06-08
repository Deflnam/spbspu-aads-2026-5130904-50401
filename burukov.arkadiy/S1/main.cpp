#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <utility>

#include "list.hpp"

namespace
{
  using pair_t = std::pair< std::string, burukov::List< unsigned long long > >;

  template< class Container >
  void printContainer(std::ostream &out, const Container &container)
  {
    auto it = container.cbegin();
    if (it != container.cend())
    {
      out << *it;
      ++it;
      for (; it != container.cend(); ++it)
      {
        out << " " << *it;
      }
    }
    out << "\n";
  }

  void printNames(std::ostream &out, const burukov::List< pair_t > &seqs)
  {
    auto it = seqs.cbegin();
    if (it != seqs.cend())
    {
      out << it->first;
      ++it;
      for (; it != seqs.cend(); ++it)
      {
        out << " " << it->first;
      }
    }
    out << "\n";
  }

  void readSequences(std::istream &in, burukov::List< pair_t > &seqs)
  {
    std::string name;
    bool hasTail = false;
    burukov::LIter< pair_t > seqTail;

    while (in >> name)
    {
      burukov::List< unsigned long long > numbers;
      unsigned long long value = 0;
      bool numHasTail = false;
      burukov::LIter< unsigned long long > numTail;

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
          numTail = numbers.insertAfter(numTail, value);
        }
      }
      in.clear();

      pair_t p;
      p.first = name;
      p.second = std::move(numbers);

      if (!hasTail)
      {
        seqs.pushFront(std::move(p));
        seqTail = seqs.begin();
        hasTail = true;
      }
      else
      {
        seqTail = seqs.insertAfter(seqTail, std::move(p));
      }
    }
  }

  size_t getMaxLen(const burukov::List< pair_t > &seqs)
  {
    size_t result = 0;
    for (auto it = seqs.cbegin(); it != seqs.cend(); ++it)
    {
      result = std::max(result, it->second.size());
    }
    return result;
  }

  void buildIterators(const burukov::List< pair_t > &seqs, burukov::List< burukov::LCIter< unsigned long long > > &iters)
  {
    bool hasIterTail = false;
    burukov::LIter< burukov::LCIter< unsigned long long > > iterTail;

    for (auto seqIt = seqs.cbegin(); seqIt != seqs.cend(); ++seqIt)
    {
      if (!hasIterTail)
      {
        iters.pushFront(seqIt->second.cbegin());
        iterTail = iters.begin();
        hasIterTail = true;
      }
      else
      {
        iterTail = iters.insertAfter(iterTail, seqIt->second.cbegin());
      }
    }
  }

  bool processColumns(std::ostream &out, std::ostream &err, const burukov::List< pair_t > &seqs, size_t maxLen)
  {
    if (maxLen == 0)
    {
      out << "0\n";
      return false;
    }

    bool overflow = false;
    burukov::List< unsigned long long > sums;
    bool sumTailSet = false;
    burukov::LIter< unsigned long long > sumTail;
    const unsigned long long maxValue = std::numeric_limits< unsigned long long >::max();

    burukov::List< burukov::LCIter< unsigned long long > > iters;
    buildIterators(seqs, iters);

    for (size_t column = 0; column < maxLen; ++column)
    {
      burukov::List< unsigned long long > row;
      bool rowTailSet = false;
      burukov::LIter< unsigned long long > rowTail;
      unsigned long long currentSum = 0;

      auto iterIt = iters.begin();
      for (auto seqIt = seqs.cbegin(); seqIt != seqs.cend(); ++seqIt, ++iterIt)
      {
        if (column < seqIt->second.size())
        {
          unsigned long long value = **iterIt;

          if (!rowTailSet)
          {
            row.pushFront(value);
            rowTail = row.begin();
            rowTailSet = true;
          }
          else
          {
            rowTail = row.insertAfter(rowTail, value);
          }

          if (!overflow)
          {
            if (currentSum > maxValue - value)
            {
              overflow = true;
            }
            else
            {
              currentSum += value;
            }
          }
          ++(*iterIt);
        }
      }
      printContainer(out, row);

      if (!overflow)
      {
        if (!sumTailSet)
        {
          sums.pushFront(currentSum);
          sumTail = sums.begin();
          sumTailSet = true;
        }
        else
        {
          sumTail = sums.insertAfter(sumTail, currentSum);
        }
      }
    }

    if (overflow)
    {
      err << "overflow\n";
      return true;
    }

    printContainer(out, sums);
    return false;
  }
}

int main()
{
  burukov::List< pair_t > sequences;
  readSequences(std::cin, sequences);

  if (sequences.empty())
  {
    std::cout << "0\n";
    return 0;
  }

  printNames(std::cout, sequences);

  size_t maxLen = getMaxLen(sequences);

  if (processColumns(std::cout, std::cerr, sequences, maxLen))
  {
    return 1;
  }

  return 0;
}
