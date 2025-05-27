#include "OSCGyroReceiver.h"
#include "ParameterBank.h"

OSCGyroReceiver::OSCGyroReceiver(int listenPort)
{
    if (!connect(listenPort))
    {
        jassertfalse;                  // failed to bind OSC port
        return;
    }

    // Only listen for the addresses we actually transmit now
    addListener(this, "/velocity");
    addListener(this, "/rotation");
}

OSCGyroReceiver::~OSCGyroReceiver()
{
    disconnect();
}

void OSCGyroReceiver::oscMessageReceived(const juce::OSCMessage& m)
{
    const juce::String addr = m.getAddressPattern().toString();

    /* ------------- /velocity ------------- */
    if (addr == "/velocity"
        && m.size() == 1 && m[0].isFloat32())
    {
        parameterBank->velocity = m[0].getFloat32();
        return;
    }

    /* ------------- /rotation ------------- */
    if (addr == "/rotation"
        && m.size() == 1 && m[0].isFloat32())
    {
		parameterBank->rotZ = m[0].getFloat32();
        return;
    }

    DBG("Unknown or malformed OSC message: " << addr);
}
