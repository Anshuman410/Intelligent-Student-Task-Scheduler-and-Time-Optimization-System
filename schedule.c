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

    for (Task *t = head; t; t = t->next)
        n++;

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

void detect_conflicts(Task *head) {
    int n = 0;
    for (Task *t = head; t; t = t->next) n++;
    if (n < 2) {
        printf("No conflicts.\n");
        return;
    }

    Task **arr = malloc(n * sizeof(Task*));
    if (!arr) return;

    int i = 0;
    for (Task *t = head; t; t = t->next)
        arr[i++] = t;

    qsort(arr, n, sizeof(Task*), compare_tasks);

    int found = 0;
    for (i = 1; i < n; i++) {
        if (strcmp(arr[i-1]->deadline, arr[i]->deadline) == 0) {
            if (found == 0) printf("\nConflicts found:\n");
            found = 1;
            printf(" - Task %d (%s) and Task %d (%s) have same deadline: %s\n",
                   arr[i-1]->id, arr[i-1]->title,
                   arr[i]->id, arr[i]->title,
                   arr[i]->deadline);
        }
    }

    if (found == 0)
        printf("No deadline conflicts.\n");

    free(arr);
}

int get_day_from_deadline(const char *deadline) {
    if (deadline == NULL) return 1;  

    int day = (deadline[8] - '0') * 10 + (deadline[9] - '0');

    if (day < 1 || day > 31) day = 1;

    return day;
}

void save_week_summary(Task *head) {
    FILE *f = fopen("data.txt", "w");
    if (f == 0) return;

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
    printf("Weekly summary saved !!!\n");
}
