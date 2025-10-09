// flow_functions.c (GUI version)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "undo_redo.h"
#include "schedule.h"

/* helper: blocking input dialog implementation
   We'll create a small window with a label, an edit control and OK/Cancel,
   run a nested message loop until user presses OK/Cancel.
*/

typedef struct {
    char *outbuf;
    int outlen;
    int is_multiline;
    int result; // 1 = OK, 0 = Cancel
} InputState;

LRESULT CALLBACK InputDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    InputState *st = (InputState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE: {
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            HWND he = GetDlgItem(hwnd, 1001);
            if (he && st && st->outbuf) {
                GetWindowText(he, st->outbuf, st->outlen);
                st->result = 1;
            }
            DestroyWindow(hwnd);
        } else if (LOWORD(wParam) == IDCANCEL) {
            if (st) st->result = 0;
            DestroyWindow(hwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0); // quit nested loop
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/* create a blocking input box.
   title: window title
   label: left static label (ignored in layout except for caption)
   buf: buffer to receive text
   buflen: buffer length
   multiline: if non-zero, use ES_MULTILINE and larger box.
   returns 1 if OK pressed, 0 if canceled.
*/
static int prompt_input(HWND hParent, const char *title, const char *label, char *buf, int buflen, int multiline) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    const char *cls = "SIMPLE_INPUT_CLASS";

    // register once
    static int reg = 0;
    if (!reg) {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = InputDlgProc;
        wc.hInstance = hInst;
        wc.lpszClassName = cls;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClass(&wc);
        reg = 1;
    }

    int width = 420, height = multiline ? 260 : 160;
    HWND hwnd = CreateWindowEx(0, cls, title,
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                               CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                               hParent, NULL, hInst, NULL);
    if (!hwnd) return 0;

    // allocate and attach state
    InputState st;
    st.outbuf = buf;
    st.outlen = buflen;
    st.is_multiline = multiline;
    st.result = 0;
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)&st);

    // create controls
    CreateWindow("STATIC", label, WS_CHILD | WS_VISIBLE, 12, 10, width - 24, 20, hwnd, NULL, hInst, NULL);

    DWORD editStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT;
    if (multiline) editStyle |= ES_AUTOVSCROLL | ES_WANTRETURN | ES_MULTILINE;
    HWND hedit = CreateWindow("EDIT", "", editStyle, 12, 40, width - 24, multiline ? 140 : 24, hwnd, (HMENU)1001, hInst, NULL);

    CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE, 80, height - 54, 80, 28, hwnd, (HMENU)IDOK, hInst, NULL);
    CreateWindow("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE, 220, height - 54, 80, 28, hwnd, (HMENU)IDCANCEL, hInst, NULL);

    // center & show
    ShowWindow(hwnd, SW_SHOW);
    SetFocus(hedit);

    // nested message loop until window destroyed
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        // after DestroyWindow, WM_QUIT posted by WM_DESTROY will break loop
        if (!IsWindow(hwnd)) break;
    }

    // result was placed in st
    return st.result;
}

/* small helper to show multi-line message (may be large) */
static void show_text_message(HWND parent, const char *title, const char *text) {
    // MessageBox has a size limit; but for moderate amount of text it's OK.
    // For larger, we could create a modal edit window; for now use MessageBox.
    MessageBox(parent, text, title, MB_OK | MB_ICONINFORMATION);
}

