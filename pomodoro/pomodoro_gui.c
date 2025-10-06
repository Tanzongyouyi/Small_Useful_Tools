#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "comctl32.lib")

// 全局变量
#define ID_TIMER 1
#define ID_START_BUTTON 1001
#define ID_PAUSE_BUTTON 1002
#define ID_RESET_BUTTON 1003
#define ID_EXIT_BUTTON 1004

HWND hMainWindow, hProgressBar, hStatusLabel, hCountLabel, hStartButton, hPauseButton, hResetButton, hExitButton;
int pomodoro_count = 1;
int completed_pomodoros = 0;
int current_session = 0; // 0:专注, 1:短休息, 2:长休息
int is_running = 0;
int time_remaining = 0;
int total_time = 0;

// 番茄钟时间设置（秒）
#define FOCUS_TIME (25 * 60)
#define SHORT_BREAK_TIME (5 * 60)
#define LONG_BREAK_TIME (20 * 60)

// 颜色定义
#define COLOR_FOCUS RGB(220, 53, 69)    // 红色
#define COLOR_BREAK RGB(40, 167, 69)    // 绿色
#define COLOR_LONG_BREAK RGB(0, 123, 255) // 蓝色

const char* GetSessionName(int session) {
    switch(session) {
        case 0: return "专注工作";
        case 1: return "短休息";
        case 2: return "长休息";
        default: return "未知";
    }
}

int GetSessionTime(int session) {
    switch(session) {
        case 0: return FOCUS_TIME;
        case 1: return SHORT_BREAK_TIME;
        case 2: return LONG_BREAK_TIME;
        default: return FOCUS_TIME;
    }
}

COLORREF GetSessionColor(int session) {
    switch(session) {
        case 0: return COLOR_FOCUS;
        case 1: return COLOR_BREAK;
        case 2: return COLOR_LONG_BREAK;
        default: return COLOR_FOCUS;
    }
}

void UpdateDisplay() {
    char buffer[256];
    int minutes = time_remaining / 60;
    int seconds = time_remaining % 60;
    
    // 更新进度条
    SendMessage(hProgressBar, PBM_SETPOS, (total_time - time_remaining) * 100 / total_time, 0);
    
    // 更新时间显示
    snprintf(buffer, sizeof(buffer), "%s - %02d:%02d", GetSessionName(current_session), minutes, seconds);
    SetWindowText(hStatusLabel, buffer);
    
    // 更新计数显示
    snprintf(buffer, sizeof(buffer), "第 %d 个番茄钟 | 已完成: %d", pomodoro_count, completed_pomodoros);
    SetWindowText(hCountLabel, buffer);
    
    // 更新窗口背景色
    InvalidateRect(hMainWindow, NULL, TRUE);
}

void StartSession() {
    if (!is_running) {
        total_time = GetSessionTime(current_session);
        time_remaining = total_time;
        is_running = 1;
        SetTimer(hMainWindow, ID_TIMER, 1000, NULL);
        UpdateDisplay();
    }
}

void PauseSession() {
    if (is_running) {
        is_running = 0;
        KillTimer(hMainWindow, ID_TIMER);
        UpdateDisplay();
    }
}

void ResetSession() {
    is_running = 0;
    KillTimer(hMainWindow, ID_TIMER);
    current_session = 0;
    time_remaining = GetSessionTime(current_session);
    total_time = time_remaining;
    UpdateDisplay();
}

void CompleteSession() {
    is_running = 0;
    KillTimer(hMainWindow, ID_TIMER);
    
    if (current_session == 0) {
        completed_pomodoros++;
        
        char message[256];
        if (completed_pomodoros % 4 == 0) {
            snprintf(message, sizeof(message), "恭喜完成第 %d 个番茄钟！\n已完成一组，开始长休息！", pomodoro_count);
            current_session = 2;
        } else {
            snprintf(message, sizeof(message), "恭喜完成第 %d 个番茄钟！\n开始短休息！", pomodoro_count);
            current_session = 1;
        }
        
        MessageBox(hMainWindow, message, "番茄钟完成", MB_OK | MB_ICONINFORMATION);
        pomodoro_count++;
    } else {
        current_session = 0;
        MessageBox(hMainWindow, "休息结束！\n开始新的专注时段。", "休息完成", MB_OK | MB_ICONINFORMATION);
    }
    
    total_time = GetSessionTime(current_session);
    time_remaining = total_time;
    UpdateDisplay();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            // 创建进度条
            hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, 
                                         WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                                         20, 80, 350, 25, hwnd, NULL, 
                                         ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(hProgressBar, PBM_SETSTEP, 1, 0);
            
            // 创建状态标签
            hStatusLabel = CreateWindow("STATIC", "准备开始", 
                                       WS_CHILD | WS_VISIBLE | SS_CENTER,
                                       20, 120, 350, 30, hwnd, NULL, 
                                       ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            // 创建计数标签
            hCountLabel = CreateWindow("STATIC", "第 1 个番茄钟 | 已完成: 0", 
                                      WS_CHILD | WS_VISIBLE | SS_CENTER,
                                      20, 160, 350, 25, hwnd, NULL, 
                                      ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            // 创建按钮
            hStartButton = CreateWindow("BUTTON", "开始", 
                                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       20, 200, 80, 30, hwnd, (HMENU)ID_START_BUTTON, 
                                       ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            hPauseButton = CreateWindow("BUTTON", "暂停", 
                                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       110, 200, 80, 30, hwnd, (HMENU)ID_PAUSE_BUTTON, 
                                       ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            hResetButton = CreateWindow("BUTTON", "重置", 
                                       WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                       200, 200, 80, 30, hwnd, (HMENU)ID_RESET_BUTTON, 
                                       ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            hExitButton = CreateWindow("BUTTON", "退出", 
                                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                      290, 200, 80, 30, hwnd, (HMENU)ID_EXIT_BUTTON, 
                                      ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            
            // 设置字体
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
                                   DEFAULT_QUALITY, DEFAULT_PITCH, "Microsoft YaHei");
            
            SendMessage(hStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hCountLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hStartButton, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hPauseButton, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hResetButton, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hExitButton, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            // 初始化显示
            ResetSession();
            break;
        }
        
        case WM_COMMAND: {
            switch(LOWORD(wParam)) {
                case ID_START_BUTTON:
                    StartSession();
                    break;
                case ID_PAUSE_BUTTON:
                    PauseSession();
                    break;
                case ID_RESET_BUTTON:
                    ResetSession();
                    break;
                case ID_EXIT_BUTTON:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        }
        
        case WM_TIMER: {
            if (is_running && time_remaining > 0) {
                time_remaining--;
                UpdateDisplay();
                
                if (time_remaining == 0) {
                    CompleteSession();
                }
            }
            break;
        }
        
        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            return (LRESULT)GetStockObject(BLACK_BRUSH);
        }
        
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH hBrush = CreateSolidBrush(GetSessionColor(current_session));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);
            
            return 1;
        }
        
        case WM_CLOSE:
            if (is_running) {
                KillTimer(hwnd, ID_TIMER);
            }
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 初始化通用控件
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);
    
    // 注册窗口类
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "PomodoroTimerClass";
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "窗口注册失败!", "错误", MB_ICONERROR);
        return 0;
    }
    
    // 创建窗口
    hMainWindow = CreateWindowEx(
        0,
        "PomodoroTimerClass",
        "Pomodoro_gui - by Wave Studios",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hMainWindow) {
        MessageBox(NULL, "窗口创建失败!", "错误", MB_ICONERROR);
        return 0;
    }
    
    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}