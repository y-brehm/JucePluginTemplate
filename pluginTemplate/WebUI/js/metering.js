/**
 * Metering module for the plugin's Web UI.
 * Handles fetching level data and updating the meter components.
 */

import * as Juce from "./juce/index.js";

/**
 * A reusable function to update a meter's bar and text display.
 * @param {number} dbValue - The decibel value to display.
 * @param {HTMLElement} meterBar - The meter bar element.
 * @param {HTMLElement} peakValueElement - The text element for the peak value.
 */
const updateMeter = (dbValue, meterBar, peakValueElement) => {
    // 1. Update the number box with the precise value
    const displayDb = isFinite(dbValue) ? dbValue.toFixed(1) : "-inf";
    peakValueElement.textContent = displayDb + " dB";

    // 2. Define the meter's visual range in dB.
    const minDb = -60.0;
    const maxDb = 6.0;

    // 3. Clamp the incoming dB value to our meter's visual range for the bar.
    const clampedDb = Math.max(minDb, Math.min(dbValue, maxDb));

    // 4. Convert the clamped dB value to a percentage (0-100).
    const percentage = ((clampedDb - minDb) / (maxDb - minDb)) * 100;

    // 5. Update the meter bar's height.
    meterBar.style.height = percentage + '%';

    // 6. Change color based on the raw dB value.
    if (dbValue >= 0.0) { // Red from 0dBFS up
        meterBar.style.backgroundColor = '#ff4500';
    } else if (dbValue >= -6.0) { // Yellow from -6dBFS to 0dBFS
        meterBar.style.backgroundColor = '#ffdd00';
    } else { // Green below -6dBFS
        meterBar.style.backgroundColor = '#00ff00';
    }
};

/**
 * Initializes all metering components. Finds the DOM elements and sets up
 * the event listener to fetch data from the C++ backend.
 */
export function initializeMetering() {
    const inputMeterBar = document.getElementById('input-meter-bar');
    const inputPeakValue = document.getElementById('input-peak-value');
    const outputMeterBar = document.getElementById('output-meter-bar');
    const outputPeakValue = document.getElementById('output-peak-value');

    if (inputMeterBar && outputMeterBar) {
        // Listen for the 'meterLevels' event dispatched from the C++ timer callback
        window.__JUCE__.backend.addEventListener("meterLevels", () => {
            // When the event is received, fetch the corresponding JSON data.
            fetch(Juce.getBackendResourceAddress("meterLevels.json"))
                .then(response => response.json()) // Directly parse JSON
                .then(levelData => {
                    updateMeter(levelData.input, inputMeterBar, inputPeakValue);
                    updateMeter(levelData.output, outputMeterBar, outputPeakValue);
                })
                .catch(error => console.error('Error fetching meter level:', error));
        });
    } else {
        console.error("Metering elements not found.");
    }
}
