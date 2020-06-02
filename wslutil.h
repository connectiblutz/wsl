#pragma once

#include <string>

namespace apfd::wsl {

class WslUtil {
    public:
        static bool isWsl(const std::string& ip);
        static std::string getWslName(const std::string& ip);
        static std::string getWslInterface(const std::string& ip);
        static std::string getWslIp(const std::string& ip);
        static unsigned long getWslVersion(const std::string& ip);
};

}