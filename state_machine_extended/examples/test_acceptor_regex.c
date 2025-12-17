#include <stdio.h>
#include <stdlib.h>
#include "acceptor_regex.h"

/* Define states */
static struct acceptor_state s0, s1, s2, s3;

static void build_machine(void) {
    /* State 0: start, non-accepting */
    s0.id = 0;
    s0.is_accepting = 0;
    s0.transition_count = 2;
    s0.transitions[0].input = 'a';
    s0.transitions[0].next = &s1;
    s0.transitions[1].input = 'b';
    s0.transitions[1].next = &s2;

    /* State 1: after 'a', accepting */
    s1.id = 1;
    s1.is_accepting = 1;
    s1.transition_count = 1;
    s1.transitions[0].input = 'c';
    s1.transitions[0].next = &s3;

    /* State 2: after 'b', non-accepting */
    s2.id = 2;
    s2.is_accepting = 0;
    s2.transition_count = 1;
    s2.transitions[0].input = 'c';
    s2.transitions[0].next = &s3;

    /* State 3: after 'c', accepting */
    s3.id = 3;
    s3.is_accepting = 1;
    s3.transition_count = 0;
}

int main() {
    build_machine();
    struct acceptor_machine am;
    acceptor_machine_init(&am, &s0);

    printf("=== Acceptor FSM (Regex) Test ===\n");
    printf("Pattern: (a|b)c? (simplified)\n");

    const char *test_strings[] = {
        "a",
        "ac",
        "b",
        "bc",
        "abc",
        "x",
        ""
    };
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);

    for (int i = 0; i < num_tests; i++) {
        const char *str = test_strings[i];
        acceptor_machine_init(&am, &s0);
        printf("\nTest '%s': ", str);
        int accept = 1;
        for (const char *p = str; *p; p++) {
            if (!acceptor_machine_feed(&am, *p)) {
                accept = 0;
                break;
            }
        }
        if (accept && acceptor_machine_is_accepting(&am)) {
            printf("ACCEPT\n");
        } else {
            printf("REJECT\n");
        }
    }

    printf("\n=== Test Complete ===\n");
    return 0;
}
