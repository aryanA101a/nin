#include "data.h"

#ifndef EDITOR_OPERATIONS_H
#define EDITOR_OPERATIONS_H

void editorInsertChar(int c,struct editorConfig* e);
void editorInsertNewline(struct editorConfig* e);
void editorDelChar(struct editorConfig* e);

#endif