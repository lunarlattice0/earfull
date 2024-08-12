// Please inherit from this file for system-specific backends.
#pragma once
#include <string>
#include <vector>

class Backend {
    public:
        // This will be a vector of IPs.
        virtual void beginDiscovery() = 0;
        virtual std::vector<std::string> GetPeers() = 0;
        virtual ~Backend(){};
};
