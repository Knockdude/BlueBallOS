#pragma once
void net_init(void);
int net_dhcp(void);
int net_http_get(const char *host,const char *path);
