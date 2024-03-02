#ifndef GEO_EXPERIMENTS_SETTINGSLAYER_HPP
#define GEO_EXPERIMENTS_SETTINGSLAYER_HPP


#include <geo/game_definitions.hpp>
#include <cocos2d.h>


namespace geo
{
	using namespace cocos2d;
	using namespace gd;


	class SettingsLayer : public CCLayer
	{
	protected:
		bool init() override;
		void keyBackClicked() override;
		void backButtonCallback(CCObject*);

	public:
		static SettingsLayer* create();

		void enterSettingsScene(CCObject*);

		static CCMenuItemSpriteExtra* makeButton(CCLayer* layer);
	};
}


#endif //GEO_EXPERIMENTS_SETTINGSLAYER_HPP
