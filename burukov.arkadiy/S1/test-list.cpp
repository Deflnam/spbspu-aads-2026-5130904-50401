#define BOOST_TEST_MODULE S1
#include <boost/test/included/unit_test.hpp>
#include "list.hpp"

BOOST_AUTO_TEST_SUITE(ListTests)

BOOST_AUTO_TEST_CASE(DefConstructorTest)
{
  burukov::List< int > list;
  BOOST_CHECK_EQUAL(list.size(), 0);
  BOOST_CHECK(list.empty());
  BOOST_CHECK(list.begin() == list.end());
}

BOOST_AUTO_TEST_CASE(CopyConstructorTest)
{
  burukov::List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2(list1);
  BOOST_CHECK_EQUAL(list2.size(), 2);
  list1.pushFront(0);
  BOOST_CHECK_EQUAL(list2.size(), 2);
  auto it = list2.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
}

BOOST_AUTO_TEST_CASE(MoveConstructorTest)
{
  burukov::List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2(std::move(list1));
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK_EQUAL(list1.size(), 0);
  BOOST_CHECK(list1.begin() == list1.end());
  auto it = list2.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  list2.pushFront(0);
  BOOST_CHECK_EQUAL(list1.size(), 0);
  BOOST_CHECK_EQUAL(list2.size(), 3);
}

BOOST_AUTO_TEST_CASE(DestructEmptyListTest)
{
  burukov::List< int > list;
}

BOOST_AUTO_TEST_CASE(DestructSingleElemTest)
{
  burukov::List< int > list;
  list.pushFront(1);
  BOOST_CHECK_EQUAL(list.size(), 1);
}

BOOST_AUTO_TEST_CASE(DestructMultElemTest)
{
  burukov::List< int > list;
  for (int i = 0; i < 10; ++i)
  {
    list.pushFront(i);
  }
  BOOST_CHECK_EQUAL(list.size(), 10);
}

BOOST_AUTO_TEST_CASE(DestructAfterCopyTest)
{
  burukov::List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2(list1);
  BOOST_CHECK_EQUAL(list1.size(), 2);
  BOOST_CHECK_EQUAL(list2.size(), 2);
}

BOOST_AUTO_TEST_CASE(DestructAfterMoveTest)
{
  burukov::List< int > list1;
  list1.pushFront(52);
  burukov::List< int > list2(std::move(list1));
  BOOST_CHECK_EQUAL(list1.size(), 0);
  BOOST_CHECK_EQUAL(list2.size(), 1);
}

BOOST_AUTO_TEST_CASE(CopyAssignTest)
{
  burukov::List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(3);
  list2 = list1;
  BOOST_CHECK_EQUAL(list2.size(), 2);
  auto it = list2.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  list1 = list1;
  BOOST_CHECK_EQUAL(list1.size(), 2);
}

BOOST_AUTO_TEST_CASE(MoveAssignTest)
{
  burukov::List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(99);
  list2 = std::move(list1);
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK_EQUAL(list2.front(), 1);
  BOOST_CHECK(list1.empty());
}

BOOST_AUTO_TEST_CASE(FrontTest)
{
  burukov::List< int > list;
  list.pushFront(1);
  BOOST_CHECK_EQUAL(list.front(), 1);
  list.pushFront(2);
  BOOST_CHECK_EQUAL(list.front(), 2);
  const burukov::List< int > &cref = list;
  BOOST_CHECK_EQUAL(cref.front(), 2);
}

BOOST_AUTO_TEST_CASE(PushFrontTest)
{
  burukov::List< int > list;
  list.pushFront(1);
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 1);
  list.pushFront(2);
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.front(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(PopFrontTest)
{
  burukov::List< int > list;
  list.pushFront(3);
  list.pushFront(2);
  list.pushFront(1);
  list.popFront();
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.front(), 2);
  list.popFront();
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 3);
  list.popFront();
  BOOST_CHECK_EQUAL(list.size(), 0);
  BOOST_CHECK(list.begin() == list.end());
}

BOOST_AUTO_TEST_CASE(InsertAfterTest)
{
  burukov::List< int > list;
  list.pushFront(1);
  auto it = list.begin();
  auto ins = list.insertAfter(it, 3);
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(*ins, 3);
  list.insertAfter(it, 2);
  BOOST_CHECK_EQUAL(list.size(), 3);
  auto check = list.begin();
  BOOST_CHECK_EQUAL(*check, 1);
  ++check;
  BOOST_CHECK_EQUAL(*check, 2);
  ++check;
  BOOST_CHECK_EQUAL(*check, 3);
}

