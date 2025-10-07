#ifndef TASK_H
#define TASK_H

#define MAX_TITLE 64
#define MAX_DESC 256
#define MAX_CATEGORY 32

typedef struct Task {
    int id;
    char title[MAX_TITLE];
    char description[MAX_DESC];
    char category[MAX_CATEGORY];
    char deadline[20]; // YYYY-MM-DD
    int priority; // 1-high, 2-med, 3-low
    int status;   // 0-pending,1-completed
    struct Task *next;
} Task;

/* Task operations */
Task* add_task(Task **head, const char *title, const char *desc, const char *cat, const char *deadline, int priority);
Task* find_task_by_id(Task *head, int id);
Task* remove_task(Task **head, int id);
void mark_task_completed(Task *t);
void free_all_tasks(Task *head);

/* persistence */
void save_tasks_to_file(Task *head, const char *filename);
void load_tasks_from_file(Task **head, const char *filename);

/* helper */
int get_next_task_id(void);

#endif