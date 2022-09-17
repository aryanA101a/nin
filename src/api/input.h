#include "data.h"

#ifndef INPUT_H
#define INPUT_H

char *editorPrompt(struct editorConfig* e,char *prompt);
void editorMoveCursor(int key,struct editorConfig* e);
void editorProcessKeypress(struct editorConfig* e);

#endif