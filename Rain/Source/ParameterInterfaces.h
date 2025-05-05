#pragma once
class ParameterBank;

class IParameterConsumer
{
public:
    virtual ~IParameterConsumer() = default;
    virtual void setParameterBank(const ParameterBank* bank) = 0;
};
