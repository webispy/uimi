#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <uimi.h>

#include "uimi_config.h"

static GSList *g_conf;

static int load_config (struct app_data *ad, const char *path)
{
	GKeyFile *file = NULL;
	gchar **groups = NULL;
	gchar **keys = NULL;
	gchar *driver = NULL;
	int ret = -1;
	int i;
	gsize keys_len;
	struct uimi_conf_data *conf;

	if (!ad || !path)
		return ret;

	file = g_key_file_new();
	if (!file)
		return ret;

	if (!g_key_file_load_from_file (file, path, 0, NULL)) {
		dbg ("can't load %s", path);
		goto OUT;
	}

	groups = g_key_file_get_groups (file, NULL);
	if (!groups) {
		ret = 0;
		goto OUT;
	}

	for (i = 0; groups[i]; i++) {
		if (g_strcmp0 (groups[i], "uimi") == 0) {
			/* Default group */
			continue;
		}

		/* Dict group */
		driver = g_key_file_get_string (file, groups[i], "driver", NULL);
		if (!driver) {
			dbg ("group[%s] is invalid", groups[i]);
			continue;
		}

		info ("Dict[%s], driver=%s", groups[i], driver);

		conf = calloc (1, sizeof (struct uimi_conf_data));
		conf->name = g_strdup (groups[i]);
		conf->driver = g_strdup (driver);
		conf->data = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
				NULL);

		keys_len = 0;
		keys = g_key_file_get_keys (file, groups[i], &keys_len, NULL);
		if (keys) {
			gsize j;
			gchar *value;

			for (j = 0; j < keys_len; j++) {
				value = g_key_file_get_string (file, groups[i], keys[j], NULL);
				if (!value)
					continue;

				dbg ("field[%s] = %s", keys[j], value);
				g_hash_table_insert (conf->data, g_strdup (keys[j]),
						g_strdup (value));

				g_free (value);
			}

			g_strfreev (keys);
		}

		g_conf = g_slist_append (g_conf, conf);

		if (driver)
			g_free (driver);
	}

	ret = 0;
	g_strfreev (groups);

OUT:
	g_key_file_free (file);
	return ret;
}

static void on_conf_item (gpointer data, gpointer user_data)
{
	struct uimi_conf_data *conf = data;
	struct app_data *ad = user_data;

	if (uimi_driver_probe (conf) < 0) {
		return;
	}

	/* DBus export */
	uimi_export_dict (conf, ad);
}

int uimi_export_config (struct app_data *ad)
{
	g_slist_foreach (g_conf, on_conf_item, ad);
}

int uimi_init_config (struct app_data *ad)
{
	if (!ad)
		return -1;

	load_config (ad, "/etc/uimi.conf");
}

int uimi_config_set_user_data (struct uimi_conf_data *cd, void *user_data)
{
	if (!cd)
		return -1;

	cd->user_data = user_data;

	return 0;
}

void *uimi_config_get_user_data (struct uimi_conf_data *cd)
{
	if (!cd)
		return NULL;

	return cd->user_data;
}
