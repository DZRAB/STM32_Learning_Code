#include "flash.h"

u8 alhour[10]; // 设置闹钟的小时，一共8个，其中8、9两位是温度报警的整数
u8 almin[10];  // 设置闹钟的分钟，一共8个，其中8、9两位是温度报警的小数

// FLASH写入数据
void FLASH_W(u32 add, u16 dat)
{ // 参数1：32位FLASH地址。参数2：16位数据
    //	 RCC_HSICmd(ENABLE); //打开HSI时钟
    FLASH_Unlock();                                                                            // 解锁FLASH编程擦除控制器
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); // 清除标志位
    FLASH_ErasePage(add);                                                                      // 擦除指定地址页
    FLASH_ProgramHalfWord(add, dat);                                                           // 从指定页的addr地址开始写
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); // 清除标志位
    FLASH_Lock();                                                                              // 锁定FLASH编程擦除控制器
}

// FLASH读出数据
u16 FLASH_R(u32 add)
{ // 参数1：32位读出FLASH地址。返回值：16位数据
    u16 a;
    a = *(u16 *)(add); // 从指定页的addr地址开始读
    return a;
}

void ALFlash_W(void) // 写闹钟数据到flash
{
    u16 t;
    u8 i;
    FLASH_Unlock();                                                                            // 解锁FLASH编程擦除控制器
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); // 清除标志位
    FLASH_ErasePage(FLASH_START_ADDR);                                                         // 擦除指定地址页
    for (i = 0; i < 10; i++)
    {
        t = (alhour[i] << 8) | almin[i];
        FLASH_ProgramHalfWord(FLASH_START_ADDR + i * 2, t);   //因为是16位，每次写2个字节,写数据的偏移量每次是2                                     // 从指定页的addr地址开始写
        FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); // 清除标志位
    }

    FLASH_Lock();
}
