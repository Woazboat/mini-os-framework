#include "multiboot2.h"
#include "stdint.h"

struct multiboot_struct
{
        struct multiboot_header header;
        struct multiboot_header_tag end;
};

__attribute__((section (".mboot"), unused)) static const struct multiboot_struct mboot = {
        {
                .magic = MULTIBOOT2_HEADER_MAGIC,
                .architecture = MULTIBOOT_ARCHITECTURE_I386,
                .header_length = sizeof(struct multiboot_struct),
                .checksum = -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + sizeof(struct multiboot_struct)),
        },
        {
                .type = MULTIBOOT_HEADER_TAG_END,
                .flags = 0,
                .size = 8,
        }
};
