#include "../utils/append_buffer.h"

#ifndef OUTPUT_H
#define OUTPUT_H 

void editorScroll(struct editorConfig* e);
void editorDrawRows(struct abuf *ab,struct editorConfig* e);
void editorDrawStatusBar(struct abuf *ab,struct editorConfig e);
void editorDrawMessageBar(struct abuf *ab,struct editorConfig e);
void editorRefreshScreen(struct editorConfig* e);
void editorSetStatusMessage(struct editorConfig *e,const char *fmt, ...);

#endif