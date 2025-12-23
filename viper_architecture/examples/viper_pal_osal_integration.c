/**
 * viper_pal_osal_integration.c - VIPER与PAL、OSAL集成示例
 * 
 * 演示VIPER架构如何通过状态机与平台抽象层(PAL)和操作系统抽象层(OSAL)交互
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "viper.h"

/* 包含状态机头文件 */
#include "../../state_machine_extended/src/mealy_machine.h"
#include "../../state_machine_extended/src/parallel_fsm.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"
#include "../../state_machine_extended/src/efsm_protocol.h"
#include "../../state_machine_extended/src/acceptor_regex.h"

/* 包含PAL和OSAL头文件 */
#include "../../platform_abstraction_layer/include/pal.h"
#include "../../platform_abstraction_layer/include/pal_os_adapter.h"
#include "../../os_abstraction_layer/include/os_abstract.h"

/* 声明viper_view函数（在viper_view.c中定义） */
extern void viper_view_handle_click(struct viper_view *view);
extern void viper_view_render(struct viper_view *view);

/* 声明viper_view_create和viper_view_destroy，它们在viper.c中是静态的 */
/* 我们将直接使用viper_module_init创建的模块中的view组件 */

/* 定义示例状态 */
struct viper_view_state_idle {
    struct mealy_state base;
    int click_count;
};

struct viper_view_state_loading {
    struct mealy_state base;
    float progress;
};

/* 状态操作函数 */
static void view_idle_event_click(struct mealy_state *state, struct mealy_machine *machine);
static void view_idle_render(struct mealy_state *state, struct mealy_machine *machine);
static void view_loading_event_click(struct mealy_state *state, struct mealy_machine *machine);
static void view_loading_render(struct mealy_state *state, struct mealy_machine *machine);

/* 状态操作表 */
static struct mealy_state_ops view_idle_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = view_idle_event_click,
    .event_hover = NULL,
    .event_keypress = NULL,
    .render = view_idle_render,
    .__super = NULL
};

static struct mealy_state_ops view_loading_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = view_loading_event_click,
    .event_hover = NULL,
    .event_keypress = NULL,
    .render = view_loading_render,
    .__super = NULL
};

/* 全局状态实例 */
static struct viper_view_state_idle idle_state = {
    .base = { .ops = &view_idle_ops },
    .click_count = 0
};

static struct viper_view_state_loading loading_state = {
    .base = { .ops = &view_loading_ops },
    .progress = 0.0f
};

/* 状态实现 */
static void view_idle_event_click(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_idle *idle = (struct viper_view_state_idle *)state;
    idle->click_count++;
    
    printf("[VIPER View] Idle state click (click count: %d)\n", idle->click_count);
    printf("[OSAL] Creating timer for simulated loading process\n");
    
    /* 通过OSAL创建定时器 */
    os_timer_t *timer = os_timer_create(NULL, NULL, "loading_timer");
    if (timer) {
        printf("[OSAL] Timer created successfully\n");
        /* 在实际应用中，这里会启动定时器 */
        /* os_timer_start(timer, 100, true); */
    }
    
    /* Convert events through PAL OS adapter */
    printf("[PAL OS Adapter] Converting OS timer event to hardware control event\n");
    
    /* 转换到加载状态 */
    mealy_machine_set_state(machine, (struct mealy_state *)&loading_state);
}

static void view_idle_render(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_idle *idle = (struct viper_view_state_idle *)state;
    printf("[VIPER View] Rendering idle state (click count: %d)\n", idle->click_count);
}

static void view_loading_event_click(struct mealy_state *state, struct mealy_machine *machine)
{
    printf("[VIPER View] Loading state click - cancel loading\n");
    
    /* Stop timer through OSAL */
    printf("[OSAL] Stopping loading timer\n");
    
    /* 转换回空闲状态 */
    mealy_machine_set_state(machine, (struct mealy_state *)&idle_state);
}

static void view_loading_render(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_loading *loading = (struct viper_view_state_loading *)state;
    
    /* 模拟进度更新 */
    loading->progress += 0.2f;
    if (loading->progress >= 1.0f) {
        loading->progress = 1.0f;
        printf("[VIPER View] Loading complete!\n");
        
        /* Control hardware LED through PAL */
        printf("[PAL] Controlling hardware LED to indicate completion\n");
        #ifdef PLATFORM_WINDOWS_SIM
            printf("[PAL Windows Simulator] Showing completion status in GUI\n");
        #elif defined(PLATFORM_STM32)
            printf("[PAL STM32] Turning on onboard LED\n");
        #endif
        
        /* 转换回空闲状态 */
        mealy_machine_set_state(machine, (struct mealy_state *)&idle_state);
    }
    
    printf("[VIPER View] Rendering loading state (progress: %.0f%%)\n", loading->progress * 100);
}

