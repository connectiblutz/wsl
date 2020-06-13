//
//    Copyright (C) Microsoft.  All rights reserved.
// Licensed under the terms described in the LICENSE file in the root of this project.
//

#include "WslApiLoader.h"
#include "logutil.h"

WslApiLoader::WslApiLoader(const std::wstring& distributionName) :
    _distributionName(distributionName)
{
    _wslApiDll = LoadLibraryEx(L"wslapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (_wslApiDll != nullptr) {
        _isDistributionRegistered = (WSL_IS_DISTRIBUTION_REGISTERED)GetProcAddress(_wslApiDll, "WslIsDistributionRegistered");
        _registerDistribution = (WSL_REGISTER_DISTRIBUTION)GetProcAddress(_wslApiDll, "WslRegisterDistribution");
        _configureDistribution = (WSL_CONFIGURE_DISTRIBUTION)GetProcAddress(_wslApiDll, "WslConfigureDistribution");
        _launchInteractive = (WSL_LAUNCH_INTERACTIVE)GetProcAddress(_wslApiDll, "WslLaunchInteractive");
        _launch = (WSL_LAUNCH)GetProcAddress(_wslApiDll, "WslLaunch");
        _getDistributionConfiguration = (WSL_GET_DISTRIBUTION_CONFIGURATION)GetProcAddress(_wslApiDll,"WslGetDistributionConfiguration");
    }
}

WslApiLoader::~WslApiLoader()
{
    if (_wslApiDll != nullptr) {
        FreeLibrary(_wslApiDll);
    }
}

BOOL WslApiLoader::WslIsOptionalComponentInstalled()
{
    return ((_wslApiDll != nullptr) && 
            (_isDistributionRegistered != nullptr) &&
            (_registerDistribution != nullptr) &&
            (_configureDistribution != nullptr) &&
            (_launchInteractive != nullptr) &&
            (_launch != nullptr) &&
            (_getDistributionConfiguration != nullptr));
}

BOOL WslApiLoader::WslIsDistributionRegistered()
{
    return _isDistributionRegistered(_distributionName.c_str());
}

HRESULT WslApiLoader::WslRegisterDistribution()
{
    HRESULT hr = _registerDistribution(_distributionName.c_str(), L"install.tar.gz");
    if (FAILED(hr)) {
        apfd::common::LogUtil::Debug()<<"MSG_WSL_REGISTER_DISTRIBUTION_FAILED "<< hr;
    }

    return hr;
}

HRESULT WslApiLoader::WslConfigureDistribution(ULONG defaultUID, WSL_DISTRIBUTION_FLAGS wslDistributionFlags)
{
    HRESULT hr = _configureDistribution(_distributionName.c_str(), defaultUID, wslDistributionFlags);
    if (FAILED(hr)) {
        apfd::common::LogUtil::Debug()<<"MSG_WSL_CONFIGURE_DISTRIBUTION_FAILED "<< hr;
    }

    return hr;
}

HRESULT WslApiLoader::WslLaunchInteractive(PCWSTR command, BOOL useCurrentWorkingDirectory, DWORD *exitCode)
{
    HRESULT hr = _launchInteractive(_distributionName.c_str(), command, useCurrentWorkingDirectory, exitCode);
    if (FAILED(hr)) {
        apfd::common::LogUtil::Debug()<<"MSG_WSL_LAUNCH_INTERACTIVE_FAILED "<< command << " " << hr;
    }

    return hr;
}

HRESULT WslApiLoader::WslLaunch(PCWSTR command, BOOL useCurrentWorkingDirectory, HANDLE stdIn, HANDLE stdOut, HANDLE stdErr, HANDLE *process)
{
    HRESULT hr = _launch(_distributionName.c_str(), command, useCurrentWorkingDirectory, stdIn, stdOut, stdErr, process);
    if (FAILED(hr)) {
        apfd::common::LogUtil::Debug()<<"MSG_WSL_LAUNCH_FAILED "<< command << " " << hr;
    }

    return hr;
}

HRESULT WslApiLoader::WslGetDistributionConfiguration(ULONG *distributionVersion, ULONG *defaultUID, WSL_DISTRIBUTION_FLAGS *wslDistributionFlags, PSTR **defaultEnvironmentVariables, ULONG *defaultEnvironmentVariableCount) {
    HRESULT hr = _getDistributionConfiguration(_distributionName.c_str(), distributionVersion, defaultUID, wslDistributionFlags, defaultEnvironmentVariables, defaultEnvironmentVariableCount);
    if (FAILED(hr)) {
        apfd::common::LogUtil::Debug()<<"MSG_WSL_LAUNCH_FAILED "<< hr;
    }

    return hr;
}
