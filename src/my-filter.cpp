// SPDX-FileCopyrightText: 2021-2026 Roy Shilkrot <roy.shil@gmail.com>
// SPDX-FileCopyrightText: 2023-2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: GPL-3.0-or-later

// I gotta figure out where to include the copyright headers above. ^^^

#include "my-filter.h"

//#include <onnxruntime_cxx_api.h>

//#ifdef _WIN32
//#include <wchar.h>
//#endif // _WIN32

// ^^^^^ do i need this????

//#include <opencv2/imgproc.hpp>

#include <memory>

#include <plugin-support.h>
//#include "ort-utils/ort-sessino-utils.hpp"
//#include "ort-utils/obs-utils.hpp"


struct my_filter : public filter_data, public std::enable_shared_from_this<my_filter> {
	//bool enableThreshold = true;
	bool stopWhenSourceIsInactive = true;
	//float threshold = 0.5f;
	//cv::Scalar backgroundColor{0, 0, 0, 0};
	//float contourFilter = 0.05f;
	//float smoothContour = 0.5f;
	//float feather = 0.0f;
	//int maskExpansion = 0;

	//cv::Mat backgroundMask;
	//cv::Mat lastBackgroundMask;
	//cv::Mat lastImageBGRA;
	//float temporalSmoothFactor = 0.0f;
	//float imageSimilarityThreshold = 35.0f;
	//bool enableImageSimilarity = true;
	//int maskEveryXFrames = 1;
	//int maskEveryXFramesCount = 0;
	//int64_t blurBackground = 0;
	//bool enableFocalBlur = false;
	//float blurFocusPoint = 0.1f;
	//float blurFocusDepth = 0.1f;

	gs_effect_t *effect;
	//gs_effect_t *kawaseBlurEffect;

	std::mutex modelMutex;

	~my_filter() { obs_log(LOG_INFO, "My filter destructor called"); }
};

void my_filter_thread(void *data); // Forward declaration

const char* my_filter_get_name(void* unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("MyFilter");
}

/* ------ ------ PROPERTIES ------ ------ */

static bool visible_on_bool(obs_properties_t* ppts, obs_data_t* settings, const char* bool_prop, const char* prop_name)
{
	const bool enabled = obs_data_get_bool(settings, bool_prop);
	obs_property_t *p = obs_properties_get(ppts, prop_name);
	obs_property_set_visible(p, enabled);
	return true;
}

static bool enable_advanced_settings(obs_properties_t *ppts, obs_property_t *p, obs_data_t *settings)
{
	//const bool enabled = obs_data_get_bool(settings, "advanced");
	//p = obs_properties_get(ppts, "blur_background");
	//obs_property_set_visible(p, true);

	//for (const char *prop_name :
	//     {"model_select", "useGPU", "mask_every_x_frames", "numThreads", "enable_focal_blur", "enable_threshold",
	//      "threshold_group", "focal_blur_group", "temporal_smooth_factor", "image_similarity_threshold",
	//      "enable_image_similarity", "mask_expansion"}) {
	//	p = obs_properties_get(ppts, prop_name);
	//	obs_property_set_visible(p, enabled);
	//}

	//if (enabled) {
	//	enable_threshold_modified(ppts, p, settings);
	//	enable_focal_blur(ppts, p, settings);
	//	enable_image_similarity(ppts, p, settings);
	//}

	return true;
}

obs_properties_t *my_filter_get_properties(void *data) 
{
	obs_properties *props = obs_properties_create();

	obs_property_t *advanced = obs_properties_add_bool(props, "advanced", obs_module_text("Advanced"));

	obs_properties_add_bool(props, "stop_when_source_is_inactive",
				obs_module_text("Stop filter when source is inactive"));

	// If advanced is selected show the advanced settings, otherwise hide them
	obs_property_set_modified_callback(advanced, enable_advanced_settings);



	// filter properties stuffs goes here :)



	UNUSED_PARAMETER(data);
	return props;
}

void my_filter_get_defaults(obs_data_t *settings)
{
	obs_data_set_default_bool(settings, "stop_when_source_is_inactive", true);
}

