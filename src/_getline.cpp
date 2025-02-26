#include "../include/_getline.h"

#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

inline constexpr const char* PROMPT = ">  ";

static inline constexpr char BEG = '\001';
static inline constexpr char DEL = '\177';
static inline constexpr char ESC = '\033';

static inline constexpr const char* ERASE = "\033[0K";
static inline constexpr const char* GREY = "\033[38;5;8m";
static inline constexpr const char* WHITE = "\033[0m";
static inline constexpr const char* CURSOR_LEFT = "\033[1D";
static inline constexpr const char* CURSOR_RIGHT = "\033[1C";

// HISTORY_CACHE MEMBER FUNCTIONS

bool HistoryCache::isLast() const { return iter == std::prev(history.end()); }

std::string HistoryCache::getCurrent() const {
  return history.empty() ? "" : *iter;
}

void HistoryCache::addEntry(const std::string& entry) {
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

// INPUT_LINE MEMBER FUNCTIONS

std::string InputLine::getText() const { return text; }

std::string InputLine::getPrediction() const { return prediction; }

InputLine::InputState InputLine::getInputState() const { return inputState; }

void InputLine::setText(const std::string& text) {
  this->cursorIndex = text.length();
  this->text = text;
}

void InputLine::reset() {
  inputState = INPUT;
  cursorIndex = 0;
  text = "";
  prediction = "";
  historyCache.end();
}

void InputLine::backspace() {
  if (cursorIndex > text.length()) return;
  --cursorIndex;
  text.erase(text.begin() + cursorIndex);
};

// evaluates expression with additional char
bool InputLine::handleChar(const char ch) {
  if (ch == '\v') {  // Ctrl-K
    if (cursorIndex != text.length()) {
      text = text.substr(0, cursorIndex);
    }
    predictFromHistory();
  } else if (ch == BEG) {  // Ctrl-A
    cursorIndex = 0;
  } else if (ch == ESC) {  // handle ANSI escape sequence

    char escCode[2];
    if (readNextChar(escCode[0]) && readNextChar(escCode[1]) &&
        escCode[0] == '[') {
      switch (escCode[1]) {
        case 'A':  // up arrow
          if (inputState == INPUT) {
            inputState = HISTORY;
          }
          historyCache.moveBackward();
          setText(historyCache.getCurrent());
          break;

        case 'B':  // down arrow

          if (inputState == INPUT) return false;

          if (historyCache.isLast()) {
            inputState = INPUT;
            reset();
          } else {
            historyCache.moveForward();
            setText(historyCache.getCurrent());
          }

          break;
        case 'D':
          if (cursorIndex) {
            std::cout << CURSOR_LEFT << std::flush;
            --cursorIndex;
          }
          break;

        case 'C':
          if (cursorIndex < text.length()) {
            std::cout << CURSOR_RIGHT << std::flush;
            ++cursorIndex;
          }
          break;
      }
    }
  } else if (ch == '\t') {  // handle tab
    if (!prediction.empty()) {
      text += prediction;
      prediction = "";
      cursorIndex = text.length();
    } else
      return false;

  } else if (ch == DEL) {  // handle backspace
    if (cursorIndex) {
      std::cout << DEL;
      backspace();
      predictFromHistory();
    }

  } else {  // handle character to display
    inputState = INPUT;
    text.insert(cursorIndex, std::string(1, ch));
    ++cursorIndex;
    predictFromHistory();
  }

  return true;
}

void InputLine::displayCurrInput() {
  std::ostringstream out;

  out << '\r' << ERASE << PROMPT << text << GREY << prediction << WHITE;
  for (size_t i = 0; i < text.length() + prediction.length() - cursorIndex;
       i++) {
    out << CURSOR_LEFT;
  }
  std::cout << out.str() << std::flush;
}

void InputLine::predictFromHistory() {
  if (text.empty()) {
    prediction = "";
    return;
  }

  while (historyCache.moveBackward()) {
    if (!historyCache.getCurrent().find(text)) {  // cache item STARTS with text
      prediction = historyCache.getCurrent().substr(text.length());
      historyCache.end();
      return;
    }
  }
  prediction = "";
  historyCache.end();
};

// UTILS

bool readNextChar(char& ch) { return read(STDIN_FILENO, &ch, 1) == 1; }

void setNonCanonicalMode(struct termios& initialSettings) {
  struct termios newSettings;

  tcgetattr(STDIN_FILENO,
            &initialSettings);  // Get current terminal attributes
  newSettings = initialSettings;

  newSettings.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
  newSettings.c_cc[VMIN] = 1;               // Read one character at a time
  newSettings.c_cc[VTIME] = 0;              // No timeout

  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);  // Apply new settings
}

// CONTROLLER

void _getline(InputLine& inputLine) {
  static struct termios terminalSettings;
  char ch;

  inputLine.reset();
  setNonCanonicalMode(terminalSettings);

  std::cout << PROMPT << std::flush;

  while (readNextChar(ch)) {
    if (ch == '\n' && !inputLine.getText().empty()) break;

    if (ch != '\n' && inputLine.handleChar(ch)) {
      inputLine.displayCurrInput();
    }
  }

  if (inputLine.getInputState() == InputLine::InputState::INPUT) {
    inputLine.historyCache.addEntry(inputLine.getText());
  }

  std::cout << '\r' << ERASE << std::flush;
  inputLine.reset();

  tcsetattr(STDIN_FILENO, TCSANOW, &terminalSettings);
}
