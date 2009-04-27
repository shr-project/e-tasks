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

#include "db_sqlite.h"
#include <string.h>

void open_database(void)
{
	int db_ret, ret;
	char db[255];
	
	sprintf(db, "%s/.tasks/tasks.db", home_dir);
	db_ret = sqlite3_open(db, &tasks);
	if (db_ret == SQLITE_ERROR || db_ret == SQLITE_CANTOPEN) {
		printf("SQL error: %s\n", sqlite3_errmsg(tasks));
		printf("Creating new db file\n");
		//exit(1);
		ret = system("mkdir ~/.tasks");
		sqlite3_close(tasks);
		db_ret = sqlite3_open(db, &tasks);
	}
	strcpy(sel_category, "");
}

void first_run(void)
{
	char *sql, *err;
	int db_ret;
	
	//create tables
	sql ="CREATE TABLE tasks(key integer primary key, cb integer, priority integer, task text, alarm integer, repeat integer, date integer, category text);";
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
		if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
		}
	}	
	sql = "CREATE TABLE category(key integer primary key, category text);";
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
		if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
		}
	}
	// add default values
	sql ="INSERT INTO category(category) VALUES('Personal');";
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
		if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
		}
	}
	sql ="INSERT INTO category(category) VALUES('Business');";
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
		if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
		}
	}
	sql ="INSERT INTO category(category) VALUES('Home');";
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
		if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
		}
	}
}

void restore_state(void)
{
	int db_ret, i=1;
	char *sql;
	const char  *tail;
	sqlite3_stmt *stmt;

	elm_genlist_clear(list);

	//load the data 
	sql = "SELECT key, cb, priority, task, strftime('%d-%m', date), category FROM tasks";	
	db_ret = sqlite3_prepare(tasks, sql, strlen(sql), &stmt, &tail);
	if(db_ret != SQLITE_OK) {
		if (strcmp(sqlite3_errmsg(tasks), "no such table: tasks")==0) first_run();
		printf("SQL error: %d %s\n", db_ret, sqlite3_errmsg(tasks));
	}
	while((db_ret = sqlite3_step(stmt)) == SQLITE_ROW) {
		Task[i].no = i;
		Task[i].key = sqlite3_column_int(stmt, 0);
		Task[i].cb = sqlite3_column_int(stmt, 1);
		Task[i].pr = sqlite3_column_int(stmt, 2);
		sprintf(Task[i].text, "%s", sqlite3_column_text(stmt, 3));
		if(sqlite3_column_text(stmt, 4)) sprintf(Task[i].date, "%s", sqlite3_column_text(stmt, 4));
		else sprintf(Task[i].date, "No Date");
		sprintf(Task[i].cat, "%s", sqlite3_column_text(stmt, 5));
		i++;
	}
	sqlite3_finalize(stmt);
	total_tasks = i;
	if(strcmp(sel_category, "")== 0) strcpy(sel_category, " All Tasks ");
	show_cat_tasks (sel_category);
}

