//
//  coding.c
//  skiring
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
#include <stdlib.h>

#include "coding.h"

char getRandChar() {
    static FILE* fp = NULL;
    if (!fp)
        fp = fopen("/dev/urandom", "r");

    char ret = getc(fp);
    while (ret == 0)
        ret = getc(fp);
    return ret;
}

void storeString(FILE* fp, char* str) {
    SkStringSize len = strlen(str);
    fwrite(&len, sizeof(SkStringSize), 1, fp);
    fwrite(str, len, 1, fp);
}

char* loadString(FILE* fp) {
    SkStringSize len;
    fread(&len, sizeof(SkStringSize), 1, fp);
    char* ret = malloc(((size_t)len) + 1);
    fread(ret, len, 1, fp);
    ret[len] = '\0';
    return ret;
}

void storeObfuscatedString(FILE* fp, char* str) {
    int i;
    char ob = getRandChar();
    putc(ob, fp);
    SkStringSize len = strlen(str);
    fwrite(&len, sizeof(SkStringSize), 1, fp);
    for (i = 0; i < len; i++)
        putc(str[i]^ob, fp);
}

char* loadObfuscatedString(FILE* fp) {
    int i;
    char ob = getc(fp);
    SkStringSize len;
    fread(&len, sizeof(SkStringSize), 1, fp);
    char* ret = malloc(((size_t)len) + 1);
    for (i = 0; i < len; i++)
        ret[i] = getc(fp) ^ ob;
    ret[len] = '\0';
    return ret;
}

void storeStringSet(FILE* fp, SkStringSet* set) {
    int count = SkStringSet_count(set);
    fwrite(&count, sizeof(int), 1, fp);
    SkStringSetItem* item = set->head;
    while (item) {
        storeString(fp, item->value);
        item = item->next;
    }
}

SkStringSet* loadStringSet(FILE* fp) {
    int i, count;
    SkStringSet* ret = SkStringSet_create();
    fread(&count, sizeof(int), 1, fp);
    for (i = 0; i < count; i++) {
        char* str = loadString(fp);
        SkStringSet_add(ret, str);
        free(str);
    }
    return ret;
}

void storeKey(FILE* fp, SkKey* key) {
    storeString(fp, key->name);
    storeObfuscatedString(fp, key->value);
    storeStringSet(fp, key->users);
}

SkKey* loadKey(FILE* fp) {
    SkKey* ret = malloc(sizeof(SkKey));
    ret->name = loadString(fp);
    ret->value = loadObfuscatedString(fp);
    ret->users = loadStringSet(fp);
    ret->next = NULL;
    return ret;
}

void storeRing(FILE* fp, SkKeyRing* ring) {
    fwrite(SK_MAGIC_NUMER, 4, 1, fp);
    int count = SkKeyRing_count(ring);
    fwrite(&count, sizeof(int), 1, fp);
    SkKey* key = ring->head;
    while(key) {
        storeKey(fp, key);
        key = key->next;
    }
}

SkLoadStatus loadRing(FILE* fp, SkKeyRing* ring) {
    int i, count;
    char magic[5];
    fread(magic, 4, 1, fp);
    magic[4] = '\0';
    if (strcmp(magic, SK_MAGIC_NUMER) != 0)
        return WRONG_MAGIC_NUMBER;

    ring->head = NULL;

    fread(&count, sizeof(int), 1, fp);
    SkKey* lastKey = NULL;
    for (i = 0; i < count; i++) {
        SkKey* key = loadKey(fp);
        if (lastKey) {
            lastKey->next = key;
        } else {
            ring->head = key;
        }        
        lastKey = key;
    }

    fclose(fp);

    return SUCCESS;
}
