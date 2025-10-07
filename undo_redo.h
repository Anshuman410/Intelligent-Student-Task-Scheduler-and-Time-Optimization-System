#ifndef UNDO_REDO_H
#define UNDO_REDO_H

#include "task.h"

#define ACTION_ADD 1
#define ACTION_DELETE 2
#define ACTION_UPDATE 3
#define MAX_STACK 200

typedef struct {
    int type;
    Task snapshot;
} Action;

void init_undo_system(void);
void push_undo(Action a);
int pop_undo(Action *out);
void push_redo(Action a);
int pop_redo(Action *out);
void clear_redo(void);

#endif
