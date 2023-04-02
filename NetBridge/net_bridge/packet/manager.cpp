#include "manager.h"

void net_bridge::c_scalarlength::fix_index( )
{
    static const std::vector<std::pair<int64_t, int>> scalar_index_ranges = {
        {std::numeric_limits<std::int8_t>::min( ), 1},
        {std::numeric_limits<std::int8_t>::max( ), 1},
        {std::numeric_limits<int16_t>::min( ), 2},
        {std::numeric_limits<int16_t>::max( ), 2},
        {std::numeric_limits<int32_t>::min( ), 4},
        {std::numeric_limits<int32_t>::max( ), 4}
    };

    auto it = std::lower_bound( scalar_index_ranges.begin( ), scalar_index_ranges.end( ),
        std::make_pair( scalar_length, 8 ),
        [ ]( const std::pair<int64_t, int>& lhs, const std::pair<int64_t, int>& rhs ) {
            return lhs.first < rhs.first;
        } );

    scalar_index = it != scalar_index_ranges.end( ) ? it->second : 8;
}
