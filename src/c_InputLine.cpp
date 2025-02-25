#include <iostream>
#include <sstream>
#include <string>

#include "../include/_getline.h"
#include "../include/ascii.h"

void InputLine::reset() {
  inputState = INPUT;
  cursorIndex = 0;
  text = "";
  prediction = "";
  historyCache.end();
}

std::string InputLine::getText() const { return text; }

std::string InputLine::getPrediction() const { return prediction; }

InputLine::InputState InputLine::getInputState() const { return inputState; }

void InputLine::setText(const std::string &text) {
  this->cursorIndex = text.length();
  this->text = text;
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

  } else if (ch == '\177') {  // handle backspace
    if (cursorIndex) {
      std::cout << "\177";
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
