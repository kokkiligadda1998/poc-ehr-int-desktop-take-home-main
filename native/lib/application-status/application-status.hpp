#ifndef APPLICATION_STATUS_HPP
#define APPLICATION_STATUS_HPP

#include <napi.h>
#include <thread>
#include <utility>
#include "../utilities/helpers.hpp"

class PlatformNativeApplicationStatus {
  public:
    virtual ~PlatformNativeApplicationStatus() {
        StopListening();
    }
    virtual void ListenForStatus(ApplicationStatusCriteria appInfo, Napi::ThreadSafeFunction &&callback) {
      _callback = std::move(callback);
      _thread = std::thread(&PlatformNativeApplicationStatus::WorkerThread, this, appInfo);
    }
    virtual void StopListening() {
      _callback = NULL;
    }
  protected:
    PlatformNativeApplicationStatus() = default;
    Napi::ThreadSafeFunction _callback = NULL;
    std::thread _thread;
    virtual void WorkerThread(ApplicationStatusCriteria appInfo) = 0;
};

class NativeApplicationStatus: public Napi::Addon<NativeApplicationStatus> {
  public:
    NativeApplicationStatus(Napi::Env env, Napi::Object exports);
    void SetPollingTime(const Napi::CallbackInfo& info);
  private:
    void ListenForStatus(const Napi::CallbackInfo& info);
    void StopListening(const Napi::CallbackInfo& info);
    std::thread lThread;
    std::atomic<int> pollTime{ 3 };
    void WorkerThread();
  protected:
    std::atomic<bool> isRunning{ false };
    ApplicationStatusCriteria currentApplication;
    Napi::ThreadSafeFunction cppCallback;

};

#endif 