/* 演示层级关系的函数 */
void demonstrate_hierarchy(void)
{
    printf("========================================\n");
    printf("VIPER, PAL, OSAL and State Machine Hierarchy Demonstration\n");
    printf("========================================\n\n");
    
    /* 1. Initialize VIPER module */
    printf("1. Initializing VIPER module\n");
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 1,
        .max_events = 10
    };
    strcpy(config.module_name, "HierarchyDemo");
    
    struct viper_module *module = viper_module_init(&config);
    if (!module) {
        printf("Error: Cannot initialize VIPER module\n");
        return;
    }
    
    printf("   VIPER module status: %s\n", viper_module_get_status(module));
    
    /* 2. Initialize OS abstraction layer */
    printf("\n2. Initializing OS abstraction layer\n");
    printf("   OS abstraction layer provides cross-platform threads, timers, synchronization primitives\n");
    printf("   Supported OS: POSIX/Linux, FreeRTOS, RT-Thread, BareMetal\n");
    
    /* 3. Initialize PAL OS adapter */
    printf("\n3. Initializing PAL OS adapter\n");
    printf("   PAL OS adapter connects OSAL and PAL\n");
    printf("   Functions: Hardware event conversion, OS service mapping, FSM context creation\n");
    
    /* 4. Initialize platform abstraction layer */
    printf("\n4. Initializing platform abstraction layer\n");
    printf("   PAL provides unified hardware access interface\n");
    #ifdef PLATFORM_WINDOWS_SIM
        printf("   Current platform: Windows simulator (SDL2 GUI)\n");
    #elif defined(PLATFORM_STM32)
        printf("   Current platform: STM32 HAL\n");
    #elif defined(PLATFORM_ESP32)
        printf("   Current platform: ESP32 IDF\n");
    #else
        printf("   Current platform: Generic platform\n");
    #endif
    
    /* 5. Get VIPER View component */
    printf("\n5. Getting VIPER View component (Mealy state machine)\n");
    struct viper_view *view = module->view;
    if (!view) {
        printf("Error: View component not created\n");
        viper_module_destroy(module);
        return;
    }
    
    printf("   View component obtained successfully\n");
    
    /* 6. Demonstrate event flow */
    printf("\n6. Demonstrating event flow (bottom-up)\n");
    printf("   a) Hardware interrupt occurs (simulated button press)\n");
    printf("   b) PAL detects hardware event\n");
    printf("   c) PAL OS adapter converts hardware event to OS event\n");
    printf("   d) OS event queue delivers to application layer\n");
    printf("   e) VIPER View state machine processes event\n");
    
    /* Simulate hardware event */
    printf("\n7. Simulating hardware button press event\n");
    printf("   [Note] Due to missing viper_view_handle_click implementation, only printing message\n");
    printf("   In actual application, this would call viper_view_handle_click(view)\n");
    
    printf("\n8. Rendering View state\n");
    printf("   [Note] Due to missing viper_view_render implementation, only printing message\n");
    printf("   In actual application, this would call viper_view_render(view)\n");
    
    /* Render again to show progress updates */
    printf("\n9. Simulating timer trigger, updating loading progress\n");
    printf("   [Simulation] Loading progress: 20%%\n");
    printf("   [Simulation] Loading progress: 40%%\n");
    printf("   [Simulation] Loading progress: 60%%\n");
    printf("   [Simulation] Loading progress: 80%%\n");
    printf("   [Simulation] Loading progress: 100%%\n");
    
    /* 10. Demonstrate control flow */
    printf("\n10. Demonstrating control flow (top-down)\n");
    printf("    a) User action triggers VIPER event\n");
    printf("    b) VIPER Presenter coordinates components\n");
    printf("    c) VIPER Interactor executes business logic\n");
    printf("    d) VIPER Entity updates data model\n");
    printf("    e) Calling system services through OSAL\n");
    printf("    f) PAL OS adapter maps to hardware control\n");
    printf("    g) PAL performs actual hardware operation\n");
    
    /* Create VIPER event demonstration */
    printf("\n11. Creating VIPER event demonstration\n");
    struct viper_event *event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        "Button click event",
        strlen("Button click event") + 1,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_PRESENTER
    );
    
    if (event) {
        printf("   VIPER event created successfully: type=%d, source=%d, target=%d\n",
               event->type, event->source_component, event->target_component);
        
        /* Process event */
        int result = viper_module_process_event(module, event);
        printf("   Event processing result: %d\n", result);
        
        viper_event_destroy(event);
    }
    
    /* Clean up resources */
    printf("\n12. Cleaning up resources\n");
    viper_module_destroy(module);
    
    printf("\n========================================\n");
    printf("Demonstration completed\n");
    printf("========================================\n");
}

/* 主函数 */
int main(void)
{
    printf("VIPER, PAL, OSAL and State Machine Hierarchy Integration Example\n");
    printf("Version: 1.0\n");
    printf("Compile time: %s %s\n", __DATE__, __TIME__);
    printf("\n");
    
    demonstrate_hierarchy();
    
    return 0;
}
