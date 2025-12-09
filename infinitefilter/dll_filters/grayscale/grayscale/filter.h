#pragma once

#ifdef FILTER_EXPORTS
#define FILTER_API __declspec(dllexport)
#else
#define FILTER_API __declspec(dllimport)
#endif

extern "C" FILTER_API void filter(unsigned char* img, int width, int height, int channels);