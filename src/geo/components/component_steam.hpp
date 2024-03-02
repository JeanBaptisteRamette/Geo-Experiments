#ifndef GEO_EXPERIMENTS_COMPONENT_STEAM_HPP
#define GEO_EXPERIMENTS_COMPONENT_STEAM_HPP

#include <geo/components/base_component.hpp>
#include <geo/utils/logging.hpp>
#include <geo/utils/hooks.hpp>
#include <Windows.h>


namespace geo
{
	constexpr auto APPLICATION_ID = "322170";

	//
	// Component's hooks
	//
	geo::iathk::ImportHook hk_steamapi_init;


	bool steamapi_init()
	{
		const bool ok = hk_steamapi_init.call<bool>();

		if (!ok)
		{
			typedef bool (*tSteamAPI_IsSteamRunning)();
			if (!nt::exported<tSteamAPI_IsSteamRunning>(L"steam_api.dll", "SteamAPI_IsSteamRunning")())
				MessageBoxA(nullptr, "Steam must be running to play the client", "Steam", MB_ICONERROR | MB_OK);
			else
				MessageBoxA(nullptr, "Error during steam initialization", "Steam", MB_ICONERROR | MB_OK);
		}

		return ok;
	}

	struct component_steam final : geo::base_component
	{
		void pre_exec() override
		{
			// Steam either looks for a steam_appid.txt file or a SteamAppId env var
			// Source: reverse-engineered SteamAPI_RestartAppIfNecessary
			SetEnvironmentVariableA("SteamAppId", APPLICATION_ID);

			hk_steamapi_init = geo::iathk::create_redirect("steam_api.dll", "SteamAPI_Init", steamapi_init);

			io::info("Steam component loaded");
		}
	};
}


#endif //GEO_EXPERIMENTS_COMPONENT_STEAM_HPP