void my_filter_update(void *data, obs_data_t *settings)
{
	obs_log(LOG_INFO, "My filter udpated");

	// Cast to shared_ptr pointer and create a local shared_ptr
	auto *ptr = static_cast<std::shared_ptr<my_filter> *>(data);
	if (!ptr) {
		return;
	}

	std::shared_ptr<my_filter> tf = *ptr;
	if (!tf) {
		return;
	}

	tf->isDisabled = true;

	tf->stopWhenSourceIsInactive = obs_data_get_bool(settings, "stop_when_source_is_inactive");
	
	// TODO: other settings updates here...

	const std::string newUseGpu = obs_data_get_string(settings, "useGPU");
	const std::string newModel = obs_data_get_string(settings, "model_select");
	const uint32_t newNumThreads = (uint32_t)obs_data_get_int(settings, "numThreads");

	// TODO: model / gpu / thread updates here...

	obs_enter_graphics();

	char *effect_path = obs_module_file(EFFECT_PATH);
	gs_effect_destroy(tf->effect);
	tf->effect = gs_effect_create_from_file(effect_path, NULL);
	bfree(effect_path);

	obs_leave_graphics();

	// Log the currently selected options
	obs_log(LOG_INFO, "My filter options:");
	// name of the source that the filter is attached to
	obs_log(LOG_INFO, "  Source: %s", obs_source_get_name(tf->source));
	obs_log(LOG_INFO, "  Model: %s", tf->modelSelection.c_str());
	//obs_log(LOG_INFO, "  Inference Device: %s", tf->useGPU.c_str());
	//obs_log(LOG_INFO, "  Num Threads: %d", tf->numThreads);

	obs_log(LOG_INFO, "  Disabled: %s", tf->isDisabled ? "true" : "false");


#ifdef _WIN32
	obs_log(LOG_INFO, "  Model file path: %S", tf->modelFilepath.c_str());
#else
	obs_log(LOG_INFO, "  Model file path: %s", tf->modelFilepath.c_str());
#endif

	// enable
	tf->isDisabled = false;
}

void background_filter_activate(void *data)
{
	auto *ptr = static_cast<std::shared_ptr<my_filter> *>(data);
	if (!ptr) {
		return;
	}

	std::shared_ptr<my_filter> tf = *ptr;
	if (tf && tf->stopWhenSourceIsInactive) {
		obs_log(LOG_INFO, "My filter activated");
		tf->isDisabled = false;
	}
}

void background_filter_deactivate(void *data)
{
	auto *ptr = static_cast<std::shared_ptr<my_filter> *>(data);
	if (!ptr) {
		return;
	}

	std::shared_ptr<my_filter> tf = *ptr;
	if (tf && tf->stopWhenSourceIsInactive) {
		obs_log(LOG_INFO, "My filter deactivated");
		tf->isDisabled = true;
	}
}

void* my_filter_create(obs_data_t* settings, obs_source_t* source)
{
	obs_log(LOG_INFO, "My filter created");
	try {
		// Create the instance as a shared_ptr
		auto instance = std::make_shared<my_filter>();

		instance->source = source;
		instance->texrender = gs_texrender_create(GS_BGRA, GS_ZS_NONE);

		std::string instanceName{"my-filter-inference"};
		// TODO: add ort
		//instance->env.reset(new Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_ERROR, instanceName.c_str()));

		//instance->modelSelection = MODEL_MEDIAPIPE;
		instance->modelSelection = nullptr;

		// Create a pointer to shared_ptr for the update call
		auto ptr = new std::shared_ptr<my_filter>(instance);
		my_filter_update(ptr, settings);

		// Create a pointer to the shared_ptr
		// This keeps the reference count at least 1 until destroy is called
		return ptr;	
	} catch (const std::exception &e) {
		obs_log(LOG_ERROR, "Failed to create my filter: %s", e.what());
		return nullptr;
	}
}

void my_filter_destroy(void* data)
{
	obs_log(LOG_INFO, "My filter destroyed");

	// Cast back to shared_ptr pointer
	auto *ptr = static_cast<std::shared_ptr<my_filter> *>(data);
	if (ptr) {
		if (*ptr) {
			// Mark as disabled to prevent further processing
			(*ptr)->isDisabled = true;

			// Perform cleanup
			obs_enter_graphics();
			gs_texrender_destroy((*ptr)->texrender);
			if ((*ptr)->stagesurface) {
				gs_stagesurface_destroy((*ptr)->stagesurface);
			}
			gs_effect_destroy((*ptr)->effect);
			gs_effect_destroy((*ptr)->kawaseBlurEffect);
			obs_leave_graphics();
		}
		// Delete the pointer to shared_ptr
		// This decrements the ref count. If no other threads hold a shared_ptr, the instance is deleted
		delete ptr;
	}
}

void my_filter_video_tick(void* data, float seconds)
{

}

void my_filter_video_render(void* data, gs_effect_t* _effect)
{

}