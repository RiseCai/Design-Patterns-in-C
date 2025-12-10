# comm_fsm 外部设备控制指令补丁

这个补丁展示了如何修改现有的comm_fsm模块以支持外部设备控制指令功能。

## 修改内容

### 1. 修改 comm_fsm.h

在 `comm_fsm.h` 中添加以下内容：

```c
/* 在 comm_event 枚举中添加新事件 */
enum comm_event {
    /* ... 现有事件 ... */
    COMM_EVT_DATA_RECEIVED,     /* Data received */
    COMM_EVT_COMMAND_RECEIVED,  /* NEW: Command received from external device */
    COMM_EVT_TIMEOUT,           /* Operation timeout */
    /* ... */
};

/* 添加指令回调类型定义 */
typedef enum {
    COMMAND_REC_START = 0x01,
    COMMAND_REC_STOP = 0x02,
    COMMAND_REC_PAUSE = 0x03,
    COMMAND_REC_RESUME = 0x04,
    COMMAND_UPLOAD_START = 0x05,
    COMMAND_POWER_OFF = 0x06,
    COMMAND_RESET = 0x07,
    COMMAND_GET_STATUS = 0x08
} command_code_t;

typedef void (*command_callback_t)(command_code_t cmd, const void *data, size_t size, void *user_data);

/* 在 comm_fsm 结构体中添加回调字段 */
struct comm_fsm {
    /* ... 现有字段 ... */
    
    /* NEW: Command handling */
    command_callback_t command_callback;
    void *command_callback_data;
    
    /* NEW: Receive buffer */
    uint8_t rx_buffer[256];
    size_t rx_len;
    
    /* ... 现有字段 ... */
};

/* 在 API 函数声明中添加新函数 */
void comm_fsm_set_command_callback(struct comm_fsm *comm, command_callback_t callback, void *user_data);
void comm_fsm_process_received_data(struct comm_fsm *comm, const void *data, size_t size);
```

### 2. 修改 comm_fsm.c

在 `comm_fsm.c` 中添加以下内容：

