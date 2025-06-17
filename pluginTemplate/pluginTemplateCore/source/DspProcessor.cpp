#include "pluginTemplateCore/DspProcessor.h"

#include <cmath>


void DspProcessor::process(juce::AudioBuffer<float>& buffer, float gain, bool clippingEnabled)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float sample = channelData[i] * gain;

            if(clippingEnabled)
                sample = std::tanh(sample);

            channelData[i] = sample;
        }
    }
}
