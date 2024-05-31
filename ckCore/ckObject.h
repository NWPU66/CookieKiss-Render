#pragma once
#include "stdInclude.h"

class ckObject
{
private:
    std::string objName;

protected:
    enum ckDebugLevel
    {
        NONE,
        HINT,
        WARNING,
        ERROR
    };

public:
    ckObject()  = default;
    ~ckObject() = default;


    inline void debugLog(const char* message, const ckDebugLevel debugLevel = NONE) const;
};