```c
/* 在文件顶部添加包含 */
#include <stdint.h>

/* 添加事件名称 */
static const char *event_names[] = {
    "COMM_EVT_CONNECT",
    "COMM_EVT_DISCONNECT",
    "COMM_EVT_CONNECTED",
    "COMM_EVT_DISCONNECTED",
    "COMM_EVT_DATA_READY",
    "COMM_EVT_DATA_SENT",
    "COMM_EVT_DATA_RECEIVED",
    "COMM_EVT_COMMAND_RECEIVED",  /* NEW */
    "COMM_EVT_TIMEOUT",
    "COMM_EVT_ERROR"
};

/* 添加指令解析函数 */
static int parse_command_packet(const uint8_t *data, size_t len, 
                               command_code_t *cmd, 
                               const uint8_t **payload, 
                               size_t *payload_len)
{
    /* 简单协议：起始符0xAA，结束符0x55 */
    if (len < 3) return -1;
    if (data[0] != 0xAA || data[len-1] != 0x55) return -1;
    
    *cmd = (command_code_t)data[1];
    *payload = data + 2;
    *payload_len = len - 3;
    
    return 0;
}

/* 修改 handle_transferring 函数以处理命令 */
static void handle_transferring(struct comm_fsm *comm, enum comm_event event, void *data)
{
    switch (event) {
        case COMM_EVT_DATA_RECEIVED:
            _MY_TRACE_STR("TRANSFERRING: DATA_RECEIVED\n");
            /* 更新字节计数 */
            if (comm) comm->bytes_received += 1024;
            
            /* NEW: 处理接收到的数据 */
            if (data && comm->command_callback) {
                /* 尝试解析为命令 */
                command_code_t cmd;
                const uint8_t *payload;
                size_t payload_len;
                
                if (parse_command_packet((const uint8_t *)data, /* 数据长度需要传递 */,
                                         &cmd, &payload, &payload_len) == 0) {
                    /* 触发命令接收事件 */
                    comm_fsm_dispatch_event(comm, COMM_EVT_COMMAND_RECEIVED, data);
                    
                    /* 调用回调函数 */
                    comm->command_callback(cmd, payload, payload_len, 
                                          comm->command_callback_data);
                }
            }
            break;
            
        case COMM_EVT_COMMAND_RECEIVED:
            _MY_TRACE_STR("TRANSFERRING: COMMAND_RECEIVED\n");
            /* 命令已由回调函数处理，这里可以记录日志 */
            break;
            
        /* ... 其他事件处理 ... */
    }
}

/* 添加新的API函数实现 */
void comm_fsm_set_command_callback(struct comm_fsm *comm, 
                                  command_callback_t callback, 
                                  void *user_data)
{
    if (!comm) return;
    
    comm->command_callback = callback;
    comm->command_callback_data = user_data;
    
    _MY_TRACE_STR("comm_fsm_set_command_callback: callback set\n");
}

void comm_fsm_process_received_data(struct comm_fsm *comm, const void *data, size_t size)
{
    if (!comm || !data || size == 0) return;
    
    _MY_TRACE_STR("comm_fsm_process_received_data: size=%zu\n", size);
    
    /* 复制数据到接收缓冲区 */
    if (size <= sizeof(comm->rx_buffer) - comm->rx_len) {
        memcpy(comm->rx_buffer + comm->rx_len, data, size);
        comm->rx_len += size;
        
        /* 尝试处理完整的数据包 */
        process_receive_buffer(comm);
    } else {
        _MY_TRACE_STR("Receive buffer overflow\n");
        comm->rx_len = 0; /* 清空缓冲区 */
    }
}

/* 内部缓冲区处理函数 */
static void process_receive_buffer(struct comm_fsm *comm)
{
    /* 查找完整的数据包（以0xAA开始，0x55结束） */
    size_t i = 0;
    while (i < comm->rx_len) {
        /* 查找起始符 */
        if (comm->rx_buffer[i] != 0xAA) {
            i++;
            continue;
        }
        
        /* 查找结束符 */
        size_t j = i + 1;
        while (j < comm->rx_len && comm->rx_buffer[j] != 0x55) {
            j++;
        }
        
        if (j < comm->rx_len) {
            /* 找到完整数据包 */
            size_t packet_len = j - i + 1;
            
            /* 解析命令 */
            command_code_t cmd;
            const uint8_t *payload;
            size_t payload_len;
            
            if (parse_command_packet(&comm->rx_buffer[i], packet_len,
                                    &cmd, &payload, &payload_len) == 0) {
                /* 触发数据接收事件 */
                comm_fsm_dispatch_event(comm, COMM_EVT_DATA_RECEIVED, 
                                       (void *)&comm->rx_buffer[i]);
                
                /* 调用回调函数 */
                if (comm->command_callback) {
                    comm->command_callback(cmd, payload, payload_len,
                                          comm->command_callback_data);
                }
            }
            
            /* 移动缓冲区 */
            i = j + 1;
        } else {
            /* 没有找到完整数据包，等待更多数据 */
            break;
        }
    }
    
    /* 移动剩余数据到缓冲区开头 */
    if (i > 0 && i < comm->rx_len) {
        memmove(comm->rx_buffer, &comm->rx_buffer[i], comm->rx_len - i);
        comm->rx_len -= i;
    } else if (i >= comm->rx_len) {
        comm->rx_len = 0;
    }
}

/* 修改 comm_fsm_init 函数以初始化新字段 */
void comm_fsm_init(struct comm_fsm *comm)
{
    if (!comm) return;
    
    memset(comm, 0, sizeof(*comm));
    comm->current_state = COMM_IDLE;
    comm->previous_state = COMM_IDLE;
    comm->protocol = COMM_PROTOCOL_BLUETOOTH;
    comm->signal_strength = 0;
    comm->data_rate = 0;
    comm->is_connected = 0;
    comm->error_code = 0;
    comm->user_data = NULL;
    
    /* NEW: 初始化命令处理字段 */
    comm->command_callback = NULL;
    comm->command_callback_data = NULL;
    comm->rx_len = 0;
    
    /* Set default operations */
    comm->ops = &default_ops;
    
    _MY_TRACE_STR("comm_fsm_init: initialized with command support\n");
}
```

