#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <uimi.h>

#include "util.h"

void dump_hash (GHashTable *h)
{
	GHashTableIter iter;
	void *key;
	void *value;

	g_hash_table_iter_init (&iter, h);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		dbg ("[%s] = [%s]", key, value);
	}
}
