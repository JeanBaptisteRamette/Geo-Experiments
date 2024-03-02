#ifndef GEO_EXPERIMENTS_GAME_DEFINITIONS_HPP
#define GEO_EXPERIMENTS_GAME_DEFINITIONS_HPP


#include <geo/utils/mm.hpp>
#include <geo/utils/traits.hpp>
#include <cocos2d.h>



namespace geo::gd
{
	using namespace cocos2d;
	using namespace mm::literals;


#define MAKE_FIELD(name, type, offset)    \
[[nodiscard]] type name() const           \
{                                         \
	return mm::field<type>(this, offset); \
}


	enum ObjectType : int
	{
		Spike = 8
	};

	struct GameObject
	{
		MAKE_FIELD(getType, ObjectType, 0x384)
	};

	struct PlayerObject
	{
		MAKE_FIELD(isShip, bool, 0x7A9)
	};

	struct MenuLayer
	{
		MAKE_FIELD(player, PlayerObject*, 0x154)
	};

	struct PlayLayer
	{
		MAKE_FIELD(player1, PlayerObject*, 0x878)
		MAKE_FIELD(player2, PlayerObject*, 0x87C)
	};

	struct GameManager
	{
		MAKE_FIELD(menuLayer, MenuLayer*, 0x1A8)
		MAKE_FIELD(playLayer, PlayLayer*, 0x198)

		static GameManager* sharedState()
		{
			using Signature = GameManager*(*)();
			return mm::subroutine<Signature>(0x121540)();
		}

		void setGVar(const char* gvar_name, bool value)
		{
			using Signature = void(__thiscall*)(GameManager*, const char*, bool);
			mm::subroutine<Signature>(0x1284E0)(this, gvar_name, value);
		}

		[[nodiscard]]
		bool getGVar(const char* gvar_name)
		{
			using Signature = bool(__thiscall*)(GameManager*, const char*);
			return mm::subroutine<Signature>(0x128730)(this, gvar_name);
		}
	};

	inline void GVar_SetBoolean(std::string_view gvar_name, bool value)
	{
		GameManager::sharedState()->setGVar(gvar_name.data(), value);
	}

	inline bool GVar_GetBoolean(std::string_view gvar_name)
	{
		return GameManager::sharedState()->getGVar(gvar_name.data());
	}


	// credits: https://github.com/poweredbypie/gd.h/blob/160af2a902bb8fc4a94a3e72c9cb7fec5e4d46aa/menu_nodes/CCMenuItemSpriteExtra.h
#pragma runtime_checks("s", off)
	struct CCMenuItemSpriteExtra : CCMenuItemSprite
	{
		[[nodiscard]]
		static CCMenuItemSpriteExtra* create(CCNode* sprite, CCObject* target, SEL_MenuHandler callback)
		{
			using Signature = CCMenuItemSpriteExtra*(__thiscall*)(CCNode*, CCObject*, SEL_MenuHandler);
			auto ret = mm::subroutine<Signature>(0x25830)(sprite, target, callback);

			__asm add esp, 0x8

			return ret;
		}
	};
#pragma runtime_checks("s", restore)
}

#endif //GEO_EXPERIMENTS_GAME_DEFINITIONS_HPP
