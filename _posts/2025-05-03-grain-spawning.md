---
title: "Grain Spawning and Processing"
date: 2025-05-03
layout: post
categories: devlog
tags: [granular synthesis, engine, performance]
---

# Grain Spawning

**When and how** new grains are created.

## 🛠️ Responsibilities

1. Pull trigger events (`MIDI`, `LFO`, `sequencer`)
2. Pick a free slot from the global grain pool
3. Initialise per-grain fields: `startPos`, `pitch`, `envPhase`, `voiceId`, etc.
4. Enforce **per-voice** and **global** polyphony limits
5. Push active slot index to `activeList`

---

## 🧠 Design Notes

Start with a **fixed-size free list**. Scale to a **ring buffer** only if needed.

To estimate **max simultaneous grains**:

```
density × maxGrainLength × maxVoices
→ round up and add 50% buffer
```

Example:  
150 grains/sec × 0.150s × 16 voices ≈ **360** → pick **1024** or **2048** for headroom.

**On overflow:** Drop the new grain silently.

---

# Grain Processing

> **Scope:** Every step that reads, updates, or mixes grain data inside the **audio callback**.

## Iteration Strategy Roadmap

We begin with a **linear pass** and optimize later based on profiling.

### 🔹 Baseline: Linear Loop

```cpp
for (int i = 0; i < kMaxGrains; ++i) {
    if (!grainPool.active[i]) continue;
    processGrain(i);
}
```

- Simple and debug-friendly
- Always touches `kMaxGrains` slots per callback (O(n))

---

### 🔸 Option A: Bit-Set Scan Loop (Preferred)

```cpp
while (word) {
    int bit = std::countr_zero(word);      // index of least-significant 1-bit
    processGrain(base + bit);              // base = w * 64
    word &= word - 1;                      // clear that 1-bit
}
```

- Use `std::bitset` or `uint64_t[]`
- Best when active grains ≤ 25%
- Cache-friendly, SIMD-ready

---

### 🔸 Option B: Active Index List

- Maintain `std::vector<int>` of active grain indices
- Fast to iterate; minor cost to maintain on spawn/despawn
- Use `.reserve()` to prevent reallocation

---

### 📈 Decision Criteria

| Scenario | Switch to Option A/B if... |
|----------|----------------------------|
| 4,096-slot pool, ≤ 1,024 active grains | Linear loop is ≥ 1.2× slower |
| Peak-density (all active)             | xRuns exceed 0.6ms per callback |

🧪 Always **profile** before optimizing.

---

# 🎛️ Grain Envelopes

Each grain has **3 adjustable envelopes** (not necessarily ADSR).  
They act as **modulators**, affecting grain parameters like:

- Volume (default: envelope 1)
- Pitch
- Filter cutoff

---

## 🔍 Grain Filter Questions

Still open design questions:

- If the filter isn’t modulated, do we skip it for performance?
- Is the filter **on by default**, or opt-in?
- What filter types will be available?
- What’s the core filter **algorithm**?

---

# 🔊 Grain Mixing

Rendered grains mix down into:

1. **Voice bus** – per-note FX, panning, etc.
2. **Master bus** – global FX, dither, limiter

### Gain-Staging Rules

- `Grain gain → Voice gain → Master gain`
- Prevent > 0 dBFS at any stage
- Planned: add dithering on master output

---

# 🧪 Future Enhancements

- SIMD inner kernel for grain DSP
- Radix-sort of `activeMask` for cache grouping
- Async processing via job system
