
#include "my-filter.h"

#include <memory>


struct my_filter : public filter_data, public std::enable_shared_from_this<my_filter> {



	~my_filter() { obs_log(LOG_INFO, "My filter destructor called"); }
};


const char* my_filter_get_name(void* unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("MyFilter");
}

void* my_filter_create(obs_data_t* settings, obs_source_t* source)
{

}

void my_filter_destroy(void* data)
{

}

void my_filter_get_defaults(obs_data_t* settings)
{

}

obs_properties_t* my_filter_get_properties(void* data)
{

}

void my_filter_update(void* data, obs_data_t* settings)
{

}

void my_filter_activate(void* data)
{

}

void my_filter_deactivate(void* data)
{

}

void my_filter_video_tick(void* data, float seconds)
{

}

void my_filter_video_render(void* data, gs_effect_t* _effect)
{

}