/* Add Task Flow (GUI) */
void add_task_flow(Task **head) {
    char title[64] = {0}, desc[256] = {0}, cat[32] = {0}, deadline[20] = {0}, prbuf[16] = {0};
    int priority = 3;

    HWND parent = NULL; // not used; dialogs are standalone

    if (!prompt_input(parent, "Add Task - Title", "Enter title:", title, sizeof(title), 0)) return;
    if (!prompt_input(parent, "Add Task - Description", "Enter description:", desc, sizeof(desc), 1)) return;
    if (!prompt_input(parent, "Add Task - Category", "Enter category:", cat, sizeof(cat), 0)) return;
    if (!prompt_input(parent, "Add Task - Deadline", "Enter deadline (YYYY-MM-DD):", deadline, sizeof(deadline), 0)) return;
    if (!prompt_input(parent, "Add Task - Priority", "Enter priority (1=High,2=Medium,3=Low):", prbuf, sizeof(prbuf), 0)) return;

    priority = atoi(prbuf);
    if (priority < 1 || priority > 3) priority = 3;

    Task *t = add_task(head, title, desc, cat, deadline, priority);
    if (t) {
        Action a = {ACTION_ADD, *t};
        push_undo(a);
        clear_redo();
        MessageBox(NULL, "Task added successfully!", "Success", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(NULL, "Failed to add task (memory?).", "Error", MB_OK | MB_ICONERROR);
    }
}

/* View tasks (GUI) */
void view_tasks(Task *head) {
    if (!head) {
        MessageBox(NULL, "No tasks to show.", "Tasks", MB_OK | MB_ICONINFORMATION);
        return;
    }

    // Build text buffer
    size_t cap = 8192;
    char *buf = (char*)malloc(cap);
    if (!buf) return;
    buf[0] = '\0';
    strcat(buf, "---- TASK LIST ----\n\n");
    for (Task *cur = head; cur; cur = cur->next) {
        char line[512];
        snprintf(line, sizeof(line), "ID: %d\nTitle: %s\nCategory: %s\nDeadline: %s\nPriority: %d\nStatus: %s\n\n",
                 cur->id, cur->title, cur->category, cur->deadline, cur->priority,
                 cur->status ? "Completed" : "Pending");
        if (strlen(buf) + strlen(line) + 1 > cap) {
            cap *= 2;
            char *tmp = (char*)realloc(buf, cap);
            if (!tmp) break;
            buf = tmp;
        }
        strcat(buf, line);
    }

    show_text_message(NULL, "Tasks", buf);
    free(buf);
}

/* Mark complete (GUI) */
void mark_complete_flow(Task *head) {
    char idbuf[32] = {0};
    if (!prompt_input(NULL, "Mark Complete", "Enter Task ID to mark complete:", idbuf, sizeof(idbuf), 0)) return;
    int id = atoi(idbuf);
    if (id <= 0) {
        MessageBox(NULL, "Invalid ID.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Task *t = find_task_by_id(head, id);
    if (!t) {
        MessageBox(NULL, "Task not found.", "Info", MB_OK | MB_ICONINFORMATION);
        return;
    }

    Action a = {ACTION_UPDATE, *t};
    t->status = 1;
    push_undo(a);
    clear_redo();
    MessageBox(NULL, "Task marked completed!", "Success", MB_OK | MB_ICONINFORMATION);
}

/* Delete task (GUI) */
void delete_task_flow(Task **head) {
    char idbuf[32] = {0};
    if (!prompt_input(NULL, "Delete Task", "Enter Task ID to delete:", idbuf, sizeof(idbuf), 0)) return;
    int id = atoi(idbuf);
    if (id <= 0) {
        MessageBox(NULL, "Invalid ID.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // confirm
    if (MessageBox(NULL, "Are you sure you want to delete this task?", "Confirm", MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    Task *deleted = remove_task(head, id);
    if (!deleted) {
        MessageBox(NULL, "Task not found.", "Info", MB_OK | MB_ICONINFORMATION);
        return;
    }

    Action a = {ACTION_DELETE, *deleted};
    push_undo(a);
    clear_redo();
    free(deleted);
    MessageBox(NULL, "Task deleted successfully!", "Success", MB_OK | MB_ICONINFORMATION);
}

/* Generate schedule (GUI) */
void generate_schedule_flow(Task *head) {
    int count = 0;
    Task **sched = generate_schedule(head, &count);
    if (!count || !sched) {
        MessageBox(NULL, "No tasks to schedule.", "Schedule", MB_OK | MB_ICONINFORMATION);
        if (sched) free(sched);
        return;
    }

    // build text
    size_t cap = 4096;
    char *buf = (char*)malloc(cap);
    if (!buf) { free(sched); return; }
    buf[0] = '\0';
    strcat(buf, "---- SCHEDULED TASKS ----\n\n");
    for (int i = 0; i < count; ++i) {
        char line[256];
        snprintf(line, sizeof(line), "%d. %s | Deadline: %s | Priority: %d\n",
                 sched[i]->id, sched[i]->title, sched[i]->deadline, sched[i]->priority);
        if (strlen(buf) + strlen(line) + 1 > cap) {
            cap *= 2;
            char *tmp = (char*)realloc(buf, cap);
            if (!tmp) break;
            buf = tmp;
        }
        strcat(buf, line);
    }

    show_text_message(NULL, "Schedule", buf);
    free(buf);
    free(sched);
}

/* Undo (GUI) */
void undo_action_flow(Task **head) {
    Action a;
    if (!pop_undo(&a)) {
        MessageBox(NULL, "No action to undo.", "Undo", MB_OK | MB_ICONINFORMATION);
        return;
    }

    switch (a.type) {
    case ACTION_ADD: {
        Task *t = remove_task(head, a.snapshot.id);
        if (t) free(t);
        break;
    }
    case ACTION_DELETE: {
        Task *t = add_task(head, a.snapshot.title, a.snapshot.description,
                           a.snapshot.category, a.snapshot.deadline,
                           a.snapshot.priority);
        if (t) t->status = a.snapshot.status;
        break;
    }
    case ACTION_UPDATE: {
        Task *t = find_task_by_id(*head, a.snapshot.id);
        if (t) t->status = a.snapshot.status;
        break;
    }
    }
    push_redo(a);
    MessageBox(NULL, "Undo performed.", "Undo", MB_OK | MB_ICONINFORMATION);
}

/* Redo (GUI) */
void redo_action_flow(Task **head) {
    Action a;
    if (!pop_redo(&a)) {
        MessageBox(NULL, "No action to redo.", "Redo", MB_OK | MB_ICONINFORMATION);
        return;
    }

    switch (a.type) {
    case ACTION_ADD: {
        Task *t = add_task(head, a.snapshot.title, a.snapshot.description,
                           a.snapshot.category, a.snapshot.deadline,
                           a.snapshot.priority);
        if (t) t->status = a.snapshot.status;
        break;
    }
    case ACTION_DELETE: {
        Task *t = remove_task(head, a.snapshot.id);
        if (t) free(t);
        break;
    }
    case ACTION_UPDATE: {
        Task *t = find_task_by_id(*head, a.snapshot.id);
        if (t) t->status = 1;
        break;
    }
    }
    push_undo(a);
    MessageBox(NULL, "Redo performed.", "Redo", MB_OK | MB_ICONINFORMATION);
}

/* Exit program (GUI) */
void exit_program(Task **head) {
    if (MessageBox(NULL, "Are you sure you want to exit?", "Exit", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        free_all_tasks(*head);
        // Attempt to save if you have a save routine - optional
        PostQuitMessage(0);
        // also call exit in case some loops are not message-driven
        exit(0);
    }
}
