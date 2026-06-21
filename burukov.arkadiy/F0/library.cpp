#include "library.hpp"
#include <stdexcept>

burukov::LibraryManager::LibraryManager():
  currentDay_(1)
{}

std::string burukov::LibraryManager::findTitleByCopy(
  const std::string &copyId) const
{
  std::string result = "";
  books_.traverseInOrder([&](const std::string &title, const BookData &b) {
    if (!result.empty()) {
      return;
    }
    for (auto it = b.copies_.cbegin(); it != b.copies_.cend(); ++it) {
      if (*it == copyId) {
        result = title;
        break;
      }
    }
  });
  if (result.empty()) {
    throw std::runtime_error("copy not found");
  }
  return result;
}

bool burukov::LibraryManager::isCopyLent(const BookData &book, const std::string &copyId) const
{
  for (auto th = book.history_.begin(); th != book.history_.end(); ++th) {
    if (th->copyId_ == copyId && th->isActive_) {
      return true;
    }
  }
  return false;
}

void burukov::LibraryManager::addTitle(const std::string &title, const std::string &author, int year, 
  const std::string &genre)
{
  if (books_.hasKey(title)) {
    throw std::runtime_error("exists");
  }
  books_.push(title, BookData(title, author, year, genre));
}

void burukov::LibraryManager::addCopy(const std::string &title, const std::string &copyId)
{
  if (!books_.hasKey(title)) {
    throw std::runtime_error("no title");
  }
  bool copyExists = false;
  books_.traverseInOrder([&](const std::string &, const BookData &b) {
    for (auto it = b.copies_.cbegin(); it != b.copies_.cend(); ++it) {
      if (*it == copyId) {
        copyExists = true;
        break;
      }
    }
  });
  if (copyExists) {
    throw std::runtime_error("copy exists");
  }
  BookData &b = books_.at(title);
  b.copies_.pushBack(copyId);
}

void burukov::LibraryManager::lend(const std::string &title,
  const std::string &reader)
{
  if (!books_.hasKey(title)) {
    throw std::runtime_error("no title");
  }
  BookData &b = books_.at(title);
  std::string availableCopy;
  bool found = false;
  for (auto it = b.copies_.begin(); it != b.copies_.end(); ++it) {
    if (!isCopyLent(b, *it)) {
      availableCopy = *it;
      found = true;
      break;
    }
  }
  if (!found) {
    throw std::runtime_error("no copies");
  }
  Transaction t(availableCopy, reader, currentDay_);
  b.history_.pushBack(t);
  b.lendCount_++;
  b.lastLendDate_ = currentDay_;
  currentDay_++;
}

void burukov::LibraryManager::returnCopy(const std::string &copyId)
{
  std::string title = findTitleByCopy(copyId);
  BookData &b = books_.at(title);
  bool found = false;
  for (auto it = b.history_.begin(); it != b.history_.end(); ++it) {
    if (it->copyId_ == copyId && it->isActive_) {
      it->endDay_ = currentDay_;
      it->isActive_ = false;
      found = true;
      break;
    }
  }
  if (!found) {
    throw std::runtime_error("not lent");
  }
  currentDay_++;
}
