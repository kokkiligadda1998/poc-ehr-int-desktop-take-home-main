
import {
  ApplicationScannerChannel,ApplicationStatusChannel, RenderMessage, RenderRequestChannel
} from "../types";
import { contextBridge, ipcRenderer } from "electron";

export const ApplicationMonitorApi = {
  StartApplicationScanner: (callback: ApplicationScannerCallback) => {
    // bind the callback
    ipcRenderer.on(ApplicationScannerChannel, (_, sa: ScannedApplications) => {
      callback(sa);
    });
    // start the service
    const message: RenderMessage = {
      request: "ApplicationScanner:StartListener"
    };
    ipcRenderer.send(RenderRequestChannel, message);
  },
  StartApplicationStatusMonitor: (appInfo: ApplicationStatusCriteria, callback: (status: ApplicationStatus) => void) => {
    ipcRenderer.on(ApplicationStatusChannel, (_, status: ApplicationStatus) => {
        callback(status);
    });

    const message: RenderMessage = {
        request: "ApplicationStatus:ListenForStatus",
        payload: appInfo
    };
    ipcRenderer.send(RenderRequestChannel, message);
},

// Stop monitoring the application status
StopApplicationStatusMonitor: () => {
    const message: RenderMessage = { request: "ApplicationStatus:StopListening" };
    ipcRenderer.send(RenderRequestChannel, message);
},
SetPollingTime: (pollingTime: number, callback?: (updatedTime: number) => void) => {

  ipcRenderer.once("pollingTimeUpdated", (_, updatedTime: number) => {
      if (callback) {
          callback(updatedTime);
      }
  });

  const message: RenderMessage = {
      request: "ApplicationStatus:SetPollingTime",
      payload: pollingTime
  };
  ipcRenderer.send(RenderRequestChannel, message);
}
}

export const ApplicationMonitorApiName = 'ApplicationMonitorApi';

process.once("loaded", () => {
  contextBridge.exposeInMainWorld(ApplicationMonitorApiName, ApplicationMonitorApi);
});
  