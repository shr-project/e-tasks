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

#include "gui.h"
#include "db_sqlite.h"
#include <time.h>

void select_category(void *data, Evas_Object *obj, void *event_info)
{
	char *ca = (char *)data, *cat;
	cat = strdup(ca);

	evas_object_hide(hs1);
	if(strcmp(cat, "Edit Categories")==0) {
		edit_cat();
		return;
	}
	elm_button_label_set(sel_cat_bt, cat);
	elm_genlist_clear(list);
	last_rec = -1;
	//show only those tasks that meet this category
	show_cat_tasks(cat);
}

 void task_select(void *data, Evas_Object *obj, void *event_info)
{
	char tystr[2];
	char te_data[255], *task_entry;
	Elm_Genlist_Item *item;
	_Task *tsk=NULL;

	//for entries, get entry data and check agianst previous task text, date
	//if changed - write
	if (last_rec != -1) { //cater for initial null data
		sprintf(te_data, "%s", (const char *)elm_entry_entry_get(tk));
		task_entry = strtok(te_data, "<");
		tsk = (_Task *)elm_genlist_item_data_get(task_list[last_rec]);
		if (strcmp(task_entry, tsk->text) != 0) {
			sprintf(tsk->text, "%s", task_entry);
			WRITE = 1;
		}
	}
	
	if (WRITE == 1) {
		elm_genlist_item_update(task_list[last_rec]);
		update_record(tsk->key);
		WRITE = 0;
	}
	
	Evas_Object *li = data;
	item = (Elm_Genlist_Item *)elm_genlist_selected_item_get(li);
	tsk = (_Task *)elm_genlist_item_data_get(item);
	last_rec = tsk->no;
	elm_button_label_set(cat_bt, tsk->cat);
	sprintf(tystr, "%d", tsk->pr);
	elm_button_label_set(pr_bt, tystr);
	elm_button_label_set(date_bt, tsk->date);
	elm_entry_entry_set(tk, tsk->text);
}

void set_priority(void *data, Evas_Object *obj, void *event_info)
{
	char tystr[2];
	int ty;

	//change priority value of selected task
	char *prio = (char *)data;
	Elm_Genlist_Item *item = (Elm_Genlist_Item *)elm_genlist_selected_item_get(list);
	evas_object_hide(hv);
	if (!item) return;
	_Task *tsk = (_Task *)elm_genlist_item_data_get(item);
	ty = atoi(prio);
	if(tsk->pr == ty) return;
	tsk->pr = ty;
	sprintf(tystr, "%d", tsk->pr);
	elm_button_label_set(pr_bt, tystr);
	if (total_tasks == 1) {
		update_record (tsk->key);
		elm_genlist_item_update(item);
	}
	else WRITE = 1;
}

void set_category(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Genlist_Item *item, *new_item;
	//change category value of selected task
	char *category = data;
	item = (Elm_Genlist_Item *)elm_genlist_selected_item_get(list);
	evas_object_hide(hs);
	if (!item) return;
	_Task *tsk = (_Task *)elm_genlist_item_data_get(item);
	if (strcmp(tsk->cat, category) == 0) return;
	sprintf(tsk->cat, "%s", category);
	elm_button_label_set(cat_bt, category);
	if (total_tasks == 1) {
		update_record (tsk->key);
		elm_genlist_item_update(item);
	}
	else WRITE = 1;
	new_item = elm_genlist_item_prev_get(item);
	if(new_item) elm_genlist_item_selected_set(new_item, 1);
	else {
		new_item = elm_genlist_first_item_get(list);
		if(new_item) elm_genlist_item_selected_set(new_item, 1);
	}
	if(strcmp(sel_category, category) != 0) elm_genlist_item_del(item);
}

void set_date(void *data, Evas_Object *obj, void *event_info)
{
	char *dt = (char *)data;
	//set the date
	elm_button_label_set(date_bt, dt);
	evas_object_hide(date_hs);
	Elm_Genlist_Item *item = (Elm_Genlist_Item *)elm_genlist_selected_item_get(list);
	_Task *tsk = (_Task *)elm_genlist_item_data_get(item);
	if (strcmp(tsk->date, dt) == 0) return;
	strcpy(tsk->date, dt);
	if (total_tasks == 1) {
		update_record (tsk->key);
		elm_genlist_item_update(item);
	}
	else WRITE = 1;
}

