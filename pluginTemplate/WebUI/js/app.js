/**
 * Main application logic for the plugin's Web UI.
 * This script uses the JUCE JavaScript library to synchronize UI controls
 * with the C++ AudioProcessorValueTreeState.
 */

// Import the entire JUCE JavaScript library. The 'Juce' alias is a convention.
import * as Juce from "./juce/index.js";

// This event listener ensures that the DOM is fully loaded before we try to manipulate it.
document.addEventListener('DOMContentLoaded', () => {
    // --- GAIN SLIDER CONTROL ---
    const gainSlider = document.getElementById('gainSlider');
    if (gainSlider) {
        // Get the state object for the 'gain' parameter from the C++ backend.
        // The string 'GAIN' must match the parameter ID in your C++ code.
        const gainSliderState = Juce.getSliderState('GAIN');

        // When the user interacts with the HTML slider, send the new value to the backend.
        // The value is automatically normalized (0.0 to 1.0) by the JUCE library.
        gainSlider.oninput = () => {
            gainSliderState.setNormalisedValue(gainSlider.value);
        };

        // When the parameter changes on the backend (e.g., loading a preset, automation),
        // update the HTML slider's position.
        gainSliderState.valueChangedEvent.addListener(() => {
            gainSlider.value = gainSliderState.getNormalisedValue();
        });

        gainSliderState.propertiesChangedEvent.addListener(() => {
            gainSlider.step = gainSliderState.properties.interval !== 0 ?
                gainSliderState.properties.interval : 0.001;
            // Note: A more robust approach might be needed depending on your NormalisableRange
        });
    } else {
        console.error("Gain slider element not found.");
    }

    // --- BYPASS BUTTON CONTROL ---
    const bypassCheckbox = document.getElementById('bypassCheckbox');
    if (bypassCheckbox) {
        // Get the state object for the 'bypass' parameter from the C++ backend.
        // The string 'BYPASS' must match the parameter ID in C++.
        const bypassToggleState = Juce.getToggleState('BYPASS');

        // When the user clicks the checkbox, send the new boolean state to the backend.
        bypassCheckbox.oninput = () => {
            bypassToggleState.setValue(bypassCheckbox.checked);
        };

        // When the bypass state changes on the backend, update the checkbox.
        bypassToggleState.valueChangedEvent.addListener(() => {
            bypassCheckbox.checked = bypassToggleState.getValue();
        });
    } else {
        console.error("Bypass checkbox element not found.");
    }

    // --- METERING ---
    const meterBar = document.getElementById('meter-bar');
    const peakValue = document.getElementById('peak-value');

    if (meterBar && peakValue) {
        // Listen for the 'outputLevel' event dispatched from the C++ timer callback
        window.__JUCE__.backend.addEventListener("outputLevel", () => {
            // When the event is received, fetch the corresponding JSON data.
            fetch(Juce.getBackendResourceAddress("outputLevel.json"))
                .then(response => response.json()) // Directly parse JSON
                .then(levelData => {
                    // The C++ side sends the level in Decibels (dBFS).
                    const dbValue = levelData.output;

                    // 1. Update the number box with the precise value
                    const displayDb = isFinite(dbValue) ? dbValue.toFixed(1) : "-inf";
                    peakValue.textContent = displayDb + " dB";

                    // 2. Define the meter's visual range in dB.
                    const minDb = -60.0;
                    const maxDb = 6.0; // Extended to +6dB

                    // 3. Clamp the incoming dB value to our meter's visual range for the bar.
                    const clampedDb = Math.max(minDb, Math.min(dbValue, maxDb));
                    
                    // 4. Convert the clamped dB value to a percentage (0-100).
                    const percentage = ((clampedDb - minDb) / (maxDb - minDb)) * 100;

                    // 5. Update the meter bar's height.
                    meterBar.style.height = percentage + '%';

                    // 6. Change color based on the raw dB value.
                    if (dbValue >= 0.0) {        // Red from 0dBFS up
                        meterBar.style.backgroundColor = '#ff4500';
                    } else if (dbValue >= -6.0) { // Yellow from -6dBFS to 0dBFS
                        meterBar.style.backgroundColor = '#ffdd00';
                    } else {                      // Green below -6dBFS
                        meterBar.style.backgroundColor = '#00ff00';
                    }
                })
                .catch(error => console.error('Error fetching meter level:', error));
        });
    } else {
        console.error("Metering elements not found.");
    }


    console.log("Plugin UI initialized.");
});
