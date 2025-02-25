#include "../include/c_HistoryCache.h"

HistoryCache::HistoryCache() {
  addEntry("The brown dog lives on the prairie");
  addEntry("Yellow is the color of sunshine");
  addEntry("My neighbor always eats bacon for breakfast");
  iter = history.end();
};

void HistoryCache::addEntry(const std::string &entry) {
  history.push_back(entry);
}

bool HistoryCache::moveForward() {
  if (iter != history.end()) {
    ++iter;
    return true;
  }
  return false;
}

bool HistoryCache::moveBackward() {
  if (iter != history.begin()) {
    --iter;
    return true;
  }
  return false;
}

void HistoryCache::end() { iter = history.end(); }

bool HistoryCache::isLast() const { return iter == std::prev(history.end()); }

std::string HistoryCache::getCurrent() const {
  return history.empty() ? "" : *iter;
}
