#include <kernel/shell.h>
#include <stdio.h>
#include <stdlib.h>
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
    {"reboot", reboot, "Reboot the system"},
    {"ls", ls, "List the current directory"},
    {"cat", cat, "Read the contents of a file"},
    {"write", write, "Write contents to file: write <file> <content>"},
    {"mkdir", mkdir, "Create a new directory"},
    {"rm", rm, "Removes a file or directory"},
    {"cd", cd, "Change directory"},
    {"cwd", cwd, "Print current working directory"}
};

vfs_node_t* cwd_node;
char* cwd_str = "/";

void shell_run(char* command) {
    if(strlen(command) == 0) {
        printf("\nError: No command specified");
        return;
    }

    char* words[64]; 
    int count = strtok(command, ' ', words);

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

void ls(int argc, char** argv) {
    (void) argc;
    (void) argv;

    size_t i = 0;
    dirent_t* cur_dir;
    vfs_node_t* tmp_node = !cwd_node ?  vfs_root : cwd_node;

    while((cur_dir = vfs_readdir(tmp_node, i))) {
        printf("\n%s\t%s", cur_dir->flags == VFS_DIRECTORY ? "D" : "F", cur_dir->name);
        i++;
    }
}

void cat(int argc, char** argv) {
    if (argc == 1) {
        printf("\nError: not enough arguments");
        return;
    }

    printf("\n");

    char filename[strlen(argv[1])+1];
    size_t len = strlen(argv[1]);
    for (size_t i = 0; i < len; i++) filename[i] = toupper(argv[1][i]);
    filename[len] = '\0';

    vfs_node_t* tmp_node = !cwd_node ?  vfs_root : cwd_node;
    vfs_node_t* node = vfs_finddir(tmp_node, filename);
    char buf[node->size+1];
    vfs_read(node, 0, node->size, buf);
    buf[node->size] = '\0';
    printf("%s", buf);
}

void write(int argc, char** argv) {
    if (argc < 3) {
        printf("\nError: not enough arguments");
        return;
    }

    char filename[strlen(argv[1])+1];
    size_t len = strlen(argv[1]);
    for (size_t i = 0; i < len; i++) filename[i] = toupper(argv[1][i]);
    filename[len] = '\0';

    vfs_node_t* tmp_node = !cwd_node ?  vfs_root : cwd_node;
    vfs_node_t* node = vfs_finddir(tmp_node, filename);
    if (!node)  node = vfs_create(tmp_node, filename, VFS_FILE);
    if (!node) { printf("\nFile not found: %s", argv[1]); return; }

    size_t total_len = 0;
    for(int i = 2; i < argc; i++) {
        size_t len = strlen(argv[i]);
        total_len += len;   
    }

    total_len += (argc - 2);
    char* buf = malloc(total_len);

    size_t k = 0;
    for(int i = 2; i < argc; i++) {
        size_t len = strlen(argv[i]);
        memcpy(buf + k, argv[i], len);
        k += len;
        if (i < argc - 1) buf[k++] = ' ';
    }
    buf[k] = '\0';

    void* out = vfs_write(node, 0, k, buf);

    if (out) {
        printf("\nWrote %d bytes to %s", total_len, argv[1]);
    } else {
        printf("\nFailed to write to %s", argv[1]);
    }
}

void mkdir(int argc, char** argv) {
    if (argc != 2) {
        printf("\nError: wrong number of arguments");
        return;
    }

    char filename[strlen(argv[1])+1];
    size_t len = strlen(argv[1]);
    for (size_t i = 0; i < len; i++) filename[i] = toupper(argv[1][i]);
    filename[len] = '\0';

    vfs_node_t* tmp_node = !cwd_node ?  vfs_root : cwd_node;
    vfs_node_t* node = vfs_finddir(tmp_node, filename);
    if (node)  { printf("\nDirectory already exists: /%s", argv[1]); return; }
    node = vfs_create(tmp_node, argv[1], VFS_DIRECTORY);
    printf("\nCreated directory: %s", node->name);
}

void rm(int argc, char** argv) {
    if (argc != 2) {
        printf("\nError: wrong number of arguments");
        return;
    }

    char filename[strlen(argv[1])+1];
    size_t len = strlen(argv[1]);
    for (size_t i = 0; i < len; i++) filename[i] = toupper(argv[1][i]);
    filename[len] = '\0';

    vfs_node_t* tmp_node = !cwd_node ?  vfs_root : cwd_node;
    vfs_node_t* node = vfs_finddir(tmp_node, filename);
    if (!node)  { printf("\nFile or directory does not exist: %s", argv[1]); return; }
    if(vfs_delete(node)) printf("\nDelete successful");
}

void cd(int argc, char** argv) {
    if (argc != 2) {
        printf("\nError: wrong number of arguments");
        return;
    }
    char filename[strlen(argv[1])+1];
    size_t len = strlen(argv[1]);
    for (size_t i = 0; i < len; i++) filename[i] = toupper(argv[1][i]);
    filename[len] = '\0';

    vfs_node_t* tmp_node = !cwd_node ?  vfs_root : cwd_node;
    vfs_node_t* node = vfs_finddir(tmp_node, filename);
    if (!node)  { printf("\nFile or directory does not exist: %s", argv[1]); return; }
    cwd_node = node;
    
}

void cwd(int argc, char** argv) {
    (void) argc;
    (void) argv;

    if(cwd_node) printf("\n%s", cwd_node->name);
    else printf("\n%s", vfs_root->name);
}