#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
/*** define ***/
#define CTRL_KEY(k) ((k)&0x1f)

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
void die(const char *s)
{
    perror(s);
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

// by default the terminal is in canonical mode
// which means that it takes input line by line only when we press enter
// One more thing, ECHO feature in terminal causes it to display what we type
// but to get control over the interface we need to kick it off and implement something of our own.
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr"); // store terminal atttributes in orig_temios
    atexit(disableRawMode);

    struct termios raw = orig_termios; // create a local copy of terminal attributes to make some changes

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

/*** init ***/
int main()
{
    enableRawMode();
    while (1)
    {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1)
            die("read");

        if (iscntrl(c))
        {
            printf("%d\r\n", c);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == CTRL_KEY('q'))
            break;
    };
    return 0;
}
