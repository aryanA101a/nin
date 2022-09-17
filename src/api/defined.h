#ifndef DEFINED_H
#define DEFINED_H

#define NIN_VERSION "0.0.1"
#define NIN_TAB_STOP 8

#define CTRL_KEY(k) ((k)&0x1f)

enum editorKey
{
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
};

#endif