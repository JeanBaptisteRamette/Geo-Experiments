#ifndef GEO_EXPERIMENTS_NT_HPP
#define GEO_EXPERIMENTS_NT_HPP

#include <string_view>
#include <filesystem>
#include <Windows.h>
#include <winternl.h>


namespace geo::nt
{
	class module_view
	{
	public:
		explicit module_view(const std::filesystem::path& path);
		explicit module_view(HMODULE handle);

		~module_view() = default;

		explicit operator bool() const;
		explicit operator HMODULE() const;

		[[nodiscard]]
		HMODULE native_pointer() const;

		[[nodiscard]]
		void* pointer() const;

		[[nodiscard]]
		uintptr_t address() const;

		[[nodiscard]]
		FARPROC entry_point() const;

		[[nodiscard]]
		uintptr_t preferred_image_base() const;

		[[nodiscard]]
		PIMAGE_DATA_DIRECTORY data_directory(int directory_entry) const;

		[[nodiscard]]
		PIMAGE_DATA_DIRECTORY operator[](int directory_entry) const;

		[[nodiscard]]
		uint32_t checksum() const;

		[[nodiscard]]
		uint32_t timestamp() const;

		[[nodiscard]]
		void** imported_entry(std::string_view module_name, std::string_view procedure_name) const;

		template<typename Procedure>
		Procedure exported_symbol(std::string_view procedure_name) const
		{
			return exported_symbol<Procedure>(internal_handle, procedure_name);
		}

		template<typename T>
		T from_base(uintptr_t VirtualAddress) const
		{
			return reinterpret_cast<T>(address() + VirtualAddress);
		}

	private:
		HMODULE internal_handle;
	};


	template<typename Procedure = FARPROC>
	Procedure exported(HMODULE module_handle, std::string_view procedure_name)
	{
		// TODO: just use our module_view wrapper
		if (!module_handle)
			return nullptr;

		FARPROC address = GetProcAddress(module_handle, procedure_name.data());

		return reinterpret_cast<Procedure>(address);
	}

	template<typename Procedure = FARPROC>
	Procedure exported(std::wstring_view library_name, std::string_view procedure_name)
	{
		// TODO: just use our module_view wrapper
		HMODULE h = GetModuleHandleW(library_name.data());

		if (!h)
			return nullptr;

		return exported<Procedure>(h, procedure_name);
	}

	module_view this_module();

	void process_relocations(module_view& module);

	void process_imports(module_view& game);

	PPEB current_peb();

	void peb_set_image_base(void* base);
}


#endif //GEO_EXPERIMENTS_NT_HPP
