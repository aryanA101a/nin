#include <time.h>
#include <termios.h>

#ifndef DATA_H
#define DATA_H

// editor row - stores a line of text
typedef struct erow
{
    int size;
    int rsize;
    char *chars;
    char *render;

} erow;

struct editorConfig
{
    int cx, cy;
    int rx;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int numrows;
    erow *row;
    int dirty;
    char *filename;
    char statusmsg[80];
    time_t statusmsg_time;
    struct termios orig_termios;
};

#endif