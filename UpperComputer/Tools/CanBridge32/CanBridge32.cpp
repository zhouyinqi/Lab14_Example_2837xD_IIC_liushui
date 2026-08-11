#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdio>
#include <cstring>
#include <cwchar>
#include <string>

namespace {

constexpr DWORD StatusOk = 1U;
constexpr DWORD RequestId = 0x321U;
constexpr DWORD ResponseId = 0x001U;
constexpr BYTE RequestData[] = {0xA5U, 0x5AU, 0xA5U, 0x5AU};

struct VciInitConfig {
    DWORD acceptanceCode;
    DWORD acceptanceMask;
    DWORD reserved;
    BYTE filter;
    BYTE timing0;
    BYTE timing1;
    BYTE mode;
};

struct VciCanObject {
    DWORD id;
    DWORD timeStamp;
    BYTE timeFlag;
    BYTE sendType;
    BYTE remoteFlag;
    BYTE externFlag;
    BYTE dataLength;
    BYTE data[8];
    BYTE reserved[3];
};

static_assert(sizeof(VciInitConfig) == 16U, "Unexpected VCI_INIT_CONFIG size");
static_assert(sizeof(VciCanObject) == 24U, "Unexpected VCI_CAN_OBJ size");

using VciOpenDevice = DWORD(__stdcall *)(DWORD, DWORD, DWORD);
using VciCloseDevice = DWORD(__stdcall *)(DWORD, DWORD);
using VciInitCan = DWORD(__stdcall *)(DWORD, DWORD, DWORD, VciInitConfig *);
using VciStartCan = DWORD(__stdcall *)(DWORD, DWORD, DWORD);
using VciResetCan = DWORD(__stdcall *)(DWORD, DWORD, DWORD);
using VciClearBuffer = DWORD(__stdcall *)(DWORD, DWORD, DWORD);
using VciTransmit = DWORD(__stdcall *)(DWORD, DWORD, DWORD, VciCanObject *, DWORD);
using VciReceive = DWORD(__stdcall *)(DWORD, DWORD, DWORD, VciCanObject *, DWORD, int);

struct Api {
    HMODULE module = nullptr;
    VciOpenDevice openDevice = nullptr;
    VciCloseDevice closeDevice = nullptr;
    VciInitCan initCan = nullptr;
    VciStartCan startCan = nullptr;
    VciResetCan resetCan = nullptr;
    VciClearBuffer clearBuffer = nullptr;
    VciTransmit transmit = nullptr;
    VciReceive receive = nullptr;
};

const wchar_t *FindArgument(int argc, wchar_t *argv[], const wchar_t *name)
{
    for(int index = 1; index + 1 < argc; ++index)
    {
        if(std::wcscmp(argv[index], name) == 0)
        {
            return argv[index + 1];
        }
    }
    return nullptr;
}

DWORD ParseUnsigned(const wchar_t *text)
{
    return static_cast<DWORD>(std::wcstoul(text, nullptr, 0));
}

std::wstring ParentDirectory(const std::wstring &path)
{
    const std::wstring::size_type separator = path.find_last_of(L"\\/");
    return (separator == std::wstring::npos) ? L"." : path.substr(0U, separator);
}

bool LoadApi(const std::wstring &libraryPath, Api *api)
{
    const std::wstring libraryDirectory = ParentDirectory(libraryPath);

    SetCurrentDirectoryW(libraryDirectory.c_str());
    SetDllDirectoryW(libraryDirectory.c_str());
    api->module = LoadLibraryW(libraryPath.c_str());
    if(api->module == nullptr)
    {
        std::printf("RESULT=FAIL;CODE=LOAD;WINERR=%lu\n", GetLastError());
        return false;
    }

    api->openDevice = reinterpret_cast<VciOpenDevice>(
        GetProcAddress(api->module, "VCI_OpenDevice"));
    api->closeDevice = reinterpret_cast<VciCloseDevice>(
        GetProcAddress(api->module, "VCI_CloseDevice"));
    api->initCan = reinterpret_cast<VciInitCan>(
        GetProcAddress(api->module, "VCI_InitCAN"));
    api->startCan = reinterpret_cast<VciStartCan>(
        GetProcAddress(api->module, "VCI_StartCAN"));
    api->resetCan = reinterpret_cast<VciResetCan>(
        GetProcAddress(api->module, "VCI_ResetCAN"));
    api->clearBuffer = reinterpret_cast<VciClearBuffer>(
        GetProcAddress(api->module, "VCI_ClearBuffer"));
    api->transmit = reinterpret_cast<VciTransmit>(
        GetProcAddress(api->module, "VCI_Transmit"));
    api->receive = reinterpret_cast<VciReceive>(
        GetProcAddress(api->module, "VCI_Receive"));

    if((api->openDevice == nullptr) || (api->closeDevice == nullptr) ||
       (api->initCan == nullptr) || (api->startCan == nullptr) ||
       (api->resetCan == nullptr) || (api->clearBuffer == nullptr) ||
       (api->transmit == nullptr) || (api->receive == nullptr))
    {
        std::printf("RESULT=FAIL;CODE=EXPORT\n");
        FreeLibrary(api->module);
        api->module = nullptr;
        return false;
    }

    return true;
}

void UnloadApi(Api *api)
{
    if(api->module != nullptr)
    {
        FreeLibrary(api->module);
        api->module = nullptr;
    }
}

bool IsExpectedResponse(const VciCanObject &frame)
{
    if((frame.id != ResponseId) || (frame.externFlag != 0U) ||
       (frame.remoteFlag != 0U) || (frame.dataLength != sizeof(RequestData)))
    {
        return false;
    }

    for(size_t index = 0U; index < sizeof(RequestData); ++index)
    {
        if(frame.data[index] != RequestData[index])
        {
            return false;
        }
    }
    return true;
}

int RunProbe(Api *api, DWORD deviceType, DWORD deviceIndex)
{
    const DWORD result = api->openDevice(deviceType, deviceIndex, 0U);
    if(result != StatusOk)
    {
        std::printf("RESULT=FAIL;CODE=OPEN;VALUE=%lu\n", result);
        return 13;
    }

    api->closeDevice(deviceType, deviceIndex);
    std::printf("RESULT=READY\n");
    return 0;
}

int RunTest(Api *api,
            DWORD deviceType,
            DWORD deviceIndex,
            DWORD channelIndex,
            DWORD timeoutMs)
{
    VciInitConfig config = {};
    VciCanObject request = {};
    VciCanObject received[16] = {};
    DWORD result;
    bool opened = false;
    bool started = false;
    const ULONGLONG deadline = GetTickCount64() + timeoutMs;

    result = api->openDevice(deviceType, deviceIndex, 0U);
    if(result != StatusOk)
    {
        std::printf("RESULT=FAIL;CODE=OPEN;VALUE=%lu\n", result);
        return 13;
    }
    opened = true;

    config.acceptanceCode = 0U;
    config.acceptanceMask = 0xFFFFFFFFU;
    config.reserved = 0U;
    config.filter = 1U;
    config.timing0 = 0x00U;
    config.timing1 = 0x1CU;
    config.mode = 0U;
    result = api->initCan(deviceType, deviceIndex, channelIndex, &config);
    if(result != StatusOk)
    {
        std::printf("RESULT=FAIL;CODE=INIT;VALUE=%lu\n", result);
        api->closeDevice(deviceType, deviceIndex);
        return 14;
    }

    result = api->clearBuffer(deviceType, deviceIndex, channelIndex);
    if(result != StatusOk)
    {
        std::printf("RESULT=FAIL;CODE=CLEAR;VALUE=%lu\n", result);
        api->closeDevice(deviceType, deviceIndex);
        return 15;
    }

    result = api->startCan(deviceType, deviceIndex, channelIndex);
    if(result != StatusOk)
    {
        std::printf("RESULT=FAIL;CODE=START;VALUE=%lu\n", result);
        api->closeDevice(deviceType, deviceIndex);
        return 16;
    }
    started = true;
    Sleep(100U);

    request.id = RequestId;
    request.sendType = 0U;
    request.remoteFlag = 0U;
    request.externFlag = 0U;
    request.dataLength = sizeof(RequestData);
    std::memcpy(request.data, RequestData, sizeof(RequestData));
    DWORD transmitAttempt;
    for(transmitAttempt = 1U; transmitAttempt <= 3U; ++transmitAttempt)
    {
        result = api->transmit(deviceType,
                               deviceIndex,
                               channelIndex,
                               &request,
                               1U);
        if(result == StatusOk)
        {
            break;
        }
        Sleep(100U);
    }
    if(result != StatusOk)
    {
        std::printf("RESULT=FAIL;CODE=TRANSMIT;VALUE=%lu;ATTEMPTS=%lu;"
                    "MESSAGE=check CANH/CANL termination and bus ACK\n",
                    result,
                    transmitAttempt - 1U);
        api->resetCan(deviceType, deviceIndex, channelIndex);
        api->closeDevice(deviceType, deviceIndex);
        return 17;
    }

    while(GetTickCount64() < deadline)
    {
        const DWORD count = api->receive(deviceType,
                                         deviceIndex,
                                         channelIndex,
                                         received,
                                         16U,
                                         0);
        if(count == 0xFFFFFFFFU)
        {
            std::printf("RESULT=FAIL;CODE=RECEIVE\n");
            api->resetCan(deviceType, deviceIndex, channelIndex);
            api->closeDevice(deviceType, deviceIndex);
            return 18;
        }

        for(DWORD index = 0U; index < count && index < 16U; ++index)
        {
            if(IsExpectedResponse(received[index]))
            {
                if(started)
                {
                    api->resetCan(deviceType, deviceIndex, channelIndex);
                }
                if(opened)
                {
                    api->closeDevice(deviceType, deviceIndex);
                }
                std::printf("RESULT=PASS\n");
                return 0;
            }
        }
        Sleep(20U);
    }

    if(started)
    {
        api->resetCan(deviceType, deviceIndex, channelIndex);
    }
    if(opened)
    {
        api->closeDevice(deviceType, deviceIndex);
    }
    std::printf("RESULT=FAIL;CODE=TIMEOUT\n");
    return 19;
}

} // namespace