void task_cb_changed(void *data, Evas_Object *obj, void *event_info)
{
	_Task *tsk = (_Task *)data;
	
	if(tsk->cb) tsk->cb = 0;
	else tsk->cb =1;

	printf("no %d key %d\n", tsk->no, tsk->key);
	//TODO - strike through the task
	//remove this task from the list
	elm_genlist_item_del(task_list[tsk->no]);
	//update the db
	update_record (tsk->no);
	last_rec = -1;
}

void add_dates(Evas_Object *win, Evas_Object *bx, Evas_Object *bt)
{
	int i;
	char dt[6], *tystr;
	time_t curtime, tm;
	struct tm *loctime;
	
	//get the time
	curtime = time (NULL);
	//loctime = localtime (&curtime);
	//strftime(dt, 6, "%d-%m", loctime);
	
	sprintf(dt, "No Date");
	bt = elm_button_add(win);
	elm_button_label_set(bt,dt);
	elm_box_pack_end(bx, bt);
	tystr = strdup(dt);
	evas_object_smart_callback_add(bt, "clicked", set_date, (char *)tystr);
	evas_object_show(bt);
	
	for(i=0; i<7; i++) {
		tm = curtime + (i * 86400);
		loctime = localtime (&tm);
		strftime(dt, 6, "%d-%m", loctime);
		//sprintf(dt, " All Tasks ");
		bt = elm_button_add(win);
		elm_button_label_set(bt,dt);
		elm_box_pack_end(bx, bt);
		tystr = strdup(dt);
		evas_object_smart_callback_add(bt, "clicked", set_date, (char *)tystr);
		evas_object_show(bt);
	}
	
	sprintf(dt, "Calendar");
	bt = elm_button_add(win);
	elm_button_label_set(bt,dt);
	elm_box_pack_end(bx, bt);
	tystr = strdup(dt);
	evas_object_smart_callback_add(bt, "clicked", set_date, (char *)tystr);
	evas_object_show(bt);
}

