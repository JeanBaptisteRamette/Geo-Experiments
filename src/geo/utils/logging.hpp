#ifndef GEO_EXPERIMENTS_LOGGING_HPP
#define GEO_EXPERIMENTS_LOGGING_HPP

#include <filesystem>
#include <string_view>
#include <fstream>
#include <Windows.h>


namespace geo::io
{
	class logger;

	///
	/// Singleton
	///
	inline logger* instance = nullptr;


	///
	/// Only has effects on console output
	///
	enum class log_indicator
	{
		info,
		warn,
		error
	};


	static std::filesystem::path GetAppData()
	{
		char* Expanded = nullptr;
		size_t sz = 0;

		if (_dupenv_s(&Expanded, &sz, "appdata") == 0 && Expanded != nullptr)
		{
			std::filesystem::path Path(Expanded);
			free(Expanded);

			return Path;
		}

		return {};
	}

	static std::ofstream OpenLoggingStream(std::string_view Filename)
	{
		auto Path = GetAppData();

		if (Path.empty())
			Path = std::filesystem::current_path();

		Path /= "geo-x";

		std::error_code ErrCode;

		if (!std::filesystem::exists(Path))
			std::filesystem::create_directory(Path, ErrCode);

		Path /= Filename;

		return std::ofstream(Path, std::ios_base::out | std::ios_base::app);
	}


	class logger
	{
	public:
		explicit logger(std::string_view Filename) : logfile_stream(OpenLoggingStream(Filename))
		{
			instance = this;

			//
			// Redirect console
			//
			AllocConsole();
			SetConsoleTitleA("geo-experiments");

			native_console_stream = GetStdHandle(STD_OUTPUT_HANDLE);

			console_stream.open("CONOUT$");
		}

		~logger()
		{
			FreeConsole();
		};

		template<typename ...Args>
		void log(log_indicator indicator, std::string_view format, Args&&...args)
		{
			if (!logfile_stream || !console_stream)
				return;

			auto time = std::time(nullptr);
			tm localtime {};

			if (localtime_s(&localtime, &time) != 0)
				return;

			const auto timestamp = std::format("[{:0>2}:{:0>2}:{:0>2}]",
											   localtime.tm_hour,
											   localtime.tm_min,
											   localtime.tm_sec);

			const auto message = std::vformat(format, std::make_format_args(args...));

			ConsoleSetCorrespondingColor(indicator);

			logfile_stream << timestamp << ' ' << message << std::endl;
			console_stream << timestamp << ' ' << message << std::endl;
		}

	private:
		void ConsoleSetCorrespondingColor(log_indicator indicator)
		{
			auto con_attributes = [](log_indicator indicator) -> WORD
			{
				switch (indicator)
				{
					case log_indicator::info:  return FOREGROUND_BLUE;
					case log_indicator::warn:  return FOREGROUND_RED | FOREGROUND_INTENSITY;

					default:
						return FOREGROUND_RED;
				}
			};

			SetConsoleTextAttribute(native_console_stream, con_attributes(indicator));
		}

	private:
		std::ofstream logfile_stream;
		std::ofstream console_stream;

		HANDLE native_console_stream;
	};


	template<typename ...Args>
	inline void info(std::string_view format, Args&& ...args)
	{
		if (instance)
			instance->log(log_indicator::info, format, args...);
	}

	template<typename ...Args>
	inline void warn(std::string_view format, Args&& ...args)
	{
		if (instance)
			instance->log(log_indicator::warn, format, args...);
	}

	template<typename ...Args>
	inline void error(std::string_view format, Args&& ...args)
	{
		if (instance)
			instance->log(log_indicator::error, format, args...);
	}

	inline void hexdump(const void* address, size_t bytes_count, std::string_view description = {})
	{
		if (!instance)
			return;

		if (!description.empty())
			instance->log(log_indicator::info, "{}:", description);

		const auto bytes = reinterpret_cast<const uint8_t*>(address);

		char fmt_buffer[64];
		char* buffer = fmt_buffer;

		for (size_t i = 0; i < bytes_count; ++i)
		{
			const int WrittenCount = snprintf(buffer, 4, "%02X ", bytes[i]);

			const bool LineJump = i && (i % 16) == 0;

			if (LineJump || i == bytes_count - 1)
			{
				instance->log(log_indicator::info, fmt_buffer);
				buffer = fmt_buffer;
			}
			else
			{
				buffer += WrittenCount;
			}
		}
	}
}


#endif //GEO_EXPERIMENTS_LOGGING_HPP
