
// requests renderer -> main

export const RenderRequestChannel = "render-request-channel";

export type RenderRequests =
    | "ApplicationScanner:StartListener"
    | "ApplicationScanner:StopListener"
    | "ApplicationStatus:ListenForStatus"
    | "ApplicationStatus:StopListening"
    | "ApplicationStatus:SetPollingTime";

export type RenderMessage =
    | { request: "ApplicationScanner:StartListener" }
    | { request: "ApplicationScanner:StopListener" }
    | { request: "ApplicationStatus:ListenForStatus"; payload: ApplicationStatusCriteria }
    | { request: "ApplicationStatus:StopListening" }
    | { request: "ApplicationStatus:SetPollingTime"; payload?: number };

// responses / callbacks main -> render

export const ApplicationScannerChannel = "applicationScannerChannel";
export const ApplicationStatusChannel = "applicationStatusChannel";

export interface ApplicationStatusCriteria {
  applicationName: string;
  windowTitle: string;
}

// Application Status Type
export interface ApplicationStatusDimensions {
  x: number;
  y: number;
  width: number;
  height: number;
}

export interface ApplicationStatus {
  status: "open" | "hidden" | "closed";
  dimensions?: ApplicationStatusDimensions;
}

// Callback Type
export type ApplicationStatusCallback = (status: ApplicationStatus) => void;
export type ApplicationStatusPollingCallback = (pollingTime: number) => void;