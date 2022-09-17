#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "append_buffer.h"


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