BOOST_AUTO_TEST_CASE(InsertAfterChainTest)
{
  burukov::List< int > list;
  list.pushFront(1);
  auto tail = list.begin();
  for (int i = 2; i <= 5; ++i)
  {
    tail = list.insertAfter(tail, i);
  }
  BOOST_CHECK_EQUAL(list.size(), 5);
  auto it = list.begin();
  for (int i = 1; i <= 5; ++i)
  {
    BOOST_CHECK_EQUAL(*it, i);
    ++it;
  }
  BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(EraseAfterTest)
{
  burukov::List< int > list;
  list.pushFront(3);
  list.pushFront(2);
  list.pushFront(1);
  auto it = list.begin();
  auto next = list.eraseAfter(it);
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(*next, 3);
  auto check = list.begin();
  BOOST_CHECK_EQUAL(*check, 1);
  ++check;
  BOOST_CHECK_EQUAL(*check, 3);
  list.eraseAfter(list.begin());
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 1);
}

BOOST_AUTO_TEST_CASE(ClearTest)
{
  burukov::List< int > list;
  list.clear();
  BOOST_CHECK_EQUAL(list.size(), 0);
  list.pushFront(3);
  list.pushFront(2);
  list.pushFront(1);
  list.clear();
  BOOST_CHECK_EQUAL(list.size(), 0);
  BOOST_CHECK(list.begin() == list.end());
  list.pushFront(4);
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 4);
}

BOOST_AUTO_TEST_CASE(SizeAndEmptyTest)
{
  burukov::List< int > list;
  BOOST_CHECK_EQUAL(list.size(), 0);
  BOOST_CHECK(list.empty());
  list.pushFront(1);
  list.pushFront(2);
  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK(!list.empty());
  list.clear();
  BOOST_CHECK_EQUAL(list.size(), 0);
  BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(SwapTest)
{
  burukov::List< int > list1;
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(3);
  list1.swap(list2);
  BOOST_CHECK_EQUAL(list1.size(), 1);
  BOOST_CHECK_EQUAL(list1.front(), 3);
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK_EQUAL(list2.front(), 1);
}

BOOST_AUTO_TEST_CASE(IteratorTest)
{
  burukov::List< int > list;
  list.pushFront(3);
  list.pushFront(2);
  list.pushFront(1);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  auto old = it++;
  BOOST_CHECK_EQUAL(*old, 1);
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(ConstIterTest)
{
  burukov::List< int > list;
  list.pushFront(2);
  list.pushFront(1);
  const burukov::List< int > &constList = list;
  auto cit = constList.cbegin();
  BOOST_CHECK_EQUAL(*cit, 1);
  ++cit;
  BOOST_CHECK_EQUAL(*cit, 2);
  ++cit;
  BOOST_CHECK(cit == constList.cend());
  BOOST_CHECK(constList.cbegin() != constList.cend());
  auto cit2 = constList.cbegin();
  BOOST_CHECK_EQUAL(*cit2, 1);
  burukov::List< int > empty;
  BOOST_CHECK(empty.cbegin() == empty.cend());
}

BOOST_AUTO_TEST_CASE(ArrowOperatorTest)
{
  burukov::List< std::pair< int, int > > list;
  list.pushFront(std::make_pair(1, 2));
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 1);
  BOOST_CHECK_EQUAL(it->second, 2);
}

BOOST_AUTO_TEST_CASE(SpliceWholeListTest)
{
  burukov::List< int > list1;
  list1.pushFront(3);
  list1.pushFront(2);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(6);
  list2.pushFront(5);
  list2.pushFront(4);
  list1.splice(list1.begin(), list2);
  BOOST_CHECK_EQUAL(list1.size(), 6);
  BOOST_CHECK(list2.empty());
  auto it = list1.begin();
  BOOST_CHECK_EQUAL(*it, 4);
  ++it;
  BOOST_CHECK_EQUAL(*it, 5);
  ++it;
  BOOST_CHECK_EQUAL(*it, 6);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
}

BOOST_AUTO_TEST_CASE(SpliceSingleElementTest)
{
  burukov::List< int > list1;
  list1.pushFront(3);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(4);
  list2.pushFront(2);
  auto it = list1.begin();
  ++it;
  auto it2 = list2.begin();
  ++it2;
  list1.splice(it, list2, it2);
  BOOST_CHECK_EQUAL(list1.size(), 3);
  BOOST_CHECK_EQUAL(list2.size(), 1);
  auto check = list1.begin();
  BOOST_CHECK_EQUAL(*check, 1);
  ++check;
  BOOST_CHECK_EQUAL(*check, 4);
  ++check;
  BOOST_CHECK_EQUAL(*check, 3);
}

BOOST_AUTO_TEST_CASE(SpliceRangeTest)
{
  burukov::List< int > list1;
  list1.pushFront(5);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(6);
  list2.pushFront(4);
  list2.pushFront(3);
  list2.pushFront(2);
  auto pos = list1.begin();
  ++pos;
  auto first = list2.begin();
  ++first;
  auto last = list2.end();
  list1.splice(pos, list2, first, last);
  BOOST_CHECK_EQUAL(list1.size(), 4);
  auto it = list1.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 4);
  ++it;
  BOOST_CHECK_EQUAL(*it, 5);
}

