#pragma once

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

class JucePluginTemplateAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit JucePluginTemplateAudioProcessorEditor (JucePluginTemplateAudioProcessor&);
    ~JucePluginTemplateAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JucePluginTemplateAudioProcessor& processorRef;

    juce::WebSliderRelay gainRelay;
    juce::WebToggleButtonRelay bypassRelay;
    
    juce::WebBrowserComponent webBrowserComponent;
    
    juce::WebSliderParameterAttachment gainSliderAttachment;
    juce::WebToggleButtonParameterAttachment bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePluginTemplateAudioProcessorEditor)
};
