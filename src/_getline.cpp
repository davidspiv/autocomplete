#include <termios.h>
#include <unistd.h>

#include "../include/c_InputLine.h"

void setNonCanonicalMode(struct termios &initialSettings) {
  struct termios newSettings;

  tcgetattr(STDIN_FILENO,
            &initialSettings);  // Get current terminal attributes
  newSettings = initialSettings;

  newSettings.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
  newSettings.c_cc[VMIN] = 1;               // Read one character at a time
  newSettings.c_cc[VTIME] = 0;              // No timeout

  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);  // Apply new settings
}

bool readNextChar(char &ch) { return read(STDIN_FILENO, &ch, 1) == 1; }

void _getline(InputLine &inputLine) {
  static struct termios terminalSettings;
  char ch;

  setNonCanonicalMode(terminalSettings);

  while (readNextChar(ch) && ch != '\n') {
    if (!inputLine.handleChar(ch)) continue;
    inputLine.displayCurrInput();
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &terminalSettings);
}
