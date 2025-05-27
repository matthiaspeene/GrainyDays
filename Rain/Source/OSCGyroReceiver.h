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

    void setParameterBank(class ParameterBank* bank)
    {
		parameterBank = bank;
    };

private:

	ParameterBank* parameterBank = nullptr;

    void oscMessageReceived(const juce::OSCMessage& message) override;
};
