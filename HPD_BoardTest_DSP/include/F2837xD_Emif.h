//###########################################################################
//
// FILE:   F2837x_Emif.h
//
// TITLE:  F2837x Device Emif Register Definitions.
//
// BUILD:  CUSTOMER RELEASE
//
//###########################################################################
// $TI Release: F2837xD Support Library v110 $
// $Release Date: Mon Apr  7 10:35:37 CDT 2014 $
//###########################################################################

#ifndef F2837x_Emif_H
#define F2837x_Emif_H
#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------
// Emif Individual Register Bit Definitions:

struct RCSR_BITS {                                    // bit descriptions
    Uint16  MINOR_REVISION:8;                         // 7:0 Minor Revision. 
    Uint16  MAJOR_REVISION:8;                         // 15:8 Major Revision. 
    Uint16  MODULE_ID:14;                             // 29:16 EMIF module ID. 
    Uint16  FR:1;                                     // 30 EMIF is running in full rate or half rate. 
    Uint16  BE:1;                                     // 31 EMIF endian mode. 
};

union RCSR_REG {
    Uint32  all;  
    struct  RCSR_BITS  bit;  
};

struct ASYNC_WCCR_BITS {                              // bit descriptions
    Uint16  MAX_EXT_WAIT:8;                           // 7:0 Maximum Extended Wait cycles. 
    Uint16  rsvd1:8;                                  // 15:8 Reserved 
    Uint16  CS2_WAIT:2;                               // 17:16 Maps the wait signal for chip select 2. 
    Uint16  CS3_WAIT:2;                               // 19:18 Maps the wait signal for chip select 3. 
    Uint16  CS4_WAIT:2;                               // 21:20 Maps the wait signal for chip select 4. 
    Uint16  CS5_WAIT:2;                               // 23:22 Maps the wait signal for chip select 5. 
    Uint16  rsvd2:4;                                  // 27:24 Reserved 
    Uint16  WP0:1;                                    // 28 Wait Polarity for pad_wait_i[0]. 
    Uint16  WP1:1;                                    // 29 Wait Polarity for pad_wait_i[1]. 
    Uint16  WP2:1;                                    // 30 Wait Polarity for pad_wait_i[2]. 
    Uint16  WP3:1;                                    // 31 Wait Polarity for pad_wait_i[3]. 
};

union ASYNC_WCCR_REG {
    Uint32  all;  
    struct  ASYNC_WCCR_BITS  bit;  
};

struct SDRAM_CR_BITS {                                // bit descriptions
    Uint16  PAGESIGE:3;                               // 2:0 Page Size. 
    Uint16  EBANK:1;                                  // 3 External chip select setup. 
    Uint16  IBANK:3;                                  // 6:4 Internal Bank setup of SDRAM devices. 
    Uint16  rsvd1:1;                                  // 7:7 Reserved 
    Uint16  BIT_11_9_LOCK:1;                          // 8 Bits 11 to 9 can only be written if this bit is set to 1. 
    Uint16  CL:3;                                     // 11:9 CAS Latency. 
    Uint16  BIT_13_LOCK:1;                            // 12 Bits 13 can only be written if this bit is set to 1. 
    Uint16  rsvd2:1;                                  // 13:13 Reserved 
    Uint16  NM:1;                                     // 14 Narrow Mode. 
    Uint16  rsvd3:1;                                  // 15:15 Reserved 
    Uint16  BIT_25_17_LOCK:1;                         // 16 Bits 25 to 17 can only be written if this bit is set to 1. 
    Uint16  rsvd4:2;                                  // 18:17 Reserved 
    Uint16  IBANK_POS:1;                              // 19 Internal bank position. 
    Uint16  ROWSIZE:3;                                // 22:20 Row Size. 
    Uint16  PASR:3;                                   // 25:23 Partial Array Self Refresh. 
    Uint16  rsvd5:3;                                  // 28:26 Reserved 
    Uint16  PDWR:1;                                   // 29 Perform refreshes during Power Down. 
    Uint16  PD:1;                                     // 30 Power Down. 
    Uint16  SR:1;                                     // 31 Self Refresh. 
};

