// schedule.c  (GUI-friendly)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"

static int compare_tasks(const void *a, const void *b) {
    Task *ta = *(Task**)a;
    Task *tb = *(Task**)b;

    if (ta->priority != tb->priority)
        return ta->priority - tb->priority;

    return strcmp(ta->deadline, tb->deadline);
}

Task** generate_schedule(Task *head, int *taskCount) {
    int n = 0;
    for (Task *t = head; t; t = t->next) n++;

    *taskCount = n;
    if (n == 0) return NULL;

    Task **arr = malloc(n * sizeof(Task*));
    if (arr == NULL) return NULL;

    int i = 0;
    for (Task *t = head; t; t = t->next)
        arr[i++] = t;

    qsort(arr, n, sizeof(Task*), compare_tasks);
    return arr;
}

/* Detect tasks having the same deadlines and show conflicts using GUI MessageBox */
void detect_conflicts(Task *head) {
    int n = 0;
    for (Task *t = head; t; t = t->next) n++;
    if (n < 2) {
        MessageBoxA(NULL, "No conflicts.", "Conflicts", MB_OK | MB_ICONINFORMATION);
        return;
    }

    Task **arr = malloc(n * sizeof(Task*));
    if (!arr) {
        MessageBoxA(NULL, "Memory error while detecting conflicts.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    int i = 0;
    for (Task *t = head; t; t = t->next)
        arr[i++] = t;

    qsort(arr, n, sizeof(Task*), compare_tasks);

    /* Build a message showing conflicts */
    size_t cap = 2048;
    char *buf = (char*)malloc(cap);
    if (!buf) { free(arr); return; }
    buf[0] = '\0';

    int found = 0;
    for (i = 1; i < n; i++) {
        if (strcmp(arr[i-1]->deadline, arr[i]->deadline) == 0) {
            if (!found) {
                strcat(buf, "Conflicts found:\n\n");
                found = 1;
            }
            char line[512];
            snprintf(line, sizeof(line),
                     "Task %d (%s)  <-->  Task %d (%s)\n  Deadline: %s\n\n",
                     arr[i-1]->id, arr[i-1]->title,
                     arr[i]->id, arr[i]->title,
                     arr[i]->deadline);
            /* ensure capacity */
            if (strlen(buf) + strlen(line) + 1 > cap) {
                cap *= 2;
                char *tmp = (char*)realloc(buf, cap);
                if (!tmp) break;
                buf = tmp;
            }
            strcat(buf, line);
        }
    }

    if (!found) {
        MessageBoxA(NULL, "No deadline conflicts.", "Conflicts", MB_OK | MB_ICONINFORMATION);
    } else {
        /* If buffer is very large MessageBox may truncate; still ok for moderate lists.
           If you expect very long output we can show a scrollable dialog instead. */
        MessageBoxA(NULL, buf, "Conflicts", MB_OK | MB_ICONWARNING);
    }

    free(buf);
    free(arr);
}

/* Extract day (DD) from deadline string "YYYY-MM-DD" */
int get_day_from_deadline(const char *deadline) {
    if (deadline == NULL) return 1;

    int day = (deadline[8] - '0') * 10 + (deadline[9] - '0');
    if (day < 1 || day > 31) day = 1;
    return day;
}

/* Save a simple weekly summary report of tasks to a file and inform user via MessageBox */
void save_week_summary(Task *head) {
    FILE *f = fopen("data.txt", "w");
    if (f == 0) {
        MessageBoxA(NULL, "Unable to create summary file (permission or disk issue).", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    int week_total[6];
    int week_completed[6];

    for (int i = 0; i < 6; i++) {
        week_total[i] = 0;
        week_completed[i] = 0;
    }

    for (Task *t = head; t; t = t->next) {
        int day = get_day_from_deadline(t->deadline);
        int week = (day - 1) / 7; // week 0–5
        if (week < 0) week = 0;
        if (week > 5) week = 5;

        week_total[week]++;
        if (t->status) week_completed[week]++;
    }

    for (int w = 0; w < 6; w++) {
        if (week_total[w])
            fprintf(f, "Week %d: Total %d | Completed %d\n",
                    w + 1, week_total[w], week_completed[w]);
    }

    fclose(f);

    MessageBoxA(NULL, "Weekly summary saved to data.txt", "Summary Saved", MB_OK | MB_ICONINFORMATION);
}
