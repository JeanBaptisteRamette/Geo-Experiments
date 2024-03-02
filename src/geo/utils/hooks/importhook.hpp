#ifndef GEO_EXPERIMENTS_IMPORTHOOK_HPP
#define GEO_EXPERIMENTS_IMPORTHOOK_HPP

#include <geo/utils/nt.hpp>
#include <geo/utils/mm.hpp>
#include <geo/utils/traits.hpp>


namespace geo::iathk
{
	//
	// Just a quick API, it can be improved, but I wanted a similar interface to safetyhook's
	//
	class ImportHook final
	{
	public:
		ImportHook() = default;

		ImportHook& operator=(ImportHook&& other) noexcept
		{
			m_trampoline = other.m_trampoline;
			return *this;
		}

		ImportHook(void** entry, void* destination) : m_trampoline(*entry)
		{
			auto guard = mm::unprotect_rw(entry, sizeof(void*));
			*entry = destination;
		}

		~ImportHook() = default;

		template<typename T>
		[[nodiscard]] T original() const
		{
			return reinterpret_cast<T>(m_trampoline);
		}

		template <typename RetT = void, typename... Args>
		RetT call(Args... args)
		{
			return m_trampoline ? original<RetT(*)(Args...)>()(args...) : RetT();
		}

		template <typename RetT = void, typename... Args>
		RetT thiscall(Args... args)
		{
			return m_trampoline ? original<RetT(__thiscall*)(Args...)>()(args...) : RetT();
		}

		template <typename RetT = void, typename... Args>
		RetT stdcall(Args... args)
		{
			return m_trampoline ? original<RetT(__stdcall*)(Args...)>()(args...) : RetT();
		}

		template <typename RetT = void, typename... Args>
		RetT ccall(Args... args)
		{
			return m_trampoline ? original<RetT(__cdecl*)(Args...)>()(args...) : RetT();
		}

		template <typename RetT = void, typename... Args>
		RetT fastcall(Args... args)
		{
			return m_trampoline ? original<RetT(__fastcall*)(Args...)>()(args...) : RetT();
		}

	private:
		void* m_trampoline;
	};

	[[nodiscard]] ImportHook create_redirect(std::string_view dll_name, std::string_view proc_name, geo::FunctionPointer auto to)
	{
		void** entry = nt::this_module().imported_entry(dll_name, proc_name);

		if (!entry)
			throw std::runtime_error("create_redirect could not find symbol");

		return {entry, reinterpret_cast<void*>(to)};
	}

	void redirect(std::string_view dll_name, std::string_view proc_name, geo::FunctionPointer auto to)
	{
		void** entry = nt::this_module().imported_entry(dll_name, proc_name);

		if (!entry)
			throw std::runtime_error("redirect could not find symbol");

		auto guard = mm::unprotect_rw(entry, sizeof(void*));

		*entry = reinterpret_cast<void*>(to);
	}
}

#endif //GEO_EXPERIMENTS_IMPORTHOOK_HPP
