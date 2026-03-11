#include <gtest/gtest.h>

#include "DSP/Gain.h"

namespace
{
float processSingleSampleWithGain (dsp::Gain& gain, float input = 1.0f)
{
    juce::AudioBuffer<float> buffer (1, 1);
    buffer.setSample (0, 0, input);
    gain.process (buffer);
    return buffer.getSample (0, 0);
}
} // namespace

TEST (GainMapping, MapsReferenceDbValuesToExpectedLinearGain)
{
    dsp::Gain gain;
    gain.prepare (1.0); // 20 ms smoothing resolves to 0 steps, so updates are immediate.

    gain.setGainDecibels (0.0f);
    EXPECT_NEAR (processSingleSampleWithGain (gain), 1.0f, 1.0e-6f);

    gain.setGainDecibels (12.0f);
    EXPECT_NEAR (processSingleSampleWithGain (gain), juce::Decibels::decibelsToGain (12.0f, -100.0f), 1.0e-5f);

    gain.setGainDecibels (-100.0f);
    EXPECT_NEAR (processSingleSampleWithGain (gain), 0.0f, 1.0e-7f);
}

TEST (GainMapping, ClampsOutOfRangeDbValues)
{
    dsp::Gain gain;
    gain.prepare (1.0);

    gain.setGainDecibels (24.0f);
    EXPECT_NEAR (processSingleSampleWithGain (gain), juce::Decibels::decibelsToGain (12.0f, -100.0f), 1.0e-5f);

    gain.setGainDecibels (-1000.0f);
    EXPECT_NEAR (processSingleSampleWithGain (gain), 0.0f, 1.0e-7f);
}

TEST (GainSmoothing, RampsUpMonotonicallyToTargetGain)
{
    dsp::Gain gain;
    gain.prepare (1000.0); // 20 ms -> 20 smoothing samples.

    gain.setGainDecibels (0.0f);

    juce::AudioBuffer<float> buffer (1, 24);
    buffer.clear();
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        buffer.setSample (0, i, 1.0f);

    gain.setGainDecibels (6.0f);
    gain.process (buffer);

    const auto target = juce::Decibels::decibelsToGain (6.0f, -100.0f);

    EXPECT_GT (buffer.getSample (0, 0), 1.0f);
    EXPECT_LT (buffer.getSample (0, 0), target);

    for (int i = 1; i < 20; ++i)
        EXPECT_GE (buffer.getSample (0, i), buffer.getSample (0, i - 1));

    EXPECT_NEAR (buffer.getSample (0, 23), target, 1.0e-5f);
}

TEST (GainSmoothing, RampsDownMonotonicallyToTargetGain)
{
    dsp::Gain gain;
    gain.prepare (1000.0);

    gain.setGainDecibels (6.0f);
    (void) processSingleSampleWithGain (gain);

    juce::AudioBuffer<float> buffer (1, 24);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
        buffer.setSample (0, i, 1.0f);

    gain.setGainDecibels (-6.0f);
    gain.process (buffer);

    const auto target = juce::Decibels::decibelsToGain (-6.0f, -100.0f);

    EXPECT_LT (buffer.getSample (0, 0), juce::Decibels::decibelsToGain (6.0f, -100.0f));
    EXPECT_GT (buffer.getSample (0, 0), target);

    for (int i = 1; i < 20; ++i)
        EXPECT_LE (buffer.getSample (0, i), buffer.getSample (0, i - 1));

    EXPECT_NEAR (buffer.getSample (0, 23), target, 1.0e-5f);
}
