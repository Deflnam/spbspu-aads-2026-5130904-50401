#include "library.hpp"
#include <boost/test/unit_test.hpp>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(LibraryBasicTests)

BOOST_AUTO_TEST_CASE(addTitleInsertsBook)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(addTitleDuplicateThrows)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  BOOST_CHECK_THROW(lib.addTitle("1984", "Orwell", 1949, "dystopia"),
    std::runtime_error);
}

BOOST_AUTO_TEST_CASE(addCopyForNonexistentTitleThrows)
{
  burukov::LibraryManager lib;
  BOOST_CHECK_THROW(lib.addCopy("NoBook", "C1"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(addCopyDuplicateThrows)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  BOOST_CHECK_THROW(lib.addCopy("1984", "C1"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(lendIssuesAvailableCopy)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  lib.lend("1984", "Ivanov");
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(lendNoCopiesThrows)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  lib.lend("1984", "Ivanov");
  BOOST_CHECK_THROW(lib.lend("1984", "Petrov"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(returnCopyFreesCopy)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  lib.lend("1984", "Ivanov");
  lib.returnCopy("C1");
  lib.lend("1984", "Petrov");
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(returnNotLentCopyThrows)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  BOOST_CHECK_THROW(lib.returnCopy("C1"), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
