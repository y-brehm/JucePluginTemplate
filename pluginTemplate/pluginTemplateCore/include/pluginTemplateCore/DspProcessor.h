#include <juce_audio_basics/juce_audio_basics.h>


class DspProcessor
{
public:
    void process(juce::AudioBuffer<float>& buffer, float gain, bool clippingEnabled);
};