void create_details_page(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *det_page, *bg, *inwin, *lb, *bt, *bt1, *entry, *vbox, *hbox, *hbox1;

	//check task selected - otherwise return

	
	det_page = elm_win_add(NULL, "inwin", ELM_WIN_BASIC);
	elm_win_title_set(det_page, "Task Details");
	elm_win_autodel_set(det_page, 1);

	bg = elm_bg_add(det_page);
	elm_win_resize_object_add(det_page, bg);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	evas_object_show(bg);

	inwin = elm_win_inwin_add(det_page);
	elm_win_resize_object_add(det_page, inwin);
	elm_win_inwin_style_set(inwin, "default");

	//add vbox 
	vbox = elm_box_add(det_page);
	//elm_win_resize_object_add(det_page, vbox);
	evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
	elm_win_inwin_content_set(inwin, vbox);
	evas_object_show(vbox);

	//add hbox to vbox
	hbox = elm_box_add(det_page);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox);
	evas_object_show(hbox);

	//add a label to hbox
	lb = elm_label_add(det_page);
	elm_label_label_set(lb,"Priority:");
	elm_box_pack_end(hbox, lb);
	evas_object_show(lb);

	//add an entry to hbox
	entry = elm_entry_add(det_page);
	evas_object_size_hint_weight_set(entry, 1.0, 0.0);
	evas_object_size_hint_align_set(entry, -1.0, 0.0);
	elm_entry_single_line_set(entry, 1);
	elm_entry_editable_set(entry, 1);
	elm_entry_line_wrap_set(entry, 0);
	elm_box_pack_end(hbox, entry);
	evas_object_show(entry);

	//add hbox to vbox
	hbox = elm_box_add(det_page);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox);
	evas_object_show(hbox);

	//add a label to hbox
	lb = elm_label_add(det_page);
	elm_label_label_set(lb,"Task:");
	elm_box_pack_end(hbox, lb);
	evas_object_show(lb);

	//add an entry to hbox
	entry = elm_entry_add(det_page);
	evas_object_size_hint_weight_set(entry, 1.0, 0.0);
	evas_object_size_hint_align_set(entry, -1.0, 0.0);
	elm_entry_single_line_set(entry, 1);
	elm_entry_editable_set(entry, 1);
	elm_entry_line_wrap_set(entry, 0);
	elm_box_pack_end(hbox, entry);
	evas_object_show(entry);

	//add hbox to vbox
	hbox = elm_box_add(det_page);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox);
	evas_object_show(hbox);

	//add a label to hbox
	lb = elm_label_add(det_page);
	elm_label_label_set(lb,"Category:");
	elm_box_pack_end(hbox, lb);
	evas_object_show(lb);

	//add an entry to hbox
	entry = elm_entry_add(det_page);
	evas_object_size_hint_weight_set(entry, 1.0, 0.0);
	evas_object_size_hint_align_set(entry, -1.0, 0.0);
	elm_entry_single_line_set(entry, 1);
	elm_entry_editable_set(entry, 1);
	elm_entry_line_wrap_set(entry, 0);
	elm_box_pack_end(hbox, entry);
	evas_object_show(entry);
	
	//add hbox to vbox
	hbox = elm_box_add(det_page);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox);
	evas_object_show(hbox);

	//add a label to hbox
	lb = elm_label_add(det_page);
	elm_label_label_set(lb,"Date:");
	elm_box_pack_end(hbox, lb);
	evas_object_show(lb);

	//add an entry to hbox
	entry = elm_entry_add(det_page);
	evas_object_size_hint_weight_set(entry, 1.0, 0.0);
	evas_object_size_hint_align_set(entry, -1.0, 0.0);
	elm_entry_single_line_set(entry, 1);
	elm_entry_editable_set(entry, 1);
	elm_entry_line_wrap_set(entry, 0);
	elm_box_pack_end(hbox, entry);
	evas_object_show(entry);
	
	//add another hbox
	hbox1 = elm_box_add(det_page);
	elm_box_horizontal_set(hbox1, 1);
	evas_object_size_hint_weight_set(hbox1, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox1, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox1);
	evas_object_show(hbox1);
	
	//add yes button
	bt = elm_button_add(det_page);
	elm_button_label_set(bt, "Done");
	evas_object_size_hint_weight_set(bt, 1.0, 1.0);
	evas_object_size_hint_align_set(bt, -1.0, -1.0);
	elm_box_pack_end(hbox1, bt);
	evas_object_show(bt);
	//evas_object_smart_callback_add(bt, "clicked", det_page_add, entry);
	
	//add yes button
	bt = elm_button_add(det_page);
	elm_button_label_set(bt, "Delete");
	evas_object_size_hint_weight_set(bt, 1.0, 1.0);
	evas_object_size_hint_align_set(bt, -1.0, -1.0);
	elm_box_pack_end(hbox1, bt);
	evas_object_show(bt);
	//evas_object_smart_callback_add(bt, "clicked", det_page_add, entry);

	//add yes button
	bt = elm_button_add(det_page);
	elm_button_label_set(bt, "Note");
	evas_object_size_hint_weight_set(bt, 1.0, 1.0);
	evas_object_size_hint_align_set(bt, -1.0, -1.0);
	elm_box_pack_end(hbox1, bt);
	evas_object_show(bt);
	//evas_object_smart_callback_add(bt, "clicked", det_page_add, entry);
	
	//add close button
	bt1 = elm_button_add(det_page);
	elm_button_label_set(bt1, "Cancel");
	evas_object_size_hint_weight_set(bt1, 1.0, 1.0);
	evas_object_size_hint_align_set(bt1, -1.0, -1.0);
	elm_box_pack_end(hbox1, bt1);
	evas_object_show(bt1);
	evas_object_smart_callback_add(bt1, "clicked", cat_win_del, det_page);

	evas_object_show(inwin);
	
	evas_object_resize(det_page, 480, 640);
	evas_object_show(det_page);
}             

//for genlist
Elm_Genlist_Item_Class itc1;

char *gl_label_get(const void *data, Evas_Object *obj, const char *part)
{
	_Task *tsk = (_Task *)data;
	char tystr[255], *ty;
	
	sprintf(tystr, "%s", tsk->text);
	ty = strdup(tystr);
	return (ty);
}

