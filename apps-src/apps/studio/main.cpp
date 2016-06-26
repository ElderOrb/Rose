#define GETTEXT_DOMAIN "studio-lib"

#include "base_instance.hpp"
#include "preferences_display.hpp"
#include "gui/dialogs/message.hpp"
#include "gui/dialogs/chat.hpp"
#include "gui/dialogs/rose.hpp"
#include "gui/dialogs/combo_box.hpp"
#include "gui/dialogs/tools.hpp"
#include "gui/dialogs/anim.hpp"
#include "gui/dialogs/opencv.hpp"
#include "gui/widgets/window.hpp"
#include "game_end_exceptions.hpp"
#include "wml_exception.hpp"
#include "gettext.hpp"
#include "hotkeys.hpp"
#include "formula_string_utils.hpp"
#include "version.hpp"
#include "mkwin_controller.hpp"
#include "help.hpp"

#include <errno.h>
#include <iostream>

class game_instance: public base_instance
{
public:
	game_instance(rtc::PhysicalSocketServer& ss, int argc, char** argv);

	void app_fill_anim_tags(std::map<const std::string, int>& tags) override;

	void start_mkwin(const std::map<std::string, std::string>& app_tdomains);
	void edit_animation();

private:
	void app_load_settings_config(const config& cfg) override;
};

game_instance::game_instance(rtc::PhysicalSocketServer& ss, int argc, char** argv)
	: base_instance(ss, argc, argv)
{
	teditor_ editor_(game_config::path);
	editor_.make_system_bins_exist();
}

void game_instance::app_fill_anim_tags(std::map<const std::string, int>& tags)
{
	// although don't use below animation, but pass program verify, define them still.
}

void game_instance::start_mkwin(const std::map<std::string, std::string>& app_tdomains)
{
	hotkey::scope_changer changer(core_cfg(), "hotkey_mkwin");

	mkwin_controller mkwin(core_cfg(), video_, app_tdomains);
	mkwin.initialize(64 * gui2::twidget::hdpi_scale);
	mkwin.main_loop();
}

void game_instance::edit_animation()
{
	gui2::tanim3 dlg(app_cfg_);
	dlg.show();
}

void game_instance::app_load_settings_config(const config& cfg)
{
	game_config::logo_png = cfg["log_png"].str();
	game_config::version = game_config::rose_version;
	game_config::wesnoth_version = version_info(game_config::version);

	game_config::absolute_path = game_config::path + "/absolute";
	game_config::apps_src_path = extract_directory(game_config::path) + "/apps-src";
	if (!check_apps_src_folder(game_config::apps_src_path)) {
		game_config::apps_src_path.clear();
	}

	preferences::set_theme(utils::generate_app_prefix_id("rose", "default"));
}

/**
 * Setups the game environment and enters
 * the titlescreen or game loops.
 */
static int do_gameloop(int argc, char** argv)
{
	rtc::PhysicalSocketServer ss;
	instance_manager<game_instance> manager(ss, argc, argv, "studio", "#rose", true);
	game_instance& game = manager.get();

	try {
		// default name maybe all digit, not safied to irc nick protocol.
		lobby->set_nick2("studio");

		std::map<std::string, std::string> app_tdomains;
		for (;;) {
			game.loadscreen_manager().reset();

			gui2::trose::tresult res = gui2::trose::NOTHING;

			const font::floating_label_context label_manager;

			cursor::set(cursor::NORMAL);

			if (res == gui2::trose::NOTHING) {
				// load/reload hero_map from file
				gui2::trose dlg(group.leader());
				dlg.show();
				res = static_cast<gui2::trose::tresult>(dlg.get_retval());
				app_tdomains = dlg.get_app_tdomains();
			}

			if (res == gui2::trose::QUIT_GAME) {
				SDL_Log("do_gameloop, received QUIT_GAME, will exit!\n");
				return 0;

			} else if (res == gui2::trose::WINDOW) {
				game.start_mkwin(app_tdomains);

			} else if (res == gui2::trose::ANIMATION) {
				game.edit_animation();

			} else if (res == gui2::trose::DESIGN) {
				gui2::ttools dlg;
				dlg.show();

			} else if (res == gui2::trose::CHANGE_LANGUAGE) {
				if (game.change_language()) {
					t_string::reset_translations();
					image::flush_cache();
				}

			} else if (res == gui2::trose::OPENCV) {
				gui2::topencv dlg;
				dlg.show();
			
			} else if (res == gui2::trose::MESSAGE) {
				gui2::tchat2 dlg("chat_module");
				dlg.show();
			
			} else if (res == gui2::trose::EDIT_PREFERENCES) {
				preferences::show_preferences_dialog(game.video());

			}
		}

	} catch (twml_exception& e) {
		e.show();

	} catch (CVideo::quit&) {
		//just means the game should quit
		SDL_Log("SDL_main, catched CVideo::quit\n");

	} catch (game_logic::formula_error& e) {
		gui2::show_error_message(e.what());
	} 

	return 0;
}

int main(int argc, char** argv)
{
	try {
		do_gameloop(argc, argv);
	} catch (twml_exception& e) {
		// this exception is generated when create instance.
		SDL_SimplerMB("%s\n", e.user_message.c_str());
	}

	return 0;
}
