#ifndef GETLINE_H
#define GETLINE_H

#include <termios.h>

#include <deque>
#include <string>

inline constexpr const char BEG = '\001';
inline constexpr const char DEL = '\177';
inline constexpr const char ESC = '\033';

inline constexpr const char* ERASE = "\033[0K";
inline constexpr const char* GREY = "\033[38;5;8m";
inline constexpr const char* YELLOW = "\033[33m";
inline constexpr const char* WHITE = "\033[0m";
inline constexpr const char* PREV_LINE = "\033[F";
inline constexpr const char* CURSOR_LEFT = "\033[1D";
inline constexpr const char* CURSOR_RIGHT = "\033[1C";

class HistoryCache {
 public:
  HistoryCache();
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

inline constexpr const char* PROMPT = ">  ";

void setNonCanonicalMode(struct termios& initialSettings);

bool readNextChar(char& ch);

void _getline(InputLine& inputLine);

#endif