void show_cat_tasks(char *ca)
{
	int i;
	Elm_Genlist_Item *item;
	
	elm_genlist_clear(list);
	if (strcmp(ca, " All Tasks ") == 0) {
		for(i=1; i< total_tasks; i++) {
			if (!Task[i].cb) {
				task_list[i] = elm_genlist_item_append(list, &itc1, &Task[i], NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			}
		}
	}

	else if (strcmp(ca, "Deleted Tasks") == 0) {
		for(i=1; i< total_tasks; i++) {
			if (Task[i].cb) {
				task_list[i] = elm_genlist_item_append(list, &itc1, &Task[i], NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			}
		}
	}
	
	else {
		for(i=1; i< total_tasks; i++) {
			if ((strcmp(Task[i].cat, ca)==0) && (!Task[i].cb)) {
				task_list[i] = elm_genlist_item_append(list, &itc1, &Task[i], NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
			}
		}
	}
	item = elm_genlist_first_item_get(list);
	if(item) elm_genlist_item_selected_set(item, 1);
}

void add_hs_items(Evas_Object *win, Evas_Object *bx, Evas_Object *bt, int i)
{
	char *sql, cate[255], *tystr;
	int db_ret;
	const char  *tail;
	sqlite3_stmt *stmt;
	
	if (i) {
		sprintf(cate, " All Tasks ");
		bt = elm_button_add(win);
		elm_button_label_set(bt,cate);
		elm_box_pack_end(bx, bt);
		tystr = strdup(cate);
		evas_object_smart_callback_add(bt, "clicked", select_category, (char *)tystr);
		evas_object_show(bt);

		sprintf(cate, "Deleted Tasks");
		bt = elm_button_add(win);
		elm_button_label_set(bt,cate);
		elm_box_pack_end(bx, bt);
		tystr = strdup(cate);
		evas_object_smart_callback_add(bt, "clicked", select_category, (char *)tystr);
		evas_object_show(bt);

		sprintf(cate, "Edit Categories");
		bt = elm_button_add(win);
		elm_button_label_set(bt,cate);
		elm_box_pack_end(bx, bt);
		tystr = strdup(cate);
		evas_object_smart_callback_add(bt, "clicked", select_category, (char *)tystr);
		evas_object_show(bt);
	}

	sql = "SELECT category FROM category";	
	db_ret = sqlite3_prepare(tasks, sql, strlen(sql), &stmt, &tail);
	if(db_ret != SQLITE_OK) {
		printf("SQL error: %d %s\n", db_ret, sqlite3_errmsg(tasks));
	}
	while((db_ret = sqlite3_step(stmt)) == SQLITE_ROW) {
		sprintf(cate, "%s", sqlite3_column_text(stmt, 0));
		if (i) {
			bt = elm_button_add(win);
			elm_button_label_set(bt,cate);
			elm_box_pack_end(bx, bt);
			tystr = strdup(cate);
			evas_object_smart_callback_add(bt, "clicked", select_category, (char *)tystr);
			evas_object_show(bt);
		}
		else {
			bt = elm_button_add(win);
			elm_button_label_set(bt, cate);
			elm_box_pack_end(bx, bt);
			tystr = strdup(cate);
			evas_object_smart_callback_add(bt, "clicked", set_category, (char *)tystr);
			evas_object_show(bt);
		}
	}
	sqlite3_finalize(stmt);
}

// TODO : Edit catgories

void select_pr(void *data, Evas_Object *obj, void *event_info)
{

}

void update_record(int rec_no)
{
	int db_ret;
	char *err, *sql, tystr[11], *dd, *mm, sql_date[11];

	//rec_no += 1;
        printf("%d\n", rec_no);
	//convert date from dd-mm to yyyy-mm-dd
	//get current year
	if(strcmp(Task[rec_no].date, "No Date") != 0) {
		sprintf(tystr, "%s-", Task[rec_no].date);
		printf("%s\n", tystr);
		dd = strtok(tystr, "-");
		mm = strtok(NULL, "-");
		sprintf(sql_date, "2009-%s-%s", mm, dd);
	}
	else strcpy(sql_date, "No Date");
	
	sql = sqlite3_mprintf("UPDATE tasks SET cb=%d, priority=%d, task='%q', date='%s', category='%s' WHERE key = %d;", Task[rec_no].cb,
	                      Task[rec_no].pr, Task[rec_no].text, sql_date, Task[rec_no].cat, Task[rec_no].key);
	printf("%s\n", sql);
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
	  if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
	  }
	}
	sqlite3_free(sql);
}

void insert_record(int i)
{
	int db_ret;
	char *err, *sql;
	
	sql = sqlite3_mprintf("INSERT INTO tasks (key, cb, priority, task, date, category) VALUES(%d, %d, %d, '%s', CURRENT_DATE, '%s');", 
	                      	Task[i].key, Task[i].cb, Task[i].pr, Task[i].text, Task[i].cat);
	printf("%s\n", sql);
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
	  if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
	  }
	}
	sqlite3_free(sql);
}

void del_record(int i)
{
	int db_ret;
	char *err, *sql;
	
	sql = sqlite3_mprintf("DELETE FROM tasks WHERE key = %d;", i);
	printf("%s\n", sql);
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
	  if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
	  }
	}
	sqlite3_free(sql);
}

void populate_cat_list(Evas_Object *lis)
{
	int db_ret;
	char *sql, cate[255], *ca;
	sqlite3_stmt *stmt;
	const char  *tail;

	//get data from categories table and add to cat_list
	sql = "SELECT category FROM category";	
	db_ret = sqlite3_prepare(tasks, sql, strlen(sql), &stmt, &tail);
	if(db_ret != SQLITE_OK) {
		printf("SQL error: %d %s\n", db_ret, sqlite3_errmsg(tasks));
	}
	while((db_ret = sqlite3_step(stmt)) == SQLITE_ROW) {
		sprintf(cate, "%s", sqlite3_column_text(stmt, 0));
		ca = strdup(cate);
		elm_genlist_item_append(lis, &itc2, (char *)ca, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	}
}
                       
void del_category(char * cat)
{
	int db_ret;
	char *err, *sql;
	
	sql = sqlite3_mprintf("DELETE FROM category WHERE category='%q';", cat);
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
	  if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
	  }
	}
	sqlite3_free(sql);
}

void add_category(char * cat)
{
	int db_ret;
	char *err, *sql;
	
	sql = sqlite3_mprintf("INSERT INTO category(category) VALUES('%q');", cat);
	db_ret = sqlite3_exec(tasks, sql, NULL, NULL, &err);
	if (db_ret != SQLITE_OK) {
	  if (err != NULL) {
		  fprintf(stderr, "SQL error: %s\n", err);
		  sqlite3_free(err);
	  }
	}
	sqlite3_free(sql);
}
