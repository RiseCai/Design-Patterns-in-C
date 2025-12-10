/**
 * audio_fsm.c - Stub implementation for Audio FSM
 */
#include "audio_fsm.h"

void audio_fsm_init(struct audio_fsm *fsm) {
    (void)fsm;
}

void audio_fsm_destroy(struct audio_fsm *fsm) {
    (void)fsm;
}

void audio_fsm_dispatch_event(struct audio_fsm *fsm, enum audio_event event, void *data) {
    (void)fsm; (void)event; (void)data;
}

enum audio_state audio_fsm_get_state(struct audio_fsm *fsm) {
    (void)fsm;
    return AUDIO_OFF;
}

const char *audio_fsm_get_state_name(enum audio_state state) {
    (void)state;
    return "AUDIO_OFF";
}