union SDRAM_CR_REG {
    Uint32  all;  
    struct  SDRAM_CR_BITS  bit;  
};

struct SDRAM_RCR_BITS {                               // bit descriptions
    Uint16  REFRESH_RATE:13;                          // 12:0 Refresh Rate. 
    Uint16  rsvd1:3;                                  // 15:13 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union SDRAM_RCR_REG {
    Uint32  all;  
    struct  SDRAM_RCR_BITS  bit;  
};

struct ASYNC_CS2_CR_BITS {                            // bit descriptions
    Uint16  ASIZE:2;                                  // 1:0 Asynchronous Memory Size. 
    Uint16  TA:2;                                     // 3:2 Turn Around cycles. 
    Uint16  R_HOLD:3;                                 // 6:4 Read Strobe Hold cycles. 
    Uint16  R_STROBE:6;                               // 12:7 Read Strobe Duration cycles. 
    Uint32  R_SETUP:4;                                // 16:13 Read Strobe Setup cycles. 
    Uint16  W_HOLD:3;                                 // 19:17 Write Strobe Hold cycles. 
    Uint16  W_STROBE:6;                               // 25:20 Write Strobe Duration cycles. 
    Uint16  W_SETUP:4;                                // 29:26 Write Strobe Setup cycles. 
    Uint16  EW:1;                                     // 30 Extend Wait mode. 
    Uint16  SS:1;                                     // 31 Select Strobe mode. 
};

union ASYNC_CS2_CR_REG {
    Uint32  all;  
    struct  ASYNC_CS2_CR_BITS  bit;  
};

struct ASYNC_CS3_CR_BITS {                            // bit descriptions
    Uint16  ASIZE:2;                                  // 1:0 Asynchronous Memory Size. 
    Uint16  TA:2;                                     // 3:2 Turn Around cycles. 
    Uint16  R_HOLD:3;                                 // 6:4 Read Strobe Hold cycles. 
    Uint16  R_STROBE:6;                               // 12:7 Read Strobe Duration cycles. 
    Uint32  R_SETUP:4;                                // 16:13 Read Strobe Setup cycles. 
    Uint16  W_HOLD:3;                                 // 19:17 Write Strobe Hold cycles. 
    Uint16  W_STROBE:6;                               // 25:20 Write Strobe Duration cycles. 
    Uint16  W_SETUP:4;                                // 29:26 Write Strobe Setup cycles. 
    Uint16  EW:1;                                     // 30 Extend Wait mode. 
    Uint16  SS:1;                                     // 31 Select Strobe mode. 
};

union ASYNC_CS3_CR_REG {
    Uint32  all;  
    struct  ASYNC_CS3_CR_BITS  bit;  
};

struct ASYNC_CS4_CR_BITS {                            // bit descriptions
    Uint16  ASIZE:2;                                  // 1:0 Asynchronous Memory Size. 
    Uint16  TA:2;                                     // 3:2 Turn Around cycles. 
    Uint16  R_HOLD:3;                                 // 6:4 Read Strobe Hold cycles. 
    Uint16  R_STROBE:6;                               // 12:7 Read Strobe Duration cycles. 
    Uint32  R_SETUP:4;                                // 16:13 Read Strobe Setup cycles. 
    Uint16  W_HOLD:3;                                 // 19:17 Write Strobe Hold cycles. 
    Uint16  W_STROBE:6;                               // 25:20 Write Strobe Duration cycles. 
    Uint16  W_SETUP:4;                                // 29:26 Write Strobe Setup cycles. 
    Uint16  EW:1;                                     // 30 Extend Wait mode. 
    Uint16  SS:1;                                     // 31 Select Strobe mode. 
};

union ASYNC_CS4_CR_REG {
    Uint32  all;  
    struct  ASYNC_CS4_CR_BITS  bit;  
};

