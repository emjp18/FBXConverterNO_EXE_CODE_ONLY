#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <fbxsdk.h>

#include <fstream>
#include <assert.h>
#include <vector>
#include <string.h>
#include <map>
#include "XFileFormat.h"



namespace Options
{
    enum class FileOptions
    {
        NONE = 0,
        MATERIAL = 1 << 0,
        SKELETON = 1 << 1,
        MESH = 1 << 2,
        MORPH = 1<<3,
        LIGHT = 1<<4

    };
   
    enum class NormalOptions
    {
        REGULAR,
        AVERAGE
    };
    enum class CoordinateOptions
    {
        LEFTHANDED,
        RIGHTHANDED
    };
    enum class SpaceOptions
    {
        GLOBAL,
        LOCAL
    };
}

inline Options::FileOptions operator| (Options::FileOptions a, Options::FileOptions b) { return (Options::FileOptions)((int)a | (int)b); }
inline Options::FileOptions operator& (Options::FileOptions a, Options::FileOptions b) { return (Options::FileOptions)((int)a & (int)b); }
inline Options::FileOptions& operator|= (Options::FileOptions& a, Options::FileOptions b) { return (Options::FileOptions&)((int&)a |= (int)b); }
struct SelectedOptions
{
    Options::FileOptions fileOptions;
    Options::NormalOptions normalOptions;
    Options::CoordinateOptions coordinateOptions;
    Options::SpaceOptions spaceOptions;
       
};

