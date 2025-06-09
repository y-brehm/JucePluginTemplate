#include "PluginProcessor.h"
#include "PluginEditor.h" 
#include "ParameterIDs.h"

#include <juce_audio_processors/juce_audio_processors.h>


JucePluginTemplateAudioProcessor::JucePluginTemplateAudioProcessor()
  : AudioProcessor (BusesProperties()
                      #if ! JucePlugin_IsMidiEffect
                       #if ! JucePlugin_IsSynth
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       #endif
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      #endif
                   ),
    valueTreeState {*this, nullptr, "Parameters", createParameterLayout(parameters)}
{
}

JucePluginTemplateAudioProcessor::~JucePluginTemplateAudioProcessor()
{
}



const juce::String JucePluginTemplateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JucePluginTemplateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JucePluginTemplateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JucePluginTemplateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JucePluginTemplateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JucePluginTemplateAudioProcessor::getNumPrograms()
{
    return 1;
}

int JucePluginTemplateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JucePluginTemplateAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String JucePluginTemplateAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void JucePluginTemplateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void JucePluginTemplateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void JucePluginTemplateAudioProcessor::releaseResources()
{
}

bool JucePluginTemplateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
 #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
 #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
 #endif
}

void JucePluginTemplateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (parameters.bypass->get() || buffer.getNumSamples() == 0)
        return;


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] *= parameters.gain->get();
        }
    }
}

//==============================================================================
bool JucePluginTemplateAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* JucePluginTemplateAudioProcessor::createEditor()
{
    // Pass the valueTreeState to the editor so it can interact with parameters
    return new JucePluginTemplateAudioProcessorEditor (*this);
    // IMPORTANT: We'll need to update PluginEditor to accept and use this valueTreeState.
    // For now, if PluginEditor's constructor doesn't take valueTreeState, this might cause
    // a compile error or it will simply not use it yet.
    // The original editor constructor was: JucePluginTemplateAudioProcessorEditor (JucePluginTemplateAudioProcessor&);
    // This remains compatible. The editor will need to *access* processorRef.valueTreeState.
}

//==============================================================================
void JucePluginTemplateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Use APVTS to get the state
    auto state = valueTreeState.copyState(); // Get the ValueTree state
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    if (xml != nullptr)
        copyXmlToBinary (*xml, destData);
}

void JucePluginTemplateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Use APVTS to restore the state
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (valueTreeState.state.getType())) // Check if it's our state
            valueTreeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout
JucePluginTemplateAudioProcessor::createParameterLayout(
    JucePluginTemplateAudioProcessor::Parameters& parameters) {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
  {
    auto parameter = std::make_unique<juce::AudioParameterFloat>(
        GAIN, "gain", juce::NormalisableRange<float>{0.f, 1.f, 0.01f, 0.9f}, 1.f);
    parameters.gain = parameter.get();
    layout.add(std::move(parameter));
  }

  {
    auto parameter = std::make_unique<juce::AudioParameterBool>(
        BYPASS, "bypass", false,
        juce::AudioParameterBoolAttributes{}.withLabel("Bypass"));
    parameters.bypass = parameter.get();
    layout.add(std::move(parameter));
  }

  return layout;
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JucePluginTemplateAudioProcessor();
}
