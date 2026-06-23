#ifndef BOARD_PINMAP_H
#define BOARD_PINMAP_H

/*
 * Confirmed from:
 * 系统主控制器DSP&FPGA引脚分配-V01-20260313(2).xls
 * Device: TMS320F28377D
 */

#define BOARD_DSP_F28377D                 1U

/* DSP1 to FPGA EMIF2 bus. */
#define BOARD_PIN_EMIF2_XBA1              112U
#define BOARD_PIN_EMIF2_XA(index)         (98U + (index))
#define BOARD_PIN_EMIF2_XBA0              111U
#define BOARD_PIN_EMIF2_XD(bit)           (68U - (bit))
#define BOARD_PIN_EMIF2_WE                120U
#define BOARD_PIN_EMIF2_CLK               118U
#define BOARD_PIN_EMIF2_CS2               116U
#define BOARD_PIN_EMIF2_WAIT              110U
#define BOARD_PIN_EMIF2_OE                121U
#define BOARD_PIN_EMIF2_RNW               119U

/* EMIF1 is shared by the W5300 Ethernet controller and external SRAM. */
#define BOARD_PIN_EMIF1_XBA1              92U
#define BOARD_PIN_EMIF1_XA(index)         \
    (((index) < 4U) ? (38U + (index)) :   \
     (((index) < 13U) ? (40U + (index)) : (73U + (index))))
#define BOARD_PIN_EMIF1_XD(bit)           (((bit) == 0U) ? 85U : (84U - (bit)))
#define BOARD_PIN_EMIF1_WE                31U
#define BOARD_PIN_EMIF1_W5300_CS2         34U
#define BOARD_PIN_EMIF1_SRAM_CS3          35U
#define BOARD_PIN_EMIF1_OE                37U

/* These Ethernet control pins are assigned to DSP2/CPU2 in the pin table. */
#define BOARD_CPU2_PIN_ETH_INT            0U
#define BOARD_CPU2_PIN_ETH_RESET          1U
#define BOARD_CPU2_PIN_ETH_BRDY           94U

/* Twelve ePWM A/B pairs are routed directly from DSP1 to the FPGA. */
#define BOARD_EPWM_PAIR_COUNT             12U
#define BOARD_PIN_EPWM_A(channel)         (145U + (((channel) - 1U) * 2U))
#define BOARD_PIN_EPWM_B(channel)         (146U + (((channel) - 1U) * 2U))

/* DSP1 and FPGA handshake/status GPIO. */
#define BOARD_PIN_DSP_TO_FPGA_ST1         140U
#define BOARD_PIN_DSP_TO_FPGA_ST2         141U
#define BOARD_PIN_DSP_TO_FPGA_ST3         142U
#define BOARD_PIN_DSP_TO_FPGA_RESET       139U
#define BOARD_PIN_DSP_TO_FPGA_LOCK        130U
#define BOARD_PIN_FPGA_TO_DSP_ST1         132U
#define BOARD_PIN_FPGA_TO_DSP_ST2         134U
#define BOARD_PIN_FPGA_TO_DSP_ST3         136U
#define BOARD_PIN_FPGA_TO_DSP_XINT        138U
#define BOARD_PIN_FPGA_TO_DSP_PLL         129U

/* Resolver interface. SPIA pins are shared with the resolver SPI. */
#define BOARD_PIN_RESOLVER_A              20U
#define BOARD_PIN_RESOLVER_B              21U
#define BOARD_PIN_RESOLVER_Z              23U
#define BOARD_PIN_RESOLVER_RES1           8U
#define BOARD_PIN_RESOLVER_RES0           7U
#define BOARD_PIN_RESOLVER_MODE           9U
#define BOARD_PIN_RESOLVER_DOS            5U
#define BOARD_PIN_RESOLVER_LOT            4U
#define BOARD_PIN_RESOLVER_RESET          10U
#define BOARD_PIN_RESOLVER_ENABLE         22U

/* SPI buses and chip selects. */
#define BOARD_PIN_SPIA_SIMO               16U
#define BOARD_PIN_SPIA_SOMI               17U
#define BOARD_PIN_SPIA_CLK                18U
#define BOARD_PIN_SPIA_CS                 19U
#define BOARD_PIN_SPIB_SIMO               24U
#define BOARD_PIN_SPIB_SOMI               25U
#define BOARD_PIN_SPIB_CLK                26U
#define BOARD_PIN_SPIB_CS                 27U
#define BOARD_PIN_SPIC_SIMO               122U
#define BOARD_PIN_SPIC_SOMI               123U
#define BOARD_PIN_SPIC_CLK                124U
#define BOARD_PIN_SPIC_FLASH_CS           125U
#define BOARD_PIN_SPIC_FRAM_CS            33U
#define BOARD_PIN_SPIC_FPGA_FLASH_CS      126U
#define BOARD_PIN_FPGA_PROGRAM            127U
#define BOARD_PIN_FPGA_DONE               128U

/* SCI-A/SCI-B are connected through RS-485 transceivers. */
#define BOARD_PIN_SCIA_DIRECTION          133U
#define BOARD_PIN_SCIA_TX                 135U
#define BOARD_PIN_SCIA_RX                 137U
#define BOARD_PIN_SCIB_DIRECTION          11U
#define BOARD_PIN_SCIB_TX                 14U
#define BOARD_PIN_SCIB_RX                 15U

/* External CAN connector J10 uses CAN-B. */
#define BOARD_PIN_CANB_RX                 13U
#define BOARD_PIN_CANB_TX                 12U

/* I2C-A RTC and I2C-B board temperature monitor. */
#define BOARD_PIN_I2CA_SCL                43U
#define BOARD_PIN_I2CA_SDA                42U
#define BOARD_PIN_I2CA_INT                114U
#define BOARD_PIN_I2CB_SCL                3U
#define BOARD_PIN_I2CB_SDA                2U
#define BOARD_PIN_I2CB_INT                6U

#define BOARD_PIN_LED1                    144U
#define BOARD_PIN_LED2                    143U
#define BOARD_PIN_WATCHDOG_INPUT          95U

#endif
