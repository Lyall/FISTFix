#include "stdafx.h"
#include "helper.hpp"
#include <inipp/inipp.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <safetyhook.hpp>

HMODULE baseModule = GetModuleHandle(NULL);
HMODULE thisModule;

// Logger and config setup
inipp::Ini<char> ini;
std::shared_ptr<spdlog::logger> logger;
string sFixName = "FISTFix";
string sFixVer = "1.0.0";
string sLogFile = "FISTFix.log";
string sConfigFile = "FISTFix.ini";
string sExeName;
filesystem::path sExePath;
filesystem::path sThisModulePath;
std::pair DesktopDimensions = { 0,0 };

// Ini Variables
bool bCustomResolution;
int iCustomResX;
int iCustomResY;
bool bAspectFix;
bool bFOVFix;
float fAdditionalFOV;
bool bDisableDLSSAutoQuality;
bool bDLSSQualityOverride;
int iDLSSQualitySetting;

// Aspect ratio + HUD stuff
float fPi = (float)3.141592653;
float fNativeAspect = (float)16 / 9;
float fAspectRatio;
float fAspectMultiplier;
float fHUDWidth;
float fHUDHeight;
float fDefaultHUDWidth = (float)1920;
float fDefaultHUDHeight = (float)1080;
float fHUDWidthOffset;
float fHUDHeightOffset;

// Variables
int iResX;
int iResY;

void Logging()
{
    // Get this module path
    WCHAR thisModulePath[_MAX_PATH] = { 0 };
    GetModuleFileNameW(thisModule, thisModulePath, MAX_PATH);
    sThisModulePath = thisModulePath;
    sThisModulePath = sThisModulePath.remove_filename();

    // Get game name and exe path
    WCHAR exePath[_MAX_PATH] = { 0 };
    GetModuleFileNameW(baseModule, exePath, MAX_PATH);
    sExePath = exePath;
    sExeName = sExePath.filename().string();
    sExePath = sExePath.remove_filename();

    // spdlog initialisation
    {
        try
        {
            logger = spdlog::basic_logger_st(sFixName.c_str(), sThisModulePath.string() + sLogFile, true);
            spdlog::set_default_logger(logger);

            spdlog::flush_on(spdlog::level::debug);
            spdlog::info("----------");
            spdlog::info("{} v{} loaded.", sFixName.c_str(), sFixVer.c_str());
            spdlog::info("----------");
            spdlog::info("Path to logfile: {}", sThisModulePath.string() + sLogFile);
            spdlog::info("----------");

            // Log module details
            spdlog::info("Module Name: {0:s}", sExeName.c_str());
            spdlog::info("Module Path: {0:s}", sExePath.string());
            spdlog::info("Module Address: 0x{0:x}", (uintptr_t)baseModule);
            spdlog::info("Module Timestamp: {0:d}", Memory::ModuleTimestamp(baseModule));
            spdlog::info("----------");
        }
        catch (const spdlog::spdlog_ex& ex)
        {
            AllocConsole();
            FILE* dummy;
            freopen_s(&dummy, "CONOUT$", "w", stdout);
            std::cout << "Log initialisation failed: " << ex.what() << std::endl;
        }
    }
}

