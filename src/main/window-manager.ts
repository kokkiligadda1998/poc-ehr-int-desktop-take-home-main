import { BrowserWindow, globalShortcut, ipcMain } from "electron";
import { ApplicationScannerChannel,ApplicationStatusChannel, RenderMessage, RenderRequestChannel } from "../types";
import { LoadNativeModule } from "./utils";
import { ApplicationStatus } from "./application-status";

const NativeApplicationScanner = LoadNativeModule("native-application-scanner");

export class WindowManager {
  private window: BrowserWindow;
  private readonly windowSize = { width: 720, height: 420 };
  private applicationScannerIsRunning: boolean = false;
  private applicationStatusIsRunning: boolean = false;
  constructor(
    preloadEntry: string,
    staticEntry: string,
    devEntry: string
  ) {

    // create the window

    this.window = new BrowserWindow({
      webPreferences: { preload: preloadEntry},
      height: this.windowSize.height,
      width: this.windowSize.width,
      skipTaskbar: true,
      show: true,
      frame: true,
      alwaysOnTop: true,
      transparent: false,
      resizable: true,
      autoHideMenuBar: true
    });

    // either load it from vite hot reload server, or static file

    if (devEntry) {
      this.window.loadURL(devEntry);
    } else {
      this.window.loadFile(staticEntry);
    }

    // register a shortcut for opening up dev tools

    globalShortcut.register("Control+Shift+I", () => {
      this.window.webContents.openDevTools({
        mode: "detach",
        activate: false,
      });
    });

    // setup IPC handlers for bi-directional communication
    this.setupIpcHandlers();

  }

  private setupIpcHandlers() {
    ipcMain.on(RenderRequestChannel, (_, message: RenderMessage) => {
      switch (message.request) {
        case "ApplicationScanner:StartListener":
          if (this.applicationScannerIsRunning) {
            NativeApplicationScanner.StopListener();
          }
          NativeApplicationScanner.ListenForApplications((scannedApplications) => {
            this.window.webContents.send(ApplicationScannerChannel, scannedApplications)
          });
          this.applicationScannerIsRunning = true;
          break;
        case "ApplicationScanner:StopListener":
          NativeApplicationScanner.StopListener();
          this.applicationScannerIsRunning = false;
          break;
        case "ApplicationStatus:StopListening":
          ApplicationStatus.getInstance().StopListening();
          this.applicationStatusIsRunning = false;
          break;
        case "ApplicationStatus:ListenForStatus":
          if (this.applicationStatusIsRunning) {
                ApplicationStatus.getInstance().StopListening();
          }

          if (!message.payload || !message.payload.applicationName || !message.payload.windowTitle) {
                throw new Error("Invalid payload. applicationName and windowTitle must be passed");
          }

          ApplicationStatus.getInstance().ListenForStatus(
                message.payload.applicationName,
                message.payload.windowTitle,
                (status) => {
                    this.window.webContents.send(ApplicationStatusChannel, status);
                }
          );

            this.applicationStatusIsRunning = true;
          break;
        case "ApplicationStatus:SetPollingTime": 
            if (typeof message.payload !== "number" || isNaN(message.payload)) {
                return;
            }

            ApplicationStatus.getInstance().SetPollingTime(message.payload, (updatedTime) => {
                _.sender.send("pollingTimeUpdated", updatedTime);
            });
        break;
        default:
          throw new Error(
            `Main thread received unhandled request type on ipc channel ${RenderRequestChannel}: ${message.request}`
          );
      }
    });
  }

}