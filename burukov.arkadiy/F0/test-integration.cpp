#include "commands.hpp"
#include "library.hpp"
#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_SUITE(IntegrationTests)

BOOST_AUTO_TEST_CASE(readTokenHandlesQuotedString)
{
  std::istringstream in("\"Physics 10\"");
  BOOST_CHECK_EQUAL(burukov::readToken(in), "Physics 10");
}

BOOST_AUTO_TEST_CASE(readTokenHandlesUnquotedString)
{
  std::istringstream in("simple");
  BOOST_CHECK_EQUAL(burukov::readToken(in), "simple");
}

BOOST_AUTO_TEST_CASE(readTokenThrowsOnUnclosedQuote)
{
  std::istringstream in("\"unclosed");
  BOOST_CHECK_THROW(burukov::readToken(in), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parsingAddTitleOutputsAdded)
{
  burukov::LibraryManager lib;
  std::istringstream in("1984 Orwell 1949 dystopia");
  std::ostringstream out;
  burukov::parsingAddTitle(in, out, lib);
  BOOST_CHECK_EQUAL(out.str(), "<ADDED>\n");
}

BOOST_AUTO_TEST_CASE(parsingAddTitleDuplicateOutputsInvalid)
{
  burukov::LibraryManager lib;
  std::istringstream in1("1984 Orwell 1949 dystopia");
  std::ostringstream out1;
  burukov::parsingAddTitle(in1, out1, lib);
  std::istringstream in2("1984 Orwell 1949 dystopia");
  std::ostringstream out2;
  BOOST_CHECK_THROW(burukov::parsingAddTitle(in2, out2, lib),
    std::runtime_error);
}

BOOST_AUTO_TEST_CASE(parsingAddCopyOutputsOk)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  std::istringstream in("1984 C1");
  std::ostringstream out;
  burukov::parsingAddCopy(in, out, lib);
  BOOST_CHECK_EQUAL(out.str(), "<OK>\n");
}

BOOST_AUTO_TEST_CASE(parsingLendOutputsIssued)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  std::istringstream in("1984 Ivanov");
  std::ostringstream out;
  burukov::parsingLend(in, out, lib);
  BOOST_CHECK_EQUAL(out.str(), "<ISSUED>\n");
}

BOOST_AUTO_TEST_CASE(parsingReturnOutputsReturned)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addCopy("1984", "C1");
  lib.lend("1984", "Ivanov");
  std::istringstream in("C1");
  std::ostringstream out;
  burukov::parsingReturn(in, out, lib);
  BOOST_CHECK_EQUAL(out.str(), "<RETURNED>\n");
}

BOOST_AUTO_TEST_CASE(fullWorkflowWithRecommendations)
{
  burukov::LibraryManager lib;
  lib.addTitle("1984", "Orwell", 1949, "dystopia");
  lib.addTitle("Brave New World", "Huxley", 1932, "dystopia");
  lib.addCopy("1984", "C1");
  lib.addCopy("Brave New World", "C2");
  lib.lend("1984", "Ivanov");
  lib.lend("Brave New World", "Ivanov");
  lib.returnCopy("C1");
  lib.returnCopy("C2");
  std::ostringstream out;
  lib.recommend(out, "1984", 5);
  BOOST_CHECK(out.str().find("Brave New World") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(deadStockReturnsNoneForEmptyLibrary)
{
  burukov::LibraryManager lib;
  std::ostringstream out;
  lib.deadStock(out, 365, 1.0);
  BOOST_CHECK_EQUAL(out.str(), "<NONE>\n");
}

BOOST_AUTO_TEST_CASE(demandBalanceEmptyLibrary)
{
  burukov::LibraryManager lib;
  std::ostringstream out;
  lib.demandBalance(out, 365);
  BOOST_CHECK(out.str().find("<NONE>") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
