import { LoadNativeModule } from "./utils";
import { ApplicationStatusCriteria, ApplicationStatus as ApplicationStatusType, ApplicationStatusCallback } from "../types";

const NativeApplicationStatus = LoadNativeModule("native-application-status");

export class ApplicationStatus {
    private static instance: ApplicationStatus;
    private isRunning: boolean = false;

    public static getInstance(): ApplicationStatus {
        if (!ApplicationStatus.instance) {
            ApplicationStatus.instance = new ApplicationStatus();
        }
        return ApplicationStatus.instance;
    }

    public ListenForStatus(applicationName: string, windowTitle: string, callback: ApplicationStatusCallback): void {
        if (this.isRunning) {
            this.StopListening();
        }

        this.isRunning = true;

        const appInfo: ApplicationStatusCriteria = { applicationName, windowTitle };

        NativeApplicationStatus.ListenForStatus(
            appInfo,
            (status: ApplicationStatusType) => {
                callback(status);
            }
        );
    }

    public StopListening(): void {
        if (!this.isRunning) return;
        NativeApplicationStatus.StopListening();
        this.isRunning = false;
    }

    public SetPollingTime(pollingTime: number, callback?: (updatedTime: number) => void): void {
        if (!this.isRunning) return;

        NativeApplicationStatus.SetPollingTime(pollingTime);

        if (callback) {
            callback(pollingTime);
        }
        this.isRunning = true;
    }
}