void ReadConfig()
{
    // Initialise config
    std::ifstream iniFile(sThisModulePath.string() + sConfigFile);
    if (!iniFile)
    {
        AllocConsole();
        FILE* dummy;
        freopen_s(&dummy, "CONOUT$", "w", stdout);
        std::cout << "" << sFixName.c_str() << " v" << sFixVer.c_str() << " loaded." << std::endl;
        std::cout << "ERROR: Could not locate config file." << std::endl;
        std::cout << "ERROR: Make sure " << sConfigFile.c_str() << " is located in " << sThisModulePath.string().c_str() << std::endl;
    }
    else
    {
        spdlog::info("Path to config file: {}", sThisModulePath.string() + sConfigFile);
        ini.parse(iniFile);
    }

    // Read ini file
    inipp::get_value(ini.sections["Custom Resolution"], "Enabled", bCustomResolution);
    inipp::get_value(ini.sections["Custom Resolution"], "Width", iCustomResX);
    inipp::get_value(ini.sections["Custom Resolution"], "Height", iCustomResY);
    inipp::get_value(ini.sections["Fix Aspect Ratio"], "Enabled", bAspectFix);
    inipp::get_value(ini.sections["Fix FOV"], "Enabled", bFOVFix);
    inipp::get_value(ini.sections["Fix FOV"], "AdditionalFOV", fAdditionalFOV);
    inipp::get_value(ini.sections["DLSS Quality"], "DisableAutoQuality", bDisableDLSSAutoQuality);
    inipp::get_value(ini.sections["DLSS Quality"], "OverrideDLSSQuality", bDLSSQualityOverride);
    inipp::get_value(ini.sections["DLSS Quality"], "QualityLevel", iDLSSQualitySetting);

    // Log config parse
    spdlog::info("Config Parse: bCustomResolution: {}", bCustomResolution);
    spdlog::info("Config Parse: iCustomResX: {}", iCustomResX);
    spdlog::info("Config Parse: iCustomResY: {}", iCustomResY);
    spdlog::info("Config Parse: bAspectFix: {}", bAspectFix);
    spdlog::info("Config Parse: bFOVFix: {}", bFOVFix);
    spdlog::info("Config Parse: fAdditionalFOV: {}", fAdditionalFOV);
    if (fAdditionalFOV < 0.0f || fAdditionalFOV > 180.0f)
    {
        fAdditionalFOV = std::clamp(fAdditionalFOV, 0.0f, 180.0f);
        spdlog::info("Config Parse: fAdditionalFOV value invalid, clamped to {}", fAdditionalFOV);
    }
    spdlog::info("Config Parse: bDisableDLSSAutoQuality: {}", bDisableDLSSAutoQuality);
    spdlog::info("Config Parse: bDLSSQualityOverride: {}", bDLSSQualityOverride);
    spdlog::info("Config Parse: iDLSSQualitySetting: {}", iDLSSQualitySetting);
    if (iDLSSQualitySetting < -2 || iDLSSQualitySetting > 2)
    {
        iDLSSQualitySetting = std::clamp(iDLSSQualitySetting, -2, 2);
        spdlog::info("Config Parse: iDLSSQualitySetting value invalid, clamped to {}", iDLSSQualitySetting);
    }
    spdlog::info("----------");

    // Get desktop resolution
    DesktopDimensions = Util::GetPhysicalDesktopDimensions();

    // Set custom resolution to desktop resolution
    if (iCustomResX == 0 || iCustomResY == 0)
    {
        iCustomResX = (int)DesktopDimensions.first;
        iCustomResY = (int)DesktopDimensions.second;
        spdlog::info("Custom Resolution: Desktop Width: {}", iCustomResX);
        spdlog::info("Custom Resolution: Desktop Height: {}", iCustomResY);
    }
}

