#include <stdio.h>
#include <stdlib.h>
#include "efsm_protocol.h"

int main()
{
    struct efsm_processor proc;
    efsm_processor_init(&proc);

    printf("=== EFSM Protocol Test ===\n");

    printf("\n1. Initial state: IDLE\n");
    efsm_processor_tick(&proc);

    printf("\n2. Dispatch packet (valid)\n");
    char packet[10] = {0};
    efsm_processor_dispatch_packet(&proc, packet);

    printf("\n3. Tick (processing)\n");
    efsm_processor_tick(&proc);

    printf("\n4. Packet complete\n");
    efsm_processor_packet_complete(&proc);

    printf("\n5. Tick (waiting for ack)\n");
    efsm_processor_tick(&proc);

    printf("\n6. ACK received\n");
    efsm_processor_ack_received(&proc);

    printf("\n7. Tick (back to idle)\n");
    efsm_processor_tick(&proc);

    printf("\n8. Dispatch invalid packet\n");
    efsm_processor_dispatch_packet(&proc, NULL);
    efsm_processor_invalid(&proc);

    printf("\n9. Tick (error handling)\n");
    efsm_processor_tick(&proc);

    printf("\n=== Test Complete ===\n");
    return 0;
}
