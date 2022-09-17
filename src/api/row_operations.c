#include <stdlib.h>
#include <string.h>

#include "defined.h"
#include "data.h"

int editorRowCxToRx(erow *row, int cx)
{
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++)
    {
        if (row->chars[j] == '\t')
            rx += (NIN_TAB_STOP - 1) - (rx % NIN_TAB_STOP);
        rx++;
    }
    return rx;
}
int editorRowRxToCx(erow* row,int rx){
    int cur_rx=0;
    int cx;
    for(cx=0;cx<row->size;cx++){
        if(row->chars[cx]=='\t')
        cur_rx+=(NIN_TAB_STOP-1)-(cur_rx%NIN_TAB_STOP);
        cur_rx++;

        if(cur_rx>rx)return cx;
    }
    return cx;
}

void editorUpdateRow(erow *row)
{
    int tabs = 0;
    int j = 0;

    // count tabs
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
            tabs++;
    }

    free(row->render);
    row->render = malloc(row->size + tabs * (NIN_TAB_STOP) + 1);

    int idx = 0;
    // add spaces in places of '/t'
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % NIN_TAB_STOP != 0)
                row->render[idx++] = ' ';
        }
        else
        {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}
void editorInsertRow(int at, char *s, size_t len,struct editorConfig* e)
{
    e->row = realloc(e->row, sizeof(erow) * (e->numrows + 1));
    memmove(&e->row[at + 1], &e->row[at], sizeof(erow) * (e->numrows - at));

    e->row[at].size = len;
    e->row[at].chars = malloc(len + 1);
    memcpy(e->row[at].chars, s, len);
    e->row[at].chars[len] = '\0';

    e->row[at].rsize = 0;
    e->row[at].render = NULL;
    editorUpdateRow(&e->row[at]);

    e->numrows++;
    e->dirty++;
}

void editorFreeRow(erow *row)
{
    free(row->render);
    free(row->chars);
}

void editorDelRow(int at,struct editorConfig* e)
{
    if (at < 0 || at >= e->numrows)
        return;
    editorFreeRow(&e->row[at]);
    memmove(&e->row[at], &e->row[at + 1], sizeof(erow) * (e->numrows - at - 1));
    e->numrows--;
    e->dirty++;
}

void editorRowInsertChar(erow *row, int at, int c,struct editorConfig* e)
{
    if (at < 0 || at > row->size)
        at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
    e->dirty++;
}

void editorRowDelChar(erow *row, int at,struct editorConfig* e)
{
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editorUpdateRow(row);
    e->dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len,struct editorConfig* e)
{
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    e->dirty++;
}