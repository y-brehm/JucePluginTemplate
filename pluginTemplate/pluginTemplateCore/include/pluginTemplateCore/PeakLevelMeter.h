#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>

/**
 * @class LevelMeter
 * @brief Calculates and holds the peak level from an audio stream for UI display.
 *
 * This class finds the true peak value within each processed block and applies
 * a hold-and-release characteristic, making it suitable for a visual meter.
 * This implementation is real-time safe.
 */
class PeakLevelMeter
{
public:
    PeakLevelMeter();

    void prepare(const juce::dsp::ProcessSpec& spec, float releaseTimeMs = 1700.0f); // Type I PPM release time: 20dB fall in 1.7s

    void process(const juce::AudioBuffer<float>& buffer);

    float getLevelDb(int channel) const;

    float getMonoPeakDb() const;

private:
    std::vector<juce::Atomic<float>> peakGains;

    float releaseCoeff = 1.0f;
    int numChannels = 0;
};
