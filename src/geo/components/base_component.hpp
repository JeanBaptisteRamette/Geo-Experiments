#ifndef GEO_EXPERIMENTS_BASE_COMPONENT_HPP
#define GEO_EXPERIMENTS_BASE_COMPONENT_HPP

#include <memory>


namespace geo
{
	struct base_component
	{
		virtual ~base_component() = default;
		virtual void pre_exec() = 0;
	};

	using any_component = std::unique_ptr<base_component>;
}


#endif //GEO_EXPERIMENTS_BASE_COMPONENT_HPP
