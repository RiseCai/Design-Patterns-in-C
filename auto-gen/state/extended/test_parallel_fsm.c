#include <stdio.h>
#include <stdlib.h>
#include "parallel_fsm.h"

int main()
{
    struct parallel_machine pm;
    parallel_machine_init(&pm, 3);

    enum comp_a_state a_state = A_IDLE;
    enum comp_b_state b_state = B_IDLE;
    enum comp_c_state c_state = C_OFF;

    struct parallel_component *compA = parallel_component_create(1, &a_state,
            component_a_step, component_a_entry, component_a_do, component_a_exit);
    struct parallel_component *compB = parallel_component_create(2, &b_state,
            component_b_step, component_b_entry, component_b_do, component_b_exit);
    struct parallel_component *compC = parallel_component_create(3, &c_state,
            component_c_step, component_c_entry, component_c_do, component_c_exit);

    parallel_machine_add_component(&pm, compA);
    parallel_machine_add_component(&pm, compB);
    parallel_machine_add_component(&pm, compC);

    printf("=== Starting Parallel FSM Test ===\n");

    /* Simulate events */
    printf("\n1. Broadcast EVENT_START\n");
    parallel_machine_broadcast_event(&pm, EVENT_START);

    printf("\n2. Broadcast EVENT_DATA_READY\n");
    parallel_machine_broadcast_event(&pm, EVENT_DATA_READY);

    printf("\n3. Broadcast EVENT_ENABLE\n");
    parallel_machine_broadcast_event(&pm, EVENT_ENABLE);

    printf("\n4. Broadcast EVENT_FINISH\n");
    parallel_machine_broadcast_event(&pm, EVENT_FINISH);

    printf("\n5. Broadcast EVENT_STOP\n");
    parallel_machine_broadcast_event(&pm, EVENT_STOP);

    printf("\n6. Broadcast EVENT_DISABLE\n");
    parallel_machine_broadcast_event(&pm, EVENT_DISABLE);

    printf("\n7. Broadcast EVENT_RESET\n");
    parallel_machine_broadcast_event(&pm, EVENT_RESET);

    printf("\n8. Sync\n");
    parallel_machine_sync(&pm);

    printf("\n=== Test Complete ===\n");

    free(compA);
    free(compB);
    free(compC);
    free(pm.components);
    return 0;
}
