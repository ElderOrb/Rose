/* $Id: mp_login.hpp 48879 2011-03-13 07:49:40Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_EDIT_BOX_HPP_INCLUDED
#define GUI_DIALOGS_EDIT_BOX_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"
#include "serialization/string_utils.hpp"
#include <boost/function.hpp>
#include "gui/widgets/text_box2.hpp"

namespace gui2 {

class ttext_box;
class ttext_box2;
class tbutton;

class tedit_box : public tdialog
{
public:
	struct tparam {
		tparam(const std::string& title, const std::string& remark, const std::string& placeholder, const std::string& def, const std::string& ok = null_str, size_t max_length = -1) 
			: title(title)
			, remark(remark)
			, placeholder(placeholder)
			, def(def)
			, ok(ok)
			, max_length(max_length)
		{}

		std::string title;
		std::string remark;
		std::string placeholder;
		std::string def;
		std::string ok;
		size_t max_length;
		boost::function<bool (const std::string& label) > text_changed;
		std::string result;
	};
	tedit_box(tparam& param);

	void text_changed_callback(ttext_box& widget);

private:

	/** Inherited from tdialog, implemented by REGISTER_DIALOG. */
	virtual const std::string& window_id() const;

	/** Inherited from tdialog. */
	void pre_show() override;

	/** Inherited from tdialog. */
	void post_show() override;

	void create(twindow& window);

private:
	tparam& param_;
	tbutton* ok_;
	std::unique_ptr<ttext_box2> txt_;
};

} // namespace gui2

#endif

