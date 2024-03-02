#include <geo/components/imgui_context.hpp>
#include <geo/layers/settings_layer.hpp>
#include <geo/game_definitions.hpp>


namespace geo
{
	using namespace gd;

	bool SettingsLayer::init()
	{
		//
		// Set background image
		//
		auto background = CCSprite::create("GJ_gradientBG.png");

		const auto winSize = CCDirector::sharedDirector()->getWinSize();
		const auto size = background->getContentSize();

		background->setScaleX(winSize.width / size.width);
		background->setScaleY(winSize.height / size.height);
		background->setAnchorPoint({0, 0});
		background->setColor({70, 70, 200});
		background->setZOrder(-1);

		addChild(background);

		//
		// Set title
		//
		auto title = CCLabelBMFont::create("Geo-Xperiments Settings", "bigFont.fnt");

		title->setAnchorPoint({0.5, 0.5});
		title->setPosition({winSize.width / 2, winSize.height - 30});
		title->setScale(0.7);

		addChild(title);

		//
		// Set return to previous scene button
		//
		auto button = CCMenuItemSpriteExtra::create(
				CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
				this,
				menu_selector(SettingsLayer::backButtonCallback)
		);

		button->setScale(0.7);

		auto menu = CCMenu::create();
		menu->addChild(button);
		menu->setPosition({25, winSize.height - 25});

		addChild(menu);

		setKeypadEnabled(true);

		return true;
	}

	void SettingsLayer::keyBackClicked()
	{
		imgui_context::hide();
		CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
	}

	void SettingsLayer::backButtonCallback(cocos2d::CCObject *)
	{
		keyBackClicked();
	}

	void SettingsLayer::enterSettingsScene(cocos2d::CCObject *)
	{
		auto layer = SettingsLayer::create();
		auto scene = CCScene::create();

		scene->addChild(layer);

		auto transition = CCTransitionFade::create(0.5f, scene);

		CCDirector::sharedDirector()->pushScene(transition);

		imgui_context::show();
	}

	SettingsLayer* SettingsLayer::create()
	{
		auto layer = new SettingsLayer();

		if (!layer->init())
		{
			delete layer;
			return nullptr;
		}

		layer->autorelease();
		return layer;
	}

	CCMenuItemSpriteExtra* SettingsLayer::makeButton(CCLayer* layer)
	{
		auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
		sprite->setScale(0.6);

		return CCMenuItemSpriteExtra::create(
				sprite,
				layer,
				menu_selector(SettingsLayer::enterSettingsScene)
		);
	}
}
