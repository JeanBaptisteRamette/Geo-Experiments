#ifndef GEO_EXPERIMENTS_COMPONENT_IMGUI_HPP
#define GEO_EXPERIMENTS_COMPONENT_IMGUI_HPP

#include <geo/components/base_component.hpp>
#include <geo/components/imgui_context.hpp>
#include <geo/game_definitions.hpp>
#include <geo/utils/logging.hpp>
#include <geo/utils/hooks.hpp>
#include <geo/imgui_menu.hpp>
#include <geo/gvars.hpp>
#include <cocos2d.h>


namespace geo
{
	HGLRC g_gl_context = nullptr;


	//
	// Component's hooks
	//
	safetyhook::InlineHook hk_gl_swapbuffers;
	geo::iathk::ImportHook hk_togglefullscreen;


	void gl_init(HDC device_context)
	{
		g_gl_context = wglCreateContext(device_context);
		wglMakeCurrent(device_context, g_gl_context);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLint m_viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_viewport);

		glOrtho(0, m_viewport[2], m_viewport[3], 0, 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glClearColor(0, 0, 0, 1);
	}

	BOOL __stdcall gl_swapbuffers(HDC device_context)
	{
		if (!imgui_context::visible())
			return hk_gl_swapbuffers.stdcall<BOOL>(device_context);

		HGLRC context = wglGetCurrentContext();

		if (!g_gl_context)
			gl_init(device_context);

		wglMakeCurrent(device_context, g_gl_context);

		if (!imgui_context::started())
			imgui_context::start(device_context);

		imgui_context::frame();

		wglMakeCurrent(device_context, context);

		return hk_gl_swapbuffers.stdcall<BOOL>(device_context);
	}

	void __fastcall cceglview_togglefullscreen(void* view, CCONV_PLACEHOLDER, bool a1, bool a2)
	{
		hk_togglefullscreen.thiscall(view, a1, a2);

		imgui_context::reset();
	}


	struct component_imgui final : geo::base_component
	{
		void pre_exec() override
		{
			mm::string(0x3FFE60_rva, "Geo-Xperiments");

			hk_gl_swapbuffers = safetyhook::create_inline(nt::exported(L"opengl32.dll", "wglSwapBuffers"), gl_swapbuffers);
			hk_togglefullscreen = geo::iathk::create_redirect("libcocos2d.dll", "?toggleFullScreen@CCEGLView@cocos2d@@QAEX_N0@Z", cceglview_togglefullscreen);

			io::info("UI component loaded");
		}
	};
}

#endif //GEO_EXPERIMENTS_COMPONENT_IMGUI_HPP
