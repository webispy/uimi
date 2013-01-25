#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <uimi.h>

#include "uimi_dbus.h"
#include "uimi_config.h"
#include "uimi_driver.h"

int main (int argc, char *argv[])
{
	struct app_data *ad;

	g_type_init();

	ad = calloc (1, sizeof (struct app_data));
	if (!ad)
		return -1;

	ad->mainloop = g_main_loop_new (NULL, FALSE);

	uimi_init_config (ad);
	uimi_init_driver (ad);
	uimi_init_dbus (ad);

	uimi_export_config (ad);

	info ("start mainloop");
	g_main_loop_run (ad->mainloop);

	return 0;
}
