#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#define UIMI_LOG_TAG "DRV_CSV"
#include <uimi.h>

struct csv_data {
	FILE *fp;
};

static int on_init (struct app_data *ad)
{
	dbg("init");

	return 0;
}

static int on_probe (struct uimi_conf_data *cd)
{
	char *file;
	struct csv_data *priv_data;
	FILE *fp;

	dbg ("probe");

	if (!cd->data)
		return -1;

	file = g_hash_table_lookup (cd->data, "file");
	if (!file) {
		err ("can't find 'file' field in config");
		return -1;
	}

	fp = fopen (file, "r");
	if (!fp) {
		err ("can't open '%s'", file);
		return -1;
	}

	priv_data = calloc (1, sizeof (struct csv_data));
	priv_data->fp = fp;

	uimi_config_set_user_data (cd, priv_data);

	return 0;
}

static GHashTable *make_result (char *data)
{
	GHashTable *result = NULL;
	char *pos_cur = data;
	char *pos_begin = data;
	char index[11];
	int i = 0;

	result = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	if (!result)
		return NULL;

	dbg ("result: %s", data);

	while (1) {
		snprintf (index, 10, "%d", i);

		pos_cur = strchr (pos_begin, ',');
		if (!pos_cur) {
			g_hash_table_insert (result, g_strdup (index), g_strdup (pos_begin));
			break;
		}

		*pos_cur = '\0';
		g_hash_table_insert (result, g_strdup (index), g_strdup (pos_begin));

		pos_begin = pos_cur + 1;
		i++;
	}

	return result;
}

static GHashTable *on_search (struct uimi_conf_data *cd, const char *word)
{
	struct csv_data *priv_data;
	char buf[4096];
	char *data;

	if (!word)
		return NULL;

	priv_data = uimi_config_get_user_data (cd);
	if (!priv_data) {
		error ("config internal error");
		return NULL;
	}

	dbg ("search: '%s'", word);

	fseek (priv_data->fp, 0, SEEK_SET);

	while (!feof(priv_data->fp)) {
		memset (buf, 0, 4096);
		fgets (buf, 4095, priv_data->fp);

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		data = strchr (buf, ',');
		if (!data)
			continue;

		buf[data - buf] = '\0';
		if (g_strcmp0 (buf, word) != 0)
			continue;

		return make_result (data + 1);
	}

	return NULL;
}

struct uimi_driver_desc driver_define_desc = {
	.name = "csv",
	.init = on_init,
	.probe = on_probe,
	.search = on_search
};
