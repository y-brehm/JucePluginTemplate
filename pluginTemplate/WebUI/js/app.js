/**
 * Main application logic for the plugin's Web UI.
 * This script uses the JUCE JavaScript library to synchronize UI controls
 * with the C++ AudioProcessorValueTreeState.
 */

// Import the entire JUCE JavaScript library. The 'Juce' alias is a convention.
import * as Juce from "./juce/index.js";
import { initializeMetering } from "./metering.js";

// This event listener ensures that the DOM is fully loaded before we try to manipulate it.
document.addEventListener('DOMContentLoaded', () => {
    // --- gain SLIDER CONTROL ---
    const gainSlider = document.getElementById('gainSlider');
    if (gainSlider) {
        // Get the state object for the 'gain' parameter from the C++ backend.
        // The string 'gain' must match the parameter ID in your C++ code.
        const gainSliderState = Juce.getSliderState('GAIN');

        gainSlider.oninput = () => {
            // Here we must convert the slider's raw value to a normalised value
            const properties = gainSliderState.properties;
            const normalisedValue = (gainSlider.value - properties.start) / (properties.end - properties.start);
            gainSliderState.setNormalisedValue(normalisedValue);
        };

        gainSliderState.valueChangedEvent.addListener(() => {
            // Here we convert the scaled value from the backend to the slider's raw value
            gainSlider.value = gainSliderState.getScaledValue();
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
    // --- CLIPPING BUTTON CONTROL (New) ---
    const clippingCheckbox = document.getElementById('clippingCheckbox');
    if (clippingCheckbox) {
        // The ID must match the C++ ParameterID: "CLIPPING"
        const clippingToggleState = Juce.getToggleState('CLIPPING');

        clippingCheckbox.oninput = () => {
            clippingToggleState.setValue(clippingCheckbox.checked);
        };

        clippingToggleState.valueChangedEvent.addListener(() => {
            clippingCheckbox.checked = clippingToggleState.getValue();
        });
    } else {
        console.error("Clipping checkbox element not found.")

    }
    
    initializeMetering();

    console.log("Plugin UI initialized.");
});
