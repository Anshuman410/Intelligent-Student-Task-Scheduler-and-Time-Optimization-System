#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "task.h"

/* Generate an array of Task pointers sorted by priority then deadline.
   Returns the number of tasks in taskCount. Caller must free the returned array. */
Task** generate_schedule(Task *head, int *taskCount);

/* Detect tasks having the same deadlines and show conflicts in a GUI dialog */
void detect_conflicts(Task *head);

/* Save a simple weekly summary report of tasks to a file and notify user with GUI */
void save_week_summary(Task *head);

/* Extract day (DD) from deadline string "YYYY-MM-DD" */
int get_day_from_deadline(const char *deadline);

#endif
