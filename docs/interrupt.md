<!--
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 14:13:43
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 14:17:12
 * @Copyright: MIT License
 * @Description: 
-->
# Interrupt

## Interrupt Test

In our threads, we have already executed 'sti' to enable interrupts. Now we need to test whether the interrupt works.

```c
void _ofp idle_thread()
{
    asm volatile("sti\n");
    ...
}
```
Now, Let's test the interrupt by asm code. (in start.asm)

```c
int 0x80;
```

We can see that the interrupt works. Init thread and idle thread are interrupted by the interrupt.