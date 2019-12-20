﻿#include "std.h"
#include "fileHelpers.h"
#include "winrt/Windows.Media.h"
#include "winrt/Windows.Storage.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

using namespace winrt;
using namespace winrt::Windows::AI::MachineLearning;
using namespace winrt::Windows::Graphics::Imaging;
using namespace winrt::Windows::Storage;

namespace FileHelpers
{
    std::wstring GetModulePath()
    {
        std::wstring val;
        wchar_t modulePath[MAX_PATH] = { 0 };
        GetModuleFileNameW((HINSTANCE)&__ImageBase, modulePath, _countof(modulePath));
        wchar_t drive[_MAX_DRIVE];
        wchar_t dir[_MAX_DIR];
        wchar_t filename[_MAX_FNAME];
        wchar_t ext[_MAX_EXT];
        _wsplitpath_s(modulePath, drive, _MAX_DRIVE, dir, _MAX_DIR, filename, _MAX_FNAME, ext, _MAX_EXT);

        val = drive;
        val += dir;

        return val;
    }

    std::wstring GetWinMLPath()
    {
        // bool inboxDll = false;
        // TODO Add command line parsing
        // if (SUCCEEDED(WEX::TestExecution::RuntimeParameters::TryGetValue(L"inbox", inboxDll)) && inboxDll)
        // {
        //     return L"";
        // }
        return GetModulePath();
    }


    winrt::Windows::Graphics::Imaging::SoftwareBitmap GetSoftwareBitmapFromFile(const std::wstring& filePath)
    {
        auto storageFile = StorageFile::GetFileFromPathAsync(filePath).get();
        auto stream = storageFile.OpenAsync(FileAccessMode::Read).get();
        auto decoder = BitmapDecoder::CreateAsync(stream).get();
        IBitmapFrameWithSoftwareBitmap bitmapFrameWithSoftwareBitmap;
        decoder.as(bitmapFrameWithSoftwareBitmap);
        auto softwareBitmap = bitmapFrameWithSoftwareBitmap.GetSoftwareBitmapAsync(
            BitmapPixelFormat::Bgra8,
            BitmapAlphaMode::Ignore,
            BitmapTransform::BitmapTransform(),
            ExifOrientationMode::IgnoreExifOrientation,
            ColorManagementMode::DoNotColorManage
        ).get();
        return softwareBitmap;
    }

    ImageFeatureValue LoadImageFeatureValue(const std::wstring& imagePath)
    {
        auto softwareBitmap = FileHelpers::GetSoftwareBitmapFromFile(FileHelpers::GetModulePath() + imagePath);
        auto videoFrame = winrt::Windows::Media::VideoFrame::CreateWithSoftwareBitmap(softwareBitmap);
        return ImageFeatureValue::CreateFromVideoFrame(videoFrame);
    }
}