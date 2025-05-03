---
title: "Grain Spawning and Processing"
layout: post
---

# Grain Spawning

**When and how** new grains are created.

## Responsibilities

1. Pull trigger events (MIDI, LFO, sequencer).
2. Pick a free slot from the global grain pool.
3. Initialise per-grain fields (_startPos, pitch, envPhase, voiceId, etc._).
4. Enforce per-voice and global polyphony limits.
5. Handle which voices are active (push slot index to `activeList`).

_...etc (paste the rest of your log here)_.
