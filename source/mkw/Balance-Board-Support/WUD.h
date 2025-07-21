#pragma once

#include <gct-use-cpp.h>
#include <types.h>
#include <string.h>

#define WUD_DEV_NAME_IS(devName_, str_) (memcmp(devName_, str_, sizeof(str_) - 1) == 0)

#define WUD_DEV_NAME_IS_CNT(devName_) WUD_DEV_NAME_IS(devName_, "Nintendo RVL-CNT")
#define WUD_DEV_NAME_IS_CNT_01(devName_) WUD_DEV_NAME_IS(devName_, "Nintendo RVL-CNT-01")
#define WUD_DEV_NAME_IS_WBC(devName_) WUD_DEV_NAME_IS(devName_, "Nintendo RVL-WBC")

#define BD_ADDR_LEN 6 /* Device address length */
typedef u8 BD_ADDR[BD_ADDR_LEN]; /* Device address */
typedef u8* BD_ADDR_PTR; /* Pointer to Device Address */

#define LINK_KEY_LEN 16
typedef u8 LINK_KEY[LINK_KEY_LEN]; /* Link Key */

struct small_dev_info {
    SIZE_ASSERT(0x40);
    char devName[20]; // size 0x14? offset 0x00 // might be 0x13?
    char at_0x14[1]; // size 0x??, offset 0x14?
    char __pad0[0x20 - (0x14 + 0x01)];
    LINK_KEY linkKey; // size 0x10, offset 0x20
    char __pad1[0x10];
};

struct WUDDevInfo {
    struct small_dev_info small;
};
