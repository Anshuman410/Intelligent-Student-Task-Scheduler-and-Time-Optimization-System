#include <string.h>
#include "undo_redo.h"

static Action undo_stack[MAX_STACK];
static int undo_top = 0;
static Action redo_stack[MAX_STACK];
static int redo_top = 0;

void init_undo_system(void) {
    undo_top = 0; redo_top = 0;
}

void push_undo(Action a) {
    if (undo_top < MAX_STACK) undo_stack[undo_top++] = a;
}

int pop_undo(Action *out) {
    if (undo_top == 0) return 0;
    *out = undo_stack[--undo_top];
    return 1;
}

void push_redo(Action a) {
    if (redo_top < MAX_STACK) redo_stack[redo_top++] = a;
}

int pop_redo(Action *out) {
    if (redo_top == 0) return 0;
    *out = redo_stack[--redo_top];
    return 1;
}

void clear_redo(void) { 
    redo_top = 0; 
}
