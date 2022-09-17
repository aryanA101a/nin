#include"data.h"

int editorRowCxToRx(erow *row, int cx);
int editorRowRxToCx(erow* row,int rx);
void editorUpdateRow(erow *row);
void editorInsertRow(int at, char *s, size_t len,struct editorConfig* e);
void editorFreeRow(erow *row);
void editorDelRow(int at,struct editorConfig* e);
void editorRowInsertChar(erow *row, int at, int c,struct editorConfig* e);
void editorRowDelChar(erow *row, int at,struct editorConfig* e);
void editorRowAppendString(erow *row, char *s, size_t len,struct editorConfig* e);