#pragma once
#include "stdInclude.h"

class ckObject
{
private:
    std::string objName;

public:
    ckObject()  = default;
    ~ckObject() = default;

    enum ckDebugLevel
    {
        NONE = "NONE",
        HINT = "HINT",
        WARNING = "WARNING",
        ERROR = "ERROR"
    };

    void debugLog(const char* message, const ckDebugLevel debugLevel = NONE) const;
};
