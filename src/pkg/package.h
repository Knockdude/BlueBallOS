#pragma once
void package_init(void);
int pkg_install(const char *name);
int pkg_remove(const char *name);
int pkg_list(void);
