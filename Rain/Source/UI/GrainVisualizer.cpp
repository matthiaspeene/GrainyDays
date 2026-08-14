#include "GrainVisualizer.h"
#include "WaveformDisplayMetrics.h"

GrainVisualizer::GrainVisualizer(GrainVisualData& visualDataToUse)
    : visualData(visualDataToUse)
{
    startTimerHz(60);    // Repaint at 60 fps
	setInterceptsMouseClicks(false, false);
}

void GrainVisualizer::paint(juce::Graphics& g)
{
    TRACE_COMPONENT();

	const uint64_t totalSamplesRendered = visualData.totalSamplesRendered.load(std::memory_order_relaxed);

    for (size_t i = 0; i < GrainVisualData::kMaxGrains; ++i)
    {
        // Skip inactive grains
        if (!visualData.active[i].load(std::memory_order_acquire))
            continue;

		const uint64_t startTime = visualData.startTime[i];
		const int visualLength = visualData.length[i];
		const int sampleLength = visualData.sampleLength[i];

        // Finished
        if (visualLength <= 0 || sampleLength <= 1
            || startTime + static_cast<uint64_t>(visualLength) <= totalSamplesRendered)
        {
            visualData.active[i].store(false, std::memory_order_release);
            continue;
        }

        // Not started
        if (totalSamplesRendered < startTime)
            continue;

        const uint64_t timeSinceStart = totalSamplesRendered - startTime;

        // horizontal pos
        const auto currentPos = visualData.samplePos[i]
            + static_cast<double>(timeSinceStart) * visualData.step[i];
        const auto lastSample = static_cast<double>(sampleLength - 1);
        const auto normalisedPosition = static_cast<float>(juce::jlimit(0.0, lastSample, currentPos) / lastSample);
        const auto sampleBounds = waveformDisplay::getSampleBounds(getLocalBounds());
        const float x = sampleBounds.getX() + normalisedPosition * sampleBounds.getWidth();

        // vertical pos
        const float maxGain = visualData.maxGain[i];
        if (maxGain <= 0.0f)
            continue;

        const int  attack = visualData.envAttackTime[i];
        const int  release = visualData.envReleaseTime[i];
        const int  totalLen = visualLength;
        const int  sustainEnd = totalLen - release;

        float gain;

        // ─────────────────────────────────────────────── Attack
        if (timeSinceStart < (uint64_t)attack)
        {
            const float norm = (float)timeSinceStart / (float)attack;            // 0…1
            const float curve = visualData.envAttackCurve[i];
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
            const float curve = visualData.envReleaseCurve[i];
            gain = (1.0f - std::pow(norm, curve)) * maxGain;
        }

        // To screenspace
        const float y = getHeight() * (1.0f - gain);
        g.setColour(juce::Colour::fromFloatRGBA(0.5f, 0.5f, 1.0f, 0.5f));
        g.fillEllipse(juce::Rectangle<float>(waveformDisplay::grainMarkerDiameter,
                                             waveformDisplay::grainMarkerDiameter)
                          .withCentre({ x, y }));
    }
}

void GrainVisualizer::timerCallback()
{
    repaint();
}
