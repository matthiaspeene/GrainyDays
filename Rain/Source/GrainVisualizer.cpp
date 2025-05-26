#include "GrainVisualizer.h"

GrainVisualizer::GrainVisualizer()
{
    startTimerHz(60);    // Repaint at 60 fps
	setInterceptsMouseClicks(false, false);
}

void GrainVisualizer::paint(juce::Graphics& g)
{
	// Clear the background
	g.fillAll(juce::Colour::fromFloatRGBA(0,0,0,0));

	const uint64_t totalSamplesRendered = gTotalSamplesRendered.load(std::memory_order_relaxed);

    for (size_t i = 0; i < GrainVisualData::kMaxGrains; ++i)
    {
        // Skip inactive grains
        if (!gGrainVisualData.active[i].load(std::memory_order_acquire))
            continue;

        const int startTime = gGrainVisualData.startTime[i];

        // Finished
        if (gGrainVisualData.startTime[i] + gGrainVisualData.length[i] < totalSamplesRendered)
        {
            gGrainVisualData.active[i].store(false, std::memory_order_release);
            continue;
        }

        // Not started
        if (totalSamplesRendered < startTime)
            continue;

        const uint64_t timeSinceStart = totalSamplesRendered - startTime;

        // horizontal pos
        const int currentPos = gGrainVisualData.samplePos[i]
            + int(timeSinceStart) * gGrainVisualData.step[i];

        const float x = (float)(currentPos % gSampleSize) / (float)gSampleSize * getWidth();

        // vertical pos
        const float maxGain = gGrainVisualData.maxGain[i];
        if (maxGain <= 0.0f)
            continue;

        const int  attack = gGrainVisualData.envAttackTime[i];
        const int  release = gGrainVisualData.envReleaseTime[i];
        const int  totalLen = gGrainVisualData.length[i];
        const int  sustainEnd = totalLen - release;

        float gain;

        // ─────────────────────────────────────────────── Attack
        if (timeSinceStart < (uint64_t)attack)
        {
            const float norm = (float)timeSinceStart / (float)attack;            // 0…1
            const float curve = gGrainVisualData.envAttackCurve[i];
            gain = std::pow(norm, curve) * maxGain;
        }
        // ─────────────────────────────────────────────── Sustain
        else if (timeSinceStart < (uint64_t)sustainEnd)
        {
            gain = maxGain;
        }
        // ─────────────────────────────────────────────── Release
        else
        {
            const float norm = (float)(timeSinceStart - sustainEnd) / (float)release; // 0…1
            const float curve = gGrainVisualData.envReleaseCurve[i];
            gain = (1.0f - std::pow(norm, curve)) * maxGain;
        }

        // To screenspace
        const float y = getHeight() * (1.0f - gain);
        g.setColour(juce::Colour::fromFloatRGBA(0.5f, 0.5f, 1.0f, 0.5f));
        g.fillEllipse(x, y, 10.0f, 10.0f);
    }
}

void GrainVisualizer::timerCallback()
{
    repaint();
}
