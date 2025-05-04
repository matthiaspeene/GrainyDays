---
title: ⏱️ Sample‑Accurate Grain Timing with `delaySamples[]`
date: 2025-05-05
layout: post
categories: devlog
tags:
  - DOD
  - granular
  - timing
  - midi-events
---
### 🎯 Why this post exists

In [[2025-05-03-grainhandeling]] we decided to pull MIDI triggers once per audio block, drop new grains into one global pool, and iterate the pool in a cache‑friendly linear pass . That post stopped right before the sticky question:

> _How do you make a grain start on_ ****_any_**** _sample, not just on buffer boundaries?_

Today’s entry answers that by introducing one deceptively small field—`delaySamples[]`—that unlocks **sample‑accurate grain timing** without breaking the data‑oriented design (DOD) you’ve been following.

---

## 1 · Anatomy of a host MIDI event

Every modern plug‑in API delivers MIDI like this on each callback:

|Field|Meaning|
|---|---|
|`sampleOffset`|Where, within the current audio buffer, the event occurred (0 … `blockSize‑1`).|
|`status`, `note`, `velocity`|Normal MIDI data.|

`sampleOffset` is _not_ look‑ahead; it points squarely **inside the buffer you are about to render**.

---

## 2 · The one‑int solution: `delaySamples[]`

Add a countdown array to the existing grain pool:

```
alignas(64) int   grainDelay   [kMaxGrains]; // ← NEW
alignas(64) int   grainFrames  [kMaxGrains];
alignas(64) int   grainVoiceId [kMaxGrains];
// …other SoA fields (pitch, envPhase, etc.)
```

### Scheduling once per block (control‑rate)

```
void scheduleGrains(Voice& v, int blockSize, int sampleOffset)
{
    const double interval = sampleRate / v.densityHz; // samples per grain
    double cursor = sampleOffset;                     // first grain @ Note‑On

    while (cursor < blockSize) {
        int slot = freelist.pop();           // drop if pool exhausted
        grainDelay   [slot] = (int)cursor;   // countdown to first frame
        grainFrames  [slot] = v.lengthInSamples;
        grainVoiceId [slot] = v.id;
        // …init other arrays here

        cursor += interval;                  // next grain for this voice
    }

    v.phase = cursor - blockSize;            // spill‑over for the next block
}
```

### Rendering every sample (audio‑rate)

```
for (int i = 0; i < kMaxGrains; ++i) {
    if (!activeMask[i]) continue;            // bit‑set or bool flag

    if (grainDelay[i]--)                     // ─┐ tiny cost
        continue;                           // ─┘ skip until zero

    processGrain(i);                        // window, resample, mix
}
```

- **No extra branches** once the delay hits zero.
    
- Works with the linear loop _or_ the bit‑set scan upgrade already on the roadmap.
    

---

## 3 · What does this buy us?

|                            |                                                                                                                                                                                              |
| -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Benefit                    | Explanation                                                                                                                                                                                  |
| **Sample‑accurate starts** | Grains can fire on _any_ of the 128 samples in a 48 kHz / 128‑frame buffer (≈2.7 ms).                                                                                                        |
| **Zero structural change** | Still one global pool, still SoA, still one pass.                                                                                                                                            |
| **Tiny overhead**          | One integer decrement per active grain—negligible beside envelope multiplies and interpolation.                                                                                              |
| **Thread‑safe**            | `delaySamples[]` is written only inside the audio thread at spawn time, aligning with the copy‑on‑init rule from the **Parameter Thread‑Safety** post [[2025-05-04-parameter-threadsafety]]. |

---

## 4 · Edge cases & safeguards

- **Pool overflow → drop** Already defined in the spawning post; nothing new.
    
- **Per‑voice spam** Keep a simple counter; if `activeGrainsInVoice ≥ limit`, stop scheduling for that voice until some finish.
    
- **Note‑Off during release** Grains spawned during the ADSR release inherit the current voice gain so the tail fades smoothly.
    

---

## 5 · Where we go next

- **Tempo‑sync density** Convert `densityHz` → beats, quantise `cursor` to the grid.
    
- **Randomised jitter** Add ±n samples before writing `grainDelay[]`.
    
- **Profiling** Switch to the bit‑set scan when the linear loop shows cache misses at high pool sizes.
    

---

### 📝 Take‑away

> One extra integer per grain buys sample‑accurate timing without compromising DOD cache‑efficiency. Everything else—voice ADSR, parameter safety, future FX—slots neatly on top.

