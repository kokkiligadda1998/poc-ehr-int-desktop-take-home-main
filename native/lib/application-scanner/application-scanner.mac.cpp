
#include "application-scanner.mac.hpp"

void MacApplicationScanner::ListenForApplications(Napi::Env &env, Napi::ThreadSafeFunction &&callback) {
  PlatformNativeApplicationScanner::ListenForApplications(env, std::move(callback));
	// TODO: platform specific initialization
}

void MacApplicationScanner::StopListener() {
	// TODO: platform specific teardown
	PlatformNativeApplicationScanner::StopListener();
}