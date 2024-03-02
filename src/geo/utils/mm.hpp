#ifndef GEO_EXPERIMENTS_MM_HPP
#define GEO_EXPERIMENTS_MM_HPP


#include <geo/utils/traits.hpp>
#include <geo/utils/nt.hpp>
#include <string_view>
#include <span>
#include <Windows.h>


namespace geo::mm
{
	template<DWORD ProtectionFlags>
	class [[nodiscard]] unprotected_guard
	{
	public:
		unprotected_guard(void* address, size_t size)
			: address(address), size(size), prev_protections(0)
		{
			if (!VirtualProtect(address, size, ProtectionFlags, &prev_protections))
				throw std::runtime_error("VirtualProtect failed");
		}

		~unprotected_guard()
		{
			VirtualProtect(address, size, prev_protections, &prev_protections);
		}

	private:
		void* const address;
		const size_t size;
		DWORD prev_protections;
	};

	using unprotect_rwx = unprotected_guard<PAGE_EXECUTE_READWRITE>;
	using unprotect_rw  = unprotected_guard<PAGE_READWRITE>;

	template<typename F>
	void execute_unprotected(void* address, size_t size, F&& callback)
	{
		const auto guard = unprotect_rwx(address, size);
		callback();
	}

	inline uintptr_t process_base()
	{
		return static_cast<uintptr_t>(nt::this_module().address());
	}

	// TODO: add constraint
	template<typename Type>
	inline Type subroutine(uintptr_t rva)
	{
		return nt::this_module().from_base<Type>(rva);
	}

	// TODO: add constraint pointer type for U
	template<typename T, typename U>
	inline T& field(U this_, uintptr_t field_offset)
	{
		auto field_addr = reinterpret_cast<uintptr_t>(this_) + field_offset;
		return *reinterpret_cast<T*>(field_addr);
	}

	template<std::ranges::range Range> requires std::is_same_v<std::ranges::range_value_t<Range>, uint8_t>
	void patch(void* address, Range const& bytes)
	{
		auto guard = unprotect_rwx(address, std::size(bytes));

		auto ptr = static_cast<uint8_t*>(address);

		for (const uint8_t b : bytes)
			*ptr++ = b;
	}

	inline void patch(void* address, const char* opcodes)
	{
		auto unhex = [](std::string_view opcodes) -> std::vector<uint8_t>
		{
			std::vector<uint8_t> code;

			for (size_t i = 0; i < opcodes.size(); i += 3)
			{
				const char c1 = opcodes[i];
				const char c2 = opcodes[i + 1];

				const uint8_t n1 = c1 >= 'A' ? (c1 - 'A' + 10) : (c1 - '0');
				const uint8_t n2 = c2 >= 'A' ? (c2 - 'A' + 10) : (c2 - '0');

				const uint8_t b = n1 << 4 | n2;

				code.push_back(b);
			}

			return code;
		};

		patch(address, unhex(opcodes));
	}

	inline void patch(void* address, const uint8_t* bytes, size_t byte_count)
	{
		return patch(address, std::span{bytes, byte_count});
	}

	inline void patch_repeat(void* address, uint8_t value, size_t times)
	{
		auto guard = unprotect_rwx(address, times);

		auto ptr = static_cast<PBYTE>(address);

		while (times--)
			*ptr++ = value;
	}

	inline void nop(void* address, size_t count)
	{
		return patch_repeat(address, 0x90, count);
	}

	inline void string(void* address, std::string_view replacer)
	{
		auto guard = unprotect_rw(address, std::size(replacer) + 1);

		auto ptr = static_cast<uint8_t*>(address);

		for (const auto b : replacer)
			*ptr++ = static_cast<uint8_t>(b);

		*ptr = '\0';
	}

	namespace literals
	{
		inline void* operator ""_rva(unsigned long long int BaseOffset)
		{
			return reinterpret_cast<void*>(process_base() + BaseOffset);
		}

		inline uint8_t* operator ""_bp(unsigned long long int BaseOffset)
		{
			return reinterpret_cast<uint8_t*>(process_base() + BaseOffset);
		}

		inline uintptr_t operator ""_ptr(unsigned long long int BaseOffset)
		{
			return process_base() + BaseOffset;
		}
	}
}



#endif //GEO_EXPERIMENTS_MM_HPP
