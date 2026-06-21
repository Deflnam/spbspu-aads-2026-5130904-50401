#include "library.hpp"
#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_SUITE(AlgorithmTests)

BOOST_AUTO_TEST_CASE(demandModelTitleNoDataThrows)
{
  burukov::LibraryManager lib;
  lib.addTitle("Physics", "Author", 2020, "education");
  std::ostringstream out;
  BOOST_CHECK_THROW(lib.demandModelTitle(out, "Physics", 365),
    std::runtime_error);
}

BOOST_AUTO_TEST_CASE(demandModelTitleWithData)
{
  burukov::LibraryManager lib;
  lib.addTitle("Physics", "Author", 2020, "education");
  lib.addCopy("Physics", "C1");
  lib.lend("Physics", "Ivanov");
  lib.returnCopy("C1");
  std::ostringstream out;
  lib.demandModelTitle(out, "Physics", 365);
  std::string result = out.str();
  BOOST_CHECK(result.find("Total:") != std::string::npos);
  BOOST_CHECK(result.find("Minimal:") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(sliceTitleShowsStats)
{
  burukov::LibraryManager lib;
  lib.addTitle("Physics", "Author", 2020, "education");
  lib.addCopy("Physics", "C1");
  for (int i = 0; i < 10; ++i) {
    lib.lend("Physics", "R" + std::to_string(i));
    lib.returnCopy("C1");
  }
  std::ostringstream out;
  lib.sliceTitle(out, "Physics", 365);
  std::string result = out.str();
  BOOST_CHECK(result.find("Seasonality:") != std::string::npos);
  BOOST_CHECK(result.find("Stability:") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(sliceGenreShowsStats)
{
  burukov::LibraryManager lib;
  lib.addTitle("A", "Author1", 2020, "fiction");
  lib.addTitle("B", "Author2", 2020, "fiction");
  lib.addCopy("A", "C1");
  lib.addCopy("B", "C2");
  lib.lend("A", "Ivanov");
  lib.returnCopy("C1");
  std::ostringstream out;
  lib.sliceGenre(out, "fiction", 365);
  std::string result = out.str();
  BOOST_CHECK(result.find("Top books:") != std::string::npos);
  BOOST_CHECK(result.find("Diversity:") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
