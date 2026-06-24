#include <kernel/shell.h>
#include <stdio.h>
#include <string.h>
#include <kernel/tty.h>
#include <sys/timer.h>
#include <sys/io.h>

cmd_t commands[] = {
    {"help", help, "Print help message"},
    {"echo", echo, "Echo string back"},
    {"add", add, "Add space-separated list of numbers"},
    {"tick", cmd_tick, "Print current timer tick"},
    {"clear", clear, "Clear the screen"},
    {"reboot", reboot, "Reboot the system"}
};

void shell_run(char* command) {
    if(strlen(command) == 0) {
        printf("\nError: No command specified");
        return;
    }

    char* words[64]; 
    int count = strtok(command, ' ', words);
    printf("\n%d", count);

    size_t com_len = sizeof(commands)/sizeof(commands[0]);

    for(size_t i = 0; i < com_len; i++) {
        if(!strcmp(words[0], (char*) commands[i].name)) {
            commands[i].handler(count, words);
            return;
        }
    }

    printf("\nError: command '%s' not found", words[0]);
}

void help(int argc, char** argv) {
    (void) argc;
    (void) argv;

    size_t com_len = sizeof(commands)/sizeof(commands[0]);

    for(size_t i = 0; i < com_len; i++) {
        printf("\n%s - %s", commands[i].name, commands[i].help);
    }
}

void echo(int argc, char** argv) {
    if (argc == 1) {
        printf("\nError: only 1 argument supplied, expected 2+");
        return;
    }

    printf("\n");

    for(int i = 1; i < argc; i++) {
        size_t len = strlen(argv[i]);
        for(size_t j = 0; j < len; j++) {
            if (argv[i][j] != '"') {
                printf("%c", argv[i][j]);
            }
        }
    }
}

void add(int argc, char** argv) {
    int sum = 0;

    for (int i = 1; i < argc; i++) {
        sum += atoi(argv[i]);
    }
    printf("\n%d", sum);
}

void cmd_tick(int argc, char** argv) {
    (void) argc;
    (void) argv;

    printf("\n%d", get_tick());
}

void clear(int argc, char** argv) {
    (void) argc;
    (void) argv;

    clear_screen();
}

void reboot(int argc, char** argv) {
    (void) argc;
    (void) argv;

    __asm__ __volatile__("cli");
    while(((inb(0x64) & 1) == 0)) {}
    outb(0x64, 0xFE);
    for(;;) {
        __asm__ __volatile__("hlt");
    }
}
