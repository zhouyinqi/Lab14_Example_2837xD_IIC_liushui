# HPD 板卡测试项目

上位机和 DSP 下位机为两个同级工程，共用本目录的 Git 仓库。

```text
HPD_BoardTest/
  HPD_BoardTest_DSP/   CCS 下位机工程，F28377D CPU1
    Source/           外设测试和调度实现
    include/          接口、板型和协议定义
    CMD/              链接文件
    Lib/              DSP 运行库
    Tests/            主机测试、工程配置检查
    Debug/            HPD_BoardTest_DSP.out
  UpperComputer/      Qt 上位机工程
    src/              界面、TCP、串口和 CAN 客户端
    Tools/            协议冒烟测试、32 位 CAN 桥接
    build/            Qt 编译结果及运行依赖
  platforms/          保留的 Qt Android 示例，不参与 DSP 编译
  tools/              文档辅助工具
  outputs/            文档产物和迁移前构建缓存备份
```

## 打开和测试

1. CCS 导入 `HPD_BoardTest_DSP`，选择 **Debug**，下载 `Debug/HPD_BoardTest_DSP.out`。不要再打开旧的 Lab14 工程。
2. Qt Creator 打开 `UpperComputer/CMakeLists.txt`，选择 Desktop Qt 6.11.1 MinGW 64-bit Debug。
3. 也可直接运行 `UpperComputer/build/Desktop_Qt_6_11_1_MinGW_64_bit_Debug/HpdBoardTestHost.exe`。
4. CAN 库选择程序目录下的 `CanDriver/ZLG32/ControlCAN.dll`，同级 `kerneldlls` 文件夹必须完整保留。

在本目录执行主机测试：

```powershell
& .\HPD_BoardTest_DSP\Tests\run_host_tests.ps1
```

下载 DSP 程序、连接网线后，可执行只读协议检查：

```powershell
python .\UpperComputer\Tools\protocol_smoke_test.py
```

此次调整只涉及目录、工程名称和路径引用，不改变 Test ID、TCP 协议、板型配置或测试算法。Git 历史和原远程地址保留，板上验证后再提交。