Evas_Object *gl_icon_get(const void *data, Evas_Object *obj, const char *part)
{
	_Task *tsk = (_Task *)data;
	
	if (!strcmp(part, "elm.swallow.icon")) {
		Evas_Object *ck, *bx, *pr_lb;
		char txt[5];

		//show check box
		bx = elm_box_add(obj);
		elm_box_horizontal_set(bx, 1);
		ck = elm_check_add(obj);
		elm_box_pack_end(bx, ck);
		elm_check_state_set(ck, tsk->cb);
		//evas_object_size_hint_weight_set(ck, 0.0, 0.0);
		//evas_object_size_hint_align_set(ck, 0.5, 0.5);
		evas_object_show(ck);
		evas_object_smart_callback_add(ck, "changed", task_cb_changed, (_Task *)tsk);

		//add a label for priority
		sprintf(txt, "%d", tsk->pr);
		pr_lb = elm_label_add(obj);
		elm_label_label_set(pr_lb, txt);
		elm_box_pack_end(bx, pr_lb);
		//evas_object_size_hint_weight_set(pr_lb, 0.0, 0.0);
		//evas_object_size_hint_align_set(pr_lb, 0.5, 0.5);
		evas_object_show(pr_lb);   
		evas_object_show(bx);
		return bx;
	}
   else if (!strcmp(part, "elm.swallow.end"))
     {
		Evas_Object *ic, *bx, *lb_date;
		 
		bx = elm_box_add(obj);
        elm_box_horizontal_set(bx, 1);
        ic= elm_icon_add(obj);
        elm_box_pack_end(bx, ic);
		evas_object_size_hint_weight_set(ic, 0.0, 0.0);
		evas_object_size_hint_align_set(ic, 0.5, -1.0);
        evas_object_show(ic);

		lb_date = elm_label_add(obj);
		elm_label_label_set(lb_date, tsk->date);
        elm_box_pack_end(bx, lb_date);
		evas_object_size_hint_weight_set(lb_date, 0.0, 0.0);
		evas_object_size_hint_align_set(lb_date, 1.0, 0.5);
		evas_object_show(lb_date);
        evas_object_show(bx);
        return bx;
     }

	return NULL;
}

Evas_Bool gl_state_get(const void *data, Evas_Object *obj, const char *part)
{
   return 0;
}

void gl_del(const void *data, Evas_Object *obj)
{
}

static void
my_hover_bt_1(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *hv = data;
   
   evas_object_show(hv);
}

void create_gui(Evas_Object *win)
{
	int i;
	char no[2], *tystr;
	Evas_Object *bg, *hbox, *new_button, *prop_button, *note_button;
	Evas_Object *vbox, *bx, *hbox1, *bt, *fr, *lb, *fr1;

	//add background
	bg = elm_bg_add(win);
	elm_win_resize_object_add(win, bg);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	evas_object_show(bg);

	//add vbox 4
	vbox = elm_box_add(win);
	elm_win_resize_object_add(win, vbox);
	evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
	evas_object_show(vbox);
	
	//add a frame 
	fr1 = elm_frame_add(win);
	elm_frame_style_set(fr1, "outdent_top");
	elm_box_pack_end(vbox, fr1);
	evas_object_show(fr1);

	//add hbox to fr
	hbox = elm_box_add(win);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, -1.0, -1.0);
	evas_object_size_hint_align_set(hbox, -1.0, -1.0);
	elm_frame_content_set(fr1, hbox);
	evas_object_show(hbox);

	//add a label
	lb = elm_label_add(win);
	elm_label_label_set(lb, "Edit: ");
	elm_box_pack_end(hbox, lb);
	evas_object_show(lb);
	
	//add hover for priority
	hv = elm_hover_add(win);
	//add box for hover
	bx = elm_box_add(win);
	evas_object_size_hint_weight_set(bx, 0.0, 0.0);
	elm_box_pack_end(hbox, bx);
	evas_object_show(bx);

	pr_bt = elm_button_add(win);
	elm_button_label_set(pr_bt, "Pr");
	evas_object_smart_callback_add(pr_bt, "clicked", my_hover_bt_1, hv);
	elm_box_pack_end(bx, pr_bt);
	evas_object_show(pr_bt);
	elm_hover_parent_set(hv, win);
	elm_hover_target_set(hv, pr_bt);

	bt = elm_button_add(win);
	elm_button_label_set(bt, "Pr");
	elm_hover_content_set(hv, "middle", bt);
	evas_object_show(bt);

	bx = elm_box_add(win);
	//add 5 buttons
	for (i=1; i<6; i++) {
		bt = elm_button_add(win);
		sprintf(no, "%d", i);
		elm_button_label_set(bt, no);
		elm_box_pack_end(bx, bt);
		tystr = strdup(no);
		evas_object_smart_callback_add(bt, "clicked", set_priority, (char *)tystr);
		evas_object_show(bt);
	}	
	evas_object_show(bx);
	elm_hover_content_set(hv, "bottom", bx);

