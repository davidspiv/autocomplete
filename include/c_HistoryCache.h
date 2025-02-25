#ifndef HISTORY_CACHE_H
#define HISTORY_CACHE_H

#include <deque>
#include <string>

class HistoryCache {
 public:
  HistoryCache();
  ~HistoryCache() {};
  void addEntry(const std::string &entry);
  bool moveForward();
  bool moveBackward();
  bool isLast() const;
  void end();
  std::string getCurrent() const;

 private:
  std::deque<std::string> history;
  std::deque<std::string>::iterator iter;
};

#endif
