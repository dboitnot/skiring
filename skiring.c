//
//  skiring.c
//  skiring
//
// Copyright (c) 2013, Dan Boitnott All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
//
// * Neither the name of the author nor the names of its contributors
//   may be used to endorse or promote products derived from this software
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
// WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "ring.h"
#include "coding.h"
#include "skiring.h"

#define SYSTEM_RING_PATH SYSCONFDIR "/skiring/ring"

#define GET_KEY(ring, keyName, key) {key = SkKeyRing_find(ring, keyName, NULL); if (!key) { fprintf(stderr, "key not found: %s\n", keyName); return KEY_NOT_FOUND; }}

#define REQUIRE_SUPERUSER() {if (strcmp(realUser,superUser)) { fprintf(stderr, "permission denied, log in as %s to do this\n", superUser); return PERMISSION_DENIED; }}

char* realUser;
char* superUser;

// Forward declaration because help must be defined after commands.
int cmd_help(int argc, char** argv, SkKeyRing* ring);

int cmd_list(int argc, char** argv, SkKeyRing* ring) {
    SkKey* key = ring->head;

    if (!key) {
        printf("No keys defined.\n");
        return SUCCESS;
    }

    printf("Keys           Authorized Users\n");
    printf("-------------- ----------------\n");

    while (key) {
        printf("%-15s", key->name);

        // Print user list
        SkStringSetItem* user = key->users->head;
        int i = 0;
        while (user) {
            if (i++)
                printf("%-15s", "");
            printf("%s\n", user->value);
            user = user->next;
        }
	if (i < 1)
        	printf("\n");

        key = key->next;
    }

    printf("\n");
    return SUCCESS;
}

int cmd_list_keys(int argc, char** argv, SkKeyRing* ring) {
    SkKey* key = ring->head;

    if (!key) {
        return SUCCESS;
    }

    while (key) {
        printf("%s\n", key->name);
        key = key->next;
    }

    return SUCCESS;
}

int cmd_grant(int argc, char** argv, SkKeyRing* ring) {
    if (argc != 4)
        return BAD_ARGUMENT;

    REQUIRE_SUPERUSER();

    char* keyName = argv[2];
    char* userName = argv[3];

    SkKey* key; GET_KEY(ring, keyName, key);
    
    if (strlen(userName) > SK_MAX_STRING_SIZE) {
        fprintf(stderr, "username too long: %s\n", userName);
        return STRING_TOO_LONG;
    }

    SkStringSet_add(key->users, userName);

    return SUCCESS;
}

int cmd_revoke(int argc, char** argv, SkKeyRing* ring) {
    if (argc != 4)
        return BAD_ARGUMENT;

    REQUIRE_SUPERUSER();

    char* keyName = argv[2];
    char* userName = argv[3];

    SkKey* key; GET_KEY(ring, keyName, key);

    SkStringSet_remove(key->users, userName);

    return SUCCESS;
}

int cmd_remove(int argc, char** argv, SkKeyRing* ring) {
    if (argc != 3)
        return BAD_ARGUMENT;

    REQUIRE_SUPERUSER();

    SkKey* key; GET_KEY(ring, argv[2], key); // Error if key doesn't exist.
    SkKeyRing_remove(ring, argv[2]);

    return SUCCESS;
}

bool readPassword(char* prompt, char** password) {
    // Prompt
    printf("%s: ", prompt);

    // Disable terminal echo
    struct termios oldt;
    tcgetattr(fileno(stdin), &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), TCSANOW, &newt);

    // read password
    *password = NULL;
    size_t linecap = 0;
    size_t size = getline(password, &linecap, stdin);

    // restore terminal
    tcsetattr(fileno(stdin), TCSANOW, &oldt);
    printf("\n");

    // TODO: handle interrupt while echo is off

    // handle user cancel
    if (size < 1) {
        fprintf(stderr, "\nuser canceled\n");
        return true;
    }

    // trim trailing newline
    int i;
    for (i = strlen(*password) - 1; i >= 0; i--) {
        char c = (*password)[i];
        if (c == '\n' || c == '\r')
            (*password)[i] = '\0';
    }

    return false;
}

int cmd_put(int argc, char** argv, SkKeyRing* ring) {
    if (argc != 3)
        return BAD_ARGUMENT;

    REQUIRE_SUPERUSER();

    char* keyName = argv[2];

    char* pw1, *pw2;
    if (readPassword("New password", &pw1))
        return USER_CANCEL;
    if (readPassword("Repeat password", &pw2))
        return USER_CANCEL;

    if (strcmp(pw1, pw2) != 0) {
        fprintf(stderr, "passwords don't match\n");
        return PASSWORD_MISMATCH;
    }

    SkKeyRing_put(ring, keyName, pw1);

    fprintf(stderr, "keychain updated\n");
    return SUCCESS;
}

