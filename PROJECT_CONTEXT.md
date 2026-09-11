# 项目上下文

更新时间：2026-09-11

## 目标与工程

- F28377D CPU1 板级自动化测试，DSP 下位机与 Qt 6 上位机配合。
- 板型：SiC 多传系统主控板、低压工业变频器 V04、低空经济统型板 V01。
- 项目根目录：C:/Users/zyq/Desktop/WORK/PCB_BOARD_TEST/HPD_BoardTest。
- DSP：HPD_BoardTest_DSP；Qt：UpperComputer；不再使用旧 Lab14 目录开发。
- Git：main；origin 为 https://github.com/zhouyinqi/Lab14_Example_2837xD_IIC_liushui.git。

## 长期约束

- 每次任务先读取 AGENTS.md 和本文件；每阶段结束替换过期进度，同时同步 PROJECT_STATUS.md。
- 每次任务完成后提交 Git 并推送；失败保留本地提交并准确报告。报告包含修改、文件、验证、结果、烧录/真机需求、风险六项。
- Test ID 绑定板型、硬件版本和实现/验证状态，保持原协议兼容。
- DSP_DI、DSP_DO、DSP_HDO、FPGA_DIDO 独立；W5300/W5500 使用独立引脚与驱动配置。
- 低空板 PWM 按 DIDO 验证；未经明确授权不启用功率驱动。PT4 0x031C 为已确认硬件问题，不做软件补偿。
- 不删除既有文件、不回退用户修改；不提交 build、Debug、.out、.map 等产物。
- platforms/ 与 tools/ 是既有未跟踪的示例/文档工具，本次沿用原记录不纳入板卡项目提交。

## 当前进度与下一阶段

- DSP/Qt 同级拆分完成，能力矩阵、硬件版本和 Test ID 状态同步已建立。
- 低空板已有实现及部分实板验证，详细验证边界见 PROJECT_STATUS.md 和工程说明。
- 低压 V04 使用 F28377D + EMIF W5300；能力与管脚矩阵、专用 ID 0x0400～0x040A 已建，专用测试仍待开发。
- 用户确认先完成核心板基础、通信/存储、ADC/PWM安全配置，再进行外部通信和0x0400～0x040A；软件通过即继续，实板状态单独记录。当前已建立 LOW_VOLTAGE_V04_REVIEW.md 覆盖表，正在修复核心基础缺口。
- 后续顺序：0x0401 DO/软启动/风扇、0x0402 HDO、0x0403 STO；其余 ADC、PT100、NTC、PWM、保护、AO、旋变仍待开发。
- 本阶段完成已有代码上传；已阅读规则、状态、工程说明和低压状态注册关键代码，全部已开发源码的逐文件阅读尚未完成，进入实现前继续完成，不能据此声称已全面审查。

## 已知问题

- 低空板 PT4 硬件异常；PWM 端子无电压的后级链路、保护故障位/D_BRK/外部中断仍待硬件确认。
- 2026-09-11 已成功将既有提交 c06fc49、f537253 推送至 origin/main，旧 github.com:443 阻塞已解除。
- 本轮进入软件开发；不自动烧录或操作真机。普通选择自行决定，假设明确记录；引脚/输出极性/存储保护不以猜测开放。

## 本阶段验证

- 2026-09-11：DSP run_host_tests.ps1 通过（源文件范围、测试调度、硬件矩阵）。
- Qt HpdHostTests 构建成功；ctest 1/1 通过，耗时约 34 秒。均为无硬件回归。

## 开发环境与补充资料

- Windows，CCS DSP 工程；Qt 6.11.1 MinGW 64 位，MinGW 13.1，CMake。DSP 编译器版本及当前烧录器型号待核对。
- 本阶段读取了临时出现的 AGENTS.md.md 补充规则和 PROJECT_CONTEXT.md.md 模板，补充规则已合入 AGENTS.md；随后原稿已被外部移除，实际状态维护在本文件。
