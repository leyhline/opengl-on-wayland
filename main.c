#include <stdlib.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "GL/glew.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/// Error handling function definitions.
int sdl_initialize(Uint32 flags);
int sdl_window_check_error(SDL_Window* window);
SDL_SYSWM_TYPE check_for_wayland(SDL_Window* window);
int sdl_glcontext_check_error(SDL_GLContext context);
int glew_initialize();
void sdl_cleanup(SDL_Window* window, SDL_GLContext context);
void gl_draw_hello_world_triangle();
void gl_shader_check_error(GLuint shader);

/// I moved most of the error handling in separate functions.
int main(void) {
    if (sdl_initialize(SDL_INIT_VIDEO) != 0) return EXIT_FAILURE;

    // Find out OpenGL core profile version with `glxinfo | grep -i opengl`
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* window = SDL_CreateWindow("Hello Wayland",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (sdl_window_check_error(window) != 0) return EXIT_FAILURE;
    check_for_wayland(window);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (sdl_glcontext_check_error(context) != 0) {
        sdl_cleanup(window, context);
        return EXIT_FAILURE;
    }

    if (glew_initialize() != 0) {
        sdl_cleanup(window, context);
        return EXIT_FAILURE;
    }

    gl_draw_hello_world_triangle();    

    SDL_GL_SwapWindow(window);
    SDL_Delay(5000);

    sdl_cleanup(window, context);
    return EXIT_SUCCESS;
}

/// Error handling function implementations.
int sdl_initialize(Uint32 flags) {
    if (SDL_Init(flags) != 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    if (atexit(SDL_Quit) != 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "Failed registering SDL_Quit");
        return 1;
    }
    return 0;
}

int sdl_window_check_error(SDL_Window* window) {
    if (window == NULL) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "Could not create window: %s", SDL_GetError());
        return 1;
    }
    return 0;
}

SDL_SYSWM_TYPE check_for_wayland(SDL_Window* window) {
    SDL_SysWMinfo info;
    SDL_bool got_info = SDL_GetWindowWMInfo(window, &info);
    if (!got_info) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
            "Could not retrieve window information: %s", SDL_GetError());
        return SDL_SYSWM_UNKNOWN;
    }
    else if (info.subsystem == SDL_SYSWM_WAYLAND) {
        SDL_Log("Using Wayland - SUCCESS");
    }
    else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
            "Not using Wayland - FAILURE");
    }
    return info.subsystem;
}

int sdl_glcontext_check_error(SDL_GLContext context) {
    if (context == NULL) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
            "Could not create OpenGL context: %s", SDL_GetError());
        return 1;
    }
    return 0;
}

int glew_initialize() {
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, 
            "Error initializing GLEW: %s", glewGetErrorString(err));
        return 1;
    }
    return 0;
}

void sdl_cleanup(SDL_Window* window, SDL_GLContext context) {
    SDL_GL_DeleteContext(context); 
    SDL_DestroyWindow(window);
}

/// Most code is from <https://learnopengl.com>
void gl_draw_hello_world_triangle() {
    // Shader source
    const GLchar* vertex_shader_source = "#version 450 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "}\0";
    const GLchar* fragment_shader_source = "#version 450 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(ourColor, 1.0f);\n"
        "}\n\0";

    // Viewport and background.
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(1.0, 0.4, 0.8, 1.);  // pink background
    glClear(GL_COLOR_BUFFER_BIT);

    // Compiling and linking the shaders.
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    gl_shader_check_error(vertex_shader);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    gl_shader_check_error(fragment_shader);
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Create buffers and fill with stuff.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertices[] = {
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // And now draw this stuff.
    glUseProgram(shader_program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void gl_shader_check_error(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
            "Shader compilation failed: %s", info);
    }
}
