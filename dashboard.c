#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "dashboard.h"
#include "task.h"
#include "flow_functions.h"
#include "undo_redo.h"
#include "schedule.h"

#define ID_ADD_TASK 1
#define ID_VIEW_TASKS 2
#define ID_MARK_DONE 3
#define ID_DELETE_TASK 4
#define ID_GENERATE 5
#define ID_CONFLICTS 6
#define ID_SAVE_SUMMARY 7
#define ID_UNDO 8
#define ID_REDO 9
#define ID_EXIT 10

static Task *head = NULL; // Global task list

// Forward declaration
void exit_program(Task **head);

LRESULT CALLBACK DashboardProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            CreateWindow("BUTTON", "Add New Task", WS_VISIBLE | WS_CHILD, 50, 50, 200, 30, hwnd, (HMENU)ID_ADD_TASK, NULL, NULL);
            CreateWindow("BUTTON", "View All Tasks", WS_VISIBLE | WS_CHILD, 50, 90, 200, 30, hwnd, (HMENU)ID_VIEW_TASKS, NULL, NULL);
            CreateWindow("BUTTON", "Mark Completed", WS_VISIBLE | WS_CHILD, 50, 130, 200, 30, hwnd, (HMENU)ID_MARK_DONE, NULL, NULL);
            CreateWindow("BUTTON", "Delete Task", WS_VISIBLE | WS_CHILD, 50, 170, 200, 30, hwnd, (HMENU)ID_DELETE_TASK, NULL, NULL);
            CreateWindow("BUTTON", "Generate Schedule", WS_VISIBLE | WS_CHILD, 50, 210, 200, 30, hwnd, (HMENU)ID_GENERATE, NULL, NULL);
            CreateWindow("BUTTON", "Detect Conflicts", WS_VISIBLE | WS_CHILD, 50, 250, 200, 30, hwnd, (HMENU)ID_CONFLICTS, NULL, NULL);
            CreateWindow("BUTTON", "Save Weekly Summary", WS_VISIBLE | WS_CHILD, 50, 290, 200, 30, hwnd, (HMENU)ID_SAVE_SUMMARY, NULL, NULL);
            CreateWindow("BUTTON", "Undo Last Action", WS_VISIBLE | WS_CHILD, 50, 330, 200, 30, hwnd, (HMENU)ID_UNDO, NULL, NULL);
            CreateWindow("BUTTON", "Redo Last Action", WS_VISIBLE | WS_CHILD, 50, 370, 200, 30, hwnd, (HMENU)ID_REDO, NULL, NULL);
            CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 50, 410, 200, 30, hwnd, (HMENU)ID_EXIT, NULL, NULL);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_ADD_TASK:
                    add_task_flow(&head);
                    MessageBox(hwnd, "Task Added!", "Info", MB_OK);
                    break;
                case ID_VIEW_TASKS:
                    view_tasks(head);
                    break;
                case ID_MARK_DONE:
                    mark_complete_flow(head);
                    MessageBox(hwnd, "Task Updated!", "Info", MB_OK);
                    break;
                case ID_DELETE_TASK:
                    delete_task_flow(&head);
                    MessageBox(hwnd, "Task Deleted!", "Info", MB_OK);
                    break;
                case ID_GENERATE:
                    generate_schedule_flow(head);
                    break;
                case ID_CONFLICTS:
                    detect_conflicts(head);
                    break;
                case ID_SAVE_SUMMARY:
                    save_week_summary(head);
                    MessageBox(hwnd, "Summary Saved!", "Info", MB_OK);
                    break;
                case ID_UNDO:
                    undo_action_flow(&head);
                    MessageBox(hwnd, "Undo Done!", "Info", MB_OK);
                    break;
                case ID_REDO:
                    redo_action_flow(&head);
                    MessageBox(hwnd, "Redo Done!", "Info", MB_OK);
                    break;
                case ID_EXIT:
                    exit_program(&head);
                    PostQuitMessage(0);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int run_dashboard() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DashboardProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "DashboardApp";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("DashboardApp", "Task Scheduler Dashboard", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 350, 520, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
