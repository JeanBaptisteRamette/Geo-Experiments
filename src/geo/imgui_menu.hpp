#ifndef GEO_EXPERIMENTS_IMGUI_MENU_HPP
#define GEO_EXPERIMENTS_IMGUI_MENU_HPP


#include <ImGui/imgui.h>
#include <geo/game_definitions.hpp>
#include <geo/gvars.hpp>


namespace geo
{
	using namespace gd;

	inline void ui_render_main()
	{

		ImGui::Begin("Geo-Xperiments settings",
					 nullptr,
					 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Modal);

		bool checkbox_state = GVar_GetBoolean(gvars::GV_UNLOCK_ALL);
		if (ImGui::Checkbox("Unlock all", &checkbox_state))
			GVar_SetBoolean(gvars::GV_UNLOCK_ALL, checkbox_state);

		checkbox_state = GVar_GetBoolean(gvars::GV_REMOVE_MENU_SPAWNS);
		if (ImGui::Checkbox("Remove menu spawns", &checkbox_state))
			GVar_SetBoolean(gvars::GV_REMOVE_MENU_SPAWNS, checkbox_state);

		checkbox_state = GVar_GetBoolean(gvars::GV_REMOVE_SPIKES);
		if (ImGui::Checkbox("Remove spikes", &checkbox_state))
			GVar_SetBoolean(gvars::GV_REMOVE_SPIKES, checkbox_state);

		checkbox_state = GVar_GetBoolean(gvars::GV_REMOVE_SHIP_FLIP);
		if (ImGui::Checkbox("Remove gravity flip as ship", &checkbox_state))
			GVar_SetBoolean(gvars::GV_REMOVE_SHIP_FLIP, checkbox_state);

		ImGui::End();
	}
}


#endif //GEO_EXPERIMENTS_IMGUI_MENU_HPP
