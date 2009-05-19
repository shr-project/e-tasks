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
 
#include <sqlite3.h>
#include <stdio.h>
#include <Elementary.h>

void add_hs_items(Evas_Object *win, Evas_Object *bx, Evas_Object *bt, int i);
void open_database(void);
extern void select_category(void *data, Evas_Object *obj, void *event_info);
extern void set_category(void *data, Evas_Object *obj, void *event_info);
void show_cat_tasks(char *ca);
void update_record(int rec_no);
void insert_record(int i);
void populate_cat_list(Evas_Object *li);
void del_category(char * cat);
void add_category(char * cat);
void del_record(int i);
void save_state(void);
void load_data(void);
void purge_tasks(void);

sqlite3 *tasks;
extern Evas_Object *list, *hs, *hs1, *cat_list, *sel_cat_bt;
void restore_state(void);
extern char home_dir[255], sel_category[255];
extern Elm_Genlist_Item_Class itc1, itc2;
extern Elm_Genlist_Item *task_list[500];
extern int total_tasks;
typedef struct{
	int no;
	int key;
	int cb;
	int pr;
	char text[255];
	//char alarm[255];
	//int repeat;
	char date[11];
	char cat[255];
}  _Task;
extern _Task Task[150];
