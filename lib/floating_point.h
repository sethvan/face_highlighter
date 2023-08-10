#ifndef FLOATING_POINT_H_
#define FLOATING_POINT_H_

#include <cmath>
#include <concepts>

// These templates were provided by reddit user u/IyeOnline, epsilon value was aset by me only

namespace floating_point
{
    template <std::floating_point T> constexpr T epsilon = 1.0e-18;

    template <std::floating_point T> [[nodiscard]] constexpr inline bool equal( T a, T b ) noexcept
    {
        if ( a == b ) // catch exact equality
        {
            return true;
        }
        else
        {
            const T diff = std::abs( a - b );

            if ( diff < epsilon<T> ) // catches one of the values being 0
            {
                return true;
            }

            a = std::abs( a );
            b = std::abs( b );
            const auto max = a > b ? a : b;
            return diff < ( max * epsilon<T> ); // equality by relative epsilon
        }
    }

    template <std::floating_point T> [[nodiscard]] constexpr inline bool not_equal( T a, T b ) noexcept
    {
        return not equal( a, b );
    }

    template <std::floating_point T> [[nodiscard]] constexpr inline bool is_zero( T value ) noexcept
    {
        return equal( value, T {} );
    }

    template <std::floating_point T> [[nodiscard]] constexpr inline bool non_zero( T value ) noexcept
    {
        return not is_zero( value );
    }

    template <std::floating_point T, bool or_equal = false> [[nodiscard]] constexpr inline bool greater( T a, T b ) noexcept
    {
        if constexpr ( or_equal )
        {
            return a > b || equal( a, b );
        }
        else
        {
            return a > b && not_equal( a, b );
        }
    }

    template <std::floating_point T> [[nodiscard]] constexpr inline bool greater_or_equal( T value ) noexcept
    {
        return greater<T, true>( value );
    }

    template <std::floating_point T, bool or_equal = false> [[nodiscard]] static constexpr inline bool less( T a, T b ) noexcept
    {
        if constexpr ( or_equal )
        {
            return a < b || equal( a, b );
        }
        else
        {
            return a < b && not_equal( a, b );
        }
    }

    template <std::floating_point T> [[nodiscard]] constexpr inline bool less_or_equal( T value ) noexcept
    {
        return less<T, true>( value );
    }
}

#endif // FLOATING_POINT_H_