#include "wslutil.h"
#include "executil.h"
#include "stringutil.h"
#include "logutil.h"
#include "WslApiLoader.h"
#include <windows.h>
#include <wslapi.h>

namespace apfd::wsl {


bool WslUtil::isWsl(const std::string& ip) {
  return (ip.size()>1 && ip[0]==':' && ip[1]!=':');
}

std::string WslUtil::getWslName(const std::string& ip) {
  if (WslUtil::isWsl(ip)) {
    auto parts = common::StringUtil::split(ip,':');
    if (parts.size()>=2) return parts[1];
  }
  return "";
}
std::string WslUtil::getWslInterface(const std::string& ip) {
  if (WslUtil::isWsl(ip)) {
    auto parts = common::StringUtil::split(ip,':');
    if (parts.size()>=3) return parts[2];
    auto version = WslUtil::getWslVersion(ip);
    switch (version) {
      case 2:
        return "eth0";
      case 1:
      default:
        return "lo";
    }
  }
  return "";
}
unsigned long WslUtil::getWslVersion(const std::string& ip) {
  if (WslUtil::isWsl(ip)) {
    auto distro = WslUtil::getWslName(ip);
    auto wDistro = common::StringUtil::toWide(distro); 
    auto loader =  WslApiLoader(wDistro);   
    ULONG Version, DefaultUID, DefaultEnvCnt;
    WSL_DISTRIBUTION_FLAGS WslFlags;
    PSTR* DefaultEnv = NULL;
    if (S_OK == loader.WslGetDistributionConfiguration(&Version,&DefaultUID,&WslFlags,&DefaultEnv,&DefaultEnvCnt)) {
      /*
      printf("Version: %lu\n"
              "DefaultUID: %lu\n"
              "Default Environment Variables Count: %lu\n",
              Version, DefaultUID, DefaultEnvCnt);
      for (unsigned int i = 0; i < DefaultEnvCnt; i++) {
        printf("Default Environment Variables Array[%u]: %s\n",i,DefaultEnv[i]);
      }
      if (WslFlags&WSL_DISTRIBUTION_FLAGS_ENABLE_INTEROP) printf("WslFlags: WSL_DISTRIBUTION_FLAGS_ENABLE_INTEROP\n");
      if (WslFlags&WSL_DISTRIBUTION_FLAGS_APPEND_NT_PATH) printf("WslFlags: WSL_DISTRIBUTION_FLAGS_APPEND_NT_PATH\n");
      if (WslFlags&WSL_DISTRIBUTION_FLAGS_ENABLE_DRIVE_MOUNTING) printf("WslFlags: WSL_DISTRIBUTION_FLAGS_ENABLE_DRIVE_MOUNTING\n");*/

      for (unsigned int i = 0; i < DefaultEnvCnt; i++) {
        free(DefaultEnv[i]);
      }
      
      common::LogUtil::Debug()<<"wsl version "<<Version;
      return Version;
    }
    return 1;
  }
  return 0;
}

std::string WslUtil::getWslIp(const std::string& ip) {
  if (!WslUtil::isWsl(ip)) return ip;
  std::string vmName = WslUtil::getWslName(ip);
  std::string vmInterface = WslUtil::getWslInterface(ip);
  std::string output = common::ExecUtil::Run("wsl -d "+vmName+" -- /bin/bash -c \"ip -4 addr show "+vmInterface+" | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'\"");
  return common::StringUtil::trim(output);
}

}