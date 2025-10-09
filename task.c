#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

static int next_id = 1;

int get_next_task_id(void) {
    return next_id++;
}

Task* add_task(Task **head, const char *title, const char *desc, const char *cat, const char *deadline, int priority) {
    Task *t = (Task*)malloc(sizeof(Task));
    if (!t) {
        MessageBox(NULL, "Memory allocation failed while adding task!", "Error", MB_OK | MB_ICONERROR);
        return NULL;
    }
    t->id = get_next_task_id();
    strncpy(t->title, title, MAX_TITLE-1); t->title[MAX_TITLE-1] = '\0';
    strncpy(t->description, desc, MAX_DESC-1); t->description[MAX_DESC-1] = '\0';
    strncpy(t->category, cat, MAX_CATEGORY-1); t->category[MAX_CATEGORY-1] = '\0';
    strncpy(t->deadline, deadline, 19); t->deadline[19] = '\0';
    t->priority = priority;
    t->status = 0;
    t->next = *head;
    *head = t;

    MessageBox(NULL, "Task added successfully!", "Success", MB_OK);
    return t;
}

Task* find_task_by_id(Task *head, int id) {
    Task *cur = head;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

Task* remove_task(Task **head, int id) {
    Task *cur = *head, *prev = NULL;
    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next;
            else *head = cur->next;
            cur->next = NULL;
            MessageBox(NULL, "Task removed successfully!", "Info", MB_OK);
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    MessageBox(NULL, "Task not found!", "Error", MB_OK | MB_ICONERROR);
    return NULL;
}

void mark_task_completed(Task *t) {
    if (t) {
        t->status = 1;
        MessageBox(NULL, "Task marked as completed!", "Success", MB_OK);
    }
}

void free_all_tasks(Task *head) {
    Task *cur = head;
    while (cur) {
        Task *n = cur->next;
        free(cur);
        cur = n;
    }
}

/* File persistence */
void save_tasks_to_file(Task *head, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        MessageBox(NULL, "Unable to save tasks to file!", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    Task *cur = head;
    while (cur) {
        fprintf(f, "%d|%s|%s|%s|%s|%d|%d\n",
                cur->id, cur->title, cur->description, cur->category, cur->deadline, cur->priority, cur->status);
        cur = cur->next;
    }
    fclose(f);
    MessageBox(NULL, "Tasks saved to file successfully!", "Success", MB_OK);
}

void load_tasks_from_file(Task **head, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        MessageBox(NULL, "No saved tasks found. Starting fresh.", "Info", MB_OK | MB_ICONINFORMATION);
        return;
    }
    free_all_tasks(*head);
    *head = NULL;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        int id, prio, status;
        char title[MAX_TITLE], desc[MAX_DESC], cat[MAX_CATEGORY], deadline[20];
        if (sscanf(line, "%d|%63[^|]|%255[^|]|%31[^|]|%19[^|]|%d|%d",
                   &id, title, desc, cat, deadline, &prio, &status) == 7) {
            Task *t = (Task*)malloc(sizeof(Task));
            if (!t) continue;
            t->id = id;
            strncpy(t->title, title, MAX_TITLE); t->title[MAX_TITLE-1] = '\0';
            strncpy(t->description, desc, MAX_DESC); t->description[MAX_DESC-1] = '\0';
            strncpy(t->category, cat, MAX_CATEGORY); t->category[MAX_CATEGORY-1] = '\0';
            strncpy(t->deadline, deadline, 20); t->deadline[19] = '\0';
            t->priority = prio; 
            t->status = status;
            t->next = *head; 
            *head = t;
            if (id >= next_id) next_id = id + 1;
        }
    }
    fclose(f);
    MessageBox(NULL, "Tasks loaded successfully!", "Success", MB_OK);
}