struct ASYNC_CS5_CR_BITS {                            // bit descriptions
    Uint16  ASIZE:2;                                  // 1:0 Asynchronous Memory Size. 
    Uint16  TA:2;                                     // 3:2 Turn Around cycles. 
    Uint16  R_HOLD:3;                                 // 6:4 Read Strobe Hold cycles. 
    Uint16  R_STROBE:6;                               // 12:7 Read Strobe Duration cycles. 
    Uint32  R_SETUP:4;                                // 16:13 Read Strobe Setup cycles. 
    Uint16  W_HOLD:3;                                 // 19:17 Write Strobe Hold cycles. 
    Uint16  W_STROBE:6;                               // 25:20 Write Strobe Duration cycles. 
    Uint16  W_SETUP:4;                                // 29:26 Write Strobe Setup cycles. 
    Uint16  EW:1;                                     // 30 Extend Wait mode. 
    Uint16  SS:1;                                     // 31 Select Strobe mode. 
};

union ASYNC_CS5_CR_REG {
    Uint32  all;  
    struct  ASYNC_CS5_CR_BITS  bit;  
};

struct SDRAM_TR_BITS {                                // bit descriptions
    Uint16  rsvd1:4;                                  // 3:0 Reserved 
    Uint16  T_RRD:3;                                  // 6:4 Activate to Activate timing for different bank. 
    Uint16  rsvd2:1;                                  // 7:7 Reserved 
    Uint16  T_RC:4;                                   // 11:8 Activate to Activate timing . 
    Uint16  T_RAS:4;                                  // 15:12 Activate to Precharge timing. 
    Uint16  T_WR:3;                                   // 18:16 Last Write to Precharge timing. 
    Uint16  rsvd3:1;                                  // 19:19 Reserved 
    Uint16  T_RCD:3;                                  // 22:20 Activate to Read/Write timing. 
    Uint16  rsvd4:1;                                  // 23:23 Reserved 
    Uint16  T_RP:3;                                   // 26:24 Precharge to Activate/Refresh timing. 
    Uint16  T_RFC:5;                                  // 31:27 Refresh/Load Mode to Refresh/Activate timing 
};

union SDRAM_TR_REG {
    Uint32  all;  
    struct  SDRAM_TR_BITS  bit;  
};

struct SDR_EXT_TMNG_BITS {                            // bit descriptions
    Uint16  T_XS:5;                                   // 4:0 Self Refresh exit to new command timing. 
    Uint16  rsvd1:11;                                 // 15:5 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union SDR_EXT_TMNG_REG {
    Uint32  all;  
    struct  SDR_EXT_TMNG_BITS  bit;  
};

struct INT_RAW_BITS {                                 // bit descriptions
    Uint16  AT:1;                                     // 0 Asynchronous Timeout. 
    Uint16  LT:1;                                     // 1 Line Trap. 
    Uint16  WR:4;                                     // 5:2 Wait Rise. 
    Uint16  rsvd1:10;                                 // 15:6 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union INT_RAW_REG {
    Uint32  all;  
    struct  INT_RAW_BITS  bit;  
};

struct INT_MSK_BITS {                                 // bit descriptions
    Uint16  AT_MASKED:1;                              // 0 Asynchronous Timeout. 
    Uint16  LT_MASKED:1;                              // 1 Line Trap. 
    Uint16  WR_MASKED:4;                              // 5:2 Wait Rise. 
    Uint16  rsvd1:10;                                 // 15:6 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union INT_MSK_REG {
    Uint32  all;  
    struct  INT_MSK_BITS  bit;  
};

struct INT_MSK_SET_BITS {                             // bit descriptions
    Uint16  AT_MASK_SET:1;                            // 0 Asynchronous Timeout. 
    Uint16  LT_MASK_SET:1;                            // 1 Line Trap. 
    Uint16  WR_MASK_SET:4;                            // 5:2 Wait Rise. 
    Uint16  rsvd1:10;                                 // 15:6 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union INT_MSK_SET_REG {
    Uint32  all;  
    struct  INT_MSK_SET_BITS  bit;  
};

struct INT_MSK_CLR_BITS {                             // bit descriptions
    Uint16  AT_MASK_CLR:1;                            // 0 Asynchronous Timeout. 
    Uint16  LT_MASK_CLR:1;                            // 1 Line Trap. 
    Uint16  WR_MASK_CLR:4;                            // 5:2 Wait Rise. 
    Uint16  rsvd1:10;                                 // 15:6 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union INT_MSK_CLR_REG {
    Uint32  all;  
    struct  INT_MSK_CLR_BITS  bit;  
};

