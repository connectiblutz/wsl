#pragma once

#include <string>

namespace apfd::wsl {

class WslUtil {
    public:
        static std::wstring getIP(const std::wstring& distro, const std::wstring& intf);
        static unsigned long getVersion(const std::wstring& distro);
        static std::wstring run(const std::wstring& distro, const std::wstring& command);
};

}