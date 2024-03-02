#ifndef GEO_EXPERIMENTS_COMPONENT_ITEMS_HPP
#define GEO_EXPERIMENTS_COMPONENT_ITEMS_HPP

#include <geo/components/base_component.hpp>
#include <geo/game_definitions.hpp>
#include <geo/utils/logging.hpp>
#include <geo/utils/hooks.hpp>
#include <geo/utils/mm.hpp>
#include <geo/gvars.hpp>



namespace geo
{
	using namespace gd;

	//
	// Component's hooks
	//
	safetyhook::InlineHook hk_gamestatsmanager_isitemunlocked;


	bool __fastcall gamestatsmanager_isitemunlocked(void* Manager, CCONV_PLACEHOLDER, int ItemType, int ItemId)
	{
		if (GVar_GetBoolean(gvars::GV_UNLOCK_ALL))
			return true;

		return hk_gamestatsmanager_isitemunlocked.thiscall<bool>(Manager, ItemType, ItemId);
	}

	struct component_items final : geo::base_component
	{
		void pre_exec() override
		{
			using namespace mm::literals;

			hk_gamestatsmanager_isitemunlocked = safetyhook::create_inline(0x176570_rva, gamestatsmanager_isitemunlocked);

			mm::patch(0x26C113_rva, "EB");                 // unlock "Clubstep", "Theory Of Everything 2 and "Deadlocked"
			mm::patch(0x26A3B5_rva, "E9 F1 01 00 00 90");  // no UI lock for the levels

			io::info("Unlock component loaded");
		}
	};
}


#endif //GEO_EXPERIMENTS_COMPONENT_ITEMS_HPP
