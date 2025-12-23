/**
 * pal_windows_simulator.c - Windows Simulator Implementation for PAL
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design-Patterns in C *
 * 
 * Windows SDL2-based simulator implementation of Platform Abstraction Layer.
 * Provides GUI visualization of hardware operations.
 */

#include "pal.h"
#include "pal_gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

/* SDL2 headers */
#include <SDL.h>
#include <SDL_ttf.h>

/* ==================== Internal Constants ==================== */

#define PAL_VERSION_STRING "1.0.0-windows-simulator"
#define MAX_GPIO_PINS 256
#define MAX_LED_MATRICES 16
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600
#define DEFAULT_TARGET_FPS 60

/* ==================== Internal Types ==================== */

typedef struct {
    pal_gpio_state_t state;
    pal_gpio_config_t config;
    pal_callback_t interrupt_callback;
    void *interrupt_context;
    bool interrupt_enabled;
    uint32_t last_change_time;
} gpio_pin_t;

typedef struct {
    pal_led_matrix_config_t config;
    bool *pixel_state;  /* rows * columns */
    uint32_t last_update_time;
    SDL_Texture *texture;
    SDL_Rect display_rect;
} led_matrix_t;

typedef struct {
    pal_config_t config;
    pal_performance_config_t perf_config;
    
    /* SDL2 resources */
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    
    /* GPIO state */
    gpio_pin_t gpio_pins[MAX_GPIO_PINS];
    
    /* LED matrices */
    led_matrix_t led_matrices[MAX_LED_MATRICES];
    uint8_t led_matrix_count;
    
    /* Timing */
    uint32_t start_time;
    uint32_t last_frame_time;
    uint32_t frame_count;
    
    /* Status */
    bool initialized;
    char status_string[256];
} pal_context_t;

/* ==================== Static Variables ==================== */

static pal_context_t g_context = {0};

/* ==================== Internal Helper Functions ==================== */

static uint32_t get_current_time_ms(void)
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

static void update_status_string(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(g_context.status_string, sizeof(g_context.status_string), format, args);
    va_end(args);
}

static void render_gpio_panel(SDL_Renderer *renderer, TTF_Font *font)
{
    /* Draw GPIO panel background */
    SDL_Rect panel_rect = {10, 10, 300, 200};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &panel_rect);
    
    /* Draw panel border */
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &panel_rect);
    
    /* Draw title */
    SDL_Color text_color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "GPIO Status", text_color);
    if (surface) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect text_rect = {20, 15, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &text_rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
    
    /* Draw GPIO pins */
    int active_pins = 0;
    for (int i = 0; i < 16; i++) {
        if (g_context.gpio_pins[i].config.mode != PAL_GPIO_MODE_INPUT) {
            int x = 20 + (i % 8) * 35;
            int y = 50 + (i / 8) * 30;
            
            /* Draw pin circle */
            SDL_Rect pin_rect = {x, y, 25, 25};
            SDL_SetRenderDrawColor(renderer, 
                g_context.gpio_pins[i].state == PAL_GPIO_HIGH ? 0 : 100,
                g_context.gpio_pins[i].state == PAL_GPIO_HIGH ? 255 : 100,
                0, 255);
            SDL_RenderFillRect(renderer, &pin_rect);
            
            /* Draw pin label */
            char label[8];
            snprintf(label, sizeof(label), "%d", i);
            surface = TTF_RenderText_Solid(font, label, text_color);
            if (surface) {
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                if (texture) {
                    SDL_Rect label_rect = {x + 8, y + 5, surface->w, surface->h};
                    SDL_RenderCopy(renderer, texture, NULL, &label_rect);
                    SDL_DestroyTexture(texture);
                }
                SDL_FreeSurface(surface);
            }
            
            active_pins++;
        }
    }
}

