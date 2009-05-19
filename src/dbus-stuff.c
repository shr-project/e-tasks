/***************************************************************************
 *            dbus.c
 *
 *  Copyright  2009  cchandel
 *  <cchandel@yahoo.com>
 ****************************************************************************/

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
#include "dbus-stuff.h"
#include <string.h>
#include <E_DBus.h>
#include <Elementary.h>

void occupy_cpu(void)
{	
	conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
	
	DBusMessage *msg;
	msg = dbus_message_new_method_call(
		"org.freesmartphone.ousaged",
		"/org/freesmartphone/Usage",
		"org.freesmartphone.Usage",
		"RequestResource"
	);
	
	const char *resource = "CPU";
	dbus_message_append_args (msg, DBUS_TYPE_STRING, &resource, DBUS_TYPE_INVALID);

	e_dbus_message_send(conn, msg, dbus_reply_cb, -1, NULL);
	dbus_message_unref(msg);
}

void release_cpu(void)
{
	DBusMessage *msg;
	msg = dbus_message_new_method_call(
		"org.freesmartphone.ousaged",
		"/org/freesmartphone/Usage",
		"org.freesmartphone.Usage",
		"ReleaseResource"
	);
	
	const char *resource = "CPU";
	dbus_message_append_args (msg, DBUS_TYPE_STRING, &resource, DBUS_TYPE_INVALID);

	e_dbus_message_send(conn, msg, dbus_reply_cb, -1, NULL);
	dbus_message_unref(msg);
}

void dbus_reply_cb(void *data, DBusMessage *replymsg, DBusError *error)
{
	if (dbus_error_is_set(error)) {
		printf("Error: %s - %s\n", error->name, error->message);
	}
}

