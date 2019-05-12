/* Copyright 2019 Jack Conger */

/*
 * This file is part of notcat.
 *
 * notcat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * notcat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with notcat.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <gio/gio.h>

#include "notcat.h"

static GDBusProxy *connect() {
    GDBusProxy *proxy;
    GDBusConnection *conn;
    GError *error = NULL;

    conn = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    g_assert_no_error(error);

    proxy = g_dbus_proxy_new_sync(conn,
                    G_DBUS_PROXY_FLAGS_NONE,
                    NULL,
                    "org.freedesktop.Notifications",  /* name */
                    "/org/freedesktop/Notifications", /* path */
                    "org.freedesktop.Notifications",  /* interface */
                    NULL,
                    &error);
    g_assert_no_error(error);

    return proxy;
}

static GVariant *call(GDBusProxy *proxy, char *name, GVariant *args) {
    GVariant *result;
    GError *error = NULL;

    result = g_dbus_proxy_call_sync(proxy,
            name,
            args,
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            NULL,
            &error);
    g_assert_no_error(error);
    return result;
}

extern int close_note(char *arg) {
    char *end;
    long id = strtol(arg, &end, 10);
    if (*arg == '\0' || *end != '\0')
        return 10;
    if (id < 0 || id > 65536)
        return 11;

    GDBusProxy *proxy = connect();
    GVariant *result = call(proxy, "CloseNotification",
            g_variant_new("(u)", id));
    g_variant_unref(result);

    return 0;
}

extern int get_capabilities() {
    GDBusProxy *proxy = connect();
    GVariant *result = call(proxy, "GetCapabilities", NULL);

    GVariant *inner = g_variant_get_child_value(result, 0);

    size_t len;
    const char **arr = g_variant_get_strv(inner, &len);
    for (size_t i = 0; i < len; i++)
        printf("%s\n", arr[i]);

    g_variant_unref(inner);
    g_variant_unref(result);

    return 0;
}

extern int get_server_information() {
    GDBusProxy *proxy = connect();
    GVariant *result = call(proxy, "GetServerInformation", NULL);

    printf("name:\t%s\n", g_variant_get_string(
                g_variant_get_child_value(result, 0), NULL));
    printf("vendor:\t%s\n", g_variant_get_string(
                g_variant_get_child_value(result, 1), NULL));
    printf("version:\t%s\n", g_variant_get_string(
                g_variant_get_child_value(result, 2), NULL));
    printf("spec version:\t%s\n", g_variant_get_string(
                g_variant_get_child_value(result, 3), NULL));

    return 0;
}