/*	//add hover for category
	hs = elm_hover_add(win);
	//add box for hover
	bx = elm_box_add(win);
	evas_object_size_hint_weight_set(bx, 0.0, 0.0);
	elm_box_pack_end(hbox, bx);
	evas_object_show(bx);

	cat_bt = elm_button_add(win);
	elm_button_label_set(cat_bt, "Category");
	evas_object_smart_callback_add(cat_bt, "clicked", my_hover_bt_1, hs);
	elm_box_pack_end(bx, cat_bt);
	evas_object_show(cat_bt);
	elm_hover_parent_set(hs, win);
	elm_hover_target_set(hs, cat_bt);

	bt = elm_button_add(win);
	elm_button_label_set(bt, "Category");
	elm_hover_content_set(hs, "middle", bt);
	evas_object_show(bt);

	bx = elm_box_add(win);
	//add categories
	add_hs_items (win, bx, bt, 0);
	evas_object_show(bx);
	elm_hover_content_set(hs, "bottom", bx); */
	
	//add entry for task
	tk = elm_entry_add(win);
	elm_entry_line_wrap_set(tk, 0);
	elm_entry_entry_set(tk, "Task");
	evas_object_size_hint_weight_set(tk, 0.0, 0.0);
	evas_object_size_hint_align_set(tk, -1.0, -1.0);
	elm_entry_single_line_set(tk ,1);
	elm_box_pack_end(hbox, tk);
	evas_object_show(tk);

	//add hover for date
	date_hs = elm_hover_add(win);
	//add box for hover
	bx = elm_box_add(win);
	evas_object_size_hint_weight_set(bx, 0.0, 0.0);
	elm_box_pack_end(hbox, bx);
	evas_object_show(bx);

	date_bt = elm_button_add(win);
	elm_button_label_set(date_bt, "Date");
	evas_object_smart_callback_add(date_bt, "clicked", my_hover_bt_1, date_hs);
	elm_box_pack_end(bx, date_bt);
	evas_object_show(date_bt);
	elm_hover_parent_set(date_hs, win);
	elm_hover_target_set(date_hs, date_bt);

	bt = elm_button_add(win);
	elm_button_label_set(bt, "Date");
	elm_hover_content_set(date_hs, "middle", bt);
	evas_object_show(bt);

	bx = elm_box_add(win);
	//add categories
	add_dates(win, bx, bt);
	evas_object_show(bx);
	elm_hover_content_set(date_hs, "bottom", bx);
	
