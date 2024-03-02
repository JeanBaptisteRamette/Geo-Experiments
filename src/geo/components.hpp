#ifndef GEO_COMPONENT
#define GEO_COMPONENT

#include <vector>
#include <iostream>

#include <geo/components/base_component.hpp>
#include <geo/components/component_steam.hpp>
#include <geo/components/component_items.hpp>
#include <geo/components/component_cheats.hpp>
#include <geo/components/component_imgui.hpp>
#include <geo/components/component_rpc.hpp>
#include <geo/components/component_ui.hpp>


namespace geo::components
{
	namespace
	{
		std::vector<any_component> components;
	}

	void make_group()
	{
		components.push_back(std::make_unique<geo::component_steam>());
		components.push_back(std::make_unique<geo::component_items>());
		components.push_back(std::make_unique<geo::component_cheats>());
		components.push_back(std::make_unique<geo::component_imgui>());
		components.push_back(std::make_unique<geo::component_rpc>());
		components.push_back(std::make_unique<geo::component_ui>());
	}

	void pre_exec()
	{
		for (const any_component& component : components)
			component->pre_exec();
	}
}


#endif // GEO_COMPONENT