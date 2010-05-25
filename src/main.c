/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) chaitanya chandel 2009 <cchandel@yahoo.com>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Elementary.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include "dbus-stuff.h"
#include "gui.h"
#include "db_sqlite.h"

char home_dir[255], sel_category[255];
Elm_Genlist_Item *task_list[500];
Evas_Object *list, *hs, *pr_hs, *date_hs, *entry, *hs1, *tk, *hv, *pr_bt, *cat_list, *win, *cat_hv_bx, *note_win;
Evas_Object *c_pr_bt, *cat_dialog, *cat_bt, *sel_cat_bt, *date_bt, *det_page, *entry, *new_button, *note_entry;
int total_tasks, WRITE=0, last_rec= -1;

//TODO : dynamic memory management
 _Task Task[150];

static void
my_win_del(void *data, Evas_Object *obj, void *event_info)
{
	save_state();
	release_cpu();
	elm_exit();
}

EAPI int
elm_main(int argc, char **argv)
{
	//const char *theme;
	
	//int ret;

	//get users home dir
	const char *name = "HOME";
	sprintf(home_dir, "%s", getenv(name));

	//adjust finger size
	elm_finger_size_set(55);

	//paroli theme fix - not required anymore
	//theme = "tasks";
	//elm_theme_overlay_add(theme);
		
	//set up win
	win = elm_win_add(NULL, "tasks", ELM_WIN_BASIC);
	elm_win_title_set(win, "Tasks");
	evas_object_smart_callback_add(win, "delete-request", my_win_del, NULL);

	//open database 
	open_database();
	
	// show the window
	create_gui(win);
	evas_object_show(win);

	//restore state
	restore_state();

	occupy_cpu();

	elm_run();
	//clean up stuff
	//clean_up();
	ecore_main_loop_quit();
	elm_shutdown();
	
	return 0;
}
ELM_MAIN()
