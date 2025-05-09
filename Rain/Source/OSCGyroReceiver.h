#pragma once
#include <juce_osc/juce_osc.h>

/*
    OSCGyroReceiver.h

    Listens for OSC messages on port 9000:
    - /gyro      → 3 floats (x, y, z)
    - /velocity  → 1 float

    Call `.getGyroX()` etc. from your audio thread.
*/

class OSCGyroReceiver : public juce::OSCReceiver,
    public juce::OSCReceiver::ListenerWithOSCAddress<juce::OSCReceiver::MessageLoopCallback>
{
public:
    OSCGyroReceiver(int listenPort = 9000);
    ~OSCGyroReceiver() override;

    float getGyroX() const noexcept { return gyroX.load(std::memory_order_relaxed); }
    float getGyroY() const noexcept { return gyroY.load(std::memory_order_relaxed); }
    float getGyroZ() const noexcept { return gyroZ.load(std::memory_order_relaxed); }
    float getVelocity() const noexcept { return velocity.load(std::memory_order_relaxed); }
	float getRotX() const noexcept { return rotX.load(std::memory_order_relaxed); }
	float getRotY() const noexcept { return rotY.load(std::memory_order_relaxed); }
	float getRotZ() const noexcept { return rotZ.load(std::memory_order_relaxed); }

private:
    std::atomic<float> gyroX{ 0.0f };
    std::atomic<float> gyroY{ 0.0f };
    std::atomic<float> gyroZ{ 0.0f };
    std::atomic<float> velocity{ 0.0f };
	std::atomic<float> rotX{ 0.0f };
	std::atomic<float> rotY{ 0.0f };
	std::atomic<float> rotZ{ 0.0f };

    void oscMessageReceived(const juce::OSCMessage& message) override;
};
