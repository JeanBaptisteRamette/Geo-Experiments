#include <cassert>
#include <string_view>
#include <geo/utils/logging.hpp>
#include <geo/utils/mm.hpp>
#include <geo/utils/nt.hpp>


namespace geo::nt
{
	module_view::module_view(const std::filesystem::path& path)
	: internal_handle(LoadLibraryW(path.c_str())) {}

	module_view::module_view(HMODULE handle) : internal_handle(handle) {}

	module_view::operator bool() const
	{
		return internal_handle;
	}

	module_view::operator HMODULE() const
	{
		return internal_handle;
	}

	HMODULE module_view::native_pointer() const
	{
		return internal_handle;
	}

	void* module_view::pointer() const
	{
		assert(internal_handle);

		return static_cast<void*>(internal_handle);
	}

	uintptr_t module_view::address() const
	{
		assert(internal_handle);

		return (uintptr_t)internal_handle;
	}

	uintptr_t module_view::preferred_image_base() const
	{
		assert(internal_handle);

		const auto hdr_dos = reinterpret_cast<PIMAGE_DOS_HEADER>(internal_handle);
		const auto hdr_new = reinterpret_cast<PIMAGE_NT_HEADERS32>(address() + hdr_dos->e_lfanew);

		return hdr_new->OptionalHeader.ImageBase;
	}

	uint32_t module_view::checksum() const
	{
		assert(internal_handle);

		const auto hdr_dos = reinterpret_cast<PIMAGE_DOS_HEADER>(internal_handle);
		const auto hdr_new = reinterpret_cast<PIMAGE_NT_HEADERS32>(address() + hdr_dos->e_lfanew);

		return hdr_new->OptionalHeader.CheckSum;
	}

	uint32_t module_view::timestamp() const
	{
		assert(internal_handle);

		const auto hdr_dos = reinterpret_cast<PIMAGE_DOS_HEADER>(internal_handle);
		const auto hdr_new = reinterpret_cast<PIMAGE_NT_HEADERS32>(address() + hdr_dos->e_lfanew);

		return hdr_new->FileHeader.TimeDateStamp;
	}

	void** module_view::imported_entry(std::string_view module_name, std::string_view procedure_name) const
	{
		assert(internal_handle);

		const module_view module(module_name);

		if (!module)
			return nullptr;

		const auto imports = data_directory(IMAGE_DIRECTORY_ENTRY_IMPORT);
		auto descriptor = from_base<PIMAGE_IMPORT_DESCRIPTOR>(imports->VirtualAddress);

		while (descriptor->Name)
		{
			const auto curr_name = std::string_view(from_base<char*>(descriptor->Name));

			if (!_stricmp(curr_name.data(), module_name.data()))
			{
				auto thunk_orig  = from_base<PIMAGE_THUNK_DATA>(descriptor->OriginalFirstThunk);
				auto thunk_first = from_base<PIMAGE_THUNK_DATA>(descriptor->FirstThunk);

				while (thunk_orig->u1.AddressOfData)
				{
					const auto by_name   = from_base<PIMAGE_IMPORT_BY_NAME>((thunk_orig->u1.AddressOfData));
					const auto curr_proc = std::string_view(by_name->Name);

					if (curr_proc == procedure_name)
						return reinterpret_cast<void**>(&thunk_first->u1.Function);

					++thunk_orig;
					++thunk_first;
				}
			}

			++descriptor;
		}

		return nullptr;
	}

	FARPROC module_view::entry_point() const
	{
		assert(internal_handle);

		const auto hdr_dos = reinterpret_cast<PIMAGE_DOS_HEADER>(internal_handle);
		const auto hdr_new = reinterpret_cast<PIMAGE_NT_HEADERS32>(address() + hdr_dos->e_lfanew);

		return from_base<FARPROC>(hdr_new->OptionalHeader.AddressOfEntryPoint);
	}

	PIMAGE_DATA_DIRECTORY module_view::data_directory(int directory_entry) const
	{
		assert(internal_handle);
		assert(directory_entry >= IMAGE_DIRECTORY_ENTRY_EXPORT &&
		       directory_entry <= IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR);

		const auto hdr_dos = reinterpret_cast<PIMAGE_DOS_HEADER>(internal_handle);
		const auto hdr_new = reinterpret_cast<PIMAGE_NT_HEADERS32>(address() + hdr_dos->e_lfanew);

		return &hdr_new->OptionalHeader.DataDirectory[directory_entry];
	}

