---
layout: home
title: "GrainyDays Devlog"
---

# 🎛️ GrainyDays Devlog

Welcome to the development blog for **GrainyDays** — a granular synthesizer engine shaped by chaos, texture, and expressive sound design.

This devlog tracks the technical and creative journey behind building the synth:  
from grain spawning algorithms and DSP optimization, to filter modulation, UI prototyping, and eventual plugin release.

Whether you're here to learn about granular synthesis, peek into engine development, or just follow the build, you're in the right place.

---

## 📬 Latest Posts

{% for post in site.posts limit:5 %}
- 📅 {{ post.date | date: "%B %d, %Y" }} — [**{{ post.title }}**]({{ site.baseurl }}{{ post.url }})

{% endfor %}

---

## 🎯 What Is GrainyDays?

**GrainyDays** is:
- A modular granular synthesis engine
- Built for creative exploration and sonic unpredictability
- Designed in C++ with per-grain filtering and SIMD-aware processing
- Targeted for VST/AU plugin formats

---

## 🌱 Dev Goals

- 🎚️ Real-time performance with large grain counts
- 🔁 Flexible modulation routing per grain
- 🔍 Optimized iteration strategies (bitsets, freelists, job systems)
- 🎨 Retro-modern aesthetic with clean audio UX

---

Want to see the code? [Check out the GitHub repo](https://github.com/matthiaspeene/grainydays)

Thanks for reading — and let the grains fall where they may ☔🎶
