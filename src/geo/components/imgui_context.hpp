//
// Created by griffith on 24/02/2024.
//

#ifndef GEO_EXPERIMENTS_IMGUI_CONTEXT_HPP
#define GEO_EXPERIMENTS_IMGUI_CONTEXT_HPP


#include <Windows.h>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_opengl2.h>
#include <ImGui/imgui_impl_win32.h>
#include <geo/imgui_menu.hpp>


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace geo
{
	namespace
	{
		LRESULT __stdcall imgui_wproc_forwarder(HWND window, UINT msg, WPARAM param1, LPARAM param2);
		void override_window_proc(HWND window, WNDPROC new_proc);
	}

	class imgui_context
	{
	public:
		[[nodiscard]] static bool started()
		{
			return m_started;
		}

		static void start(HDC device_context)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			HWND window = WindowFromDC(device_context);

			ImGui_ImplWin32_Init(window);
			ImGui_ImplOpenGL2_Init();

			override_window_proc(window, imgui_wproc_forwarder);

			m_started = true;
		}

		static void reset()
		{
			if (m_started)
			{
				ImGui_ImplOpenGL2_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();

				m_started = false;
			}
		}

		static bool visible()
		{
			return m_visible;
		}

		static void show()
		{
			m_visible = true;
		}

		static void hide()
		{
			m_visible = false;
		}

		static void frame()
		{
			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ui_render_main();

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		}

	private:
		inline static bool m_started = false;
		inline static bool m_visible = false;
	};


	namespace
	{
		WNDPROC original_window_proc = nullptr;

		LRESULT __stdcall imgui_wproc_forwarder(HWND window, UINT msg, WPARAM param1, LPARAM param2)
		{
			if (imgui_context::visible())
				ImGui_ImplWin32_WndProcHandler(window, msg, param1, param2);

			return CallWindowProcA(original_window_proc, window, msg, param1, param2);
		}

		void override_window_proc(HWND window, WNDPROC new_proc)
		{
			original_window_proc = (WNDPROC)SetWindowLongA(window, GWLP_WNDPROC, (LONG)new_proc);
		}
	}
}

#endif //GEO_EXPERIMENTS_IMGUI_CONTEXT_HPP
