#pragma once

#if defined(CAPTURECONTROL_EXPORTS)  // inside DLL
#define CAPTURE_API __declspec(dllexport)
#else  // outside DLL
#define CAPTURE_API __declspec(dllimport)
#endif  // CAPTURECONTROL_EXPORTS