int cmd_get(int argc, char** argv, SkKeyRing* ring) {
    if (argc < 3)
        BAD_ARGUMENT;    

    SkKey* key = NULL;
    char* keyName;
    int i = 2;
    for (; i < argc; i++) {
        keyName = argv[i];
        key = SkKeyRing_find(ring, keyName, NULL);
        if (key)
            break;
    }

    if (!key) {
        fprintf(stderr, "key not found: %s\n", keyName);
        return KEY_NOT_FOUND;
    }

    if (strcmp(realUser, superUser) && (!SkStringSet_contains(key->users, realUser))) {
        fprintf(stderr, "user '%s' is not authorized for key '%s'\n", realUser, keyName);
        return PERMISSION_DENIED;
    }

    printf("%s\n", key->value);

    return SUCCESS;
}

int cmd_init(int argc, char** argv, SkKeyRing* ring) {
    // Do Nothing. Just let the keyring get created.
    return SUCCESS;
}

typedef struct {
    char* command;
    char* usage;
    char* shortHelp;
    bool loadFile;
    bool storeFile;
    int (*func)(int argc, char** argv, SkKeyRing* ring);
} SkCommand;

SkCommand commands[] = {
    { "help", "", "print this information", false, false, &cmd_help },
    { "list", "", "list all keys and authorized users", true, false, &cmd_list },
    { "list-keys", "", "lists all keys", true, false, &cmd_list_keys },
    { "grant", "<key> <user>", "grant <key> to <user>", true, true, &cmd_grant },
    { "revoke", "<key> <user>", "revoke <key> from <user>", true, true, &cmd_revoke },
    { "remove", "<key>", "remove <key> from keyring", true, true, &cmd_remove },
    { "put", "<key>", "set password for <key>", true, true, &cmd_put },
    { "get", "<key> [<key> ...]", "print password for <key>. With multiple keys, print first match.", true, false, &cmd_get },
    { "init", "", "creates the keyring if it doesn't already exist", true, true, &cmd_init }
};

int cmd_help(int argc, char** argv, SkKeyRing* ring) {    
    fprintf(stderr, "usage: %s ", argv[0]);
    int indent = strlen(argv[0]) + 8;
    int i = 0;
    for (; i < sizeof(commands)/sizeof(SkCommand); i++) {
        SkCommand cmd = commands[i];
        if (i > 0)
            fprintf(stderr, "%*s", indent, "");
        fprintf(stderr, "%s %s\n", cmd.command, cmd.usage);
    }

    fprintf(stderr, "\nCommand Information:\n");
    for (i = 0; i < sizeof(commands)/sizeof(SkCommand); i++) {
        SkCommand cmd = commands[i];
        fprintf(stderr, "  %-12s %s\n", cmd.command, cmd.shortHelp);
    }

    return SUCCESS;
}

bool loadSystemRing(SkKeyRing* ring) {
    char* path = SYSTEM_RING_PATH;

    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        if (errno == ENOENT) {
            // File doesn't exist. No problem.
            ring->head = NULL;
            return false;
        }
        perror("error opening key ring");
        return true;
    }

    if(loadRing(fp, ring)) {
        fprintf(stderr, "Error loading keyring");
        return true;
    }

    return false;
}

bool storeSystemRing(SkKeyRing* ring) {
    umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    
    char* path = SYSTEM_RING_PATH;

    FILE* fp = fopen(path, "w");
    if (fp == NULL) {
        perror("error writing key ring");
        printf("   %s\n", path);
        return true;
    }

    storeRing(fp, ring);

    return false;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cmd_help(argc, argv, NULL);
        return BAD_ARGUMENT;
    }

    // Get real user ID
    uid_t realUID = getuid();
    struct passwd *pwd = getpwuid(realUID);
    realUser = strdup(pwd->pw_name);

    // Get effective (superuser) ID
    uid_t effectUID = geteuid();
    pwd = getpwuid(effectUID);
    superUser = strdup(pwd->pw_name);

    char* cmd = argv[1];
    int i = 0;
    for (; i < sizeof(commands)/sizeof(SkCommand); i++) {
        if (strcmp(cmd, commands[i].command) == 0) {
            SkKeyRing ring;

            // load the file if necessary
            if (commands[i].loadFile && loadSystemRing(&ring))
                return RING_LOAD_ERROR;

            // run the command
            int ret = commands[i].func(argc, argv, &ring);

            if (ret) {
                // An error occured in the command.
                if (ret == BAD_ARGUMENT)
                    cmd_help(argc, argv, NULL);
                return ret; 
            }

            // save the file if necessary
            if (commands[i].storeFile && storeSystemRing(&ring))
                return RING_SAVE_ERROR;

            return SUCCESS;
        }
    }

    fprintf(stderr, "Unrecognized command: %s\n\n", cmd);
    cmd_help(argc, argv, NULL);
    return BAD_ARGUMENT;
}
