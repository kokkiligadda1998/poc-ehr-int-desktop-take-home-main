#ifndef WINDOWS_APPLICATION_STATUS_HPP
#define WINDOWS_APPLICATION_STATUS_HPP

#include "application-status.hpp"

#include "../utilities/classes.hpp"
#include "../utilities/classes.windows.hpp"
#include "../utilities/helpers.windows.hpp"

#include <map>


#include "application-status.hpp"

class WindowsApplicationStatus : public NativeApplicationStatus {
public:
    WindowsApplicationStatus() {}
    void ListenForStatus(const Napi::CallbackInfo& info);
    void StopListening(const Napi::CallbackInfo& info);
    void WorkerThread();
};


#endif 