int wmain(int argc, wchar_t *argv[])
{
    const wchar_t *mode = FindArgument(argc, argv, L"--mode");
    const wchar_t *libraryPath = FindArgument(argc, argv, L"--dll");
    const wchar_t *deviceTypeText = FindArgument(argc, argv, L"--type");
    const wchar_t *deviceIndexText = FindArgument(argc, argv, L"--index");
    const wchar_t *channelText = FindArgument(argc, argv, L"--channel");
    const wchar_t *timeoutText = FindArgument(argc, argv, L"--timeout-ms");
    Api api;
    int result;

    if((mode == nullptr) || (libraryPath == nullptr) ||
       (deviceTypeText == nullptr) || (deviceIndexText == nullptr) ||
       (channelText == nullptr))
    {
        std::printf("RESULT=FAIL;CODE=ARGUMENT\n");
        return 10;
    }

    if(!LoadApi(libraryPath, &api))
    {
        return 11;
    }

    if(std::wcscmp(mode, L"probe") == 0)
    {
        result = RunProbe(&api,
                          ParseUnsigned(deviceTypeText),
                          ParseUnsigned(deviceIndexText));
    }
    else if(std::wcscmp(mode, L"test") == 0)
    {
        const DWORD timeoutMs = (timeoutText != nullptr) ?
            ParseUnsigned(timeoutText) : 2000U;
        result = RunTest(&api,
                         ParseUnsigned(deviceTypeText),
                         ParseUnsigned(deviceIndexText),
                         ParseUnsigned(channelText),
                         timeoutMs);
    }
    else
    {
        std::printf("RESULT=FAIL;CODE=MODE\n");
        result = 10;
    }

    UnloadApi(&api);
    return result;
}
