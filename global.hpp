#ifndef _GLOBAL_HPP_
#define _GLOBAL_HPP_

#include <type_traits>

/********************----- Enumeration Helpers -----********************/
//From http://www.reddit.com/r/cpp/comments/14oqo9/a_nice_helper_function_for_c11s_enum_classes/
template <typename T>
constexpr typename std::underlying_type<T>::type enum_value(T val)
{
    return static_cast<typename std::underlying_type<T>::type>(val);
}

#define ENUM_GENERATE_HELPERS(x)  \
  inline x operator|=(x &i_a, x const i_b){return (i_a = static_cast<x>(enum_value(i_a)|enum_value(i_b)));} \
  inline x operator&=(x &i_a, x const i_b){return (i_a = static_cast<x>(enum_value(i_a)|enum_value(i_b)));} \
  inline x operator~(x const i_a){return static_cast<x>(~enum_value(i_a));}                              \
  inline x operator|(x const i_a, x const i_b){return static_cast<x>(enum_value(i_a)|enum_value(i_b));}  \
  inline x operator&(x const i_a, x const i_b){return static_cast<x>(enum_value(i_a)&enum_value(i_b));}  \
  inline bool operator!(x const i_a){return !(enum_value(i_a) ? true : false);}

/**************************************************/

/********************----- ENUM: CompareResult -----********************/
enum class CompareResult
{
  LESS=-1,
  EQUAL=0,
  GREATER=1,
};
/**************************************************/

#endif  /* _GLOBAL_HPP_ */
