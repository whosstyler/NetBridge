#include "manager.h"

void net_bridge::c_scalarlength::fix_index( )
{
    uint64_t obj = scalar_length;
    if ( ( std::numeric_limits<std::int8_t>::min )( ) <= obj && obj <= ( std::numeric_limits<std::int8_t>::max )( ) )
        scalar_index = 1;
    else if ( ( std::numeric_limits<int16_t>::min )( ) <= obj && obj <= ( std::numeric_limits<int16_t>::max )( ) )
        scalar_index = 2;
    else if ( ( std::numeric_limits<int32_t>::min )( ) <= obj && obj <= ( std::numeric_limits<int32_t>::max )( ) )
        scalar_index = 4;
    else
        scalar_index = 8;
}