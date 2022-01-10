#include <stdint.h>
#include <stddef.h>
#include "stivale2.h"

static uint8_t stack[8192];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {
        if (current_tag == NULL) {
            return NULL;
        }
        if (current_tag->identifier == id) {
            return current_tag;
        }
        current_tag = (void *)current_tag->next;
    }
}

const char * NOCOLOR = "\x001b[0m";
const char * RED = "\x1b[31m";
const char * BLACK = "\x1b[30m";
const char * GREEN = "\x1b[32m";
const char * YELLOW = "\x1b[33m";
const char * BLUE = "\x1b[34m";
const char * MAGENTA = "\x1b[35m";
const char * CYAN = "\x1b[36m";
const char * WHITE = "\x1b[37m";
const char * BRIGHTRED = "\x1b[1;31m";
const char * BRIGHTBLACK = "\x1b[1;30m";
const char * BRIGHTGREEN = "\x1b[1;32m";
const char * BRIGHTYELLOW = "\x1b[1;33m";
const char * BRIGHTBLUE = "\x1b[1;34m";
const char * BRIGHTMAGENTA = "\x1b[1;35m";
const char * BRIGHTCYAN = "\x1b[1;36m";
const char * BRIGHTWHITE = "\x1b[1;37m";
const char * PANIC = "\x1b[2;37;41m";
const char * INFO = "\x1b[2;37;44m";
const char * SUCCESS = "\x1b[1;37;42m";
const char * SKYBLUE = "\x1b[1;34;24m";

size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

void (*twrite)(const char *string, size_t length);

void write(const char* string) {
    twrite(string, strlen(string));
}

char * concat(char * string1, char * string2) {
    int i, j = 0;
    for (i=strlen(string1); string2[j]!='\0'; i++) {
        string1[i] = string2[j];
        j++;
    }
    return string1;
}

char * remove(char * str, int index) {
    char * temp = str;
    temp[strlen(str)-index] = '\0';
    return temp;
}

void _start(struct stivale2_struct *stivale2_struct) {
    struct stivale2_struct_tag_terminal *term_str_tag;
    term_str_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_TERMINAL_ID);
 
    if (term_str_tag == NULL) {
        for (;;) {
            asm ("hlt");
        }
    }
    void *term_write_ptr = (void *)term_str_tag->term_write;
    twrite = term_write_ptr;

    write(SUCCESS);
    write("stivale2.h     loaded\n");
    write("src/kernel.c   loaded\n");
    write("limine.cfg     loaded\n");
    for (;;) {
        asm ("hlt");
    }
}
