#include "ckObject.h"

inline void ckObject::debugLog(const char* message, const ckDebugLevel debugLevel) const
{
    if (!message || strlen(message) == 0) { return; }
    std::string head = (objName.empty() ? "Unknown" : objName) + "::";
    switch (debugLevel)
    {
        case NONE: break;
        case HINT: head.append("HINT::");
            break;
        case WARNING: head.append("WARNING::");
            break;
        case ERROR: head.append("ERROR::");
            break;
    }
    std::cout << head << message << "\n";
}
