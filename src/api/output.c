#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "row_operations.h"
#include "defined.h"
#include "../utils/append_buffer.h"

void editorScroll(struct editorConfig* e)
{
    e->rx = 0;
    if (e->cy < e->numrows)
    {
        e->rx = editorRowCxToRx(&e->row[e->cy], e->cx);
    }

    // vertical scroll
    if (e->cy < e->rowoff)
    {
        e->rowoff = e->cy;
    }
    if (e->cy >= e->rowoff + e->screenrows)
    {
        e->rowoff = e->cy - e->screenrows + 1;
    }

    // horizontal scroll
    if (e->rx < e->coloff)
    {
        e->coloff = e->rx;
    }
    if (e->rx >= e->coloff + e->screencols)
    {
        e->coloff = e->rx - e->screencols + 1;
    }
}

void editorDrawRows(struct abuf *ab,struct editorConfig* e)
{
    int y;

    for (y = 0; y < e->screenrows; y++)
    {
        int filerow = y + e->rowoff;
        if (filerow >= e->numrows)
        {
            if (e->numrows == 0 && y == e->screenrows / 3)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome), "nin editor -- version %s", NIN_VERSION);
                if (welcomelen > e->screencols)
                    welcomelen = e->screencols;

                int padding = (e->screencols - welcomelen) / 2;
                if (padding)
                {
                    abAppend(ab, "~", 1);
                    padding--;
                }
                while (padding--)
                    abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            }
            else
            {
                abAppend(ab, "~", 1);
            }
        }
        else
        {
            int len = e->row[filerow].rsize - e->coloff;
            if (len < 0)
                len = 0;
            if (len > e->screencols)
                len = e->screencols;
            abAppend(ab, &e->row[filerow].render[e->coloff], len);
        }

        abAppend(ab, "\x1b[K", 3); // clear line

        abAppend(ab, "\r\n", 2); // carriage return and next line
    }
}
void editorDrawStatusBar(struct abuf *ab,struct editorConfig e)
{
    abAppend(ab, "\x1b[7m", 4); // invert the colors
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s", e.filename ? e.filename : "[No Name]", e.numrows, e.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", e.cy + 1, e.numrows);
    if (len > e.screencols)
        len = e.screencols;
    abAppend(ab, status, len);

    while (len < e.screencols)
    {
        if (e.screencols - len == rlen)
        {
            abAppend(ab, rstatus, rlen);
            break;
        }
        else
        {
            abAppend(ab, " ", 1);
            len++;
        }
    }
    abAppend(ab, "\x1b[m", 3); // reset inversion of color
    abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab,struct editorConfig e)
{
    abAppend(ab, "\x1b[K", 3);
    int msglen = strlen(e.statusmsg);
    if (msglen > e.screencols)
        msglen = e.screencols;
    if (msglen && time(NULL) - e.statusmsg_time < 5)
        abAppend(ab, e.statusmsg, msglen);
}
void editorRefreshScreen(struct editorConfig* e)
{
    editorScroll(e);

    struct abuf ab = ABUF_INIT;

    abAppend(&ab, "\x1b[?25l", 6); // hide cursor
    abAppend(&ab, "\x1b[H", 3);    // cursor to top left

    editorDrawRows(&ab,e);
    editorDrawStatusBar(&ab,*e);
    editorDrawMessageBar(&ab,*e);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (e->cy - e->rowoff) + 1, (e->rx - e->coloff) + 1); // updates the position of cursor/
    abAppend(&ab, buf, strlen(buf));

    abAppend(&ab, "\x1b[?25h", 6); // show cursor

    write(STDOUT_FILENO, ab.buf, ab.len);
    abFree(&ab);
}
void editorSetStatusMessage(struct editorConfig *e,const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(e->statusmsg, sizeof(e->statusmsg), fmt, ap);
    va_end(ap);
    e->statusmsg_time = time(NULL);
}