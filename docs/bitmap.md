<!--
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-10-31 00:25:14
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 10:23:24
 * @Copyright: MIT License
 * @Description: 
-->
# Bitmap

## Test

```c
#include <snailix/bitmap.h>

bitmap_t bitmap;
#define BITMAP_SIZE 2
u8 buffer[BITMAP_SIZE];

void bitmap_test()
{
    bitmap_init(&bitmap, buffer, BITMAP_SIZE, 0);
    for (size_t i = 0; i < 16; i++)
    {
        bitmap_set(&bitmap, i, 1);
        printk("buffer: %d, %d\n", buffer[0], buffer[1]);
    }
}
```