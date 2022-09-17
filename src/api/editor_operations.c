#include "data.h"
#include "row_operations.h"

void editorInsertChar(int c,struct editorConfig* e)
{
    if (e->cy == e->numrows)
    {
        editorInsertRow(e->numrows, "", 0,e);
    }
    editorRowInsertChar(&e->row[e->cy], e->cx, c,e);
    e->cx++;
}
void editorInsertNewline(struct editorConfig* e)
{
    if (e->cx == 0)
    {
        editorInsertRow(e->cy, "", 0,e);
    }
    else
    {
        erow *row = &e->row[e->cy];
        editorInsertRow(e->cy + 1, &row->chars[e->cx], row->size - e->cx,e);
        row = &e->row[e->cy];
        row->size = e->cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(row);
    }
    e->cy++;
    e->cx = 0;
}
void editorDelChar(struct editorConfig* e)
{
    if (e->cy == e->numrows)
        return;
    if (e->cx == 0 && e->cy == 0)
        return;

    erow *row = &e->row[e->cy];
    if (e->cx > 0)
    {
        editorRowDelChar(row, e->cx - 1,e);
        e->cx--;
    }
    else
    {
        e->cx = e->row[e->cy - 1].size;
        editorRowAppendString(&e->row[e->cy - 1], row->chars, row->size,e);
        editorDelRow(e->cy,e);
        e->cy--;
    }
}
