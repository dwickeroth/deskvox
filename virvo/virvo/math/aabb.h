#ifndef VV_MATH_AABB_H
#define VV_MATH_AABB_H

#include "vector.h"

#include <virvo/vvmacros.h>

#if VV_CXXLIB_HAS_HDR_ARRAY
#include <array>
#else
#include <boost/array.hpp>
#endif

namespace virvo
{


namespace math
{


template < typename T >
class base_aabb
{
public:

    typedef vector< 3, T > vec_type;
#if VV_CXXLIB_HAS_HDR_ARRAY
    typedef std::array< vec_type, 8 > vertex_list;
#else
    typedef boost::array< vec_type, 8 > vertex_list;
#endif

    vec_type min;
    vec_type max;

    inline base_aabb(vec_type const& min, vec_type const& max)
        : min(min)
        , max(max)
    {
    }

    inline vec_type getMin() const
    {
        return min;
    }

    inline vec_type getMax() const
    {
        return max;
    }

};

} // math


} // virvo

#include "detail/aabb.inl"

#endif


