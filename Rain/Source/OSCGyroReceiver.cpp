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
	else if (addr == "/rotation" && m.size() >= 3
		&& m[0].isFloat32() && m[1].isFloat32() && m[2].isFloat32() && m[3].isFloat32())
	{
        // this is never reached
		rotX.store(m[0].getFloat32(), std::memory_order_relaxed);
		rotY.store(m[1].getFloat32(), std::memory_order_relaxed);
		rotZ.store(m[2].getFloat32(), std::memory_order_relaxed);
	}
	else
	{
		DBG("Unknown OSC message: " << addr);
	}

    /* 
	DBG("Received OSC message: " << addr
		<< " | gyro: (" << gyroX.load() << ", " << gyroY.load() << ", " << gyroZ.load() << ")"
		<< " | velocity: " << velocity.load()
		<< " | rotation: (" << rotX.load() << ", " << rotY.load() << ", " << rotZ.load() << ")"

    );*/
}
