#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "undo_redo.h"
#include "schedule.h"

/* Add Task Flow */
void add_task_flow(Task **head) {
    char title[64], desc[256], cat[32], deadline[20];
    int priority;
    printf("Enter title: ");
    fgets(title, sizeof(title), stdin);
    title[strcspn(title, "\n")] = '\0';
    printf("Enter description: ");
    fgets(desc, sizeof(desc), stdin);
    desc[strcspn(desc, "\n")] = '\0';
    printf("Enter category: ");
    fgets(cat, sizeof(cat), stdin);
    cat[strcspn(cat, "\n")] = '\0';
    printf("Enter deadline (YYYY-MM-DD): ");
    fgets(deadline, sizeof(deadline), stdin);
    deadline[strcspn(deadline, "\n")] = '\0';
    printf("Enter priority (1=High, 2=Medium, 3=Low): ");
    scanf("%d", &priority);
    getchar(); // consume newline

    Task *t = add_task(head, title, desc, cat, deadline, priority);
    if (t) {
        Action a = {ACTION_ADD, *t};
        push_undo(a);
        clear_redo();
        printf("Task added successfully!\n");
    }
}
void view_tasks(Task *head) {
    if (!head) {
        printf("No tasks to show.\n");
        return;
    }
    printf("\n--- TASK LIST ---\n");
    for (Task *cur = head; cur; cur = cur->next) {
        printf("ID: %d | %s | %s | Deadline: %s | Priority: %d | Status: %s\n",
               cur->id, cur->title, cur->category, cur->deadline,
               cur->priority, cur->status ? "Completed" : "Pending");
    }
}

void mark_complete_flow(Task *head) {
    int id;
    printf("Enter task ID to mark complete: ");
    scanf("%d", &id);
    getchar();

    Task *t = find_task_by_id(head, id);
    if (!t) {
        printf("Task not found.\n");
        return;
    }

    Action a = {ACTION_UPDATE, *t}; // store snapshot before change
    t->status = 1;                   // mark completed
    push_undo(a);
    clear_redo();
    printf("Task marked completed!\n");
}

void delete_task_flow(Task **head) {
    int id;
    printf("Enter task ID to delete: ");
    scanf("%d", &id);
    getchar();

    Task *deleted = remove_task(head, id);
    if (!deleted) {
        printf("Task not found.\n");
        return;
    }

    Action a = {ACTION_DELETE, *deleted};
    push_undo(a);
    clear_redo();
    free(deleted);
    printf("Task deleted successfully!\n");
}

void generate_schedule_flow(Task *head) {
    int count;
    Task **sched = generate_schedule(head, &count);
    if (!count) {
        printf("No tasks to schedule.\n");
        return;
    }
    printf("\n--- SCHEDULED TASKS ---\n");
    for (int i = 0; i < count; i++)
        printf("%d. %s | Deadline: %s | Priority: %d\n",
               sched[i]->id, sched[i]->title, sched[i]->deadline, sched[i]->priority);
    free(sched);
}

void undo_action_flow(Task **head) {
    Action a;
    if (!pop_undo(&a)) {
        printf("No action to undo.\n");
        return;
    }

    switch(a.type) {
        case ACTION_ADD: { // undo add => remove task
            Task *t = remove_task(head, a.snapshot.id);
            if (t) free(t);
            break;
        }
        case ACTION_DELETE: { // undo delete => add task back
            Task *t = add_task(head, a.snapshot.title, a.snapshot.description,
                                a.snapshot.category, a.snapshot.deadline,
                                a.snapshot.priority);
            if (t) t->status = a.snapshot.status;
            break;
        }
        case ACTION_UPDATE: { // undo mark complete => revert status
            Task *t = find_task_by_id(*head, a.snapshot.id);
            if (t) t->status = a.snapshot.status;
            break;
        }
    }
    push_redo(a);
    printf("Undo performed for action type %d.\n", a.type);
}

void redo_action_flow(Task **head) {
    Action a;
    if (!pop_redo(&a)) {
        printf("No action to redo.\n");
        return;
    }

    switch(a.type) {
        case ACTION_ADD: { // redo add => add task back
            Task *t = add_task(head, a.snapshot.title, a.snapshot.description,
                                a.snapshot.category, a.snapshot.deadline,
                                a.snapshot.priority);
            if (t) t->status = a.snapshot.status;
            break;
        }
        case ACTION_DELETE: { // redo delete => remove task
            Task *t = remove_task(head, a.snapshot.id);
            if (t) free(t);
            break;
        }
        case ACTION_UPDATE: { // redo update => apply status again
            Task *t = find_task_by_id(*head, a.snapshot.id);
            if (t) t->status = 1; // redo completed
            break;
        }
    }
    push_undo(a);
    printf("Redo performed for action type %d.\n", a.type);
}

void exit_program(Task **head) {
    free_all_tasks(*head);  // free all tasks
    printf("Exiting system...\n");
    exit(0);
}

