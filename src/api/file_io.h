#ifndef FILE_IO_H
#define FILE_IO_H

char *editorRowsToString(int *buflen,struct editorConfig e);
void editorOpen(char *filename,struct editorConfig* e);
void editorSave(struct editorConfig* e);

#endif