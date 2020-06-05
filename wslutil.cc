#include "wslutil.h"
#include "executil.h"
#include "stringutil.h"
#include "logutil.h"
#include "WslApiLoader.h"
#include <windows.h>
#include <wslapi.h>
#include <sstream>

namespace apfd::wsl {


unsigned long WslUtil::getVersion(const std::string& distro) {
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

std::string WslUtil::getIP(const std::string& distro, const std::string& intf) {
  std::string output = run(distro,"/bin/bash -c \"ip -4 addr show "+intf+" | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'\"");
  return common::StringUtil::trim(output);
}

std::string WslUtil::run(const std::string& distro, const std::string& command) {
  std::string ret;
  // Create a pipe to read the output of the launched process.
  HANDLE readPipe;
  HANDLE writePipe;
  SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, true};
  auto wDistro = common::StringUtil::toWide(distro); 
  auto loader =  WslApiLoader(wDistro);   
  if (CreatePipe(&readPipe, &writePipe, &sa, 0)) {
    // Query the UID of the supplied username.
    common::LogUtil::Debug()<<command;
    auto wCommand = common::StringUtil::toWide(command); 
    HANDLE child;
    HRESULT hr = loader.WslLaunch(wCommand.c_str(), true, GetStdHandle(STD_INPUT_HANDLE), writePipe, GetStdHandle(STD_ERROR_HANDLE), &child);
    if (SUCCEEDED(hr)) {
      // Wait for the child to exit and ensure process exited successfully.
      WaitForSingleObject(child, INFINITE);
      DWORD exitCode;
      if ((GetExitCodeProcess(child, &exitCode) == false) || (exitCode != 0)) {
        hr = E_INVALIDARG;
      }
      CloseHandle(child);
      char buffer[1024];
      DWORD bytesRead;
      std::stringstream strbuf;
      while (true) {
        DWORD bytesAvail = 0;
        if (!PeekNamedPipe(readPipe, NULL, 0, NULL, &bytesAvail, NULL)) {
            break;
        }
        if (bytesAvail==0) break;
        if (ReadFile(readPipe, buffer, (sizeof(buffer)-1), &bytesRead, nullptr)) {
          if (bytesRead==0) break;
          buffer[bytesRead] = ANSI_NULL;
          strbuf<<buffer;
        }
      }
      ret = strbuf.str();
    }
    CloseHandle(readPipe);
    CloseHandle(writePipe);
  }
  common::LogUtil::Debug()<<ret;
  return ret;
}

}