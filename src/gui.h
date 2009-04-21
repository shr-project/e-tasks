/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <Elementary.h>
#include <string.h>

void create_gui(Evas_Object *win);
void new_button_clicked(void *data, Evas_Object *obj, void *event_info);
void create_new_task(void *data, Evas_Object *obj, void *event_info);
void select_category(void *data, Evas_Object *obj, void *event_info);
void set_category(void *data, Evas_Object *obj, void *event_info);
void edit_cat(void);
void cat_win_del(void *data, Evas_Object *obj, void *event_info);

extern char home_dir[255], sel_category[255];
extern Evas_Object *list, *hs, *pr_hs, *date_hs, *entry, *hs1, *tk, *hv, *pr_bt, *cat_list, *cat_dialog, *cat_bt, *sel_cat_bt, *date_bt;
extern int total_tasks, WRITE, last_rec;
