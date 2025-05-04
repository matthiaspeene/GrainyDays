---
title: 🔧 Grain Parameter Thread Safety in a Granular Synth
date: 2025-05-04
layout: post
categories: devlog
tags:
  - granular
  - parameters
  - performance
  - DOD
---
### 🎛️ Introduction

In my granular synthesizer engine, one of the key challenges is how to safely share control parameters — like pitch — between the UI thread and the real-time audio thread without introducing performance issues or race conditions.  

After careful evaluation, I’ve settled on a **copy-on-initialization strategy** backed by a **single atomic for each modifiable parameter**. This post explains how that works, why it’s thread-safe, and how it fits perfectly with my data-oriented design (DOD) architecture.

---

### 🧵 The Problem: Multithreaded Parameter Control

In typical granular synths, external parameters (like a pitch knob from the UI) may influence many grains. However, naively reading shared values from both the **audio thread** and **UI thread** can introduce:
- 🔁 **Data races**
- 🧠 **Cache thrashing**
- 🐌 **Performance bottlenecks**

You can’t just let the UI read or write into the grainpool — it’s running at 60Hz, while the audio thread may call the render loop 80–100 times per second. We need a strategy where each thread can safely do its job **without stepping on each other**.

---

### ✅ The Solution: Copy-on-Init + Atomic Control

Here’s how I solved it:

1. **Atomic Parameter Storage:**  
   Each shared parameter (e.g. `Pitch`) lives in an `std::atomic<float>` — this allows the UI thread to safely write new values **without locking**.

2. **Grain Activation Copies the Value:**  
   When a new grain is activated, it reads the latest atomic value and stores it locally in the grainpool’s `grainPitch[]` array.

3. **No Cross-Thread Access Post-Init:**  
   After that, grains are fully self-contained. They do not read shared parameters anymore, so the audio thread is completely deterministic and real-time safe.

Here’s a visual summary of the flow:
![[ThreadSafetyVisualization.png]]

---

### 🧠 Why This Works

This approach has a few major advantages:

- ✅ **Thread Safety:** The only cross-thread operation is an atomic read at grain spawn time.
- ✅ **No Locks:** Everything happens lock-free — essential for real-time audio.
- ✅ **No Contention:** After initialization, grains operate independently.
- ✅ **DOD Friendly:** Each grain owns its own immutable copy of initial parameters.

This fits perfectly with a **data-oriented design** where each audio system accesses only the data it needs in a linear, cache-coherent way.

---

### 🧩 Bonus Optimization: Visualizing Grains Without Sync

In a related optimization, I’ve extended this idea to the UI thread for **grain visualization**. Instead of pulling live data from the audio thread, I copy only the grain’s **initial parameters** at activation (e.g. pitch, start position, amplitude).  

From that, the UI can calculate:
- **Position** = `startPos + playBackSpeed × (now - startTime)` (*Simplified math*)
- **Volume** = envelope function evaluated at `t = now - startTime`

This makes it completely thread-safe with no need for ring buffers or shadow copies — just a simple **immutable snapshot** and a shared clock.

Here's a simplified visualization added to the flowchart.
![[VisualizationWithoutSync.png]]


> “Since we’re using DOD, it’s probably faster to copy the initial data and calculate the path using that data than getting the data from the other thread every frame.”  
> *– From my design notes*

---

### 🧪 Implementation Snippet

```cpp

// This struct is used to safely send grain init data to the UI thread
struct VisualGrainInit {
    int index;
    float pitch;
    float volume;
    double startTime;
    // etc.
};

// Shared parameter
std::atomic<float> pitchParam = 1.0f;
// Other atomic parameters...

// Lock-free queue shared between threads (audio producer → UI consumer)
LockFreeQueue<VisualGrainInit, 128> visualGrainQueue; // e.g. SPSC queue

// Audio thread (grain spawn)
void activateGrain(int index) {
    // Audio init (thread-local / thread-safe)
    grainPool.grainPitch[index] = pitchParam.load(std::memory_order_relaxed);
    grainPool.grainVolume[index] = 1.0f;
    grainPool.grainStartTime[index] = currentAudioTime;
    // ... other initialization

	// copy for UI
    VisualGrainInit visualData {
        .index = index,
        .pitch = grainPool.grainPitch[index],
        .volume = grainPool.grainVolume[index],
        .startTime = grainPool.grainStartTime[index],
    };


    // No locks needed like this. UI adds the data to its array when its safe
    visualGrainQueue.push(visualData);
}
```

```cpp
// UI thread visualization
float now = Time::getMillisecondCounterHiRes() / 1000.0;
for (auto& grain : activeGrainsUI) {
    float dt = now - grain.startTime;
    float x = grain.startPosition + grain.pitch * dt;
    float amp = evaluateEnvelope(dt, grain.envParams);
    drawGrain(x, amp);
}
```

---

### 🧱 Summary

By limiting cross-thread access to a single atomic read **at the moment of activation**, the entire system remains:
- **Fast**
- **Safe**
- **Modular**
- **Easy to reason about**

This strategy scales well even with 1000+ grains and opens the door to efficient real-time visualization and modulation.

Next up: I'll start work on these systems. Hopefully I'll have the first prototype around next week.

Thanks for reading! 👋