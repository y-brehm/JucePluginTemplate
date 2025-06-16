#include "pluginTemplateCore/PeakLevelMeter.h"

#include <algorithm>

PeakLevelMeter::PeakLevelMeter() = default;

void PeakLevelMeter::prepare(const juce::dsp::ProcessSpec& spec, float releaseTimeMs)
{
    numChannels = (int)spec.numChannels;
    peakGains.resize(numChannels);

    for (auto& p : peakGains)
        p.set(0.0f);

     // According to the IEC TR 60268-18:1995 standard for PPMs, the release time
     // is defined as the time it takes for the meter to drop by 20 dB.
     // A 20 dB drop corresponds to a linear amplitude ratio of 10^(-20/20) = 0.1.
     constexpr float targetRatio = 0.1f; // Target for a 20dB drop.
     const auto timeInSamples = spec.sampleRate * (releaseTimeMs / 1000.0f);
     releaseCoeff = static_cast<float>(std::pow(targetRatio, 1.0 / timeInSamples));
}

void PeakLevelMeter::process(const juce::AudioBuffer<float>& buffer)
{
    const int numBufferChannels = buffer.getNumChannels();
    const int numBufferSamples = buffer.getNumSamples();

    jassert(numBufferChannels == numChannels);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* channelData = buffer.getReadPointer(ch);

        float runningPeak = peakGains[ch].get();
        for (int i = 0; i < numBufferSamples; ++i)
        {
            const float sampleMagnitude = std::abs(channelData[i]);
            if (sampleMagnitude > runningPeak)
                // Attack: The new peak is updated instantly.
                runningPeak = sampleMagnitude;
            else
                // Release: The stored peak decays sample-by-sample.
                runningPeak *= releaseCoeff;
        }
        peakGains[ch].set(runningPeak);
    }
}

float PeakLevelMeter::getLevelDb(int channel) const
{
    if (juce::isPositiveAndBelow(channel, numChannels))
    {
        const auto currentPeak = peakGains[channel].get();
        return juce::Decibels::gainToDecibels(currentPeak, -100.0f);
    }

    return -100.0f;
}

float PeakLevelMeter::getMonoPeakDb() const
{
    if (numChannels == 0)
        return -100.0f;

    float maxPeak = 0.0f;
    for (int i = 0; i < numChannels; ++i)
        maxPeak = std::max(maxPeak, peakGains[i].get());

    return juce::Decibels::gainToDecibels(maxPeak, -100.0f);
}
