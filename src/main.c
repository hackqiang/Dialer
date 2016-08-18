/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>

#include "dialer.h"
#include "view.h"
#include "data.h"

static void _btn_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _btn_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

/*
 * @brief Hook to take necessary actions before main event loop starts
 * @param[in] user_data The user data to be passed to the callback function
 * Initialize UI resources and application's data
 * If this function returns true, the main loop of application starts
 * If this function returns false, the application is terminated
 */
static bool app_create(void *user_data)
{
	/* Hook to take necessary actions before main event loop starts
	   Initialize UI resources and application's data
	   If this function returns true, the main loop of application starts
	   If this function returns false, the application is terminated */

	char *image = NULL;
	char full_path[PATH_MAX] = { 0, };

	/* Create main view */
	view_create();
	data_get_full_path(EDJ_FILE, full_path, (int)PATH_MAX);
	view_dialer_create(full_path);

	/* Set background image to "sw.button.bg" part of EDC */
	image = data_get_image_path("sw.button.bg");
	view_set_image(view_dialer_get_layout_object(), "sw.button.bg", image);
	view_set_color(view_dialer_get_layout_object(), "sw.button.bg", 8, 36, 61, 255);
	free(image);

	/* Set Call button effect image to "sw.button.call.ef" part of EDC */
	image = data_get_image_path("sw.button.call.ef");
	view_set_image(view_dialer_get_layout_object(), "sw.button.call.ef", image);
	view_set_color(view_dialer_get_layout_object(), "sw.button.call.ef", 0, 0, 0, 255);
	free(image);

	/* Set Call button image to "sw.button.call" part of EDC */
	image = data_get_image_path("sw.button.call");
	view_set_image(view_dialer_get_layout_object(), "sw.button.call", image);
	view_set_color(view_dialer_get_layout_object(), "sw.button.call", 0, 214, 46, 255);
	free(image);

	/* Set Delete button to "sw.button.delete" part of EDC */
	image = data_get_image_path("sw.button.delete");
	view_set_button(view_dialer_get_layout_object(), "sw.button.delete", "focus", image, NULL, _btn_down_cb, _btn_up_cb, NULL, NULL);
	view_set_color(view_dialer_get_layout_object(), "sw.button.delete", 250, 250, 250, 255);
	free(image);

	/* Set Entry widget to "sw.entry.dial" part of EDC to display input dial number */
	view_dialer_set_entry("sw.entry.dial");
	view_set_color(view_dialer_get_layout_object(), "sw.entry.dial", 250, 250, 250, 255);

	/* Set full size Rectangle to catch circular dial button touch */
	view_dialer_create_rectangle();

	return true;
}

/*
 * @brief This callback function is called when another application
 * @param[in] app_control The handle to the app_control
 * @param[in] user_data The user data to be passed to the callback function
 * sends the launch request to the application
 */
static void app_control(app_control_h app_control, void *user_data)
{
	/* Handle the launch request. */
}

/*
 * @brief This callback function is called each time
 * @param[in] user_data The user data to be passed to the callback function
 * the application is completely obscured by another application
 * and becomes invisible to the user
 */
static void app_pause(void *user_data)
{
	/* Take necessary actions when application becomes invisible. */
}

/*
 * @brief This callback function is called each time
 * @param[in] user_data The user data to be passed to the callback function
 * the application becomes visible to the user
 */
static void app_resume(void *user_data)
{
	/* Take necessary actions when application becomes visible. */
}

/*
 * @brief This callback function is called once after the main loop of the application exits
 * @param[in] user_data The user data to be passed to the callback function
 */
static void app_terminate(void *user_data)
{
	/*
	 * Destroy window component.
	 */
	view_destroy();
}

/*
 * @brief This function will be called when the language is changed
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);

	if (locale != NULL) {
		elm_language_set(locale);
		free(locale);
	}
	return;
}

/*
 * @brief Main function of the application
 * @param[in] argc The argument count
 * @param[in] argv The argument vector
 */
int main(int argc, char *argv[])
{
	int ret;

	ui_app_lifecycle_callback_s event_callback = {0, };
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	/*
	 * If you want to handling more events,
	 * Please check the application lifecycle guide
	 */
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, NULL);

	ret = ui_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
	}

	return ret;
}


/*
 * @brief Function will be operated when registered event is triggered.
 * @param[in] data The data to be passed to the callback function
 * @param[in] e The handle to an Evas canvas to be passed to the callback function
 * @param[in] obj The Evas object handle to be passed to the callback function
 * @param[in] event_info The system event information
 */
static void _btn_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "button is pressed.");

	/* Delete One Character of Entry */
	view_dialer_set_entry_text(ENTRY_TEXT_BACKSPACE, NULL);

	/* Change Delete button color */
	evas_object_color_set(obj, 250, 250, 250, 102);
}

/*
 * @brief Function will be operated when registered event is triggered.
 * @param[in] data The data to be passed to the callback function
 * @param[in] e The handle to an Evas canvas to be passed to the callback function
 * @param[in] obj The Evas object handle to be passed to the callback function
 * @param[in] event_info The system event information
 */
static void _btn_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "button is released.");

	/* Change Delete button color */
	evas_object_color_set(obj, 250, 250, 250, 255);
}
