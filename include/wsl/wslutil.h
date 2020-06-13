#pragma once

#include <string>

namespace wsl {

class WslUtil {
    public:
        static std::string getIP(const std::string& distro, const std::string& intf);
        static unsigned long getVersion(const std::string& distro);
        static std::string run(const std::string& distro, const std::string& command);
};

}