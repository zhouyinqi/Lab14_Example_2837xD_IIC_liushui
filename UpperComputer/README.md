# HPD控制模组测试上位机

## Board Protocol Smoke Test

After downloading the Debug firmware and connecting Ethernet, run this command
from the project root:

```powershell
python .\UpperComputer\Tools\protocol_smoke_test.py
```

The script sends only `PING` and `GET_STATUS`; it does not start a test or drive
any output. Both replies must show `protocolStatus: 0`.
该目录是独立的 Qt 6 Windows 桌面工程，不修改 `platforms` 下的 Android 示例。

## 当前功能

- TCP 连接 DSP1 的 W5300 或 W5500，默认地址为 `192.168.1.111:12001`。
- DSP 自动识别 W5300/W5500 硬件配置；板型由操作员独立选择并确认，两者不互相绑定。
- 三个一键按钮：板载自检、外设连接后自检、HPD注入模拟量自检。
- 单项测试、DO安全确认、状态轮询和结果记录表。
- 以太网测试重配 W5300/W5500 期间，客户端每2秒自动重连；测试结束后自动恢复状态轮询。
- 协议使用固定长度帧和 CRC16，旧 `BTST -> BTOK` 连通性测试保持兼容。

## 构建

在 Qt Creator 中打开本目录的 `CMakeLists.txt`，选择 Qt 6 Windows Desktop Kit 构建运行。工程需要 `Qt6::Widgets` 和 `Qt6::Network`。

## DSP协议

请求为12字节：`BT`、版本、命令、序号、阶段、标志、参数、CRC16。

响应为40字节：`BR`、版本、命令、序号、协议状态、测试管理器状态、单条记录、CRC16。所有16/32位字段均为大端序；浮点数为IEEE-754单精度大端序。

当前命令包括：连接检测、三类自动测试、停止、单项测试、读取状态、读取单条记录。EDO/HDO单项测试必须携带输出解锁标志，Qt界面会要求人工确认。
