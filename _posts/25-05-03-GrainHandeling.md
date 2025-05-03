---
title: Grain Spawning and Processing
date: 2025-05-03
layout: post
categories: devlog
tags:
  - granular
  - synthesis
  - engine
  - performance
---
---

# Grain Spawning

**When and how** new grains are created.

## Responsibilities

1. Pull trigger events (`MIDI` / `sequencer`)
2. Pick a free slot from the global grain pool
3. Initialize per-grain fields (`startPos`, `pitch`, `envPhase`, `voiceId`, etc.)
4. Enforce per-voice and global polyphony limits
5. Push slot index to `activeList`

---

### Design

Start with a **fixed-size free list**, and scale to a **ring buffer** in the future if really needed.

To estimate **max simultaneous grains**:

```
density × max(grainLen) × maxVoices → round up and add 50%
```

Example:  
150 grains/sec × 0.150s × 16 voices ≈ **360** → choose 1024 or 2048 for headroom.

**Freelist overflow behaviour:** Drop the new grain.

---

# Grain Processing

> **Scope:** Every step that reads, updates, or mixes grain data inside the audio callback.

---

## 2.1 Iteration Strategy Roadmap

We start with the simplest **linear pass**, and only adopt a more advanced scan when profiling shows measurable benefit.

---

### 2.1.1 Baseline – Linear Pass (MVP)

Linear loop over the entire grain pool, skipping silent slots via a boolean flag.

```cpp
for (int i = 0; i < kMaxGrains; ++i) {
    if (!grainPool.active[i]) continue;
    processGrain(i);
}
```

- Quick to implement and debug
- Works for any pool size
- Always touches every slot each callback (O(kMaxGrains))

---

### 2.1.2 Option A – Bit-Set Scan Loop (Preferred Upgrade)

Replace the `active` bool array with a compact **bit-set** (`std::bitset` or `uint64_t[]`).  
Use `popcnt`/`ctz` tricks to visit only active grains.

```cpp
while (word) {
    int bit = std::countr_zero(word);      // index of least-significant 1-bit
    processGrain(base + bit);              // base = w * 64
    word &= word - 1;                      // clear that 1-bit
}
```

- Best when active grains ≤ 25% of the pool
- Cache-friendly, SIMD-ready
- Slightly harder to read/test

---

### 2.1.3 Option B – Active Index List

Keep a `std::vector<int>` of active indices; spawn/despawn pushes or pops.

- Easiest sparse strategy
- Overhead to maintain list at spawn/despawn
- Vector reallocation possible (use `.reserve()`)

---

### 2.1.4 Decision Criteria

| Scenario (512-sample block)          | Adopt Option A/B if...              |
|--------------------------------------|-------------------------------------|
| 4096-slot pool, ≤ 1024 active grains | Linear loop ≥ 1.2× slower           |
| Peak-density stress test (4096 active) | Linear loop causes xRuns ≥ 0.6 ms |

_Always profile the current build before upgrading._

---

# Implementation Summary

## Option A – Bit-Set Scan Loop  
_This is the **preferred** option._

- Uses compact `std::bitset` or `uint64_t[]`
- Scans with `popcnt`/`ctz` tricks
- Excellent cache locality, SIMD-friendly

## Option B – Active Index List

- Simple vector of active slot indices
- Easier than bitset math
- Requires careful push/pop management

---

## Future Enhancements

- SIMD inner kernel
- Radix-sort of `activeMask`
- Job system for parallel grain processing

---

# Grain Envelopes

Each grain has **three adjustable envelopes**. These don’t follow a typical ADSR pattern and can be customized.

Envelopes act as **modulators** that can affect various grain parameters:
- Envelope 1 modulates volume (by default)
- Others may modulate pitch, filter cutoff, etc.

---

## Grain Volume

Envelope 1 scales grain amplitude. Implementation will determine whether it's a direct multiplier or part of a modulation matrix.

---

## Grain Pitch

Currently uses playback speed (tape-style).  
Future options could include:
- Semitone quantization
- Time-stretching without pitch change

---

## Grain Filters

Questions still open:
- If the filter isn’t modulated, can we skip it for performance?
- Is it on by default or opt-in?
- What filter shapes are available? (e.g., LPF, BPF, HPF, etc.)
- What’s the algorithm used? Biquad, SVF, or something custom?

---

# Grain Mixing

Takes rendered grain frames and mixes them into:

1. **Voice bus** – allows per-note effects and automation
2. **Master bus** – used for global FX, limiting, and dither

### Gain-Staging Rules

- Grain gain → Voice gain → Master gain
- Avoid > 0 dBFS at any stage
- Add dither on master output (planned)

---