BOOST_AUTO_TEST_CASE(SortTest)
{
  burukov::List< int > list;
  list.pushFront(5);
  list.pushFront(3);
  list.pushFront(1);
  list.pushFront(4);
  list.pushFront(2);
  list.sort();
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 4);
  ++it;
  BOOST_CHECK_EQUAL(*it, 5);
}

BOOST_AUTO_TEST_CASE(SortDescendingTest)
{
  burukov::List< int > list;
  list.pushFront(1);
  list.pushFront(2);
  list.pushFront(3);
  list.pushFront(4);
  list.pushFront(5);
  list.sort(std::greater< int >());
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 5);
  ++it;
  BOOST_CHECK_EQUAL(*it, 4);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(SortEmptyTest)
{
  burukov::List< int > list;
  list.sort();
  BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(SortSingleElementTest)
{
  burukov::List< int > list;
  list.pushFront(42);
  list.sort();
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 42);
}

BOOST_AUTO_TEST_CASE(MergeTest)
{
  burukov::List< int > list1;
  list1.pushFront(3);
  list1.pushFront(1);
  burukov::List< int > list2;
  list2.pushFront(4);
  list2.pushFront(2);
  list1.merge(list2);
  BOOST_CHECK_EQUAL(list1.size(), 4);
  BOOST_CHECK(list2.empty());
  auto it = list1.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 4);
}

BOOST_AUTO_TEST_CASE(MergeWithComparatorTest)
{
  burukov::List< int > list1;
  list1.pushFront(1);
  list1.pushFront(3);
  burukov::List< int > list2;
  list2.pushFront(2);
  list2.pushFront(4);
  list1.merge(list2, std::greater< int >());
  auto it = list1.begin();
  BOOST_CHECK_EQUAL(*it, 4);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 1);
}

BOOST_AUTO_TEST_CASE(MergeEmptyTest)
{
  burukov::List< int > list1;
  list1.pushFront(1);
  burukov::List< int > list2;
  list1.merge(list2);
  BOOST_CHECK_EQUAL(list1.size(), 1);
  BOOST_CHECK(list2.empty());
}

BOOST_AUTO_TEST_CASE(PartitionTest)
{
  burukov::List< int > list;
  list.pushFront(5);
  list.pushFront(2);
  list.pushFront(4);
  list.pushFront(1);
  list.pushFront(3);
  auto isOdd = [](const int &x)
  {
    return x % 2 == 1;
  };
  list.partition(isOdd);
  auto it = list.begin();
  bool foundEven = false;
  for (; it != list.end(); ++it)
  {
    if (*it % 2 == 0)
    {
      foundEven = true;
    }
    else if (foundEven)
    {
      BOOST_CHECK(false);
    }
  }
}

BOOST_AUTO_TEST_CASE(PartitionEmptyTest)
{
  burukov::List< int > list;
  auto isOdd = [](const int &x)
  {
    return x % 2 == 1;
  };
  auto result = list.partition(isOdd);
  BOOST_CHECK(result == list.end());
}

BOOST_AUTO_TEST_CASE(PartitionAllTrueTest)
{
  burukov::List< int > list;
  list.pushFront(5);
  list.pushFront(3);
  list.pushFront(1);
  auto isOdd = [](const int &x)
  {
    return x % 2 == 1;
  };
  list.partition(isOdd);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 1);
  ++it;
  BOOST_CHECK_EQUAL(*it, 3);
  ++it;
  BOOST_CHECK_EQUAL(*it, 5);
}

BOOST_AUTO_TEST_CASE(PartitionAllFalseTest)
{
  burukov::List< int > list;
  list.pushFront(4);
  list.pushFront(2);
  list.pushFront(6);
  auto isOdd = [](const int &x)
  {
    return x % 2 == 1;
  };
  list.partition(isOdd);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 6);
  ++it;
  BOOST_CHECK_EQUAL(*it, 2);
  ++it;
  BOOST_CHECK_EQUAL(*it, 4);
}

BOOST_AUTO_TEST_SUITE_END()
