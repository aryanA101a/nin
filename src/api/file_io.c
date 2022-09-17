#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "data.h"
#include "output.h"
#include "input.h"
#include "row_operations.h"
#include "terminal.h"
#include "editor_operations.h"

char *editorRowsToString(int *buflen, struct editorConfig e)
{
    int totlen = 0;
    int j;
    for (j = 0; j < e.numrows; j++)
    {
        totlen += e.row[j].size + 1;
    }
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < e.numrows; j++)
    {
        memcpy(p, e.row[j].chars, e.row[j].size);
        p += e.row[j].size;
        *p = '\n';
        p++;
    }

    return buf;
}

void editorOpen(char *filename, struct editorConfig *e)
{
    free(e->filename);
    e->filename = strdup(filename);

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    while ((linelen = getline(&line, &linecap, fp)) != -1)
    {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;

        editorInsertRow(e->numrows, line, linelen, e);
    }
    free(line);
    fclose(fp);
    e->dirty = 0;
}

void editorSave(struct editorConfig *e)
{
    if (e->filename == NULL)
    {
        e->filename = editorPrompt(e,"Save as:%s (ESC to cancel)");
        if (e->filename == NULL)
        {
            editorSetStatusMessage(e,"Save aborted");
            return;
        }
    }

    int len;
    char *buf = editorRowsToString(&len, *e);

    int fd = open(e->filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1)
    {
        if (ftruncate(fd, len) != -1) // sets file size to specified length
        {
            if (write(fd, buf, len) == len)
            {
                close(fd);
                free(buf);
                e->dirty = 0;
                editorSetStatusMessage(e,"%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }
    free(buf);
    editorSetStatusMessage(e,"Can't save! I/O error: %s", strerror(errno));
}