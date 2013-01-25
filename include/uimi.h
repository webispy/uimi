#ifndef __UIMI_H__
#define __UIMI_H__

#include <uimi_log.h>

__BEGIN_DECLS

#define UIMI_DBUS_SERVICE "org.uimi"
#define UIMI_DBUS_PATH    "/org/uimi"

struct uimi_conf_data {
	char *name;
	char *driver;
	char *dbus_path;
	GHashTable *data;
	void *user_data;
};

struct app_data {
	GMainLoop *mainloop;
	struct dbus_data *dbus;
};

struct uimi_driver_desc {
	char *name;
	int (*init)(struct app_data *ad);
	int (*probe)(struct uimi_conf_data *cd);
	GHashTable* (*search)(struct uimi_conf_data *cd, const char *word);
};

struct uimi_driver_data {
	const char *driver_path;
	struct uimi_driver_desc *desc;
	void *user_data;
};

int   uimi_driver_set_user_data (struct uimi_driver_data *dd, void *user_data);
void* uimi_driver_get_user_data (struct uimi_driver_data *dd);
int   uimi_config_set_user_data (struct uimi_conf_data *cd, void *user_data);
void* uimi_config_get_user_data (struct uimi_conf_data *cd);

__END_DECLS

#endif
