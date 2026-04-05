#include <obs-module.h>

#ifndef __cplusplus
extern "C" {
#endif

const char *my_filter_get_name(void *unused);
void *my_filter_create(obs_data_t *settings, obs_source_t *source);
void my_filter_destroy(void *data);
void my_filter_get_defaults(obs_data_t *settings);
obs_properties_t *my_filter_get_properties(void *data);
void my_filter_update(void *data, obs_data_t *settings);
void my_filter_activate(void *data);
void my_filter_deactivate(void *data);
void my_filter_video_tick(void *data, float seconds);
void my_filter_video_render(void *data, gs_effect_t *_effect);

#ifdef __cplusplus
}
#endif