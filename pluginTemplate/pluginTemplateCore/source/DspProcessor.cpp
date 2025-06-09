#include "pluginTemplateCore/DspProcessor.h"


void DspProcessor::process(juce::AudioBuffer<float>& buffer, float gainToApply)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            channelData[i] *= gainToApply;
        }
    }
}
