#include "my-filter.h"

struct obs_source_info my_filter_info = {
	.id = "my_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = my_filter_get_name,
	.create = my_filter_create,
	.destroy = my_filter_destroy,
	.get_defaults = my_filter_get_defaults,
	.get_properties = my_filter_get_properties,
	.update = my_filter_update,
	.activate = my_filter_activate,
	.deactivate = my_filter_deactivate,
	.video_tick = my_filter_video_tick,
	.video_render = my_filter_video_render,
};