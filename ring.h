//
//  ring.h
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

#ifndef skiring_ring_h
#define skiring_ring_h

#include <stdbool.h>

typedef struct _SkStringSetItem {
    char* value;
    struct _SkStringSetItem* next;
} SkStringSetItem;

typedef struct _SkStringSet {
    SkStringSetItem* head;
} SkStringSet;

typedef struct _SkKey {
    char* name;
    char* value;
    SkStringSet* users;
    struct _SkKey* next;
} SkKey;

typedef struct _SkKeyRing {
    SkKey* head;
} SkKeyRing;

SkStringSet* SkStringSet_create();
int SkStringSet_count(SkStringSet* set);
SkStringSetItem* SkStringSet_add(SkStringSet* set, char* str);
bool SkStringSet_contains(SkStringSet* set, char* str);
void SkStringSet_remove(SkStringSet* set, char* str);

SkKey* SkKey_create(char* name, char* value, SkKey* next);

SkKeyRing* SkKeyRing_create();
SkKey* SkKeyRing_find(SkKeyRing* ring, char* name, SkKey** prev);
SkKey* SkKeyRing_put(SkKeyRing* ring, char* name, char* value);
void SkKeyRing_remove(SkKeyRing* ring, char* name);
int SkKeyRing_count(SkKeyRing* ring);

#endif