	PIMAGE_DATA_DIRECTORY module_view::operator[](int directory_entry) const
	{
		return data_directory(directory_entry);
	}

	module_view this_module()
	{
		return module_view(GetModuleHandleW(nullptr));
	}

#define RELOCATION_TYPE(block)   ((block) >> 12u)
#define RELOCATION_OFFSET(block) ((block) &  0xFFFu)
#define RELOCATION_NEXT(rel) (PIMAGE_BASE_RELOCATION)((LPBYTE)rel + rel->SizeOfBlock)
#define RELOCATION_BLOCK_FIRST(rel) (PUINT16)((LPBYTE)rel + sizeof(IMAGE_BASE_RELOCATION))
#define RELOCATION_BLOCK_END(rel, first_block) (PUINT16)((LPBYTE)block_beg + (rel->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)))

	void process_relocations(module_view& module)
	{
		if (!module)
			return;

		const uint32_t delta = module.address() - module.preferred_image_base();

		if (module[IMAGE_DIRECTORY_ENTRY_BASERELOC]->Size == 0 || delta == 0)
			return;

		auto rel = module.from_base<PIMAGE_BASE_RELOCATION>(module[IMAGE_DIRECTORY_ENTRY_BASERELOC]->VirtualAddress);

		while (rel->VirtualAddress > 0)
		{
			const auto page_addr = module.from_base<PBYTE>(rel->VirtualAddress);

			const auto block_beg = RELOCATION_BLOCK_FIRST(rel);
			const auto block_end = RELOCATION_BLOCK_END(rel, block_beg);

			for (auto block = block_beg; block < block_end; ++block)
			{
				const int page_offset = RELOCATION_OFFSET(*block);

				switch (RELOCATION_TYPE(*block))
				{
					//
					// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#base-relocation-types
					//
					case IMAGE_REL_BASED_ABSOLUTE:
						break;

					case IMAGE_REL_BASED_HIGHLOW:
					{
						auto addr = reinterpret_cast<DWORD*>(page_addr + page_offset);
						mm::unprotect_rw _(addr, sizeof(DWORD));
						*addr = *addr + delta;
						break;
					}

					default:
						throw std::runtime_error("Unknown relocation type");
				}
			}

			rel = RELOCATION_NEXT(rel);
		}
	}

	void process_imports(module_view& game)
	{
		const auto imports = game[IMAGE_DIRECTORY_ENTRY_IMPORT];

		if (!imports)
			return;

		auto descriptor = game.from_base<PIMAGE_IMPORT_DESCRIPTOR>(imports->VirtualAddress);

		while (descriptor && descriptor->Name)
		{
			auto dll_name = game.from_base<char*>(descriptor->Name);

			// TODO: redo those 4 following lines
			auto name_entry = game.from_base<uintptr_t*>(descriptor->OriginalFirstThunk);
			auto addr_entry = game.from_base<uintptr_t*>(descriptor->FirstThunk);

			if (descriptor->OriginalFirstThunk == 0)
				name_entry = addr_entry;

			while (*name_entry)
			{
				const char* procedure_name = game.from_base<PIMAGE_IMPORT_BY_NAME>(*name_entry)->Name;

				auto library = LoadLibraryA(dll_name);

				if (!library)
				 	throw std::runtime_error("Error loading " + std::string(dll_name));

				if (FARPROC ProcAddress = GetProcAddress(library, procedure_name))
				{
					mm::unprotect_rw _(addr_entry, sizeof(DWORD));
					*addr_entry = (uintptr_t)ProcAddress;
				}
				else
				 	throw std::runtime_error("Error loading " + std::string(procedure_name) + " from " + dll_name);


				name_entry++;
				addr_entry++;
			}

			descriptor++;
		}
	}

	PPEB current_peb()
	{
		return reinterpret_cast<PPEB>(__readfsdword(0x30));
	}

	void peb_set_image_base(void* base)
	{
		PPEB peb = current_peb();

		//
		// Undocumented Peb->ImagebaseAddress
		//
		peb->Reserved3[1] = base;
	}
}