void IntroSkip()
{
    // Skip waiting for intro videos to finish
    uint8_t* SkipIntroWaitScanResult = Memory::PatternScan(baseModule, "44 38 7D 67 4C 8D 4D C7");
    if (SkipIntroWaitScanResult)
    {
        spdlog::info("Intro Skip: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)SkipIntroWaitScanResult - (uintptr_t)baseModule);

        static SafetyHookMid SkipIntroWaitMidHook{};
        SkipIntroWaitMidHook = safetyhook::create_mid(SkipIntroWaitScanResult,
            [](SafetyHookContext& ctx)
            {
                if (ctx.rbp + 0x67)
                {
                    *reinterpret_cast<int*>(ctx.rbp + 0x67) = (int)0;
                }
            });
    }
    else if (!SkipIntroWaitScanResult)
    {
        spdlog::error("Intro Skip: Pattern scan failed.");
    }
}

void Resolution()
{
    if (bCustomResolution)
    {
        // Apply custom resolution
        uint8_t* ApplyResolutionScanResult = Memory::PatternScan(baseModule, "4C ?? ?? E8 ?? ?? ?? ?? 83 ?? 01 0F 85 ?? ?? ?? ?? 41 ?? ?? ?? 85 ??");
        if (ApplyResolutionScanResult)
        {
            spdlog::info("Custom Resolution: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)ApplyResolutionScanResult - (uintptr_t)baseModule);

            static SafetyHookMid ApplyResolutionMidHook{};
            ApplyResolutionMidHook = safetyhook::create_mid(ApplyResolutionScanResult,
                [](SafetyHookContext& ctx)
                {
                    if (ctx.rcx + 0x28)
                    {
                        DWORD64 ScreenResolutionsAddr = *(uintptr_t*)(ctx.rcx + 0x28);
                        if (ScreenResolutionsAddr)
                        {
                            *reinterpret_cast<int*>(ScreenResolutionsAddr) = iCustomResX;
                            *reinterpret_cast<int*>(ScreenResolutionsAddr + 0x4) = iCustomResY;
                        }
                    }

                    if (ctx.rcx + 0x1B8)
                    {
                        DWORD64 SupportedResolutionsAddr = *(uintptr_t*)(ctx.rcx + 0x1B8);
                        if (SupportedResolutionsAddr)
                        {
                            *reinterpret_cast<int*>(SupportedResolutionsAddr) = iCustomResX;
                            *reinterpret_cast<int*>(SupportedResolutionsAddr + 0x4) = iCustomResY;
                        }
                    }
                });
        }
        else if (!ApplyResolutionScanResult)
        {
            spdlog::error("Custom Resolution: Pattern scan failed.");
        }
    }

    // Get current resolution
    uint8_t* CurrentResolutionScanResult = Memory::PatternScan(baseModule, "48 8D ?? ?? ?? ?? ?? 48 8D ?? ?? ?? ?? ?? 48 0F ?? ?? 48 8D ?? ?? ?? ?? ?? 48 8D ?? ?? ??");
    if (CurrentResolutionScanResult)
    {
        spdlog::info("Current Resolution: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)CurrentResolutionScanResult - (uintptr_t)baseModule);

        static SafetyHookMid CurrentResolutionMidHook{};
        CurrentResolutionMidHook = safetyhook::create_mid(CurrentResolutionScanResult,
            [](SafetyHookContext& ctx)
            {
                iResX = (int)ctx.r8;
                iResY = (int)ctx.r9;

                fAspectRatio = (float)iResX / iResY;
                fAspectMultiplier = fAspectRatio / fNativeAspect;

                // HUD variables
                fHUDWidth = (float)iResY * fNativeAspect;
                fHUDHeight = (float)iResY;
                fHUDWidthOffset = (float)(iResX - fHUDWidth) / 2;
                fHUDHeightOffset = 0;
                if (fAspectRatio < fNativeAspect)
                {
                    fHUDWidth = (float)iResX;
                    fHUDHeight = (float)iResX / fNativeAspect;
                    fHUDWidthOffset = 0;
                    fHUDHeightOffset = (float)(iResY - fHUDHeight) / 2;
                }

                // Log aspect ratio stuff
                spdlog::info("----------");
                spdlog::info("Resolution: Resolution: {}x{}", iResX, iResY);
                spdlog::info("Resolution: fAspectRatio: {}", fAspectRatio);
                spdlog::info("Resolution: fAspectMultiplier: {}", fAspectMultiplier);
                spdlog::info("Resolution: fHUDWidth: {}", fHUDWidth);
                spdlog::info("Resolution: fHUDHeight: {}", fHUDHeight);
                spdlog::info("Resolution: fHUDWidthOffset: {}", fHUDWidthOffset);
                spdlog::info("Resolution: fHUDHeightOffset: {}", fHUDHeightOffset);
                spdlog::info("----------");
            });
    }
    else if (!CurrentResolutionScanResult)
    {
        spdlog::error("Current Resolution: Pattern scan failed.");
    }
}

void AspectFOV()
{
    if (bAspectFix)
    {
        // Aspect Ratio
        uint8_t* AspectRatioScanResult = Memory::PatternScan(baseModule, "89 ?? ?? 0F ?? ?? ?? ?? ?? 00 33 ?? ?? 83 ?? 01");
        if (AspectRatioScanResult)
        {
            spdlog::info("Aspect Ratio: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)AspectRatioScanResult - (uintptr_t)baseModule);

            static SafetyHookMid AspectRatioMidHook{};
            AspectRatioMidHook = safetyhook::create_mid(AspectRatioScanResult ,
                [](SafetyHookContext& ctx)
                {
                    ctx.rax = *(uint32_t*)&fAspectRatio;
                });
        }
        else if (!AspectRatioScanResult)
        {
            spdlog::error("Aspect Ratio: Pattern scan failed.");
        }
    }

    if (bFOVFix)
    {
        // FOV
        uint8_t* FOVScanResult = Memory::PatternScan(baseModule, "74 ?? F3 0F ?? ?? ?? ?? ?? 00 F3 0F ?? ?? ?? ?? ?? 00 EB ?? F3 0F ?? ?? ?? ?? ?? 00 F3 0F ?? ?? ?? 8B ?? ?? ?? ?? 00");
        uint8_t* GameplayFOVScanResult = Memory::PatternScan(baseModule, "F3 ?? ?? ?? ?? ?? 44 0F ?? ?? ?? ?? ?? 00 75 ?? 48 8D ?? ?? ?? ?? ??");
        if (FOVScanResult && GameplayFOVScanResult)
        {
            spdlog::info("FOV: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)FOVScanResult - (uintptr_t)baseModule);
            spdlog::info("Gameplay FOV: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)GameplayFOVScanResult - (uintptr_t)baseModule);

            static SafetyHookMid FOVMidHook{};
            FOVMidHook = safetyhook::create_mid(FOVScanResult + 0x1C,
                [](SafetyHookContext& ctx)
                {
                    if (fAspectRatio > fNativeAspect)
                    {
                        ctx.xmm0.f32[0] = atanf(tanf(ctx.xmm0.f32[0] * (fPi / 360)) / fNativeAspect * fAspectRatio) * (360 / fPi);
                    }
                });

            // ZingangCameraVolume
            static SafetyHookMid GameplayFOVMidHook{};
            GameplayFOVMidHook = safetyhook::create_mid(GameplayFOVScanResult,
                [](SafetyHookContext& ctx)
                {
                    if (fAspectRatio > fNativeAspect)
                    {
                        ctx.xmm8.f32[0] = atanf(tanf(ctx.xmm8.f32[0] * (fPi / 360)) / fNativeAspect * fAspectRatio) * (360 / fPi);
                    }
                    ctx.xmm8.f32[0] += fAdditionalFOV;
                });
        }
        else if (!FOVScanResult || !GameplayFOVScanResult)
        {
            spdlog::error("FOV: Pattern scan failed.");
        }
    }
}

void DLSS()
{
    if (bDisableDLSSAutoQuality)
    {
        // Disable DLSS auto quality
        // r.NGX.DLSS.Quality.Auto
        uint8_t* DLSSAutoQualityScanResult = Memory::PatternScan(baseModule, "0F 84 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 ?? 74 ?? 48 ?? ?? ?? 48 ?? ?? 0F 84 ?? ?? ?? ??");
        if (DLSSAutoQualityScanResult)
        {
            spdlog::info("DLSS: Auto: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)DLSSAutoQualityScanResult - (uintptr_t)baseModule);
            DWORD64 DLSSAutoQualityFuncAddr = Memory::GetAbsolute((uintptr_t)DLSSAutoQualityScanResult + 0x7);
            spdlog::info("DLSS: Auto: Func address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)DLSSAutoQualityFuncAddr - (uintptr_t)baseModule);

            static SafetyHookMid DLSSAutoQualityMidHook{};
            DLSSAutoQualityMidHook = safetyhook::create_mid(DLSSAutoQualityFuncAddr + 0x27,
                [](SafetyHookContext& ctx)
                {
                    if (ctx.rcx + ctx.rbx)
                    {
                        *reinterpret_cast<BYTE*>(ctx.rcx + ctx.rbx) = 0;
                    }
                });
        }
        else if (!DLSSAutoQualityScanResult)
        {
            spdlog::error("DLSS: Auto: Pattern scan failed.");
        }

        // DLSS quality override will only take effect if the auto quality setting is disabled
        if (bDLSSQualityOverride)
        {
            // Override DLSS quality setting
            // r.NGX.DLSS.Quality
            uint8_t* DLSSQualitySettingScanResult = Memory::PatternScan(baseModule, "48 ?? ?? ?? ?? 66 0F ?? ?? ?? ?? ?? ?? 4C ?? ?? ?? ?? ?? ?? F3 0F ?? ?? ?? ?? 8B ?? C7 ?? ?? ?? 02 00 00 00");
            if (DLSSQualitySettingScanResult)
            {
                spdlog::info("DLSS: Quality Setting: Address is {:s}+{:x}", sExeName.c_str(), (uintptr_t)DLSSQualitySettingScanResult - (uintptr_t)baseModule);

                static SafetyHookMid DLSSQualitySettingMidHook{};
                DLSSQualitySettingMidHook = safetyhook::create_mid(DLSSQualitySettingScanResult,
                    [](SafetyHookContext& ctx)
                    {
                        if (ctx.rax)
                        {
                            *reinterpret_cast<int*>(ctx.rax) = iDLSSQualitySetting;
                        }
                    });
            }
            else if (!DLSSQualitySettingScanResult)
            {
                spdlog::error("DLSS: Quality Setting: Pattern scan failed.");
            }
        }
    }
}

DWORD __stdcall Main(void*)
{
    Logging();
    ReadConfig();
    IntroSkip();
    Resolution();
    AspectFOV();
    DLSS();
    return true;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        thisModule = hModule;
        HANDLE mainHandle = CreateThread(NULL, 0, Main, 0, NULL, 0);
        if (mainHandle)
        {
            SetThreadPriority(mainHandle, THREAD_PRIORITY_HIGHEST); // set our Main thread priority higher than the games thread
            CloseHandle(mainHandle);
        }
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

