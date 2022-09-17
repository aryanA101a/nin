#include <string.h>

#include "data.h"
#include "input.h"
#include "row_operations.h"

void editorFind(struct editorConfig* e)
{
    char *query = editorPrompt(e,"Search: %s (ESC to cancel)");
    if (query == NULL)
        return;

    int i;
    for (i = 0; i < e->numrows; i++)
    {
        erow *row = &e->row[i];
        char *match = strstr(row->render, query);
        if (match)
        {
            e->cy = i;
            e->cx = editorRowRxToCx(row, match - row->render);
            e->rowoff = e->numrows;
            break;
        }
    }
}