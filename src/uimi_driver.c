#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <glib.h>
#include <uimi.h>

#ifndef UIMI_DRIVER_PATH
#define UIMI_DRIVER_PATH "/lib/uimi/drivers"
#endif

#include "uimi_driver.h"

static GSList *g_drivers;

static gint find_driver (gconstpointer a, gconstpointer b)
{
	const struct uimi_driver_data *d = a;
	const struct uimi_conf_data *c = b;

	if (!a || !b)
		return -1;

	return g_strcmp0 (d->desc->name, c->driver);
}

static void on_driver_init (gpointer data, gpointer user_data)
{
	struct uimi_driver_data *dd = data;
	struct app_data *ad = user_data;
	int ret;

	if (!dd || !ad)
		return;

	if (!dd->desc)
		return;

	if (!dd->desc->init)
		return;

	ret = dd->desc->init (ad);
	if (ret < 0) {
		/* TODO: disable driver */
		dbg("init fail");
		return;
	}
}

static int load_driver (const char *path, struct app_data *ad)
{
	GDir *dir;
	struct uimi_driver_data *dd;
	struct uimi_driver_desc *desc;
	char *filename;
	const char *file;
	void *h;

	dir = g_dir_open (path, 0, NULL);
	if (!dir)
		return -1;

	while ((file = g_dir_read_name (dir)) != NULL) {
		if (g_str_has_prefix (file, "lib") == TRUE
				|| g_str_has_suffix (file, ".so") == FALSE)
			continue;

		filename = g_build_filename (path, file, NULL);

		h = dlopen (filename, RTLD_NOW);
		if (!h) {
			dbg ("dlopen(%s) fail: %s", filename, dlerror());
			g_free (filename);
			continue;
		}

		desc = dlsym (h, "driver_define_desc");
		if (!desc) {
			dbg ("invalid driver: %s", dlerror());
			dlclose (h);
			g_free (filename);
			continue;
		}

		dbg ("load driver - %s", filename);

		dd = calloc (1, sizeof (struct uimi_driver_data));
		if (!dd) {
			dlclose (h);
			g_free (filename);
			continue;
		}

		dd->desc = desc;
		dd->driver_path = g_strdup (filename);

		g_drivers = g_slist_append (g_drivers, dd);

		g_free (filename);
	}

	g_dir_close (dir);

	g_slist_foreach (g_drivers, on_driver_init, ad);
}

GHashTable *uimi_driver_search (struct uimi_conf_data *cd, const char *word)
{
	struct uimi_driver_data *dd;
	GSList *node;

	if (!cd)
		return NULL;

	node = g_slist_find_custom (g_drivers, cd, find_driver);
	if (!node)
		return NULL;

	dd = node->data;

	if (!dd->desc->probe)
		return NULL;

	return dd->desc->search (cd, word);
}

int uimi_driver_probe (struct uimi_conf_data *cd)
{
	struct uimi_driver_data *dd;
	GSList *node;

	if (!cd) {
		dbg ("no config data");
		return -1;
	}

	node = g_slist_find_custom (g_drivers, cd, find_driver);
	if (!node) {
		dbg ("unknown driver (%s)", cd->driver);
		return -1;
	}

	dd = node->data;

	if (!dd->desc->probe)
		return -1;

	return dd->desc->probe (cd);
}

int uimi_init_driver (struct app_data *ad)
{
	load_driver ("/usr" UIMI_DRIVER_PATH, ad);
	load_driver ("/usr/local" UIMI_DRIVER_PATH, ad);

	return 0;
}

int uimi_driver_set_user_data (struct uimi_driver_data *dd, void *user_data)
{
	if (!dd)
		return -1;

	dd->user_data = user_data;

	return 0;
}

void *uimi_driver_get_user_data (struct uimi_driver_data *dd)
{
	if (!dd)
		return NULL;

	return dd->user_data;
}
