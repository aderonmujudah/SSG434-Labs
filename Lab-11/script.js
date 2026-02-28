let updateInterval;

// Initialize on page load
document.addEventListener("DOMContentLoaded", function () {
  updateSensorData();
  updateInterval = setInterval(updateSensorData, 2000); // Update every 2 seconds
  getLEDStates();
});

// Toggle LED function
function toggleLED(ledNumber) {
  fetch("/led" + ledNumber + "/toggle")
    .then((response) => response.json())
    .then((data) => {
      updateLEDButton(ledNumber, data.state);
    })
    .catch((error) => {
      console.error("Error:", error);
      showStatus("Connection Error", false);
    });
}

// Update LED button appearance
function updateLEDButton(ledNumber, state) {
  const btn = document.getElementById("led" + ledNumber + "Btn");
  if (state) {
    btn.textContent = "ON";
    btn.className = "btn btn-on";
  } else {
    btn.textContent = "OFF";
    btn.className = "btn btn-off";
  }
}

// Get current LED states
function getLEDStates() {
  fetch("/led/status")
    .then((response) => response.json())
    .then((data) => {
      updateLEDButton(1, data.led1);
      updateLEDButton(2, data.led2);
    })
    .catch((error) => {
      console.error("Error:", error);
    });
}

// Update sensor data
function updateSensorData() {
  fetch("/sensor/data")
    .then((response) => response.json())
    .then((data) => {
      document.getElementById("temperature").textContent =
        data.temperature.toFixed(1);
      document.getElementById("humidity").textContent =
        data.humidity.toFixed(1);

      const now = new Date();
      const timeString = now.toLocaleTimeString();
      document.getElementById("lastUpdate").textContent = timeString;

      showStatus("Connected", true);
    })
    .catch((error) => {
      console.error("Error:", error);
      document.getElementById("temperature").textContent = "--";
      document.getElementById("humidity").textContent = "--";
      showStatus("Connection Error", false);
    });
}

// Show connection status
function showStatus(message, isConnected) {
  const statusText = document.getElementById("statusText");
  const statusIndicator = document.getElementById("statusIndicator");

  statusText.textContent = message;

  if (isConnected) {
    statusIndicator.style.background = "#4caf50";
    statusIndicator.style.animation = "pulse 2s infinite";
  } else {
    statusIndicator.style.background = "#f44336";
    statusIndicator.style.animation = "none";
  }
}
