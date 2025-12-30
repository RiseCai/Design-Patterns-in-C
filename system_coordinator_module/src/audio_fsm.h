/**
 * audio_fsm.h - Audio FSM interface
 */
#ifndef __AUDIO_FSM_H__
#define __AUDIO_FSM_H__

#include "mycommon.h"

/* Audio states */
enum audio_state {
    AUDIO_OFF,
    AUDIO_IDLE,
    AUDIO_PLAYING,
    AUDIO_PAUSED,
    AUDIO_RECORDING,
    AUDIO_ERROR
};

/* Audio events */
enum audio_event {
    AUDIO_EVT_PLAY,
    AUDIO_EVT_PAUSE,
    AUDIO_EVT_STOP,
    AUDIO_EVT_RECORD_START,
    AUDIO_EVT_RECORD_STOP,
    AUDIO_EVT_VOLUME_UP,
    AUDIO_EVT_VOLUME_DOWN,
    AUDIO_EVT_MUTE,
    AUDIO_EVT_UNMUTE,
    AUDIO_EVT_ERROR
};

/* Audio FSM context */
struct audio_fsm {
    enum audio_state current_state;
    enum audio_state previous_state;
    void *user_data;
};

/* API functions */
void audio_fsm_init(struct audio_fsm *fsm);
void audio_fsm_destroy(struct audio_fsm *fsm);
void audio_fsm_dispatch_event(struct audio_fsm *fsm, enum audio_event event, void *data);
enum audio_state audio_fsm_get_state(struct audio_fsm *fsm);
const char *audio_fsm_get_state_name(enum audio_state state);
const char *audio_fsm_get_event_name(enum audio_event event);

/* Additional API functions for system coordinator */
void audio_fsm_start_recording(struct audio_fsm *fsm);
void audio_fsm_sleep(struct audio_fsm *fsm);
void audio_fsm_reset(struct audio_fsm *fsm);

#endif /* __AUDIO_FSM_H__ */
