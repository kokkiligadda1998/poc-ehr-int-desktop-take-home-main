
#include "application-status.hpp"
#include <windows.h>
#include <iostream>

NativeApplicationStatus::NativeApplicationStatus(Napi::Env env, Napi::Object exports) {
  /* Advertise to node the functions this module supports */
  DefineAddon(
    exports,
    {
        InstanceMethod("ListenForStatus", &NativeApplicationStatus::ListenForStatus),
        InstanceMethod("StopListening", &NativeApplicationStatus::StopListening),
        InstanceMethod("SetPollingTime", &NativeApplicationStatus::SetPollingTime)
    }
  );
}

void NativeApplicationStatus::ListenForStatus(const Napi::CallbackInfo& info) {
  
  auto env = info.Env();
  if (info.Length() != 2) {
    Napi::TypeError::New(env, "Expected 2 arguments").ThrowAsJavaScriptException();
    return;
  }

  // First argument: object with applicationName and windowTitle
  if (!info[0].IsObject()) {
    Napi::TypeError::New(env, "First argument must be an object").ThrowAsJavaScriptException();
    return;
  }

  Napi::Object appStatusObj = info[0].As<Napi::Object>();

  // Check for required properties: applicationName and windowTitle
  if (!appStatusObj.Has("applicationName") || !appStatusObj.Get("applicationName").IsString() ||
      !appStatusObj.Has("windowTitle") || !appStatusObj.Get("windowTitle").IsString()) {
    Napi::TypeError::New(env, "Object must have 'applicationName' and 'windowTitle' properties of type string")
        .ThrowAsJavaScriptException();
    return;
  }


  if (!info[1].IsFunction()) {
    Napi::TypeError::New(env, "Second argument must be a callback function")
        .ThrowAsJavaScriptException();
    return;
  }
  Napi::Function callback = info[1].As<Napi::Function>();

  currentApplication.applicationName = appStatusObj.Get("applicationName").As<Napi::String>().Utf8Value();
  currentApplication.windowTitle = appStatusObj.Get("windowTitle").As<Napi::String>().Utf8Value();

  // Create a threadsafe function to call JavaScript asynchronously
  cppCallback = Napi::ThreadSafeFunction::New(
        env,
        callback,
        "ApplicationStatusCallback", 
        0, 
        1    
    );

    // Start the worker thread to monitor the application
    isRunning = true;
    lThread = std::thread(&NativeApplicationStatus::WorkerThread, this);

  Napi::Function consoleLog = env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>();
  consoleLog.Call({Napi::String::New(env, "Successfully started NativeApplicationStatus")});
}

void NativeApplicationStatus::StopListening(const Napi::CallbackInfo& info) {
    if (!isRunning) return;

    isRunning = false;

    // Signal the thread to stop and wait for completion
    if (lThread.joinable()) {
        lThread.join();
    }

    //Release resources used by the callback
    if (cppCallback) {
        cppCallback.Release();
        cppCallback = nullptr;
    }


  Napi::Function consoleLog = info.Env().Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>();
  consoleLog.Call({Napi::String::New(info.Env(), "Successfully stopped NativeApplicationStatus")});
}

void NativeApplicationStatus::SetPollingTime(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected a number for polling time").ThrowAsJavaScriptException();
        return;
    }

    int newPollTime = info[0].As<Napi::Number>().Int32Value();

    if (newPollTime < 1 || newPollTime > 10) {
        Napi::TypeError::New(env, "Polling time must be between 1sec and 10sec").ThrowAsJavaScriptException();
        return;
    }

    pollTime.store(newPollTime);

    // Log updated polling time to JavaScript console
    Napi::Function consoleLog = env.Global().Get("console").As<Napi::Object>().Get("log").As<Napi::Function>();
    consoleLog.Call({ Napi::String::New(env, "Updated polling time to: " + std::to_string(newPollTime) + " sec") });

    // Invoke JavaScript callback if provided
    if (info.Length() > 1 && info[1].IsFunction()) {
        Napi::Function callback = info[1].As<Napi::Function>();
        callback.Call(env.Global(), { Napi::Number::New(env, newPollTime) });
    }
}

NODE_API_ADDON(NativeApplicationStatus)
