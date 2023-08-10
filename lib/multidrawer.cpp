#include "multidrawer.h"

val MultiDrawer::getStartIndices() const
{
    return val( typed_memory_view( startIndices.size(), startIndices.data() ) );
}
val MultiDrawer::getCounts() const
{
    return val( typed_memory_view( counts.size(), counts.data() ) );
}

EMSCRIPTEN_BINDINGS( MultiDrawer )
{
    class_<MultiDrawer>( "MultiDrawer" )
        .property( "startIndices", &MultiDrawer::getStartIndices )
        .property( "counts", &MultiDrawer::getCounts );
}