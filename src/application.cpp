#include "application.hpp"
#include "graphics/renderer.hpp"

#include <GLFW/glfw3.h>

namespace Garage
{
    namespace 
    {
        /**
         * @brief 윈도우 핸들.
         */
        GLFWwindow* window;

        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
            }
        }
    }

	bool Application::Initialize(const Specification& specification) noexcept
	{
        // 1. 창 초기화
        {
            glfwInit();

#if defined(GARAGE_GRAPHICS_GL)
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

            window = glfwCreateWindow(800, 600, specification.title.c_str(), nullptr, nullptr);

            if (!window)
            {
                glfwTerminate();
                return false;
            }

            glfwMakeContextCurrent(window);
            glfwSetKeyCallback(window, KeyCallback);
        }

        //2. 렌더러 초기화
        {
            Renderer::Descriptor desc;
            desc.window = window;

            Renderer::Initialize(desc);
        }

        return true;
    }

    void Application::Run() noexcept
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            glfwSwapBuffers(window);
        }

        glfwDestroyWindow(window);
        glfwTerminate();
    }
}