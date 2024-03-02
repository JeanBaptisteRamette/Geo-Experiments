#ifndef GEO_EXPERIMENTS_COMPONENT_CHEATS_HPP
#define GEO_EXPERIMENTS_COMPONENT_CHEATS_HPP

#include <geo/components/base_component.hpp>
#include <geo/game_definitions.hpp>
#include <geo/utils/logging.hpp>
#include <geo/utils/hooks.hpp>
#include <geo/utils/mm.hpp>


namespace geo
{
	using namespace mm::literals;

	//
	// Component's hooks
	//
	safetyhook::MidHook hk_portal_flip_gravity_collided;
	safetyhook::InlineHook hk_playlayer_addobject;


	void portal_flip_gravity_collided(SafetyHookContext & CpuContext)
	{
		if (!gd::GVar_GetBoolean(gvars::GV_REMOVE_SHIP_FLIP))
			return;

		// We will also need to do that when the player is already flipped
		// and is entering ship
		auto player = reinterpret_cast<gd::PlayerObject*>(CpuContext.ebx);

		if (!player->isShip())
			CpuContext.eip += 6;
		else
			CpuContext.eip = 0x19CE3C_ptr;
	}

	void __fastcall playlayer_addobject(void* playLayer, CCONV_PLACEHOLDER, gd::GameObject* gameObject)
	{
		// RobTop implemented a simple yet clever anticheat measure here, before launching the level, it registers
		// a single spike at the player start position, if the collision is not triggered, it means the player has
		// some kind of noclip hack enabled. So we have to make sure this one spike is registered. I stumbled upon it
		// when trying to remove spikes from being registered.
		if (_ReturnAddress() == 0x2DD8D4_rva)
			return hk_playlayer_addobject.thiscall(playLayer, gameObject);

		if (gameObject->getType() == Spike && GVar_GetBoolean(gvars::GV_REMOVE_SPIKES))
			return;

		return hk_playlayer_addobject.thiscall(playLayer, gameObject);
	}

	struct component_cheats final : geo::base_component
	{
		void pre_exec() override
		{
			hk_portal_flip_gravity_collided = safetyhook::create_mid(0x19C50F_bp, portal_flip_gravity_collided);
			hk_playlayer_addobject = safetyhook::create_inline(0x2E19B0_rva, playlayer_addobject);

			io::info("Cheat component loaded");
		}
	};
}


#endif //GEO_EXPERIMENTS_COMPONENT_CHEATS_HPP
