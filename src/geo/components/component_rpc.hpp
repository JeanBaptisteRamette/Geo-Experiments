#ifndef GEO_EXPERIMENTS_COMPONENT_RPC_HPP
#define GEO_EXPERIMENTS_COMPONENT_RPC_HPP

#include <geo/components/base_component.hpp>
#include <geo/utils/logging.hpp>


namespace geo
{
	std::string current_scene()
	{
		return "In Menu";
	}

	std::string current_level_name()
	{
		return "Stereo Madness";
	}

	int current_level_id()
	{
		return 0;
	}

	struct component_rpc final : geo::base_component
	{
		void pre_exec() override
		{

		}
	};
}



#endif //GEO_EXPERIMENTS_COMPONENT_RPC_HPP
