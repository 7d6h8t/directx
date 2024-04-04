#include "pch.h"
#include "include/conversion/media_type_converter.h"

#include <mfapi.h>
#include <exception>
#include <format>

#include "include/com_throw.h"

using namespace conversion;

std::wstring MediaTypeConverter::ToString(
    const Microsoft::WRL::ComPtr<IMFMediaType>& type) {
  MFRatio ratio{};
  com::ThrowIfFailed(MFGetAttributeRatio(type.Get(), MF_MT_FRAME_RATE,
                                         (uint32_t*)&ratio.Numerator,
                                         (uint32_t*)&ratio.Denominator));

  uint32_t width = 0, height = 0;
  com::ThrowIfFailed(MFGetAttributeSize(type.Get(), MF_MT_FRAME_SIZE, &width, &height));

  GUID sub_type = {};
  com::ThrowIfFailed(type->GetGUID(MF_MT_SUBTYPE, &sub_type));

  return std::format(L"{} {}x{} {}", SubTypeGuidToString(sub_type), width,
                     height, ratio.Numerator / ratio.Denominator);
}

std::wstring MediaTypeConverter::SubTypeGuidToString(const GUID& guid) {
  if (guid == MFVideoFormat_AI44)
    return L"AI44";
  else if (guid == MFVideoFormat_ARGB32)
    return L"ARGB32";
  else if (guid == MFVideoFormat_AYUV)
    return L"AYUV";
  else if (guid == MFVideoFormat_DV25)
    return L"dv25";
  else if (guid == MFVideoFormat_DV50)
    return L"dv50";
  else if (guid == MFVideoFormat_DVH1)
    return L"dvh1";
  else if (guid == MFVideoFormat_DVSD)
    return L"dvsd";
  else if (guid == MFVideoFormat_DVSL)
    return L"dvsl";
  else if (guid == MFVideoFormat_H264)
    return L"H264";
  else if (guid == MFVideoFormat_I420)
    return L"I420";
  else if (guid == MFVideoFormat_IYUV)
    return L"IYUV";
  else if (guid == MFVideoFormat_M4S2)
    return L"M4S2";
  else if (guid == MFVideoFormat_MJPG)
    return L"MJPG";
  else if (guid == MFVideoFormat_MP43)
    return L"MP43";
  else if (guid == MFVideoFormat_MP4S)
    return L"MP4S";
  else if (guid == MFVideoFormat_MP4V)
    return L"MP4V";
  else if (guid == MFVideoFormat_MPG1)
    return L"MPG1";
  else if (guid == MFVideoFormat_MSS1)
    return L"MSS1";
  else if (guid == MFVideoFormat_MSS2)
    return L"MSS2";
  else if (guid == MFVideoFormat_NV11)
    return L"NV11";
  else if (guid == MFVideoFormat_NV12)
    return L"NV12";
  else if (guid == MFVideoFormat_P010)
    return L"P010";
  else if (guid == MFVideoFormat_P016)
    return L"P016";
  else if (guid == MFVideoFormat_P210)
    return L"P210";
  else if (guid == MFVideoFormat_P216)
    return L"P216";
  else if (guid == MFVideoFormat_RGB24)
    return L"RGB24";
  else if (guid == MFVideoFormat_RGB32)
    return L"RGB32";
  else if (guid == MFVideoFormat_RGB555)
    return L"RGB555";
  else if (guid == MFVideoFormat_RGB565)
    return L"RGB565";
  else if (guid == MFVideoFormat_RGB8)
    return L"RGB8";
  else if (guid == MFVideoFormat_UYVY)
    return L"UYVY";
  else if (guid == MFVideoFormat_v210)
    return L"v210";
  else if (guid == MFVideoFormat_v410)
    return L"v410";
  else if (guid == MFVideoFormat_WMV1)
    return L"WMV1";
  else if (guid == MFVideoFormat_WMV2)
    return L"WMV2";
  else if (guid == MFVideoFormat_WMV3)
    return L"WMV3";
  else if (guid == MFVideoFormat_WVC1)
    return L"WVC1";
  else if (guid == MFVideoFormat_Y210)
    return L"Y210";
  else if (guid == MFVideoFormat_Y216)
    return L"Y216";
  else if (guid == MFVideoFormat_Y410)
    return L"Y410";
  else if (guid == MFVideoFormat_Y416)
    return L"Y416";
  else if (guid == MFVideoFormat_Y41P)
    return L"Y41P";
  else if (guid == MFVideoFormat_Y41T)
    return L"Y41T";
  else if (guid == MFVideoFormat_YUY2)
    return L"YUY2";
  else if (guid == MFVideoFormat_YV12)
    return L"YV12";
  else if (guid == MFVideoFormat_YVYU)
    return L"YVYU";
  else
    throw std::exception("[error] MFVideoFormat invalid");
}
