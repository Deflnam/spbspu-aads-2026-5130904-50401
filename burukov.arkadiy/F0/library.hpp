#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include "AVLTree.hpp"
#include <list.hpp>
#include <string>
#include <iostream>

namespace burukov
{
  struct Transaction
  {
    std::string copyId_;
    std::string reader_;
    int startDay_;
    int endDay_;
    bool isActive_;

    Transaction():
      startDay_(0),
      endDay_(0),
      isActive_(false)
    {}

    Transaction(const std::string &copyId, const std::string &reader, int startDay):
      copyId_(copyId),
      reader_(reader),
      startDay_(startDay),
      endDay_(0),
      isActive_(true)
    {}
  };

  struct BookData
  {
    std::string title_;
    std::string author_;
    int year_;
    std::string genre_;
    List< std::string > copies_;
    List< Transaction > history_;
    int lendCount_;
    int lastLendDate_;

    BookData():
      year_(0),
      lendCount_(0),
      lastLendDate_(0)
    {}

    BookData(const std::string &t, const std::string &a, int y, const std::string &g):
      title_(t),
      author_(a),
      year_(y),
      genre_(g),
      lendCount_(0),
      lastLendDate_(0)
    {}
  };

  class LibraryManager
  {
  public:
    LibraryManager();

    void addTitle(const std::string &title, const std::string &author, int year, const std::string &genre);
    void addCopy(const std::string &title, const std::string &copyId);
    void lend(const std::string &title, const std::string &reader);
    void returnCopy(const std::string &copyId);

  private:
    AVLTree< std::string, BookData > books_;
    int currentDay_;

    std::string findTitleByCopy(const std::string &copyId) const;
    bool isCopyLent(const BookData &book, const std::string &copyId) const;
  };
}

#endif
