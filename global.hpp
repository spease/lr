#ifndef _GLOBAL_HPP_
#define _GLOBAL_HPP_

#include <type_traits>

//From http://www.reddit.com/r/cpp/comments/14oqo9/a_nice_helper_function_for_c11s_enum_classes/
template <typename T>
constexpr typename std::underlying_type<T>::type enum_value(T val)
{
    return static_cast<typename std::underlying_type<T>::type>(val);
}

/********************----- ENUM: CompareResult -----********************/
enum class CompareResult
{
  LESS=-1,
  EQUAL=0,
  GREATER=1,
};
/**************************************************/

#endif  /* _GLOBAL_HPP_ */