/////////////////////////////////////////////////////////////////
	//add list to vbox now
	list = elm_genlist_add(win);
	evas_object_size_hint_weight_set(list, 1.0, 1.0);
	evas_object_size_hint_align_set(list, -1.0, -1.0);
	elm_list_multi_select_set(list, 0);
	elm_box_pack_end(vbox, list);
	elm_genlist_horizontal_mode_set(list, ELM_LIST_LIMIT);
	evas_object_show(list);
	evas_object_smart_callback_add(list, "selected", task_select, list);

	//genlist class defs
	itc1.item_style     		= "default";
	itc1.func.label_get 	= gl_label_get;
	itc1.func.icon_get  	= gl_icon_get;
	itc1.func.state_get 	= gl_state_get;
	itc1.func.del      		= gl_del;

	//add hbox to vbox at pos 4
	hbox1 = elm_box_add(win);
	elm_box_horizontal_set(hbox1, 1);
	evas_object_size_hint_weight_set(hbox1, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox1, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox1);
	elm_box_homogenous_set(hbox1, 1);
	evas_object_show(hbox1);

	//add new button to the hbox
	new_button = elm_button_add(win);
	elm_button_label_set(new_button, "New Task");
	evas_object_size_hint_weight_set(new_button, 1.0, 1.0);
	evas_object_size_hint_align_set(new_button, -1.0, -1.0);
	elm_box_pack_end(hbox1, new_button);
	evas_object_show(new_button);
	evas_object_smart_callback_add(new_button, "clicked", create_new_task, list);

	//add Properties button to the hbox
	prop_button = elm_button_add(win);
	elm_button_label_set(prop_button, "Properties");
	evas_object_size_hint_weight_set(prop_button, 1.0, 1.0);
	evas_object_size_hint_align_set(prop_button, -1.0, -1.0);
	elm_box_pack_end(hbox1, prop_button);
	evas_object_show(prop_button);
	evas_object_smart_callback_add(prop_button, "clicked", create_details_page, NULL);

	//add note button to the hbox
	note_button = elm_button_add(win);
	elm_button_label_set(note_button, "Note");
	evas_object_size_hint_weight_set(note_button, 1.0, 1.0);
	evas_object_size_hint_align_set(note_button, -1.0, -1.0);
	//elm_box_pack_end(hbox1, note_button);
	//evas_object_show(note_button);
	//evas_object_smart_callback_add(note_button, "clicked", note_button_clicked, list);

	//add hover for category select
	hs1 = elm_hover_add(win);
	//add box for hover
	bx = elm_box_add(win);
	evas_object_size_hint_weight_set(bx, 0.0, 0.0);
	elm_box_pack_end(hbox1, bx);
	evas_object_show(bx);

	sel_cat_bt = elm_button_add(win);
	elm_button_label_set(sel_cat_bt, " All Tasks ");
	evas_object_smart_callback_add(sel_cat_bt, "clicked", my_hover_bt_1, hs1);
	elm_box_pack_end(bx, sel_cat_bt);
	evas_object_show(sel_cat_bt);
	elm_hover_parent_set(hs1, win);
	elm_hover_target_set(hs1, sel_cat_bt);

	bt = elm_button_add(win);
	elm_button_label_set(bt, " All Tasks ");
	elm_hover_content_set(hs1, "middle", bt);
	evas_object_show(bt);

	bx = elm_box_add(win);
	//add categories
	add_hs_items (win, bx, bt, 1);
	evas_object_show(bx);
	elm_hover_content_set(hs1, "top", bx);

	// make window full screen
	evas_object_resize(win, 480, 640);
}

void create_new_task(void *data, Evas_Object *obj, void *event_info)
{
	int i = total_tasks;
	total_tasks ++;

	time_t curtime;
	struct tm *loctime;
	char dt[6];
	
	//get the time
	curtime = time (NULL);
	loctime = localtime (&curtime);
	strftime(dt, 6, "%d-%m", loctime);
	//get selected task if any
	Evas_Object *li = data;
	Elm_Genlist_Item *item = (Elm_Genlist_Item *)elm_genlist_selected_item_get(li);
	if (!item) item = elm_genlist_last_item_get(list);	//insert new line at end of list

	Task[i].key = Task[i-1].key + 1;
	Task[i].no = i;
	Task[i].cb = 0;
	Task[i].pr = 1;
	strcpy(Task[i].text, "Task");
	strcpy(Task[i].date, dt);//set current date
	strcpy(Task[i].cat, "Personal");
	if(item) { //cater for no items in list
		task_list[i] = elm_genlist_item_insert_after(list, &itc1, &Task[i], item, ELM_GENLIST_ITEM_NONE,
								  NULL, NULL);
	}
	else task_list[i] = elm_genlist_item_append(list, &itc1, &Task[i], NULL, ELM_GENLIST_ITEM_NONE,
								  NULL, NULL);
	last_rec = -1;
	//insert record
	insert_record(i);
	elm_genlist_item_selected_set(task_list[i], 1);
}

void cat_dialog_add(void *data, Evas_Object *obj, void *event_info)
{
	char *ty, *ty_data;
	Evas_Object *en = data;

	//get data from entry and add to category table
	char *_cat = (char *)elm_entry_entry_get(en);
	ty = strtok(_cat, "<");
	if (strcmp(ty, "") == 0) return;
	ty_data = strdup(ty);
	add_category(ty);
	elm_genlist_item_append(cat_list, &itc2, ty_data, NULL, ELM_GENLIST_ITEM_NONE,
								  NULL, NULL);
	evas_object_del(cat_dialog);
	//TODO : add button to cat hover
}