struct IO_CTRL_BITS {                                 // bit descriptions
    Uint16  IO_CTRL:16;                               // 15:0 VTP calibration control for the IOs 
    Uint16  rsvd1:16;                                 // 31:16 Reserved 
};

union IO_CTRL_REG {
    Uint32  all;  
    struct  IO_CTRL_BITS  bit;  
};

struct IO_STAT_BITS {                                 // bit descriptions
    Uint16  IO_STAT:16;                               // 15:0 VTP calibration status of the IOs 
    Uint16  rsvd1:16;                                 // 31:16 Reserved 
};

union IO_STAT_REG {
    Uint32  all;  
    struct  IO_STAT_BITS  bit;  
};

struct MODEL_REL_NUM_BITS {                           // bit descriptions
    Uint16  RELEASE_NUM:8;                            // 7:0 Release Number. 
    Uint16  rsvd1:8;                                  // 15:8 Reserved 
    Uint16  rsvd2:16;                                 // 31:16 Reserved 
};

union MODEL_REL_NUM_REG {
    Uint32  all;  
    struct  MODEL_REL_NUM_BITS  bit;  
};

struct EMIF_REGS {
    union   RCSR_REG             RCSR;                // Revision Code and Status Register 
    union   ASYNC_WCCR_REG       ASYNC_WCCR;          // Async Wait Cycle Config Register 
    union   SDRAM_CR_REG         SDRAM_CR;            // SDRAM (pad_cs_o_n[0]/pad_cs_o_n[1]) Config Register 
    union   SDRAM_RCR_REG        SDRAM_RCR;           // SDRAM Refresh Control Register 
    union   ASYNC_CS2_CR_REG     ASYNC_CS2_CR;        // Async 1 (pad_cs_o_n[2]) Config Register 
    union   ASYNC_CS3_CR_REG     ASYNC_CS3_CR;        // Async 2 (pad_cs_o_n[3]) Config Register 
    union   ASYNC_CS4_CR_REG     ASYNC_CS4_CR;        // Async 3 (pad_cs_o_n[4]) Config Register 
    union   ASYNC_CS5_CR_REG     ASYNC_CS5_CR;        // Async 4 (pad_cs_o_n[5]) Config Register 
    union   SDRAM_TR_REG         SDRAM_TR;            // SDRAM Timing Register 
    Uint16                       rsvd1[6];            // Reserved 
    Uint32                       TOTAL_SDRAM_AR;      // Total SDRAM Accesses Register 
    Uint32                       TOTAL_SDRAM_ACTR;    // Total SDRAM Activate Register 
    Uint16                       rsvd2[2];            // Reserved 
    union   SDR_EXT_TMNG_REG     SDR_EXT_TMNG;        // SDRAM SR/PD Exit Timing Register 
    union   INT_RAW_REG          INT_RAW;             // Interrupt Raw Register 
    union   INT_MSK_REG          INT_MSK;             // Interrupt Masked Register 
    union   INT_MSK_SET_REG      INT_MSK_SET;         // Interrupt Mask Set Register 
    union   INT_MSK_CLR_REG      INT_MSK_CLR;         // Interrupt Mask Clear Register 
    union   IO_CTRL_REG          IO_CTRL;             // IO Control Register 
    union   IO_STAT_REG          IO_STAT;             // IO Status Register 
    Uint16                       rsvd3[42];           // Reserved 
    union   MODEL_REL_NUM_REG    MODEL_REL_NUM;       // Module Release Number Register 
    Uint16                       rsvd4[24];           // Reserved 
};

//---------------------------------------------------------------------------
// Emif External References & Function Declarations:
//

extern volatile struct EMIF_REGS Emif1Regs;
extern volatile struct EMIF_REGS Emif2Regs;

#ifdef __cplusplus
}
#endif                                                /* extern "C" */


#endif                                                // end of F2837x_EMIF_H definition
//===========================================================================
// End of file.
//===========================================================================
