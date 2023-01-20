#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <ios>
#include <iostream>

namespace ram{
	void getRAMInfomation() {
        struct sysinfo s_info;
        char info_buff[100];
        if (sysinfo(&s_info) == 0)
        {
            sprintf(info_buff, "Total memory: %.ld Mib", s_info.totalram / 1024 / 1024);
            printf("%s\n", info_buff);

            sprintf(info_buff, "Buffer memory: %.ld Mib", s_info.bufferram / 1024 / 1024);
            printf("%s\n", info_buff);

            sprintf(info_buff, "Shared memory: %.ld Mib", s_info.sharedram / 1024 / 1024);
            printf("%s\n", info_buff);

            sprintf(info_buff, "Free memory: %.ld Mib", s_info.freeram / 1024 / 1024);
            printf("%s\n", info_buff);

            sprintf(info_buff, "Total swap memory: %.ld Mib", s_info.totalswap / 1024 / 1024);
            printf("%s\n", info_buff);

            sprintf(info_buff, "Free swap memory: %.ld Mib", s_info.freeswap / 1024 / 1024);
            printf("%s\n", info_buff);

            sprintf(info_buff, "System running time: %.ld minute", s_info.uptime / 60);
            printf("%s\n", info_buff);

            printf("\n");
        }
	}
}