static void render_performance_panel(SDL_Renderer *renderer, TTF_Font *font)
{
    /* Calculate FPS */
    uint32_t current_time = get_current_time_ms();
    uint32_t elapsed = current_time - g_context.last_frame_time;
    
    if (elapsed >= 1000) {
        float fps = (float)g_context.frame_count * 1000.0f / elapsed;
        
        /* Draw FPS counter */
        SDL_Rect fps_rect = {10, 220, 200, 40};
        SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
        SDL_RenderFillRect(renderer, &fps_rect);
        
        SDL_Color text_color = {255, 255, 255, 255};
        char fps_text[64];
        snprintf(fps_text, sizeof(fps_text), "FPS: %.1f / Target: %d", 
                 fps, g_context.perf_config.target_fps);
        
        SDL_Surface *surface = TTF_RenderText_Solid(font, fps_text, text_color);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect text_rect = {15, 225, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &text_rect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
        
        g_context.last_frame_time = current_time;
        g_context.frame_count = 0;
    }
    
    g_context.frame_count++;
}

static void render_led_matrices(SDL_Renderer *renderer)
{
    for (int i = 0; i < g_context.led_matrix_count; i++) {
        led_matrix_t *matrix = &g_context.led_matrices[i];
        
        /* Create texture if not exists */
        if (!matrix->texture) {
            matrix->texture = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_TARGET,
                matrix->config.columns * 20,
                matrix->config.rows * 20);
                
            matrix->display_rect.x = 320 + (i % 2) * 220;
            matrix->display_rect.y = 10 + (i / 2) * 220;
            matrix->display_rect.w = matrix->config.columns * 20;
            matrix->display_rect.h = matrix->config.rows * 20;
        }
        
        /* Render to texture */
        SDL_SetRenderTarget(renderer, matrix->texture);
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        
        /* Draw LED grid */
        for (int row = 0; row < matrix->config.rows; row++) {
            for (int col = 0; col < matrix->config.columns; col++) {
                bool pixel_on = matrix->pixel_state[row * matrix->config.columns + col];
                
                SDL_Rect led_rect = {
                    col * 20 + 2,
                    row * 20 + 2,
                    16, 16
                };
                
                if (pixel_on) {
                    SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
                }
                
                SDL_RenderFillRect(renderer, &led_rect);
                
                /* Draw LED border */
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &led_rect);
            }
        }
        
        /* Restore default render target */
        SDL_SetRenderTarget(renderer, NULL);
        
        /* Draw texture to screen */
        SDL_RenderCopy(renderer, matrix->texture, NULL, &matrix->display_rect);
        
        /* Draw matrix border */
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_RenderDrawRect(renderer, &matrix->display_rect);
    }
}

/* ==================== Public API Implementation ==================== */

