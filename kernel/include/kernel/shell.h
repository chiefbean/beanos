#ifndef _KERNEL_SHELL_H
#define _KERNEL_SHELL_H 1

#include <stdio.h>
#include <string.h>
#include <kernel/tty.h>

#ifdef __cplusplus
extern "C" {
#endif

void shell_run(char* command);
void parse_command(char* command);
void help(int argc, char** argv);
void echo(int argc, char** argv);
void add(int argc, char** argv);
void cmd_tick(int argc, char** argv);
void clear(int argc, char** argv);
void reboot(int argc, char** argv);

typedef void (*cmd_fn)(int, char**);

typedef struct {
    const char* name;
    cmd_fn handler;
    const char* help;
} cmd_t;

#ifdef __cplusplus
}
#endif

#endif