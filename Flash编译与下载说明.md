# Flash 编译与下载说明

## 当前配置

工程目标芯片为 `TMS320F28377D`。Debug 和 Release 均使用 `CMD/2837x_FLASH_lnk_cpu1.cmd`，并定义 `CPU1` 与 `_FLASH`。

`Debug` 这一名称只表示保留符号信息，不能据此判断程序位于 RAM 还是 Flash。是否为 Flash 程序应以链接文件和 map 地址为准。当前 Debug map 的启动入口为 `0x080000`，因此它是可调试的 Flash 映像。

## 构建产物

- `Debug/Lab14_Example_2837xD_IIC_liushui.out`：CCS 下载使用。
- `Debug/Lab14_Example_2837xD_IIC_liushui.hex`：外部烧录工具使用。
- `Debug/Lab14_Example_2837xD_IIC_liushui.map`：核对段地址和内存占用。

## 运行检查

1. CCS 下载 `.out` 后点击运行，两只程序灯应每 250 ms 交替。
2. `codestart` 必须位于 `0x080000`。
3. `.text` 和 `.econst` 必须位于 Flash，`ramfuncs` 必须具有 Flash 加载地址和 RAM 运行地址。
4. 断开仿真器并重新上电前，确认板卡硬件启动模式配置为 Flash Boot；否则 Flash 中已有程序也不会被 Boot ROM 启动。

当前尚未通过实际开发板完成下载和断电重启验证，最终硬件结论以该验证为准。
