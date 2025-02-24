<template>
  <h1>Application Monitor and Status</h1>
  <p>
      <template v-if="!hasReceivedApplications">
          Waiting for application scanner to return results
      </template>
      <template v-else-if="!hasScannedApplications">
          No scanned applications received
      </template>
      <template v-else>
          <label for="app-select">Choose an application:</label>
          <select id="app-select" v-model="selectedApplication" @change="addApplication">
              <option v-for="app in scannedApplications" :key="app.procedureName" :value="app">
                  {{ app.windowTitle }}
              </option>
          </select>

          <div>
                <label for="polling-time"><strong>Set Polling Time (sec):</strong></label>
                <input type="number"
                       id="polling-time"
                       v-model="pollingTime"
                       :min="minPolling"
                       :max="maxPolling"
                       @input="validatePollingTime" />
                <button @click="submitPollingTime">
                    Submit
                </button>
                <p v-if="pollingError" class="error-message">
                    Polling time must be between {{ minPolling }} and {{ maxPolling }} ms.
                </p>
            </div>

          <table>
              <thead>
                  <tr>
                      <th>Application Name</th>
                      <th>Window Title</th>
                      <th>Action</th>
                  </tr>
              </thead>
              <tbody>
                  <tr v-for="app in tableApplications" :key="app.procedureName">
                      <td>{{ app.procedureName }}</td>
                      <td>{{ app.windowTitle }}</td>
                      <td><button @click="removeApplication(app)">Remove</button></td>
                  </tr>
              </tbody>
          </table>

          <!-- Application Status -->
          <div v-if="tableApplications.length > 0">
              <h2>Monitoring</h2>
              <p><strong>Status:</strong> {{ applicationStatus?.status || "Unknown" }}</p>
              <template v-if="applicationStatus?.dimensions">
                  <p><strong>Window Dimensions:</strong></p>
                  <ul>
                      <li>X: {{ applicationStatus.dimensions.x }}</li>
                      <li>Y: {{ applicationStatus.dimensions.y }}</li>
                      <li>Width: {{ applicationStatus.dimensions.width }}</li>
                      <li>Height: {{ applicationStatus.dimensions.height }}</li>
                  </ul>
              </template>
          </div>
      </template>
  </p>
</template>

<script setup lang="ts">
import { computed, onMounted, ref, watch } from 'vue';

const hasReceivedApplications = ref(false);
const scannedApplications = ref<ApplicationStatusCriteria[]>([]);
const hasScannedApplications = computed(() => scannedApplications.value.length > 0);

const selectedApplication = ref<ApplicationStatusCriteria | null>(null);
const tableApplications = ref<ApplicationStatusCriteria[]>([]);
const applicationStatus = ref<ApplicationStatus | null>(null);
const pollingTime = ref(3);
const minPolling = 1;
const maxPolling = 10;
const pollingError = ref(false);
const isInitialized = ref(false); 

// Scan for applications on component mount
onMounted(() => {
  console.log("Vue Mounted: Starting application scanner...");

  window.ApplicationMonitorApi.StartApplicationScanner((newScannedApplications) => {
      console.log("Applications Found:", newScannedApplications);
      hasReceivedApplications.value = true;
      scannedApplications.value = newScannedApplications;

      // if (newScannedApplications.length > 0) {
      //     selectedApplication.value = newScannedApplications[0];
      //     addApplication();
      // }

      isInitialized.value = true; 
  });
});

// Adds selected application to monitored list
const addApplication = () => {
  if (selectedApplication.value && !tableApplications.value.includes(selectedApplication.value)) {
      tableApplications.value.push(selectedApplication.value);
      checkApplicationStatus();
  }
};

// Removes application from monitored list
const removeApplication = (app: ApplicationStatusCriteria) => {
  const index = tableApplications.value.findIndex(application => application.procedureName === app.procedureName);
  if (index !== -1) {
      tableApplications.value.splice(index, 1);
      checkApplicationStatus();
  }
};

// Checks and updates the monitoring status of applications
const checkApplicationStatus = () => {
  if (tableApplications.value.length > 0) {
      startMonitoring(tableApplications.value[tableApplications.value.length - 1]);
  } else {
      window.ApplicationMonitorApi.StopApplicationStatusMonitor();
      applicationStatus.value = null;
  }
};

// Starts monitoring a specific application
const startMonitoring = (app: ApplicationStatusCriteria) => {
  window.ApplicationMonitorApi.StopApplicationStatusMonitor();
  setTimeout(() => {
      window.ApplicationMonitorApi.StartApplicationStatusMonitor(
          { applicationName: app.procedureName, windowTitle: app.windowTitle },
          (status) => {
              applicationStatus.value = status;
          }
      );
  }, 100);
};

// Watch for changes, but only after applications have been initialized
watch(scannedApplications, (newScannedApplications) => {
  if (!isInitialized.value) return; 

  //Remove applications from the monitoring table if they no longer appear in the latest scan results
  tableApplications.value = tableApplications.value.filter(app => newScannedApplications.some(newApp => newApp.procedureName === app.procedureName));
  checkApplicationStatus();
});

/* 
  validatePollingTime:
  Checks if the user-entered pollingTime is within the allowed range.
*/
const validatePollingTime = () => {
  pollingError.value = pollingTime.value < minPolling || pollingTime.value > maxPolling;
};

/* 
  submitPollingTime:
  Called when the user clicks the Submit button for polling time.
  If the value is valid, update the polling time for all monitored applications.
*/
const submitPollingTime = () => {
  validatePollingTime();
  if (!pollingError.value) {
      window.ApplicationMonitorApi.SetPollingTime(pollingTime.value, (updatedTime) => {
          pollingTime.value = updatedTime;
      });
  }
};
</script>

<style lang="css" scoped>
li {
  line-height: 1.5rem;
}

li + li {
  padding-top: 1rem;
}

table {
  width: 100%;
  border-collapse: collapse;
  margin-top: 1rem;
}

th, td {
  border: 1px solid #ccc;
  padding: 8px;
  text-align: left;
}
</style>