pal_status_t pal_init(const pal_config_t *config)
{
    if (g_context.initialized) {
        return PAL_STATUS_OK;
    }
    
    if (!config) {
        return PAL_STATUS_INVALID_PARAM;
    }
    
    /* Store configuration */
    memcpy(&g_context.config, config, sizeof(pal_config_t));
    
    /* Initialize SDL2 */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        update_status_string("SDL_Init failed: %s", SDL_GetError());
        return PAL_STATUS_ERROR;
    }
    
    if (TTF_Init() < 0) {
        update_status_string("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return PAL_STATUS_ERROR;
    }
    
    /* Create window if GUI enabled */
    if (config->enable_gui) {
        g_context.window = SDL_CreateWindow(
            "Platform Abstraction Layer - Windows Simulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            DEFAULT_WINDOW_WIDTH,
            DEFAULT_WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        
        if (!g_context.window) {
            update_status_string("SDL_CreateWindow failed: %s", SDL_GetError());
            TTF_Quit();
            SDL_Quit();
            return PAL_STATUS_ERROR;
        }
        
        g_context.renderer = SDL_CreateRenderer(g_context.window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
        if (!g_context.renderer) {
            update_status_string("SDL_CreateRenderer failed: %s", SDL_GetError());
            SDL_DestroyWindow(g_context.window);
            TTF_Quit();
            SDL_Quit();
            return PAL_STATUS_ERROR;
        }
        
        /* Load font */
        g_context.font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
        if (!g_context.font) {
            /* Try fallback font */
            g_context.font = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 16);
        }
    }
    
    /* Initialize GPIO pins */
    for (int i = 0; i < MAX_GPIO_PINS; i++) {
        g_context.gpio_pins[i].config.mode = PAL_GPIO_MODE_INPUT;
        g_context.gpio_pins[i].state = PAL_GPIO_LOW;
        g_context.gpio_pins[i].interrupt_enabled = false;
    }
    
    /* Set default performance configuration */
    g_context.perf_config.target_fps = DEFAULT_TARGET_FPS;
    g_context.perf_config.update_frequency_hz = 1000;
    g_context.perf_config.vsync_enabled = true;
    g_context.perf_config.limit_cpu_usage = true;
    g_context.perf_config.max_frame_time_ms = 100;
    
    /* Initialize timing */
    g_context.start_time = get_current_time_ms();
    g_context.last_frame_time = g_context.start_time;
    g_context.frame_count = 0;
    
    g_context.initialized = true;
    update_status_string("PAL Windows Simulator initialized successfully");
    
    return PAL_STATUS_OK;
}

pal_status_t pal_deinit(void)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    
    /* Free LED matrix textures */
    for (int i = 0; i < g_context.led_matrix_count; i++) {
        if (g_context.led_matrices[i].texture) {
            SDL_DestroyTexture(g_context.led_matrices[i].texture);
        }
        if (g_context.led_matrices[i].pixel_state) {
            free(g_context.led_matrices[i].pixel_state);
        }
    }
    
    /* Free SDL2 resources */
    if (g_context.font) {
        TTF_CloseFont(g_context.font);
    }
    if (g_context.renderer) {
        SDL_DestroyRenderer(g_context.renderer);
    }
    if (g_context.window) {
        SDL_DestroyWindow(g_context.window);
    }
    
    TTF_Quit();
    SDL_Quit();
    
    printf("[PAL] pal_deinit: resetting context\n");
    memset(&g_context, 0, sizeof(g_context));
    
    return PAL_STATUS_OK;
}

pal_platform_t pal_get_platform(void)
{
    return PAL_PLATFORM_WINDOWS_SIMULATOR;
}

const char *pal_get_version(void)
{
    return PAL_VERSION_STRING;
}

const char *pal_get_status_string(void)
{
    return g_context.status_string;
}

pal_status_t pal_set_performance_config(const pal_performance_config_t *config)
{
    if (!config) {
        return PAL_STATUS_INVALID_PARAM;
    }
    
    memcpy(&g_context.perf_config, config, sizeof(pal_performance_config_t));
    
    /* Apply VSYNC setting */
    if (g_context.renderer) {
        SDL_RenderSetVSync(g_context.renderer, config->vsync_enabled ? 1 : 0);
    }
    
    return PAL_STATUS_OK;
}

pal_status_t pal_get_performance_config(pal_performance_config_t *config)
{
    if (!config) {
        return PAL_STATUS_INVALID_PARAM;
    }
    
    memcpy(config, &g_context.perf_config, sizeof(pal_performance_config_t));
    return PAL_STATUS_OK;
}

bool pal_process_events(void)
{
    if (!g_context.initialized || !g_context.config.enable_gui) {
        return true;
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                return false;
            }
        }
    }
    
    /* Render GUI */
    SDL_SetRenderDrawColor(g_context.renderer, 20, 20, 30, 255);
    SDL_RenderClear(g_context.renderer);
    
    render_gpio_panel(g_context.renderer, g_context.font);
    render_performance_panel(g_context.renderer, g_context.font);
    render_led_matrices(g_context.renderer);
    
    SDL_RenderPresent(g_context.renderer);
    
    /* Limit frame rate if needed */
    if (g_context.perf_config.target_fps > 0) {
        static uint32_t last_frame = 0;
        uint32_t frame_time = 1000 / g_context.perf_config.target_fps;
        uint32_t current_time = get_current_time_ms();
        uint32_t elapsed = current_time - last_frame;
        
        if (elapsed < frame_time) {
            SDL_Delay(frame_time - elapsed);
        }
        last_frame = get_current_time_ms();
    }
    
    return true;
}

uint32_t pal_get_elapsed_time_ms(void)
{
    return get_current_time_ms() - g_context.start_time;
}