void create_cat_dialog(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *bg, *inwin, *lb, *bt, *bt1, *entry, *vbox, *hbox, *hbox1;

	cat_dialog = elm_win_add(NULL, "inwin", ELM_WIN_DIALOG_BASIC);
	elm_win_title_set(cat_dialog, "Add Category");
	elm_win_autodel_set(cat_dialog, 1);

	bg = elm_bg_add(cat_dialog);
	elm_win_resize_object_add(cat_dialog, bg);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	evas_object_show(bg);

	inwin = elm_win_inwin_add(cat_dialog);
	elm_win_inwin_style_set(inwin, "minimal_vertical");

	//add vbox 
	vbox = elm_box_add(cat_dialog);
	elm_win_resize_object_add(cat_dialog, vbox);
	evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
	elm_win_inwin_content_set(inwin, vbox);
	evas_object_show(vbox);

	//add hbox to vbox
	hbox = elm_box_add(cat_dialog);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox);
	evas_object_show(hbox);

	//add a label to hbox
	lb = elm_label_add(cat_dialog);
	elm_label_label_set(lb,"Category:");
	elm_box_pack_end(hbox, lb);
	evas_object_show(lb);

	//add an entry to hbox
	entry = elm_entry_add(cat_dialog);
	evas_object_size_hint_weight_set(entry, 1.0, 0.0);
	evas_object_size_hint_align_set(entry, -1.0, 0.0);
	elm_entry_single_line_set(entry, 1);
	elm_entry_editable_set(entry, 1);
	elm_entry_line_wrap_set(entry, 0);
	elm_box_pack_end(hbox, entry);
	evas_object_show(entry);

	//add another hbox
	hbox1 = elm_box_add(cat_dialog);
	elm_box_horizontal_set(hbox1, 1);
	evas_object_size_hint_weight_set(hbox1, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox1, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox1);
	evas_object_show(hbox1);
	
	//add yes button
	bt = elm_button_add(cat_dialog);
	elm_button_label_set(bt, "Add");
	evas_object_size_hint_weight_set(bt, 1.0, 1.0);
	evas_object_size_hint_align_set(bt, -1.0, -1.0);
	elm_box_pack_end(hbox1, bt);
	evas_object_show(bt);
	evas_object_smart_callback_add(bt, "clicked", cat_dialog_add, entry);
	
	//add close button
	bt1 = elm_button_add(cat_dialog);
	elm_button_label_set(bt1, "Cancel");
	evas_object_size_hint_weight_set(bt1, 1.0, 1.0);
	evas_object_size_hint_align_set(bt1, -1.0, -1.0);
	elm_box_pack_end(hbox1, bt1);
	evas_object_show(bt1);
	evas_object_smart_callback_add(bt1, "clicked", cat_win_del, cat_dialog);

	evas_object_show(inwin);
	
	evas_object_resize(cat_dialog, 480, 640);
	evas_object_show(cat_dialog);
}

//for cat genlist
Elm_Genlist_Item_Class itc2;

void cat_win_del(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *c_win = data;

	evas_object_del(c_win);
}

void cat_list_selected(void *data, Evas_Object *obj, void *event_info)
{
	//drop an entry from the list
	
}

void  del_cat_button_clicked(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *li = data;
	Elm_Genlist_Item *item = (Elm_Genlist_Item *)elm_genlist_selected_item_get(li);
	char *_cat = (char *)elm_genlist_item_data_get(item);
	//delete category from db
	del_category(_cat);
	//remove item from list
	elm_genlist_item_del(item);
	//TODO : remove button from category hover
}

char *cat_label_get(const void *data, Evas_Object *obj, const char *part)
{
	char *_cat = (char *)data;
	char *ty = strdup(_cat);
	return (ty);
}

Evas_Object *cat_icon_get(const void *data, Evas_Object *obj, const char *part)
{
	return NULL;
}

Evas_Bool cat_state_get(const void *data, Evas_Object *obj, const char *part)
{
   return 0;
}

void cat_del (const void *data, Evas_Object *obj)
{
}

