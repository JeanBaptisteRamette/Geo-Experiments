#ifndef GEO_EXPERIMENTS_COMPONENT_UI_HPP
#define GEO_EXPERIMENTS_COMPONENT_UI_HPP


#include <geo/components/base_component.hpp>
#include <geo/layers/settings_layer.hpp>
#include <geo/game_definitions.hpp>
#include <geo/utils/hooks.hpp>
#include <geo/utils/mm.hpp>
#include <geo/gvars.hpp>
#include <cocos2d.h>


namespace geo
{
	using namespace cocos2d;

	//
	// Component's hooks
	//
	safetyhook::InlineHook hk_menulayer_init;
	safetyhook::InlineHook hk_menulayer_resetplayer;


	bool __fastcall menulayer_init(CCLayer* menu_layer, CCONV_PLACEHOLDER)
	{
		if (!hk_menulayer_init.thiscall<bool>(menu_layer))
			return false;

		const auto winSize = CCDirector::sharedDirector()->getWinSize();
		auto menu = CCMenu::create();
		menu->addChild(SettingsLayer::makeButton(menu_layer));
		menu->setPosition({winSize.width - 20, winSize.height - 20});

		menu_layer->addChild(menu);

		return true;
	}

	void __fastcall menulayer_resetplayer(CCLayer* menu_layer, CCONV_PLACEHOLDER)
	{
		if (gd::GVar_GetBoolean(gvars::GV_REMOVE_MENU_SPAWNS))
			return;

		hk_menulayer_resetplayer.thiscall(menu_layer);
	}


	struct component_ui final : geo::base_component
	{
		void pre_exec() override
		{
			using namespace mm::literals;

			// TODO: Add a settings button in the menu

			hk_menulayer_init = safetyhook::create_inline(0x27B450_rva, menulayer_init);
			hk_menulayer_resetplayer = safetyhook::create_inline(0x279FD0_rva, menulayer_resetplayer);
		}
	};
}


#endif //GEO_EXPERIMENTS_COMPONENT_UI_HPP
