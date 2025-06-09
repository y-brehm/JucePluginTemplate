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
        // The string 'gain' must match the parameter ID in your C++ code.
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
            gainSlider.step = gainSliderState.properties.interval !== 0 ? gainSliderState.properties.interval : 0.001;
            // Note: A more robust approach might be needed depending on your NormalisableRange
        });
    } else {
        console.error("Gain slider element not found.");
    }

    // --- BYPASS BUTTON CONTROL ---
    const bypassCheckbox = document.getElementById('bypassCheckbox');
    if (bypassCheckbox) {
        // Get the state object for the 'bypass' parameter from the C++ backend.
        // The string 'bypass' must match the parameter ID we will create in C++.
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

    console.log("Plugin UI initialized.");
});