void edit_cat(void) 
{
	Evas_Object *bg, *cat_win, *vbox, *hbox, *add_cat_button, *del_cat_button, *done_button, *fr, *lb, *hbox1;

	cat_win = elm_win_add(NULL, "cat", ELM_WIN_BASIC);
	elm_win_title_set(cat_win, "Edit Categories");
	evas_object_smart_callback_add(cat_win, "delete-request", cat_win_del, cat_win);
	
	//add background
	bg = elm_bg_add(cat_win);
	elm_win_resize_object_add(cat_win, bg);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	evas_object_show(bg);

	//add vbox 
	vbox = elm_box_add(cat_win);
	elm_win_resize_object_add(cat_win, vbox);
	evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
	evas_object_show(vbox);

	//add a hbox
	hbox1 = elm_box_add(cat_win);
	elm_box_horizontal_set(hbox1, 1);
	evas_object_size_hint_weight_set(hbox1, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox1, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox1);
	evas_object_show(hbox1);
	
	// add a frame
	fr = elm_frame_add(cat_win);
	elm_frame_style_set(fr, "outdent_top");
	evas_object_size_hint_weight_set(fr, 0.0, 0.0);
	evas_object_size_hint_align_set(fr, 0.0, -1.0);
	elm_box_pack_end(hbox1, fr);
	evas_object_show(fr);

	// add a label
	lb = elm_label_add(cat_win);
	elm_label_label_set(lb, "Task Categories");
	elm_frame_content_set(fr, lb);
	evas_object_show(lb);  
	
	//add list to vbox now
	cat_list = elm_genlist_add(cat_win);
	evas_object_size_hint_weight_set(cat_list, 1.0, 1.0);
	evas_object_size_hint_align_set(cat_list, -1.0, -1.0);
	elm_list_multi_select_set(cat_list, 0);
	elm_box_pack_end(vbox, cat_list);
	evas_object_show(cat_list);
	evas_object_smart_callback_add(cat_list, "clicked", cat_list_selected, cat_list);

	//genlist class defs
	itc2.item_style     		= "default";
	itc2.func.label_get 	= cat_label_get;
	itc2.func.icon_get  	= cat_icon_get;
	itc2.func.state_get 	= cat_state_get;
	itc2.func.del      		= cat_del;
	
	//add hbox to vbox
	hbox = elm_box_add(cat_win);
	elm_box_horizontal_set(hbox, 1);
	evas_object_size_hint_weight_set(hbox, 1.0, 0.0);
	evas_object_size_hint_align_set(hbox, -1.0, 0.0);
	elm_box_pack_end(vbox, hbox);
	evas_object_show(hbox);
	
	//add button to add categories
	add_cat_button = elm_button_add(cat_win);
	elm_button_label_set(add_cat_button, "Add");
	evas_object_size_hint_weight_set(add_cat_button, 1.0, 1.0);
	evas_object_size_hint_align_set(add_cat_button, -1.0, -1.0);
	elm_box_pack_end(hbox, add_cat_button);
	evas_object_show(add_cat_button);
	evas_object_smart_callback_add(add_cat_button, "clicked", create_cat_dialog, cat_list);
	
	//add button to del categories
	del_cat_button = elm_button_add(cat_win);
	elm_button_label_set(del_cat_button, "Delete");
	evas_object_size_hint_weight_set(del_cat_button, 1.0, 1.0);
	evas_object_size_hint_align_set(del_cat_button, -1.0, -1.0);
	elm_box_pack_end(hbox, del_cat_button);
	evas_object_show(del_cat_button);
	evas_object_smart_callback_add(del_cat_button, "clicked", del_cat_button_clicked, cat_list);

	//add done button
	done_button = elm_button_add(cat_win);
	elm_button_label_set(done_button, "Done");
	evas_object_size_hint_weight_set(done_button, 1.0, 1.0);
	evas_object_size_hint_align_set(done_button, -1.0, -1.0);
	elm_box_pack_end(hbox, done_button);
	evas_object_show(done_button);
	evas_object_smart_callback_add(done_button, "clicked", cat_win_del, cat_win);

	populate_cat_list(cat_list);
	// make window full screen
	evas_object_resize(cat_win, 480, 640);
	evas_object_show(cat_win);
}
