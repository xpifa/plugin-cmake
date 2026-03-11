#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace dsp
{
class Gain
{
public:
	static constexpr float minGainDb = -100.0f;
	static constexpr float maxGainDb = 12.0f;

	void prepare (double sampleRate);
	void reset();
	void setGainDecibels (float gainDb);
	void process (juce::AudioBuffer<float>& buffer);

private:
	static constexpr double smoothingTimeSeconds = 0.02;

	juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGainLinear { 1.0f };
};
} // namespace dsp
