#include "application-status.hpp"
#include <windows.h>
#include <iostream>

class WindowsApplicationStatus : public NativeApplicationStatus {
public:
    WindowsApplicationStatus(Napi::Env env, Napi::Object exports) : NativeApplicationStatus(env, exports) {}
    void WorkerThread();
};

// Worker thread function that periodically checks application status
void NativeApplicationStatus::WorkerThread() {
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(pollTime.load()));
        // Attempt to find the window handle based on the window title
        HWND hwnd = FindWindow(NULL, currentApplication.windowTitle.c_str());
        ApplicationStatus appStatus;

        if (!hwnd) {
            appStatus.status = "closed";
        }
        else {
            // Application window is open and visible; retrieve window dimensions
            WINDOWPLACEMENT wplacement = { sizeof(WINDOWPLACEMENT) };
            GetWindowPlacement(hwnd, &wplacement);

            if (wplacement.showCmd == SW_SHOWMINIMIZED) {
                appStatus.status = "hidden";
            }
            else {
                RECT rect;
                GetWindowRect(hwnd, &rect);
                appStatus.dimensions = { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
                appStatus.status = "open";
            }
        }

         // Ensure the thread is still running before invoking the callback
        if (!isRunning) return;
        
        
         // Send the application status back to JavaScript via callback
        cppCallback.BlockingCall([&](Napi::Env env, Napi::Function callback) {
                Napi::Object result = Napi::Object::New(env);
                result.Set("status", appStatus.status);
                if (appStatus.dimensions) {
                    Napi::Object dim = Napi::Object::New(env);
                    dim.Set("x", appStatus.dimensions->x);
                    dim.Set("y", appStatus.dimensions->y);
                    dim.Set("width", appStatus.dimensions->width);
                    dim.Set("height", appStatus.dimensions->height);
                    result.Set("dimensions", dim);
                }
                callback.Call({ result });
            });
    }
}