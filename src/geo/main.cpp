#include <Windows.h>
#include <geo/utils/nt.hpp>
#include <geo/utils/logging.hpp>
#include <geo/components.hpp>


#define REQUIRED_GAME_TIMESTAMP 0x65A0DC84


namespace fs = std::filesystem;


FARPROC map_process(const std::filesystem::path& game_path)
{
	auto game_module = geo::nt::module_view(game_path);

	if (!game_module)
		throw std::runtime_error(game_path.string() + " could not be loaded in memory");

	if (game_module.timestamp() != REQUIRED_GAME_TIMESTAMP)
		throw std::runtime_error("You seem to be running on an unsupported game version");

	// The LoadLibrary API does not process relocations or imports, we have to do it ourselves.
	// Then make sure to set our own PEB's image base to the loaded module's base
	geo::nt::process_relocations(game_module);
	geo::nt::process_imports(game_module);
	geo::nt::peb_set_image_base(game_module.pointer());

	return game_module.entry_point();
}

int process_wrapper(const fs::path& game_path)
{
	try
	{
		const auto proc_ep = map_process(game_path);

		if (!proc_ep)
			throw std::runtime_error("Invalid process entry point");

		geo::components::make_group();
		geo::components::pre_exec();

		return proc_ep();
	}
	catch (std::runtime_error& e)
	{
		MessageBoxA(nullptr, e.what(), "Launch error", MB_ICONERROR | MB_OK);
		return EXIT_FAILURE;
	}
}

// The save is stored at appdata/Local/geo-experiments instead of appdata/Local/GeometryDash
// Let the user choose if he wants to have a separated profile (geo-experiments) or load the original one (GeometryDash)

int main(int argc, char** argv)
{
	const auto _ = std::make_unique<geo::io::logger>("geo-x.log");


	const auto game_path = fs::path("GeometryDash.exe");

	if (!fs::exists(game_path))
	{
		MessageBoxA(
				nullptr,
				"GeometryDash.exe was not found in the current folder, make sure the mod is in the same folder as the game",
				"Game not found",
				MB_OK | MB_ICONERROR
		);

		return EXIT_FAILURE;
	}

	return process_wrapper(game_path);
}




