#ifndef __UIMI_DRIVER_H__
#define __UIMI_DRIVER_H__

__BEGIN_DECLS

GHashTable *uimi_driver_search (struct uimi_conf_data *cd, const char *word);
int uimi_driver_probe (struct uimi_conf_data *cd);
int uimi_init_driver (struct app_data *ad);

__END_DECLS

#endif
