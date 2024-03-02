#ifndef GEO_EXPERIMENTS_TRAITS_HPP
#define GEO_EXPERIMENTS_TRAITS_HPP


#include <type_traits>


namespace geo
{
	template <typename T>
	concept FunctionPointer = requires(T f)
	{
		std::is_pointer_v<T> &&
		std::is_function_v<std::remove_pointer_t<T>>;
	};
}

#endif //GEO_EXPERIMENTS_TRAITS_HPP
