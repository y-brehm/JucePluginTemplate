#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <pluginTemplateCore/DspProcessor.h>
#include <pluginTemplateCore/PeakLevelMeter.h>

class JucePluginTemplateAudioProcessor  : public juce::AudioProcessor
{
public:
    JucePluginTemplateAudioProcessor();
    ~JucePluginTemplateAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    [[nodiscard]] juce::AudioProcessorValueTreeState& getValueTreeState() noexcept {
        return _valueTreeState;
    }

    [[nodiscard]] float getMonoOutputPeakLevelDb() const;
    [[nodiscard]] float getMonoInputPeakLevelDb() const;


private:
    struct Parameters
    {
        juce::AudioParameterFloat* gain   { nullptr };
        juce::AudioParameterBool* clipping { nullptr };
        juce::AudioParameterBool* bypass { nullptr };
    };
    [[nodiscard]] static juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout(Parameters&);

    Parameters _parameters;

    juce::AudioProcessorValueTreeState _valueTreeState;
    DspProcessor _dspProcessor;
    PeakLevelMeter _outputLevelMeter;
    PeakLevelMeter _inputLevelMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePluginTemplateAudioProcessor)
};
