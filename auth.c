#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "auth.h"

#define ID_LOGIN 1
#define ID_SIGNUP 2
#define ID_SUBMIT_LOGIN 3
#define ID_SUBMIT_SIGNUP 4

HWND hUser, hPass, hName, hUserSignup, hPassSignup;
int loginSuccess = 0;

int checkCredentials(const char *id, const char *password) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return 0;
    char fileName[50], fileId[50], filePass[50];
    while (fscanf(fp, "%s %s %s", fileName, fileId, filePass) != EOF) {
        if (strcmp(fileId, id) == 0 && strcmp(filePass, password) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int idExists(const char *id) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return 0;
    char fileName[50], fileId[50], filePass[50];
    while (fscanf(fp, "%s %s %s", fileName, fileId, filePass) != EOF) {
        if (strcmp(fileId, id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void addUser(const char *name, const char *id, const char *password) {
    FILE *fp = fopen("users.txt", "a");
    if (!fp) return;
    fprintf(fp, "%s %s %s\n", name, id, password);
    fclose(fp);
}

LRESULT CALLBACK AuthProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static int loginDrawn = 0, signupDrawn = 0;
    switch (msg) {
    case WM_CREATE:
        CreateWindow("BUTTON", "Login", WS_VISIBLE | WS_CHILD, 100, 100, 80, 30, hwnd, (HMENU)ID_LOGIN, NULL, NULL);
        CreateWindow("BUTTON", "Sign Up", WS_VISIBLE | WS_CHILD, 200, 100, 80, 30, hwnd, (HMENU)ID_SIGNUP, NULL, NULL);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_LOGIN && !loginDrawn) {
            loginDrawn = 1;
            CreateWindow("STATIC", "User ID:", WS_VISIBLE | WS_CHILD, 50, 150, 80, 20, hwnd, NULL, NULL, NULL);
            hUser = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 150, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD, 50, 180, 80, 20, hwnd, NULL, NULL, NULL);
            hPass = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 150, 180, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Submit", WS_VISIBLE | WS_CHILD, 150, 220, 80, 30, hwnd, (HMENU)ID_SUBMIT_LOGIN, NULL, NULL);
        }
        if (LOWORD(wParam) == ID_SIGNUP && !signupDrawn) {
            signupDrawn = 1;
            CreateWindow("STATIC", "Name:", WS_VISIBLE | WS_CHILD, 50, 150, 80, 20, hwnd, NULL, NULL, NULL);
            hName = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 150, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "User ID:", WS_VISIBLE | WS_CHILD, 50, 180, 80, 20, hwnd, NULL, NULL, NULL);
            hUserSignup = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 180, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("STATIC", "Password:", WS_VISIBLE | WS_CHILD, 50, 210, 80, 20, hwnd, NULL, NULL, NULL);
            hPassSignup = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 150, 210, 120, 20, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Submit", WS_VISIBLE | WS_CHILD, 150, 250, 80, 30, hwnd, (HMENU)ID_SUBMIT_SIGNUP, NULL, NULL);
        }
        if (LOWORD(wParam) == ID_SUBMIT_LOGIN) {
            char user[50], pass[50];
            GetWindowText(hUser, user, 50);
            GetWindowText(hPass, pass, 50);
            if (checkCredentials(user, pass)) {
                MessageBox(hwnd, "Login Successful!", "Success", MB_OK);
                loginSuccess = 1;
                PostQuitMessage(0);
            } else {
                MessageBox(hwnd, "Invalid Credentials!", "Error", MB_OK | MB_ICONERROR);
            }
        }
        if (LOWORD(wParam) == ID_SUBMIT_SIGNUP) {
            char name[50], user[50], pass[50];
            GetWindowText(hName, name, 50);
            GetWindowText(hUserSignup, user, 50);
            GetWindowText(hPassSignup, pass, 50);
            if (idExists(user)) {
                MessageBox(hwnd, "ID already exists!", "Error", MB_OK | MB_ICONERROR);
            } else {
                addUser(name, user, pass);
                MessageBox(hwnd, "User Registered Successfully!", "Success", MB_OK);
            }
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

int run_auth_gui(void) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = AuthProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "AUTH_CLASS";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx(0, "AUTH_CLASS", "Login / Sign Up",
                               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                               CW_USEDEFAULT, CW_USEDEFAULT, 400, 350,
                               NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    MSG msg={0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return loginSuccess;
}
