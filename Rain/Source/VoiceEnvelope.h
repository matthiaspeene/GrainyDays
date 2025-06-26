#pragma once
#include "VoicePool.h"
#include <cmath>
#include <limits>
#include <algorithm>

namespace voice::env
{
    inline float powCurve(float x, float power) noexcept
    {
        return std::pow(std::clamp(x, 0.0f, 1.0f), power);
    }

    /*--------------------------------------------------------------------
        VoicePool::setStage  – initialise a voice stage
    --------------------------------------------------------------------*/
    inline void setStage(VoicePool& vp, std::size_t v, Stage st) noexcept
    {
        vp.stage[v] = st;

        switch (st)
        {
        case Stage::Attack:   /* level     : 0 → 1                        */
            vp.level[v] = 0.0f;
            vp.stageSamplesLeft[v] = std::max(1, vp.attackSamples[v]);
            vp.levelStep[v] = 1.0f / vp.stageSamplesLeft[v];        
            break;

        case Stage::Decay:    /* level     : 1 → sustainLevel             */
            vp.stageSamplesLeft[v] = std::max(1, vp.decaySamples[v]);
            vp.levelStep[v] = 1.0f / vp.stageSamplesLeft[v];
            break;

        case Stage::Sustain:  /* level stays at sustainLevel              */
            vp.level[v] = vp.sustainLevel[v];
            vp.stageSamplesLeft[v] = std::numeric_limits<int>::max();
            vp.levelStep[v] = 0.0f;
            break;

        case Stage::Release:                  /* level : current → 0          */
            vp.stageSamplesLeft[v] = std::max(1, vp.releaseSamples[v]);
            vp.levelStep[v] = 1.0f / vp.stageSamplesLeft[v];  // <-- POSITIVE
            vp.releaseStart[v] = vp.level[v];                    // store current
            break;

        default: /* Idle */   vp.stageSamplesLeft[v] = 0;  break;
        }
    }

    /*--------------------------------------------------------------------
        Advance every active voice envelope by ONE sample
    --------------------------------------------------------------------*/
    inline void updateOneSample(VoicePool& vp) noexcept
    {
        /* Quick early-out: nothing sounding, nothing to do */
        if (!vp.active.any())
            return;

        for (std::size_t v = 0; v < VoicePool::kMaxVoices; ++v)
        {
            if (!vp.active.test(v))
                continue;

            /* -------- per-voice state snapshot ----------------------------------- */
            int   samplesLeft = --vp.stageSamplesLeft[v];   // pre-decrement
            float progStep = vp.levelStep[v];            // 1 / stageLen
            float level = vp.level[v];

            /* ---------- stage integrator ----------------------------------------- */
            switch (vp.stage[v])
            {
                case Stage::Attack: {
                    const float p = 1.0f - samplesLeft * progStep;           // 0→1
                    level = powCurve(p, vp.attackPower[v]);
                    if (samplesLeft == 0)
                        setStage(vp, v, Stage::Decay);
                    break;
                }

                case Stage::Decay:
                {
                    const float p = 1.0f - samplesLeft * progStep;   // 0 → 1 through decay
                    const float k = powCurve(1.0f - p, vp.decayPower[v]);

                    /* cross-fade from 1.0 down to sustainLevel */
                    level = vp.sustainLevel[v] + (1.0f - vp.sustainLevel[v]) * k;

                    if (samplesLeft == 0)
                        setStage(vp, v, Stage::Sustain);

					if (level < 0.0001f)  // stop silent notes
					{
						vp.active.reset(v);
						vp.stage[v] = Stage::Idle;  // no more sound
						level = 0.0f;               // reset level
					}

                    break;
                }


                case Stage::Sustain:
                    /* nothing to do – level already == sustainLevel */
                    break;

                case Stage::Release: 
                {
                    const float p = samplesLeft * progStep;       // 1 → 0  (positive now)
                    level = vp.releaseStart[v] *                 // fade from stored level
                        powCurve(p, vp.releasePower[v]);    // curved toward 0

                    if (samplesLeft == 0 || level < 0.001)
                    {
                        vp.active.reset(v);
                        vp.stage[v] = Stage::Idle;
                    }
                    break;
                }
            }// End of switch

            vp.level[v] = level;
		}
		/* --------- end of per-voice loop --------------------------------------- */
	}


    /*--------------------------------------------------------------------
        Simple note-on / note-off helpers
    --------------------------------------------------------------------*/
    inline void noteOn(VoicePool& vp, std::size_t v) noexcept
    {
        vp.active.set(v);
        setStage(vp, v, Stage::Attack);
    }

    inline void noteOff(VoicePool& vp, std::size_t v) noexcept
    {
        if (vp.active.test(v))
            setStage(vp, v, Stage::Release);
    }

}