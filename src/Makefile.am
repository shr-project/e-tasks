## Process this file with automake to produce Makefile.in
## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(TASKS_CFLAGS)

AM_CFLAGS =\
         $(EDBUS_CFLAGS) \
	 -Wall\
	 -g

bin_PROGRAMS = tasks

tasks_SOURCES = \
	callbacks.c \
	callbacks.h \
	main.c \
	gui.c \
	gui.h \
	db_sqlite.c \
	db_sqlite.h \
	dbus-stuff.c \
	dbus-stuff.h

tasks_LDFLAGS = \
	-Wl,--export-dynamic

tasks_LDADD = $(TASKS_LIBS)

uninstall-local:
	-rm -r $(datadir)/tasks
