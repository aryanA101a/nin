#include <termios.h>
#ifndef TERMINAL_H
#define TERMINAL_H

void die(const char *s);
void disableRawMode();
void enableRawMode();
int editorReadKey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);
void terminalInitOrigTermios(struct termios *orig);

#endif