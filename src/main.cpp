#include <csignal>
#include <iostream>

#include "../include/_getline.h"

int main() {
  // handles Ctrl+C gracefully
  signal(SIGINT, [](int) {
    std::cout << "\nSuccessfully exited\r\n" << std::flush;
    exit(0);
  });

  InputLine inputLine;

  std::cout << "Autocomplete Demo\nUse Ctrl+C to exit\nStart Typing!\n\n"
            << std::flush;

  while (true) {
    _getline(inputLine);
  }
}
