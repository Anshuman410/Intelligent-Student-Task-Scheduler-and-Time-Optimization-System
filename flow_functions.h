#ifndef FLOW_FUNCTIONS_H
#define FLOW_FUNCTIONS_H

#include "task.h"

void add_task_flow(Task **head);
void view_tasks(Task *head);
void mark_complete_flow(Task *head);
void delete_task_flow(Task **head);
void generate_schedule_flow(Task *head);
void undo_action_flow(Task **head);
void redo_action_flow(Task **head);
void exit_program(Task **head);

#endif