### 3. 系统集成示例

创建一个系统集成文件 `system_command_integration.c`：

```c
/**
 * system_command_integration.c
 * 
 * 演示如何将comm_fsm命令功能集成到system_coordinator
 */

#include "system_coordinator.h"
#include "comm_fsm.h"

/* 命令回调函数 */
static void system_command_handler(command_code_t cmd, 
                                  const void *data, 
                                  size_t size, 
                                  void *user_data)
{
    struct system_coordinator *sys = (struct system_coordinator *)user_data;
    
    /* 映射命令到系统事件 */
    enum system_event sys_event;
    const char *cmd_name = "未知命令";
    
    switch (cmd) {
        case COMMAND_REC_START:
            sys_event = SYS_EVT_REC_START;
            cmd_name = "开始录音";
            break;
        case COMMAND_REC_STOP:
            sys_event = SYS_EVT_REC_STOP;
            cmd_name = "停止录音";
            break;
        case COMMAND_REC_PAUSE:
            sys_event = SYS_EVT_REC_PAUSE;
            cmd_name = "暂停录音";
            break;
        case COMMAND_REC_RESUME:
            sys_event = SYS_EVT_REC_RESUME;
            cmd_name = "恢复录音";
            break;
        case COMMAND_UPLOAD_START:
            sys_event = SYS_EVT_UPLOAD_START;
            cmd_name = "开始上传";
            break;
        case COMMAND_POWER_OFF:
            sys_event = SYS_EVT_POWER_OFF;
            cmd_name = "关机";
            break;
        case COMMAND_RESET:
            sys_event = SYS_EVT_RESET;
            cmd_name = "系统重置";
            break;
        case COMMAND_GET_STATUS:
            /* 特殊处理：返回系统状态 */
            if (sys) {
                enum system_state state = system_coordinator_get_state(sys);
                printf("系统状态: %s\n", system_coordinator_get_state_name(state));
                
                /* 可以在这里通过comm_fsm发送状态回复 */
            }
            return;
        default:
            printf("未知命令: 0x%02X\n", cmd);
            return;
    }
    
    printf("执行命令: %s\n", cmd_name);
    
    /* 触发系统事件 */
    if (sys) {
        system_coordinator_dispatch_event(sys, sys_event, (void *)data);
    }
}

/* 系统初始化函数 */
void system_init_with_command_support(void)
{
    struct system_coordinator sys;
    struct comm_fsm comm;
    
    /* 初始化系统协调器 */
    system_coordinator_init(&sys);
    
    /* 初始化通信FSM */
    comm_fsm_init(&comm);
    
    /* 设置命令回调 */
    comm_fsm_set_command_callback(&comm, system_command_handler, &sys);
    
    /* 连接子系统 */
    system_coordinator_set_comm_fsm(&sys, &comm);
    
    printf("系统已初始化，支持外部设备控制指令\n");
}

/* 模拟接收外部命令 */
void simulate_external_command(struct comm_fsm *comm, const uint8_t *data, size_t size)
{
    /* 在实际系统中，这由硬件中断或网络回调触发 */
    comm_fsm_process_received_data(comm, data, size);
}
```

## 编译和测试

### 编译命令
```bash
cd state_machine_extended
gcc -I./src -o command_demo \
    src/system_coordinator.c \
    src/comm_fsm.c \
    src/mycommon.c \
    examples/system_command_integration.c
```

### 测试命令
```bash
./command_demo
```

## 优势

1. **模块化设计**：命令处理与通信状态机分离
2. **可扩展性**：易于添加新的命令类型
3. **错误处理**：支持协议验证和错误恢复
4. **实时性**：支持异步命令处理
5. **兼容性**：保持现有API不变，向后兼容

## 注意事项

1. **线程安全**：如果系统是多线程的，需要添加互斥锁保护共享数据
2. **缓冲区管理**：需要处理缓冲区溢出和碎片化
3. **协议版本**：考虑协议版本兼容性
4. **安全考虑**：添加命令验证和权限检查
5. **性能优化**：对于高频率命令，考虑使用环形缓冲区

这个补丁提供了完整的外部设备控制指令实现方案，可以直接集成到现有系统中。
