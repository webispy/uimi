#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <uimi.h>

#include "uimi_dbus.h"
#include "uimi_driver.h"

static gboolean on_dict_search (UimiDict *dict,
		GDBusMethodInvocation *invocation, const char *word,
		gpointer user_data)
{
	GVariant *result = NULL;
	GVariantBuilder b;
	gchar **list;
	GHashTable *d_result = NULL;
	GHashTableIter iter;
	void *key;
	void *value;

	/* TODO: driver search request */
	dbg ("sender[%s] search request[%s] to [%s]",
			g_dbus_method_invocation_get_sender (invocation),
			word,
			g_dbus_method_invocation_get_object_path (invocation));

	g_variant_builder_init (&b, G_VARIANT_TYPE("aa{sv}"));

	/* request word */
	g_variant_builder_open (&b, G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add (&b, "{sv}", "word", g_variant_new_string(word));
	g_variant_builder_close (&b);

	/* search result */
	d_result = uimi_driver_search (user_data, word);
	if (d_result) {
		g_variant_builder_open (&b, G_VARIANT_TYPE("a{sv}"));

		g_hash_table_iter_init (&iter, d_result);
		while (g_hash_table_iter_next (&iter, &key, &value)) {
			g_variant_builder_add (&b, "{sv}", key,
					g_variant_new_string(value));
		}
		g_variant_builder_close (&b);

		g_hash_table_destroy (d_result);
	}

	result = g_variant_builder_end (&b);

	uimi_dict_complete_search (dict, invocation, result);
}

static gboolean on_manager_get_dicts (UimiManager *mgr,
		GDBusMethodInvocation *invocation, gpointer user_data)
{
	gchar **list;

	list = calloc (1, sizeof (gchar *) * 2);
	list[0] = g_strdup ("test");
	list[1] = g_strdup ("sample");

	uimi_manager_complete_get_dicts (mgr, invocation, (const char **) list);

	g_free (list[0]);
	g_free (list[1]);
	g_free (list);
}

static void on_bus_acquired (GDBusConnection *conn, const gchar *name,
		gpointer user_data)
{
	struct app_data *ad = user_data;
	UimiManager *mgr;

	mgr = uimi_manager_skeleton_new();
	g_signal_connect (mgr, "handle-get-dicts",
			G_CALLBACK (on_manager_get_dicts), NULL);

	g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON(mgr),
			conn, UIMI_DBUS_PATH, NULL);
	g_dbus_object_manager_server_set_connection (ad->dbus->manager, conn);
}

int uimi_export_dict (struct uimi_conf_data *cd, struct app_data *ad)
{
	UimiObjectSkeleton *object;
	UimiDict *dict;

	if (!cd || !ad)
		return -1;

	if (!cd->name)
		return -1;

	cd->dbus_path = g_strdup_printf ("%s/%s", UIMI_DBUS_PATH, cd->name);

	object = uimi_object_skeleton_new (cd->dbus_path);
	if (!object)
		return -1;

	dict = uimi_dict_skeleton_new();
	uimi_object_skeleton_set_dict (object, dict);
	g_object_unref (dict);

	g_signal_connect (dict, "handle-search",
			G_CALLBACK (on_dict_search), cd);

	g_dbus_object_manager_server_export (ad->dbus->manager,
			G_DBUS_OBJECT_SKELETON (object));
}

int uimi_init_dbus (struct app_data *ad)
{
	guint id;

	if (!ad)
		return -1;

	ad->dbus = calloc (1, sizeof (struct dbus_data));
	if (!ad->dbus)
		return -1;

	id = g_bus_own_name (G_BUS_TYPE_SESSION,
			UIMI_DBUS_SERVICE,
			G_BUS_NAME_OWNER_FLAGS_REPLACE,
			on_bus_acquired,
			NULL, NULL,
			ad, NULL);

	ad->dbus->manager = g_dbus_object_manager_server_new (UIMI_DBUS_PATH);
	if (!ad->dbus->manager)
		return -1;

	dbg ("g_dbus_object_manager_server: %p", ad->dbus->manager);

	return 0;
}
