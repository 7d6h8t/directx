#pragma once
#include <guiddef.h>
#include <wrl/client.h>

#include <string>

#include "../export.h"

#pragma warning(push)
#pragma warning(disable : 4251)

struct IMFMediaType;

namespace conversion {
class CAPTURE_API MediaTypeConverter final {
  public:
   static std::wstring ToString(
       const Microsoft::WRL::ComPtr<IMFMediaType>& type);

   static std::wstring SubTypeGuidToString(const GUID& guid);
 };
}  // namespace conversion

#pragma warning(pop)