#ifndef INCLUDED_MULTIDRAWER_H_
#define INCLUDED_MULTIDRAWER_H_

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>

using namespace emscripten;

struct MultiDrawer
{
    MultiDrawer() = default;
    std::vector<int> startIndices;
    std::vector<int> counts;
    val getStartIndices() const;
    val getCounts() const;
    bool enabled = true;
};

#endif // INCLUDED_MULTIDRAWER_H_