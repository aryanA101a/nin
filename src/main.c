#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE



#include "api/data.h"
#include "api/terminal.h"
#include"api/file_io.h"
#include "api/input.h"
#include "api/output.h"


struct editorConfig E;


void initEditor()
{
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.numrows = 0;
    E.row = NULL;
    E.dirty = 0;
    E.filename = NULL;
    E.statusmsg[0] = '\0';
    E.statusmsg_time = 0;

    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindowSize");
    E.screenrows -= 2;
}
int main(int argc, char *argv[])
{
    terminalInitOrigTermios(&E.orig_termios);
    enableRawMode();
    initEditor();
    if (argc >= 2)
    {
        editorOpen(argv[1],&E);
    }

    editorSetStatusMessage(&E,"HELP: Ctrl+S = save | Ctrl+Q = quit | Ctrl+F = find");

    while (1)
    {
        editorRefreshScreen(&E);

        editorProcessKeypress(&E);
    }
    return 0;
}
