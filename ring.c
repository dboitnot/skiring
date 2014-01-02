//
// ring.c
// skiring
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
#include <stdlib.h>

#include "ring.h"

#pragma mark SkStringSetItem methods

SkStringSetItem* SkStringSetItem_create(char* str, SkStringSetItem* next) {
    SkStringSetItem* ret = malloc(sizeof(SkStringSetItem));
    ret->value = strdup(str);
    ret->next = next;
    return ret;
}

#pragma mark SkStringSet methods

SkStringSet* SkStringSet_create() {
    SkStringSet* ret = malloc(sizeof(SkStringSet));
    ret->head = NULL;
    return ret;
}

SkStringSetItem* SkStringSet_find(SkStringSet* set, char* str, SkStringSetItem** prev) {
    SkStringSetItem* ret = set->head;
    if (prev)
        *prev = NULL;

    while (ret) {
        if (strcmp(ret->value, str) == 0)
            return ret;
        if (prev)
            *prev = ret;
        ret = ret->next;
    }
    return NULL;
}

bool SkStringSet_contains(SkStringSet* set, char* str) {
    return SkStringSet_find(set, str, NULL) != NULL;
}

SkStringSetItem* SkStringSet_add(SkStringSet* set, char* str) {
    SkStringSetItem* ret = SkStringSet_find(set, str, NULL);
    if (ret != NULL)
        return ret;

    ret = SkStringSetItem_create(str, set->head);
    set->head = ret;
    return ret;
}

void SkStringSet_remove(SkStringSet* set, char* str) {
    SkStringSetItem* prev;
    SkStringSetItem* found = SkStringSet_find(set, str, &prev);
    if (!found)
        return;
    if (prev == NULL) {
        /* Item was first. */
        set->head = found->next;
    } else {
        prev->next = found->next;
    }

    /* TODO: free found */
}

int SkStringSet_count(SkStringSet* set) {
    int ret = 0;
    SkStringSetItem* item = set->head;
    while (item) {
        ret++;
        item = item->next;
    }
    return ret;
}

#pragma mark SkKey methods

SkKey* SkKey_create(char* name, char* value, SkKey* next) {
    SkKey* ret = malloc(sizeof(SkKey));

    ret->name = strdup(name);
    ret->value = strdup(value);
    ret->users = SkStringSet_create();
    ret->next = next;
    return ret;
}

#pragma mark SkKeyRing methods

SkKeyRing* SkKeyRing_create() {
    SkKeyRing* ret = malloc(sizeof(SkKeyRing));
    ret->head = NULL;
    return ret;
}

SkKey* SkKeyRing_find(SkKeyRing* ring, char* name, SkKey** prev) {
    SkKey* ret = ring->head;
    if (prev)
        *prev = NULL;

    while (ret) {
        if (strcasecmp(ret->name, name) == 0)
            return ret;
        if (prev)
            *prev = ret;
        ret = ret->next;
    }

    return NULL;
}

SkKey* SkKeyRing_put(SkKeyRing* ring, char* name, char* value) {
    SkKey* ret = SkKeyRing_find(ring, name, NULL);
    if (ret) {
        /* Key is already present. Update value. */
        /* TODO: free old value */
        ret->value = strdup(value);
        return ret;
    }

    ret = SkKey_create(name, value, ring->head);
    ring->head = ret;
    return ret;
}

void SkKeyRing_remove(SkKeyRing* ring, char* name) {
    SkKey* prev;
    SkKey* found = SkKeyRing_find(ring, name, &prev);
    if (!found)
        return;
    if (prev == NULL) {
        /* Item was first. */
        ring->head = found->next;
    } else {
        prev->next = found->next;
    }

    /* TODO: free found */
}

int SkKeyRing_count(SkKeyRing* ring) {
    int ret = 0;
    SkKey* key = ring->head;
    while (key) {
        ret++;
        key = key->next;
    }
    return ret;
}
