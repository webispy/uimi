/**
 *
 * dictd driver
 *
 * [configuration]
 * - 2 files are needed. (index, dz)
 *
 * [dependency]
 * - 'dictzip' tools must be installed.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib.h>

#define UIMI_LOG_TAG "DRV_DICTDZ"
#include <uimi.h>

struct dictdz_data {
	char *file_dz;
	FILE *fp;
};

static int on_init (struct app_data *ad)
{
	dbg("init");

	return 0;
}

static int on_probe (struct uimi_conf_data *cd)
{
	char *file_dz;
	char *file_index;
	struct dictdz_data *priv_data;
	FILE *fp;
	struct stat stat_buf;

	dbg ("probe");

	if (!cd->data)
		return -1;

	file_dz = g_hash_table_lookup (cd->data, "dz");
	if (!file_dz) {
		err ("can't find 'dz' field in config");
		return -1;
	}

	if (stat (file_dz, &stat_buf) < 0) {
		err ("can't open '%s'", file_dz);
		return -1;
	}

	file_index = g_hash_table_lookup (cd->data, "index");
	if (!file_index) {
		err ("can't find 'index' field in config");
		return -1;
	}

	fp = fopen (file_index, "r");
	if (!fp) {
		err ("can't open '%s'", file_index);
		return -1;
	}

	priv_data = calloc (1, sizeof (struct dictdz_data));
	priv_data->fp = fp;
	priv_data->file_dz = g_strdup (file_dz);

	uimi_config_set_user_data (cd, priv_data);

	return 0;
}

static int update_result (GHashTable *result, int base_index, char *data)
{
	char *pos_cur = data;
	char *pos_begin = data;
	char index[11];
	int i = base_index;

	while (1) {
		snprintf (index, 10, "%d", i);

		while (1) {
			if (*pos_begin == '\0')
				return i;

			if (*pos_begin == ' ' || *pos_begin == '\t') {
				pos_begin++;
				continue;
			}

			break;
		}

		pos_cur = strchr (pos_begin, ',');
		if (!pos_cur) {
			g_hash_table_insert (result, g_strdup (index),
					g_strdup (pos_begin));
			break;
		}

		*pos_cur = '\0';
		g_hash_table_insert (result, g_strdup (index), g_strdup (pos_begin));

		pos_begin = pos_cur + 1;
		i++;
	}

	return i;
}

static GHashTable *search_dz (struct dictdz_data *priv_data,
		const char *start, const char *length)
{
	char buf[4096] = { 0, };
	FILE *fp;
	GHashTable *result = NULL;
	int i = 0;

	dbg ("start: %s (base64)", start);
	dbg ("size : %s (base64)", length);

	snprintf (buf, 4095, "dictzip -d -S %s -E %s %s -c",
			start, length, priv_data->file_dz);

	fp = popen (buf, "r");
	if (!fp)
		return NULL;

	result = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	if (!result)
		return NULL;

	/* first line is meta info */
	fgets (buf, 4095, fp);
	dbg ("info : %s", buf);

	while (!feof (fp)) {
		memset (buf, 0, 4096);
		fgets (buf, 4095, fp);

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		if (buf[strlen(buf) - 1] == '\r')
			buf[strlen(buf) - 1] = '\0';

		i = update_result (result, i, buf);
	}

	pclose (fp);

	return result;
}

static GHashTable *on_search (struct uimi_conf_data *cd, const char *word)
{
	struct dictdz_data *priv_data;
	char buf[4096];
	char *data;
	char *start;

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

		data = strchr (buf, '\t');
		if (!data)
			continue;

		buf[data - buf] = '\0';
		if (g_strcmp0 (buf, word) != 0)
			continue;

		start = data + 1;
		data = strchr (data + 1, '\t');
		if (!data)
			continue;

		buf[data - buf] = '\0';

		return search_dz (priv_data, start, data + 1);
	}

	return NULL;
}

struct uimi_driver_desc driver_define_desc = {
	.name = "dictdz",
	.init = on_init,
	.probe = on_probe,
	.search = on_search
};
