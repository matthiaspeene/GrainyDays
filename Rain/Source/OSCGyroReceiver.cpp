#include "OSCGyroReceiver.h"

OSCGyroReceiver::OSCGyroReceiver(int listenPort)
{
    if (!connect(listenPort))
    {
        jassertfalse; // Failed to bind OSC port
        return;
    }

    addListener(this, "/gyro");
    addListener(this, "/velocity");
}

OSCGyroReceiver::~OSCGyroReceiver()
{
    disconnect();
}

void OSCGyroReceiver::oscMessageReceived(const juce::OSCMessage& m)
{
    const auto& addr = m.getAddressPattern().toString();

    if (addr == "/gyro" && m.size() >= 3
        && m[0].isFloat32() && m[1].isFloat32() && m[2].isFloat32())
    {
        gyroX.store(m[0].getFloat32(), std::memory_order_relaxed);
        gyroY.store(m[1].getFloat32(), std::memory_order_relaxed);
        gyroZ.store(m[2].getFloat32(), std::memory_order_relaxed);
    }
    else if (addr == "/velocity" && m.size() >= 1 && m[0].isFloat32())
    {
        velocity.store(m[0].getFloat32(), std::memory_order_relaxed);
    }

	DBG("Received OSC message: " << addr
		<< " | gyro: (" << gyroX.load() << ", "
		<< gyroY.load() << ", "
		<< gyroZ.load() << ")"
		<< " | velocity: " << velocity.load());
}
