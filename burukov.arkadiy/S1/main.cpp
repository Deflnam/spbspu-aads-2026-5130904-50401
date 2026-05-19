#include "list.hpp"
#include <iostream>
#include <string>
#include <utility>
#include <limits>

namespace burukov
{
  using pair_t = std::pair< std::string, List< unsigned long long > >;

  template< class Container >
  void printContainer(std::ostream &out, const Container &c)
  {
    auto it = c.begin();
    if (it != c.end())
    {
      out << *it;
      ++it;
    }
    for (; it != c.end(); ++it)
    {
      out << " " << *it;
    }
  }

  template< class Container >
  void printPairFirst(std::ostream &out, const Container &c)
  {
    auto it = c.begin();
    if (it != c.end())
    {
      out << it->first;
      ++it;
    }
    for (; it != c.end(); ++it)
    {
      out << " " << it->first;
    }
  }

  void readSequences(std::istream &in, List< pair_t > &seqs)
  {
    LIter< pair_t > seqsTail;
    bool seqsHasTail = false;
    std::string name;
    while (in >> name)
    {
      List< unsigned long long > nums;
      LIter< unsigned long long > numTail;
      bool numHasTail = false;
      unsigned long long num = 0;
      while (in >> num)
      {
        if (!numHasTail)
        {
          nums.pushFront(num);
          numTail = nums.begin();
          numHasTail = true;
        }
        else
        {
          numTail = nums.insertAfter(numTail, num);
        }
      }
      in.clear();
      pair_t p;
      p.first = name;
      p.second = std::move(nums);
      if (!seqsHasTail)
      {
        seqs.pushFront(std::move(p));
        seqsTail = seqs.begin();
        seqsHasTail = true;
      }
      else
      {
        seqsTail = seqs.insertAfter(seqsTail, std::move(p));
      }
    }
  }

  void printNames(std::ostream &out, const List< pair_t > &seqs)
  {
    printPairFirst(out, seqs);
  }

  size_t getMaxLen(const List< pair_t > &seqs)
  {
    size_t maxLen = 0;
    for (auto it = seqs.cbegin(); it != seqs.cend(); ++it)
    {
      size_t len = it->second.size();
      if (len > maxLen)
      {
        maxLen = len;
      }
    }
    return maxLen;
  }

  void printRow(std::ostream &out, const List< unsigned long long > &row)
  {
    printContainer(out, row);
  }

  void collectIters(List< pair_t > &seqs, List< LIter< unsigned long long > > &iters)
  {
    LIter< LIter< unsigned long long > > itersTail;
    bool itersHasTail = false;
    for (auto it = seqs.begin(); it != seqs.end(); ++it)
    {
      LIter< unsigned long long > numIt = it->second.begin();
      if (!itersHasTail)
      {
        iters.pushFront(numIt);
        itersTail = iters.begin();
        itersHasTail = true;
      }
      else
      {
        itersTail = iters.insertAfter(itersTail, numIt);
      }
    }
  }

  void processColumn(List< pair_t > &seqs, List< LIter< unsigned long long > > &iters,
      size_t col, List< List< unsigned long long > > &rows,
      LIter< List< unsigned long long > > &rowsTail, bool &rowsTailSet,
      List< unsigned long long > &sums, LIter< unsigned long long > &sumsTail,
      bool &sumsTailSet, bool &overflow, unsigned long long maxVal)
  {
    List< unsigned long long > row;
    LIter< unsigned long long > rowTail;
    bool rowTailSet = false;
    unsigned long long sum = 0;
    bool colOverflow = false;

    auto curIt = iters.begin();
    for (auto seqIt = seqs.begin(); seqIt != seqs.end(); ++seqIt, ++curIt)
    {
      if (col >= seqIt->second.size())
      {
        continue;
      }
      unsigned long long val = *(*curIt);
      if (!rowTailSet)
      {
        row.pushFront(val);
        rowTail = row.begin();
        rowTailSet = true;
      }
      else
      {
        rowTail = row.insertAfter(rowTail, val);
      }
      if (!overflow && !colOverflow)
      {
        if (sum > maxVal - val)
        {
          colOverflow = true;
        }
        else
        {
          sum += val;
        }
      }
      ++(*curIt);
    }

    if (!rowsTailSet)
    {
      rows.pushFront(row);
      rowsTail = rows.begin();
      rowsTailSet = true;
    }
    else
    {
      rowsTail = rows.insertAfter(rowsTail, row);
    }

    if (!colOverflow)
    {
      if (!sumsTailSet)
      {
        sums.pushFront(sum);
        sumsTail = sums.begin();
        sumsTailSet = true;
      }
      else
      {
        sumsTail = sums.insertAfter(sumsTail, sum);
      }
    }
    else
    {
      overflow = true;
    }
  }

  int buildAndPrint(std::ostream &out, std::ostream &err, List< pair_t > &seqs, size_t maxLen)
  {
    List< LIter< unsigned long long > > iters;
    collectIters(seqs, iters);

    List< List< unsigned long long > > rows;
    LIter< List< unsigned long long > > rowsTail;
    bool rowsTailSet = false;

    List< unsigned long long > sums;
    LIter< unsigned long long > sumsTail;
    bool sumsTailSet = false;
    bool overflow = false;

    const unsigned long long maxVal = std::numeric_limits< unsigned long long >::max();

    for (size_t col = 0; col < maxLen; ++col)
    {
      processColumn(seqs, iters, col, rows, rowsTail, rowsTailSet,
          sums, sumsTail, sumsTailSet, overflow, maxVal);
    }

    for (auto it = rows.begin(); it != rows.end(); ++it)
    {
      printContainer(out, *it);
      out << "\n";
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
  burukov::List< burukov::pair_t > seqs;
  burukov::readSequences(std::cin, seqs);
  if (seqs.empty())
  {
    std::cout << 0 << "\n";
    return 0;
  }
  burukov::printNames(std::cout, seqs);
  std::cout << "\n";
  size_t maxLen = burukov::getMaxLen(seqs);
  if (maxLen == 0)
  {
    std::cout << 0 << "\n";
    return 0;
  }
  return burukov::buildAndPrint(std::cout, std::cerr, seqs, maxLen);
}