void pal_delay_ms(uint32_t ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/* ==================== GPIO Implementation ==================== */

pal_status_t pal_gpio_init(pal_gpio_pin_t pin, const pal_gpio_config_t *config)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    
    if (pin >= MAX_GPIO_PINS || !config) {
        return PAL_STATUS_INVALID_PARAM;
    }
    
/* Store configuration */
    memcpy(&g_context.gpio_pins[pin].config, config, sizeof(pal_gpio_config_t));

    /* Set initial state */
    if (config->mode == PAL_GPIO_MODE_OUTPUT) {
        g_context.gpio_pins[pin].state = config->initial_state ? PAL_GPIO_HIGH : PAL_GPIO_LOW;
    } else {
        g_context.gpio_pins[pin].state = PAL_GPIO_LOW;
    }

    g_context.gpio_pins[pin].last_change_time = get_current_time_ms();

    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_deinit(pal_gpio_pin_t pin)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }

    if (pin >= MAX_GPIO_PINS) {
        return PAL_STATUS_INVALID_PARAM;
    }

    /* Reset configuration */
    memset(&g_context.gpio_pins[pin].config, 0, sizeof(pal_gpio_config_t));
    g_context.gpio_pins[pin].config.mode = PAL_GPIO_MODE_INPUT;
    g_context.gpio_pins[pin].state = PAL_GPIO_LOW;
    g_context.gpio_pins[pin].interrupt_callback = NULL;
    g_context.gpio_pins[pin].interrupt_context = NULL;
    g_context.gpio_pins[pin].interrupt_enabled = false;

    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_write(pal_gpio_pin_t pin, pal_gpio_state_t state)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }

    if (pin >= MAX_GPIO_PINS) {
        return PAL_STATUS_INVALID_PARAM;
    }

    /* In simulator, allow writing to any pin (simulate external drive) */
    /* No mode check */

    g_context.gpio_pins[pin].state = state;
    g_context.gpio_pins[pin].last_change_time = get_current_time_ms();

    /* Trigger interrupt if configured */
    if (g_context.gpio_pins[pin].interrupt_enabled &&
        g_context.gpio_pins[pin].interrupt_callback) {
        g_context.gpio_pins[pin].interrupt_callback(g_context.gpio_pins[pin].interrupt_context);
    }

    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_read(pal_gpio_pin_t pin, pal_gpio_state_t *state)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }

    if (pin >= MAX_GPIO_PINS || !state) {
        return PAL_STATUS_INVALID_PARAM;
    }

    *state = g_context.gpio_pins[pin].state;
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_toggle(pal_gpio_pin_t pin)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }

    if (pin >= MAX_GPIO_PINS) {
        return PAL_STATUS_INVALID_PARAM;
    }

    if (g_context.gpio_pins[pin].config.mode != PAL_GPIO_MODE_OUTPUT) {
        return PAL_STATUS_ERROR;
    }

    g_context.gpio_pins[pin].state = (g_context.gpio_pins[pin].state == PAL_GPIO_HIGH) 
                                      ? PAL_GPIO_LOW : PAL_GPIO_HIGH;
    g_context.gpio_pins[pin].last_change_time = get_current_time_ms();

    /* Trigger interrupt if configured */
    if (g_context.gpio_pins[pin].interrupt_enabled &&
        g_context.gpio_pins[pin].interrupt_callback) {
        g_context.gpio_pins[pin].interrupt_callback(g_context.gpio_pins[pin].interrupt_context);
    }

    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_set_interrupt_callback(pal_gpio_pin_t pin, 
                                             pal_callback_t callback, 
                                             void *context)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }

    if (pin >= MAX_GPIO_PINS) {
        return PAL_STATUS_INVALID_PARAM;
    }

    g_context.gpio_pins[pin].interrupt_callback = callback;
    g_context.gpio_pins[pin].interrupt_context = context;
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_interrupt_enable(pal_gpio_pin_t pin, bool enable)
{
    if (!g_context.initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }

    if (pin >= MAX_GPIO_PINS) {
        return PAL_STATUS_INVALID_PARAM;
    }

    g_context.gpio_pins[pin].interrupt_enabled = enable;
    return PAL_STATUS_OK;
}
