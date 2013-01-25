#ifndef __UIMI_DBUS_H__
#define __UIMI_DBUS_H__

#include "generated-code.h"

__BEGIN_DECLS

struct dbus_data {
	GDBusObjectManagerServer *manager;
};

int uimi_export_dict (struct uimi_conf_data *cd, struct app_data *ad);
int uimi_init_dbus (struct app_data *ad);


__END_DECLS

#endif
