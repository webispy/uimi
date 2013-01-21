#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "generated-code.h"
#include <uimi.h>

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
	UimiManager *mgr;
	GDBusObjectManagerServer *manager;

	manager = g_dbus_object_manager_server_new (UIMI_DBUS_PATH);
	
	mgr = uimi_manager_skeleton_new();
	g_signal_connect (mgr, "handle-get-dicts",
			G_CALLBACK (on_manager_get_dicts), NULL);

	g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON(mgr),
			conn, UIMI_DBUS_PATH, NULL);
	g_dbus_object_manager_server_set_connection (manager, conn);
}

int main (int argc, char *argv[])
{
	GMainLoop *mainloop;
	guint id;

	g_type_init();

	id = g_bus_own_name (G_BUS_TYPE_SESSION,
			UIMI_DBUS_SERVICE,
			G_BUS_NAME_OWNER_FLAGS_REPLACE,
			on_bus_acquired,
			NULL, NULL,
			NULL, NULL);

	mainloop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (mainloop);
	
	return 0;
}
