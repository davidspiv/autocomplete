#ifndef GETLINE_H
#define GETLINE_H

#include <termios.h>

#include <deque>
#include <string>

class HistoryCache {
 public:
  HistoryCache() : history(), iter(history.end()) {}
  ~HistoryCache() {};
  void addEntry(const std::string& entry);
  bool moveForward();
  bool moveBackward();
  bool isLast() const;
  void end();
  std::string getCurrent() const;

 private:
  std::deque<std::string> history;
  std::deque<std::string>::iterator iter;
};

class InputLine {
 public:
  InputLine() { reset(); };
  ~InputLine() {};
  void reset();

  enum InputState { INPUT, HISTORY };
  HistoryCache historyCache;

  std::string getText() const;
  InputState getInputState() const;
  std::string getPrediction() const;
  void setText(const std::string& text);

  bool handleChar(const char ch);
  void predictFromHistory();
  void displayCurrInput();

 private:
  InputState inputState;
  size_t cursorIndex;
  std::string text;
  std::string prediction;
  void backspace();
};

void setNonCanonicalMode(struct termios& initialSettings);

bool readNextChar(char& ch);

void _getline(InputLine& inputLine);

#endif
