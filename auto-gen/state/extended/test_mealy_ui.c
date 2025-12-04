#include <stdio.h>
#include <stdlib.h>
#include "mealy_ui.h"
#include "../state/mealy_machine.h"

static void render_current_state(struct mealy_machine *m) {
    struct mealy_state *state = mealy_machine_get_state(m);
    if (state && state->ops && state->ops->render) {
        state->ops->render(state, m);
    }
}

int main()
{
    struct mealy_machine machine;
    /* start with idle state */
    struct mealy_state_idle *idle = malloc(sizeof(struct mealy_state_idle));
    mealy_state_idle_init(idle);
    mealy_machine_init(&machine, (struct mealy_state *)idle);

    printf("=== Mealy UI State Machine Test ===\n");

    printf("\n1. Initial state: idle\n");
    render_current_state(&machine);

    printf("\n2. Hover event\n");
    mealy_machine_event_hover(&machine);

    printf("\n3. Click event (while hovered)\n");
    mealy_machine_event_click(&machine);

    printf("\n4. Click event (while pressed) -> back to idle\n");
    mealy_machine_event_click(&machine);

    printf("\n5. Keypress event (ignored)\n");
    mealy_machine_event_keypress(&machine, 'A');

    printf("\n6. Hover again\n");
    mealy_machine_event_hover(&machine);

    printf("\n7. Render final state\n");
    render_current_state(&machine);

    printf("\n=== Test Complete ===\n");

    /* cleanup */
    free(idle);
    /* Note: machine does not own the state, we allocated idle separately */
    return 0;
}
