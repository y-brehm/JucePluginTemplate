#pragma once

#include "PluginProcessor.h"
#include "DynamicResourceProvider.h"
#include <juce_gui_extra/juce_gui_extra.h>

class JucePluginTemplateAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                                private juce::Timer
{
public:
    explicit JucePluginTemplateAudioProcessorEditor (JucePluginTemplateAudioProcessor&);
    ~JucePluginTemplateAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    void registerDynamicEndpoints();

    JucePluginTemplateAudioProcessor& _processorRef;
    DynamicResourceProvider _dynamicResourceProvider;


    juce::WebSliderRelay _gainRelay;
    juce::WebToggleButtonRelay _bypassRelay;
    juce::WebToggleButtonRelay _clippingRelay;

    juce::WebBrowserComponent _webBrowserComponent;

    juce::WebSliderParameterAttachment _gainSliderAttachment;
    juce::WebToggleButtonParameterAttachment _bypassAttachment;
    juce::WebToggleButtonParameterAttachment _clippingAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePluginTemplateAudioProcessorEditor)
};
