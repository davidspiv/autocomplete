#ifndef GETLINE_H
#define GETLINE_H

#include <termios.h>

#include <string>

#include "c_HistoryCache.h"

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

inline constexpr const char* PROMPT = ">  ";

void setNonCanonicalMode(struct termios& initialSettings);

bool readNextChar(char& ch);

void _getline(InputLine& inputLine);

#endif
