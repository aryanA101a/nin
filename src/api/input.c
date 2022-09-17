#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "defined.h"
#include "terminal.h"
#include "editor_operations.h"
#include "data.h"
#include "find.h"
#include "file_io.h"
#include "output.h"


char *editorPrompt(struct editorConfig* e,char *prompt)
{
    size_t bufsize = 128;
    char *buf = malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while (1)
    {
        editorSetStatusMessage(e,prompt, buf);
        editorRefreshScreen(e);

        int c = editorReadKey();

        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE)
        {
            if (buflen != 0)
                buf[--buflen] = '\0';
        }
        else if (c == '\x1b')
        {
            editorSetStatusMessage(e,"");
            free(buf);
            return NULL;
        }
        else if (c == '\r')
        {
            if (buflen != 0)
            {
                editorSetStatusMessage(e,"");
                return buf;
            }
        }
        else if (!iscntrl(c) && c < 128)
        {
            if (buflen == bufsize - 1)
            {
                buflen *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
    }
}

void editorMoveCursor(int key,struct editorConfig* e)
{
    erow *row = (e->cy >= e->numrows) ? NULL : &e->row[e->cy];
    switch (key)
    {
    case ARROW_LEFT:
        if (e->cx != 0)
        {
            e->cx--;
        }
        else if (e->cy > 0) // move to previous line
        {
            e->cy--;
            e->cx = e->row[e->cy].size;
        }
        break;
    case ARROW_RIGHT:
        if (row && e->cx < row->size)
        {
            e->cx++;
        }
        else if (row && e->cx == row->size) // move to next after the end of line
        {
            e->cy++;
            e->cx = 0;
        }
        break;
    case ARROW_UP:
        if (e->cy != 0)
            e->cy--;
        break;
    case ARROW_DOWN:
        if (e->cy < e->numrows)
            e->cy++;
        break;
    }

    // snap cursor to end of line
    row = (e->cy >= e->numrows) ? NULL : &e->row[e->cy];
    int rowlen = row ? row->size : 0;
    if (e->cx > rowlen)
    {
        e->cx = rowlen;
    }
}
void editorProcessKeypress(struct editorConfig* e)
{
    static int quit_times = 2;
    int c = editorReadKey();

    switch (c)
    {
    case '\r':
        editorInsertNewline(e);
        break;

    case CTRL_KEY('q'):
        if (e->dirty && quit_times > 0)
        {
            editorSetStatusMessage(e,"Warning!!! File has unsaved changes. "
                                   "Press Ctrl-Q %d more times to quit.",
                                   quit_times);
            quit_times--;
            return;
        }
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);

        exit(0);
        break;

    case CTRL_KEY('s'):
        editorSave(e);
        break;

    case HOME_KEY:
        e->cx = 0;
        break;
    case END_KEY:
        if (e->cy < e->numrows)
            e->cx = e->row[e->cy].size;
        break;

    case CTRL_KEY('f'):
        editorFind(e);
        break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (c == DEL_KEY)
            editorMoveCursor(ARROW_RIGHT,e);
        editorDelChar(e);
        break;

    case PAGE_UP:
    case PAGE_DOWN:
    {
        if (c == PAGE_UP)
        {
            e->cy = e->rowoff;
        }
        else if (c == PAGE_DOWN)
        {
            e->cy = e->rowoff + e->screenrows - 1;
            if (e->cy > e->numrows)
                e->cy = e->numrows;
        }
        int times = e->screenrows;
        while (times--)
        {
            editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN,e);
        }
        break;
    }
    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        editorMoveCursor(c,e);
        break;

    case CTRL_KEY('l'):
    case '\x1b':
        break;

    default:
        editorInsertChar(c,e);
        break;
    }
    quit_times = 2;
}