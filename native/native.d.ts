
////

// Application Scanner

declare interface ScannedApplication {
  windowTitle: string;
  procedurePath: string;
  procedureName: string;
}

declare type ScannedApplications = Array<ScannedApplication>;
declare type ApplicationScannerCallback = (sa: ScannedApplications) => void;

declare interface NativeApplicationScannerModule {
  ListenForApplications: (callback: ApplicationScannerCallback) => void;
  StopListener: () => void;
}

declare interface ApplicationStatusCriteria {
  applicationName: string;
  windowTitle: string;
}


declare interface ApplicationStatusDimensions {
  x: int;
  y: int;
  width: int;
  height: int;
}

declare interface ApplicationStatus {
  status: "open" | "hidden" | "closed";
  dimensions?: ApplicationStatusDimensions;
}

declare type ApplicationStatusCallback = (appInfo: ApplicationStatusCriteria, status: ApplicationStatus) => void;

declare interface NativeApplicationStatusModule {
    ListenForStatus: (
        appInfo: ApplicationStatusCriteria,
        callback: (status: ApplicationStatus) => void 
    ) => void;
    StopListening: () => void;
    SetPollingTime: (
        pollingNumber: number,
        callback?: (pollingTime: number) => void
    ) => void;
}

////

// Native Modules

declare type NativeModuleMap = {
  "native-application-scanner": NativeApplicationScannerModule,
  "native-application-status": NativeApplicationStatusModule,
};

declare type NativeModuleTypes = keyof NativeModuleMap;
