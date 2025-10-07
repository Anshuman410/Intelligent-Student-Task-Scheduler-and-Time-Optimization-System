#include <stdio.h>
#include <stdlib.h>
#include "task.c"
#include "task.h"
#include "undo_redo.c"
#include "undo_redo.h"
#include "schedule.c"
#include "schedule.h"
#include "flow_functions.c"
#include "flow_functions.h"


// Flow function prototypes
void add_task_flow(Task **head);
void view_tasks(Task *head);
void mark_complete_flow(Task *head);
void delete_task_flow(Task **head);
void generate_schedule_flow(Task *head);
void undo_action_flow(Task **head);
void redo_action_flow(Task **head);
void exit_program(Task **head);

void menu() {
    printf("\n==============================\n");
    printf("   TASK SCHEDULER & OPTIMISER\n");
    printf("==============================\n");
    printf("1. Add New Task\n");
    printf("2. View All Tasks\n");
    printf("3. Mark Task Completed\n");
    printf("4. Delete Task\n");
    printf("5. Generate Schedule\n");
    printf("6. Detect Conflicts\n");
    printf("7. Save Weekly Summary\n");
    printf("8. Undo Last Action\n");
    printf("9. Redo Last Action\n");
    printf("0. Exit\n");
    printf("------------------------------\n");
    printf("Enter your choice: ");
}

int main() {
    Task *head = NULL;
    init_undo_system();

    int choice;
    while (1) {
        menu();
        scanf("%d", &choice);
        getchar(); // consume newline

        switch(choice) {
            case 1: add_task_flow(&head); break;
            case 2: view_tasks(head); break;
            case 3: mark_complete_flow(head); break;
            case 4: delete_task_flow(&head); break;
            case 5: generate_schedule_flow(head); break;
            case 6: detect_conflicts(head); break;
            case 7: save_weekly_summary(head); break;
            case 8: undo_action_flow(&head); break;
            case 9: redo_action_flow(&head); break;
            case 0: exit_program(&head); break;

            default: printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}
