#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include<string.h>

/*** define ***/
#define CTRL_KEY(k) ((k)&0x1f)

/*** data ***/
struct editorConfig
{
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;

/*** terminal ***/
void die(const char *s)
{

    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    printf("\r");
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

// by default the terminal is in canonical mode
// which means that it takes input line by line only when we press enter
// One more thing, ECHO feature in terminal causes it to display what we type
// but to get control over the interface we need to kick it off and implement something of our own.
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr"); // store terminal atttributes in orig_temios
    atexit(disableRawMode);

    struct termios raw = E.orig_termios; // create a local copy of terminal attributes to make some changes

    // lflag->local flags
    // flags are set in binary, so we use bit manipulation to take them out
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cflag |= (CS8);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr"); // set terminal attributes
}

char editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1)
            die("read");
    }
    return c;
}

int getCursorPosition(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';
    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;
    return 0;
}

int getWindowSize(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        return getCursorPosition(rows, cols);
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** append buffer ***/
struct abuf
{
    char *buf;
    int len;
};

#define ABUF_INIT \
    {             \
        NULL, 0   \
    }

void abAppend(struct abuf *ab, const char *s, int len)
{
    char *new = realloc(ab->buf, ab->len + len);

    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, len);
    ab->buf = new;
    ab->len += len;
}

void abFree(struct abuf *ab)
{
    free(ab->buf);
}
/*** input ***/
void editorProcessKeypress()
{
    char c = editorReadKey();
    // printf("%c\r\n", c);
    switch (c)
    {
    case CTRL_KEY('q'):
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);

        exit(0);
        break;
    }
}

/*** output ***/
void editorDrawRows(struct abuf *ab)
{
    int y;
    for (y = 0; y < E.screenrows; y++)
    {

        
        abAppend(ab, "~", 1);

        abAppend(ab,"\x1b[K",3);//clear line
        if (y < E.screenrows - 1)
        {
            abAppend(ab, "\r\n", 2);//carriage return and next line
        }
    }
}
void editorRefreshScreen()
{
    struct abuf ab=ABUF_INIT;
    abAppend(&ab, "\x1b[?25l", 6);//hide cursor
    
    abAppend(&ab, "\x1b[H", 3);  // cursor to top left

    editorDrawRows(&ab);
    abAppend(&ab, "\x1b[H", 3);
    abAppend(&ab, "\x1b[?25h", 6);//show cursor

    write(STDOUT_FILENO,ab.buf,ab.len);
    abFree(&ab);

}

/*** init ***/

void initEditor()
{
    if (getWindowSize(&E.screenrows, &E.screencols) == -1)
        die("getWindowSize");
}
int main()
{
    enableRawMode();
    initEditor();
    while (1)
    {
        editorRefreshScreen();

        editorProcessKeypress();
    }
    return 0;
}
