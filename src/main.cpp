#include <csignal>
#include <iostream>
#include <sstream>

#include "../include/_getline.h"
#include "../include/ascii.h"

int main() {
  // handles Ctrl+C gracefully
  signal(SIGINT, [](int) {
    std::cout << "\nSuccessfully exited\r\n" << std::flush;
    exit(0);
  });

  InputLine inputLine;

  std::cout << "Autocomplete Demo\nUse Ctrl+C to exit\nStart Typing!\n\n"
            << PROMPT << std::flush;

  while (true) {
    _getline(inputLine);
    if (inputLine.getText().empty()) continue;

    std::cout << ERASE << '\n'
              << YELLOW << ERASE << inputLine.getText() << WHITE << '\n'
              << PROMPT << std::flush;

    if (inputLine.getInputState() == InputLine::InputState::INPUT) {
      inputLine.historyCache.addEntry(inputLine.getText());
    }

    inputLine.reset();
  }
}
