#include "Gain.h"

namespace dsp
{
void Gain::prepare (double sampleRate)
{
	smoothedGainLinear.reset (sampleRate, smoothingTimeSeconds);
	smoothedGainLinear.setCurrentAndTargetValue (1.0f);
}

void Gain::reset()
{
	smoothedGainLinear.setCurrentAndTargetValue (smoothedGainLinear.getTargetValue());
}

void Gain::setGainDecibels (float gainDb)
{
	const auto clampedDb = juce::jlimit (minGainDb, maxGainDb, gainDb);
	const auto linearGain = juce::Decibels::decibelsToGain (clampedDb, minGainDb);
	smoothedGainLinear.setTargetValue (linearGain);
}

void Gain::process (juce::AudioBuffer<float>& buffer)
{
	const auto numChannels = buffer.getNumChannels();
	const auto numSamples = buffer.getNumSamples();

	for (int sample = 0; sample < numSamples; ++sample)
	{
		const auto gain = smoothedGainLinear.getNextValue();

		for (int channel = 0; channel < numChannels; ++channel)
			buffer.getWritePointer (channel)[sample] *= gain;
	}
}
} // namespace dsp
