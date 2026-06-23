//###########################################################################
//
// FILE:   F2837x_Xbar.h
//
// TITLE:  F2837x Device Xbar Register Definitions.
//
// BUILD:  CUSTOMER RELEASE
//
//###########################################################################
// $TI Release: F2837xD Support Library v110 $
// $Release Date: Mon Apr  7 10:35:37 CDT 2014 $
//###########################################################################

#ifndef F2837x_Xbar_H
#define F2837x_Xbar_H
#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------
// Xbar Individual Register Bit Definitions:

struct INPUTLOCK_BITS {                                           // bit descriptions
    Uint16  INPUT1SELECT:1;                                          // 0 Lock bit for INPUT1SEL Register 
    Uint16  INPUT2SELECT:1;                                          // 1 Lock bit for INPUT2SEL Register 
    Uint16  INPUT3SELECT:1;                                          // 2 Lock bit for INPUT3SEL Register 
    Uint16  INPUT4SELECT:1;                                          // 3 Lock bit for INPUT4SEL Register 
    Uint16  INPUT5SELECT:1;                                          // 4 Lock bit for INPUT5SEL Register 
    Uint16  INPUT6SELECT:1;                                          // 5 Lock bit for INPUT7SEL Register 
    Uint16  INPUT7SELECT:1;                                          // 6 Lock bit for INPUT8SEL Register 
    Uint16  INPUT8SELECT:1;                                          // 7 Lock bit for INPUT9SEL Register 
    Uint16  INPUT9SELECT:1;                                          // 8 Lock bit for INPUT10SEL Register 
    Uint16  INPUT10SELECT:1;                                         // 9 Lock bit for INPUT11SEL Register 
    Uint16  INPUT11SELECT:1;                                         // 10 Lock bit for INPUT11SEL Register 
    Uint16  INPUT12SELECT:1;                                         // 11 Lock bit for INPUT12SEL Register 
    Uint16  INPUT13SELECT:1;                                         // 12 Lock bit for INPUT13SEL Register 
    Uint16  INPUT14SELECT:1;                                         // 13 Lock bit for INPUT14SEL Register 
    Uint16  INPUT15SELECT:1;                                         // 14 Lock bit for INPUT15SEL Register 
    Uint16  INPUT16SELECT:1;                                         // 15 Lock bit for INPUT16SEL Register 
    Uint16  rsvd1:16;                                              // 31:16 Reserved 
};

union INPUTLOCK_REG {
    Uint32  all;  
    struct  INPUTLOCK_BITS  bit;  
};

struct TRIGLATCH1_BITS {                                            // bit descriptions
    Uint16  CMPSS0_CTRIPL:1;                                       // 0 Input Latch for CMPSS0.CTRIPL Signal 
    Uint16  CMPSS0_CTRIPH:1;                                       // 1 Input Latch for CMPSS0.CTRIPH Signal 
    Uint16  CMPSS1_CTRIPL:1;                                       // 2 Input Latch for CMPSS1.CTRIPL Signal 
    Uint16  CMPSS1_CTRIPH:1;                                       // 3 Input Latch for CMPSS1.CTRIPH Signal 
    Uint16  CMPSS2_CTRIPL:1;                                       // 4 Input Latch for CMPSS2.CTRIPL Signal 
    Uint16  CMPSS2_CTRIPH:1;                                       // 5 Input Latch for CMPSS2.CTRIPH Signal 
    Uint16  CMPSS3_CTRIPL:1;                                       // 6 Input Latch for CMPSS3.CTRIPL Signal 
    Uint16  CMPSS3_CTRIPH:1;                                       // 7 Input Latch for CMPSS3.CTRIPH Signal 
    Uint16  CMPSS4_CTRIPL:1;                                       // 8 Input Latch for CMPSS4.CTRIPL Signal 
    Uint16  CMPSS4_CTRIPH:1;                                       // 9 Input Latch for CMPSS4.CTRIPH Signal 
    Uint16  CMPSS5_CTRIPL:1;                                       // 10 Input Latch for CMPSS5.CTRIPL Signal 
    Uint16  CMPSS5_CTRIPH:1;                                       // 11 Input Latch for CMPSS5.CTRIPH Signal 
    Uint16  CMPSS6_CTRIPL:1;                                       // 12 Input Latch for CMPSS6.CTRIPL Signal 
    Uint16  CMPSS6_CTRIPH:1;                                       // 13 Input Latch for CMPSS6.CTRIPH Signal 
    Uint16  CMPSS7_CTRIPL:1;                                       // 14 Input Latch for CMPSS7.CTRIPL Signal 
    Uint16  CMPSS7_CTRIPH:1;                                       // 15 Input Latch for CMPSS7.CTRIPH Signal 
    Uint16  CMPSS0_CTRIPOUTL:1;                                    // 16 Input Latch for CMPSS0.CTRIPOUTL Signal 
    Uint16  CMPSS0_CTRIPOUTH:1;                                    // 17 Input Latch for CMPSS0.CTRIPOUTH Signal 
    Uint16  CMPSS1_CTRIPOUTL:1;                                    // 18 Input Latch for CMPSS1.CTRIPOUTL Signal 
    Uint16  CMPSS1_CTRIPOUTH:1;                                    // 19 Input Latch for CMPSS1.CTRIPOUTH Signal 
    Uint16  CMPSS2_CTRIPOUTL:1;                                    // 20 Input Latch for CMPSS2.CTRIPOUTL Signal 
    Uint16  CMPSS2_CTRIPOUTH:1;                                    // 21 Input Latch for CMPSS2.CTRIPOUTH Signal 
    Uint16  CMPSS3_CTRIPOUTL:1;                                    // 22 Input Latch for CMPSS3.CTRIPOUTL Signal 
    Uint16  CMPSS3_CTRIPOUTH:1;                                    // 23 Input Latch for CMPSS3.CTRIPOUTH Signal 
    Uint16  CMPSS4_CTRIPOUTL:1;                                    // 24 Input Latch for CMPSS4.CTRIPOUTL Signal 
    Uint16  CMPSS4_CTRIPOUTH:1;                                    // 25 Input Latch for CMPSS4.CTRIPOUTH Signal 
    Uint16  CMPSS5_CTRIPOUTL:1;                                    // 26 Input Latch for CMPSS5.CTRIPOUTL Signal 
    Uint16  CMPSS5_CTRIPOUTH:1;                                    // 27 Input Latch for CMPSS5.CTRIPOUTH Signal 
    Uint16  CMPSS6_CTRIPOUTL:1;                                    // 28 Input Latch for CMPSS6.CTRIPOUTL Signal 
    Uint16  CMPSS6_CTRIPOUTH:1;                                    // 29 Input Latch for CMPSS6.CTRIPOUTH Signal 
    Uint16  CMPSS7_CTRIPOUTL:1;                                    // 30 Input Latch for CMPSS7.CTRIPOUTL Signal 
    Uint16  CMPSS7_CTRIPOUTH:1;                                    // 31 Input Latch for CMPSS7.CTRIPOUTH Signal 
};

union TRIGLATCH1_REG {
    Uint32  all;  
    struct  TRIGLATCH1_BITS  bit;  
};

struct TRIGLATCH2_BITS {                                            // bit descriptions
    Uint16  INPUT1:1;                                             // 0 Input Latch for INPUT1 Signal 
    Uint16  INPUT2:1;                                             // 1 Input Latch for INPUT2 Signal 
    Uint16  INPUT3:1;                                             // 2 Input Latch for INPUT3 Signal 
    Uint16  INPUT4:1;                                             // 3 Input Latch for INPUT4 Signal 
    Uint16  INPUT5:1;                                             // 4 Input Latch for INPUT5 Signal 
    Uint16  INPUT7:1;                                             // 5 Input Latch for INPUT7 Signal 
    Uint16  ADCSOCA:1;                                             // 6 Input Latch for ADCSOCA Signal 
    Uint16  ADCSOCB:1;                                             // 7 Input Latch for ADCSOCB Signal 
    Uint16  CLB1_4_1:1;                                            // 8 Input Latch for CLB1_4.1 Signal 
    Uint16  CLB1_5_1:1;                                            // 9 Input Latch for CLB1_5.1 Signal 
    Uint16  CLB2_4_1:1;                                            // 10 Input Latch for CLB2_4.1 Signal 
    Uint16  CLB2_5_1:1;                                            // 11 Input Latch for CLB2_5.1 Signal 
    Uint16  CLB3_4_1:1;                                            // 12 Input Latch for CLB3_4.1 Signal 
    Uint16  CLB3_5_1:1;                                            // 13 Input Latch for CLB3_5.1 Signal 
    Uint16  CLB4_4_1:1;                                            // 14 Input Latch for CLB4_4.1 Signal 
    Uint16  CLB4_5_1:1;                                            // 15 Input Latch for CLB4_5.1 Signal 
    Uint16  ECAP1_OUT:1;                                           // 16 Input Latch for ECAP1.OUT Signal 
    Uint16  ECAP2_OUT:1;                                           // 17 Input Latch for ECAP2.OUT Signal 
    Uint16  ECAP3_OUT:1;                                           // 18 Input Latch for ECAP3.OUT Signal 
    Uint16  ECAP4_OUT:1;                                           // 19 Input Latch for ECAP4.OUT Signal 
    Uint16  ECAP5_OUT:1;                                           // 20 Input Latch for ECAP5.OUT Signal 
    Uint16  ECAP6_OUT:1;                                           // 21 Input Latch for ECAP6.OUT Signal 
    Uint16  EXTSYNCOUT:1;                                          // 22 Input Latch for EXTSYNCOUT Signal 
    Uint16  ADCAEVT1:1;                                            // 23 Input Latch for ADCAEVT1 Signal 
    Uint16  ADCAEVT2:1;                                            // 24 Input Latch for ADCAEVT2 Signal 
    Uint16  ADCAEVT3:1;                                            // 25 Input Latch for ADCAEVT3 Signal 
    Uint16  ADCAEVT4:1;                                            // 26 Input Latch for ADCAEVT4 Signal 
    Uint16  ADCBEVT1:1;                                            // 27 Input Latch for ADCBEVT1 Signal 
    Uint16  ADCBEVT2:1;                                            // 28 Input Latch for ADCBEVT2 Signal 
    Uint16  ADCBEVT3:1;                                            // 29 Input Latch for ADCBEVT3 Signal 
    Uint16  ADCBEVT4:1;                                            // 30 Input Latch for ADCBEVT4 Signal 
    Uint16  ADCCEVT1:1;                                            // 31 Input Latch for ADCCEVT1 Signal 
};

union TRIGLATCH2_REG {
    Uint32  all;  
    struct  TRIGLATCH2_BITS  bit;  
};

struct TRIGLATCH3_BITS {                                            // bit descriptions
    Uint16  ADCCEVT2:1;                                            // 0 Input Latch for ADCCEVT2 Signal 
    Uint16  ADCCEVT3:1;                                            // 1 Input Latch for ADCCEVT3 Signal 
    Uint16  ADCCEVT4:1;                                            // 2 Input Latch for ADCCEVT4 Signal 
    Uint16  ADCDEVT1:1;                                            // 3 Input Latch for ADCDEVT1 Signal 
    Uint16  ADCDEVT2:1;                                            // 4 Input Latch for ADCDEVT2 Signal 
    Uint16  ADCDEVT3:1;                                            // 5 Input Latch for ADCDEVT3 Signal 
    Uint16  ADCDEVT4:1;                                            // 6 Input Latch for ADCDEVT4 Signal 
    Uint16  SD1FLT1_COMPL:1;                                       // 7 Input Latch for SD1FLT1.COMPL Signal 
    Uint16  SD1FLT1_COMPH:1;                                       // 8 Input Latch for SD1FLT1.COMPH Signal 
    Uint16  SD1FLT2_COMPL:1;                                       // 9 Input Latch for SD1FLT2.COMPL Signal 
    Uint16  SD1FLT2_COMPH:1;                                       // 10 Input Latch for SD1FLT2.COMPH Signal 
    Uint16  SD1FLT3_COMPL:1;                                       // 11 Input Latch for SD1FLT3.COMPL Signal 
    Uint16  SD1FLT3_COMPH:1;                                       // 12 Input Latch for SD1FLT3.COMPH Signal 
    Uint16  SD1FLT4_COMPL:1;                                       // 13 Input Latch for SD1FLT4.COMPL Signal 
    Uint16  SD1FLT4_COMPH:1;                                       // 14 Input Latch for SD1FLT4.COMPH Signal 
    Uint16  SD2FLT1_COMPL:1;                                       // 15 Input Latch for SD2FLT1.COMPL Signal 
    Uint16  SD2FLT1_COMPH:1;                                       // 16 Input Latch for SD2FLT1.COMPH Signal 
    Uint16  SD2FLT2_COMPL:1;                                       // 17 Input Latch for SD2FLT2.COMPL Signal 
    Uint16  SD2FLT2_COMPH:1;                                       // 18 Input Latch for SD2FLT2.COMPH Signal 
    Uint16  SD2FLT3_COMPL:1;                                       // 19 Input Latch for SD2FLT3.COMPL Signal 
    Uint16  SD2FLT3_COMPH:1;                                       // 20 Input Latch for SD2FLT3.COMPH Signal 
    Uint16  SD2FLT4_COMPL:1;                                       // 21 Input Latch for SD2FLT4.COMPL Signal 
    Uint16  SD2FLT4_COMPH:1;                                       // 22 Input Latch for SD2FLT4.COMPH Signal 
    Uint16  rsvd1:9;                                               // 31:23 Reserved 
};

union TRIGLATCH3_REG {
    Uint32  all;  
    struct  TRIGLATCH3_BITS  bit;  
};

struct TRIGLATCHCLR1_BITS {                                         // bit descriptions
    Uint16  CMPSS0_CTRIPL:1;                                       // 0 Input Latch Clear for CMPSS0.CTRIPL Signal 
    Uint16  CMPSS0_CTRIPH:1;                                       // 1 Input Latch Clear for CMPSS0.CTRIPH Signal 
    Uint16  CMPSS1_CTRIPL:1;                                       // 2 Input Latch Clear for CMPSS1.CTRIPL Signal 
    Uint16  CMPSS1_CTRIPH:1;                                       // 3 Input Latch Clear for CMPSS1.CTRIPH Signal 
    Uint16  CMPSS2_CTRIPL:1;                                       // 4 Input Latch Clear for CMPSS2.CTRIPL Signal 
    Uint16  CMPSS2_CTRIPH:1;                                       // 5 Input Latch Clear for CMPSS2.CTRIPH Signal 
    Uint16  CMPSS3_CTRIPL:1;                                       // 6 Input Latch Clear for CMPSS3.CTRIPL Signal 
    Uint16  CMPSS3_CTRIPH:1;                                       // 7 Input Latch Clear for CMPSS3.CTRIPH Signal 
    Uint16  CMPSS4_CTRIPL:1;                                       // 8 Input Latch Clear for CMPSS4.CTRIPL Signal 
    Uint16  CMPSS4_CTRIPH:1;                                       // 9 Input Latch Clear for CMPSS4.CTRIPH Signal 
    Uint16  CMPSS5_CTRIPL:1;                                       // 10 Input Latch Clear for CMPSS5.CTRIPL Signal 
    Uint16  CMPSS5_CTRIPH:1;                                       // 11 Input Latch Clear for CMPSS5.CTRIPH Signal 
    Uint16  CMPSS6_CTRIPL:1;                                       // 12 Input Latch Clear for CMPSS6.CTRIPL Signal 
    Uint16  CMPSS6_CTRIPH:1;                                       // 13 Input Latch Clear for CMPSS6.CTRIPH Signal 
    Uint16  CMPSS7_CTRIPL:1;                                       // 14 Input Latch Clear for CMPSS7.CTRIPL Signal 
    Uint16  CMPSS7_CTRIPH:1;                                       // 15 Input Latch Clear for CMPSS7.CTRIPH Signal 
    Uint16  CMPSS0_CTRIPOUTL:1;                                    // 16 Input Latch Clear for CMPSS0.CTRIPOUTL Signal 
    Uint16  CMPSS0_CTRIPOUTH:1;                                    // 17 Input Latch Clear for CMPSS0.CTRIPOUTH Signal 
    Uint16  CMPSS1_CTRIPOUTL:1;                                    // 18 Input Latch Clear for CMPSS1.CTRIPOUTL Signal 
    Uint16  CMPSS1_CTRIPOUTH:1;                                    // 19 Input Latch Clear for CMPSS1.CTRIPOUTH Signal 
    Uint16  CMPSS2_CTRIPOUTL:1;                                    // 20 Input Latch Clear for CMPSS2.CTRIPOUTL Signal 
    Uint16  CMPSS2_CTRIPOUTH:1;                                    // 21 Input Latch Clear for CMPSS2.CTRIPOUTH Signal 
    Uint16  CMPSS3_CTRIPOUTL:1;                                    // 22 Input Latch Clear for CMPSS3.CTRIPOUTL Signal 
    Uint16  CMPSS3_CTRIPOUTH:1;                                    // 23 Input Latch Clear for CMPSS3.CTRIPOUTH Signal 
    Uint16  CMPSS4_CTRIPOUTL:1;                                    // 24 Input Latch Clear for CMPSS4.CTRIPOUTL Signal 
    Uint16  CMPSS4_CTRIPOUTH:1;                                    // 25 Input Latch Clear for CMPSS4.CTRIPOUTH Signal 
    Uint16  CMPSS5_CTRIPOUTL:1;                                    // 26 Input Latch Clear for CMPSS5.CTRIPOUTL Signal 
    Uint16  CMPSS5_CTRIPOUTH:1;                                    // 27 Input Latch Clear for CMPSS5.CTRIPOUTH Signal 
    Uint16  CMPSS6_CTRIPOUTL:1;                                    // 28 Input Latch Clear for CMPSS6.CTRIPOUTL Signal 
    Uint16  CMPSS6_CTRIPOUTH:1;                                    // 29 Input Latch Clear for CMPSS6.CTRIPOUTH Signal 
    Uint16  CMPSS7_CTRIPOUTL:1;                                    // 30 Input Latch Clear for CMPSS7.CTRIPOUTL Signal 
    Uint16  CMPSS7_CTRIPOUTH:1;                                    // 31 Input Latch Clear for CMPSS7.CTRIPOUTH Signal 
};

union TRIGLATCHCLR1_REG {
    Uint32  all;  
    struct  TRIGLATCHCLR1_BITS  bit;  
};

struct TRIGLATCHCLR2_BITS {                                         // bit descriptions
    Uint16  INPUT1:1;                                             // 0 Input Latch Clear for INPUT1 Signal 
    Uint16  INPUT2:1;                                             // 1 Input Latch Clear for INPUT2 Signal 
    Uint16  INPUT3:1;                                             // 2 Input Latch Clear for INPUT3 Signal 
    Uint16  INPUT4:1;                                             // 3 Input Latch Clear for INPUT4 Signal 
    Uint16  INPUT5:1;                                             // 4 Input Latch Clear for INPUT5 Signal 
    Uint16  INPUT7:1;                                             // 5 Input Latch Clear for INPUT7 Signal 
    Uint16  ADCSOCA:1;                                             // 6 Input Latch Clear for ADCSOCA Signal 
    Uint16  ADCSOCB:1;                                             // 7 Input Latch Clear for ADCSOCB Signal 
    Uint16  CLB1_4_1:1;                                            // 8 Input Latch Clear for CLB1_4.1 Signal 
    Uint16  CLB1_5_1:1;                                            // 9 Input Latch Clear for CLB1_5.1 Signal 
    Uint16  CLB2_4_1:1;                                            // 10 Input Latch Clear for CLB2_4.1 Signal 
    Uint16  CLB2_5_1:1;                                            // 11 Input Latch Clear for CLB2_5.1 Signal 
    Uint16  CLB3_4_1:1;                                            // 12 Input Latch Clear for CLB3_4.1 Signal 
    Uint16  CLB3_5_1:1;                                            // 13 Input Latch Clear for CLB3_5.1 Signal 
    Uint16  CLB4_4_1:1;                                            // 14 Input Latch Clear for CLB4_4.1 Signal 
    Uint16  CLB4_5_1:1;                                            // 15 Input Latch Clear for CLB4_5.1 Signal 
    Uint16  ECAP1_OUT:1;                                           // 16 Input Latch Clear for ECAP1.OUT Signal 
    Uint16  ECAP2_OUT:1;                                           // 17 Input Latch Clear for ECAP2.OUT Signal 
    Uint16  ECAP3_OUT:1;                                           // 18 Input Latch Clear for ECAP3.OUT Signal 
    Uint16  ECAP4_OUT:1;                                           // 19 Input Latch Clear for ECAP4.OUT Signal 
    Uint16  ECAP5_OUT:1;                                           // 20 Input Latch Clear for ECAP5.OUT Signal 
    Uint16  ECAP6_OUT:1;                                           // 21 Input Latch Clear for ECAP6.OUT Signal 
    Uint16  EXTSYNCOUT:1;                                          // 22 Input Latch Clear for EXTSYNCOUT Signal 
    Uint16  ADCAEVT1:1;                                            // 23 Input Latch Clear for ADCAEVT1 Signal 
    Uint16  ADCAEVT2:1;                                            // 24 Input Latch Clear for ADCAEVT2 Signal 
    Uint16  ADCAEVT3:1;                                            // 25 Input Latch Clear for ADCAEVT3 Signal 
    Uint16  ADCAEVT4:1;                                            // 26 Input Latch Clear for ADCAEVT4 Signal 
    Uint16  ADCBEVT1:1;                                            // 27 Input Latch Clear for ADCBEVT1 Signal 
    Uint16  ADCBEVT2:1;                                            // 28 Input Latch Clear for ADCBEVT2 Signal 
    Uint16  ADCBEVT3:1;                                            // 29 Input Latch Clear for ADCBEVT3 Signal 
    Uint16  ADCBEVT4:1;                                            // 30 Input Latch Clear for ADCBEVT4 Signal 
    Uint16  ADCCEVT1:1;                                            // 31 Input Latch Clear for ADCCEVT1 Signal 
};

union TRIGLATCHCLR2_REG {
    Uint32  all;  
    struct  TRIGLATCHCLR2_BITS  bit;  
};

struct TRIGLATCHCLR3_BITS {                                         // bit descriptions
    Uint16  ADCCEVT2:1;                                            // 0 Input Latch Clear for ADCCEVT2 Signal 
    Uint16  ADCCEVT3:1;                                            // 1 Input Latch Clear for ADCCEVT3 Signal 
    Uint16  ADCCEVT4:1;                                            // 2 Input Latch Clear for ADCCEVT4 Signal 
    Uint16  ADCDEVT1:1;                                            // 3 Input Latch Clear for ADCDEVT1 Signal 
    Uint16  ADCDEVT2:1;                                            // 4 Input Latch Clear for ADCDEVT2 Signal 
    Uint16  ADCDEVT3:1;                                            // 5 Input Latch Clear for ADCDEVT3 Signal 
    Uint16  ADCDEVT4:1;                                            // 6 Input Latch Clear for ADCDEVT4 Signal 
    Uint16  SD1FLT1_COMPL:1;                                       // 7 Input Latch Clear for SD1FLT1.COMPL Signal 
    Uint16  SD1FLT1_COMPH:1;                                       // 8 Input Latch Clear for SD1FLT1.COMPH Signal 
    Uint16  SD1FLT2_COMPL:1;                                       // 9 Input Latch Clear for SD1FLT2.COMPL Signal 
    Uint16  SD1FLT2_COMPH:1;                                       // 10 Input Latch Clear for SD1FLT2.COMPH Signal 
    Uint16  SD1FLT3_COMPL:1;                                       // 11 Input Latch Clear for SD1FLT3.COMPL Signal 
    Uint16  SD1FLT3_COMPH:1;                                       // 12 Input Latch Clear for SD1FLT3.COMPH Signal 
    Uint16  SD1FLT4_COMPL:1;                                       // 13 Input Latch Clear for SD1FLT4.COMPL Signal 
    Uint16  SD1FLT4_COMPH:1;                                       // 14 Input Latch Clear for SD1FLT4.COMPH Signal 
    Uint16  SD2FLT1_COMPL:1;                                       // 15 Input Latch Clear for SD2FLT1.COMPL Signal 
    Uint16  SD2FLT1_COMPH:1;                                       // 16 Input Latch Clear for SD2FLT1.COMPH Signal 
    Uint16  SD2FLT2_COMPL:1;                                       // 17 Input Latch Clear for SD2FLT2.COMPL Signal 
    Uint16  SD2FLT2_COMPH:1;                                       // 18 Input Latch Clear for SD2FLT2.COMPH Signal 
    Uint16  SD2FLT3_COMPL:1;                                       // 19 Input Latch Clear for SD2FLT3.COMPL Signal 
    Uint16  SD2FLT3_COMPH:1;                                       // 20 Input Latch Clear for SD2FLT3.COMPH Signal 
    Uint16  SD2FLT4_COMPL:1;                                       // 21 Input Latch Clear for SD2FLT4.COMPL Signal 
    Uint16  SD2FLT4_COMPH:1;                                       // 22 Input Latch Clear for SD2FLT4.COMPH Signal 
    Uint16  rsvd1:9;                                               // 31:23 Reserved 
};

union TRIGLATCHCLR3_REG {
    Uint32  all;  
    struct  TRIGLATCHCLR3_BITS  bit;  
};

struct SYNCSEL_BITS {                                              // bit descriptions
    Uint16  EPWM4SYNCIN:3;                                         // 2:0 Selects Sync Input Source for EPWM4 
    Uint16  EPWM7SYNCIN:3;                                         // 5:3 Selects Sync Input Source for EPWM7 
    Uint16  EPWM10SYNCIN:3;                                        // 8:6 Selects Sync Input Source for EPWM10 
    Uint16  ECAP1SYNCIN:3;                                         // 11:9 Selects Sync Input Source for ECAP1 
    Uint16  ECAP4SYNCIN:3;                                         // 14:12 Selects Sync Input Source for ECAP4 
    Uint16  rsvd1:1;                                               // 15 Reserved 
    Uint16  rsvd2:11;                                              // 26:16 Reserved 
    Uint16  SYNCOUT:2;                                             // 28:27 Select Syncout Source 
    Uint16  rsvd3:3;                                               // 31:29 Reserved 
};

union SYNCSEL_REG {
    Uint32  all;  
    struct  SYNCSEL_BITS  bit;  
};

struct EXTADCSOCSEL_BITS {                                         // bit descriptions
    Uint16  PWM1SOCAEN:1;                                          // 0 PWM1SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM2SOCAEN:1;                                          // 1 PWM2SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM3SOCAEN:1;                                          // 2 PWM3SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM4SOCAEN:1;                                          // 3 PWM4SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM5SOCAEN:1;                                          // 4 PWM5SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM6SOCAEN:1;                                          // 5 PWM6SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM7SOCAEN:1;                                          // 6 PWM7SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM8SOCAEN:1;                                          // 7 PWM8SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM9SOCAEN:1;                                          // 8 PWM9SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM10SOCAEN:1;                                         // 9 PWM10SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM11SOCAEN:1;                                         // 10 PWM11SOCAEN Enable for ADCSOCAOn 
    Uint16  PWM12SOCAEN:1;                                         // 11 PWM12SOCAEN Enable for ADCSOCAOn 
    Uint16  rsvd1:4;                                               // 15:12 Reserved 
    Uint16  PWM1SOCBEN:1;                                          // 16 PWM1SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM2SOCBEN:1;                                          // 17 PWM2SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM3SOCBEN:1;                                          // 18 PWM3SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM4SOCBEN:1;                                          // 19 PWM4SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM5SOCBEN:1;                                          // 20 PWM5SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM6SOCBEN:1;                                          // 21 PWM6SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM7SOCBEN:1;                                          // 22 PWM7SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM8SOCBEN:1;                                          // 23 PWM8SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM9SOCBEN:1;                                          // 24 PWM9SOCBEN Enable for ADCSOCBOn 
    Uint16  PWM10SOBAEN:1;                                         // 25 PWM10SOBAEN Enable for ADCSOCBOn 
    Uint16  PWM11SOBAEN:1;                                         // 26 PWM11SOBAEN Enable for ADCSOCBOn 
    Uint16  PWM12SOBAEN:1;                                         // 27 PWM12SOBAEN Enable for ADCSOCBOn 
    Uint16  rsvd2:4;                                               // 31:28 Reserved 
};

union EXTADCSOCSEL_REG {
    Uint32  all;  
    struct  EXTADCSOCSEL_BITS  bit;  
};

struct SYNCSOCLOCK_BITS {                                          // bit descriptions
    Uint16  SYNCSELECT:1;                                             // 0 SYNCSEL Register Lock bit 
    Uint16  EXTADCSOCSELECT:1;                                        // 1 EXTADCSOCSEL Register Lock bit 
    Uint16  rsvd1:14;                                              // 15:2 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union SYNCSOCLOCK_REG {
    Uint32  all;  
    struct  SYNCSOCLOCK_BITS  bit;  
};

struct TRIP4MUX0TO15CFG_BITS {                                     // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP4 of EPwmXbar 
};

union TRIP4MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP4MUX0TO15CFG_BITS  bit;  
};

struct TRIP5MUX0TO15CFG_BITS {                                     // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP5 of EPwmXbar 
};

union TRIP5MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP5MUX0TO15CFG_BITS  bit;  
};

struct TRIP7MUX0TO15CFG_BITS {                                     // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP7 of EPwmXbar 
};

union TRIP7MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP7MUX0TO15CFG_BITS  bit;  
};

struct TRIP8MUX0TO15CFG_BITS {                                     // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP8 of EPwmXbar 
};

union TRIP8MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP8MUX0TO15CFG_BITS  bit;  
};

struct TRIP9MUX0TO15CFG_BITS {                                     // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP9 of EPwmXbar 
};

union TRIP9MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP9MUX0TO15CFG_BITS  bit;  
};

struct TRIP10MUX0TO15CFG_BITS {                                    // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP10 of EPwmXbar 
};

union TRIP10MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP10MUX0TO15CFG_BITS  bit;  
};

struct TRIP11MUX0TO15CFG_BITS {                                    // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP11 of EPwmXbar 
};

union TRIP11MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP11MUX0TO15CFG_BITS  bit;  
};

struct TRIP12MUX0TO15CFG_BITS {                                    // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for TRIP12 of EPwmXbar 
};

union TRIP12MUX0TO15CFG_REG {
    Uint32  all;  
    struct  TRIP12MUX0TO15CFG_BITS  bit;  
};

struct TRIP4MUXENABLE_BITS {                                       // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP4 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP4 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP4 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP4 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP4 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP4 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP4 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP4 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP4 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP4 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP4 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP4 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP4 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP4 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP4 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP4 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP4 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP4 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP4 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP4 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP4 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP4 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP4 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP4 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP4 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP4 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP4 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP4 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP4 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP4 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP4 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP4 of EPwmXbar 
};

union TRIP4MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP4MUXENABLE_BITS  bit;  
};

struct TRIP5MUXENABLE_BITS {                                       // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP5 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP5 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP5 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP5 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP5 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP5 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP5 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP5 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP5 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP5 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP5 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP5 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP5 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP5 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP5 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP5 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP5 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP5 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP5 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP5 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP5 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP5 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP5 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP5 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP5 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP5 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP5 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP5 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP5 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP5 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP5 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP5 of EPwmXbar 
};

union TRIP5MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP5MUXENABLE_BITS  bit;  
};

struct TRIP7MUXENABLE_BITS {                                       // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP7 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP7 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP7 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP7 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP7 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP7 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP7 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP7 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP7 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP7 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP7 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP7 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP7 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP7 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP7 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP7 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP7 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP7 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP7 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP7 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP7 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP7 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP7 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP7 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP7 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP7 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP7 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP7 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP7 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP7 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP7 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP7 of EPwmXbar 
};

union TRIP7MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP7MUXENABLE_BITS  bit;  
};

struct TRIP8MUXENABLE_BITS {                                       // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP8 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP8 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP8 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP8 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP8 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP8 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP8 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP8 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP8 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP8 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP8 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP8 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP8 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP8 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP8 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP8 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP8 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP8 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP8 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP8 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP8 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP8 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP8 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP8 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP8 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP8 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP8 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP8 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP8 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP8 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP8 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP8 of EPwmXbar 
};

union TRIP8MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP8MUXENABLE_BITS  bit;  
};

struct TRIP9MUXENABLE_BITS {                                       // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP9 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP9 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP9 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP9 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP9 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP9 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP9 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP9 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP9 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP9 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP9 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP9 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP9 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP9 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP9 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP9 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP9 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP9 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP9 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP9 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP9 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP9 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP9 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP9 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP9 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP9 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP9 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP9 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP9 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP9 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP9 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP9 of EPwmXbar 
};

union TRIP9MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP9MUXENABLE_BITS  bit;  
};

struct TRIP10MUXENABLE_BITS {                                      // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP10 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP10 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP10 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP10 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP10 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP10 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP10 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP10 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP10 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP10 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP10 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP10 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP10 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP10 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP10 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP10 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP10 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP10 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP10 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP10 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP10 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP10 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP10 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP10 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP10 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP10 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP10 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP10 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP10 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP10 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP10 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP10 of EPwmXbar 
};

union TRIP10MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP10MUXENABLE_BITS  bit;  
};

struct TRIP11MUXENABLE_BITS {                                      // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP11 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP11 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP11 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP11 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP11 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP11 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP11 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP11 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP11 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP11 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP11 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP11 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP11 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP11 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP11 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP11 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP11 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP11 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP11 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP11 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP11 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP11 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP11 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP11 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP11 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP11 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP11 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP11 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP11 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP11 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP11 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP11 of EPwmXbar 
};

union TRIP11MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP11MUXENABLE_BITS  bit;  
};

struct TRIP12MUXENABLE_BITS {                                      // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive TRIP12 of EPwmXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive TRIP12 of EPwmXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive TRIP12 of EPwmXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive TRIP12 of EPwmXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive TRIP12 of EPwmXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive TRIP12 of EPwmXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive TRIP12 of EPwmXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive TRIP12 of EPwmXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive TRIP12 of EPwmXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive TRIP12 of EPwmXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive TRIP12 of EPwmXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive TRIP12 of EPwmXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive TRIP12 of EPwmXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive TRIP12 of EPwmXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive TRIP12 of EPwmXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive TRIP12 of EPwmXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive TRIP12 of EPwmXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive TRIP12 of EPwmXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive TRIP12 of EPwmXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive TRIP12 of EPwmXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive TRIP12 of EPwmXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive TRIP12 of EPwmXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive TRIP12 of EPwmXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive TRIP12 of EPwmXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive TRIP12 of EPwmXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive TRIP12 of EPwmXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive TRIP12 of EPwmXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive TRIP12 of EPwmXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive TRIP12 of EPwmXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive TRIP12 of EPwmXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive TRIP12 of EPwmXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive TRIP12 of EPwmXbar 
};

union TRIP12MUXENABLE_REG {
    Uint32  all;  
    struct  TRIP12MUXENABLE_BITS  bit;  
};

struct TRIPOUTINV_BITS {                                           // bit descriptions
    Uint16  OUT0:1;                                                // 0 Selects polarity for TRIP4 of EPwmXbar 
    Uint16  OUT1:1;                                                // 1 Selects polarity for TRIP5 of EPwmXbar 
    Uint16  OUT2:1;                                                // 2 Selects polarity for TRIP7 of EPwmXbar 
    Uint16  OUT3:1;                                                // 3 Selects polarity for TRIP8 of EPwmXbar 
    Uint16  OUT4:1;                                                // 4 Selects polarity for TRIP9 of EPwmXbar 
    Uint16  OUT5:1;                                                // 5 Selects polarity for TRIP10 of EPwmXbar 
    Uint16  OUT6:1;                                                // 6 Selects polarity for TRIP11 of EPwmXbar 
    Uint16  OUT7:1;                                                // 7 Selects polarity for TRIP12 of EPwmXbar 
    Uint16  rsvd1:8;                                               // 15:8 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union TRIPOUTINV_REG {
    Uint32  all;  
    struct  TRIPOUTINV_BITS  bit;  
};

struct TRIPLOCK_BITS {                                             // bit descriptions
    Uint16  LOCK:1;                                                // 0 Locks the configuration for EPwmXbar 
    Uint16  rsvd1:15;                                              // 15:1 Reserved 
    Uint16  KEY:16;                                                // 31:16 Write protection KEY 
};

union TRIPLOCK_REG {
    Uint32  all;  
    struct  TRIPLOCK_BITS  bit;  
};

struct AUXSIG0MUX0TO15CFG_BITS {                                   // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for AUXSIG0 of ClbXbar 
};


struct OUTPUT1MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT1 of GpioOutXbar 
};

union OUTPUT1MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT1MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT2MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT2 of GpioOutXbar 
};

union OUTPUT2MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT2MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT3MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT3 of GpioOutXbar 
};

union OUTPUT3MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT3MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT4MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT4 of GpioOutXbar 
};

union OUTPUT4MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT4MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT5MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT5 of GpioOutXbar 
};

union OUTPUT5MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT5MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT6MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT6 of GpioOutXbar 
};

union OUTPUT6MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT6MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT7MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT7 of GpioOutXbar 
};

union OUTPUT7MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT7MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT8MUX0TO15CFG_BITS {                                 // bit descriptions
    Uint16  Mux0:2;                                                // 1:0 Mux0 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux1:2;                                                // 3:2 Mux1 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux2:2;                                                // 5:4 Mux2 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux3:2;                                                // 7:6 Mux3 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux4:2;                                                // 9:8 Mux4 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux5:2;                                                // 11:10 Mux5 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux6:2;                                                // 13:12 Mux6 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux7:2;                                                // 15:14 Mux7 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux8:2;                                                // 17:16 Mux8 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux9:2;                                                // 19:18 Mux9 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux10:2;                                               // 21:20 Mux10 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux11:2;                                               // 23:22 Mux11 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux12:2;                                               // 25:24 Mux12 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux13:2;                                               // 27:26 Mux13 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux14:2;                                               // 29:28 Mux14 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux15:2;                                               // 31:30 Mux15 Configuration for OUTPUT8 of GpioOutXbar 
};

union OUTPUT8MUX0TO15CFG_REG {
    Uint32  all;  
    struct  OUTPUT8MUX0TO15CFG_BITS  bit;  
};

struct OUTPUT1MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT1 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT1 of GpioOutXbar 
};

union OUTPUT1MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT1MUXENABLE_BITS  bit;  
};

struct OUTPUT2MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT2 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT2 of GpioOutXbar 
};

union OUTPUT2MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT2MUXENABLE_BITS  bit;  
};

struct OUTPUT3MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT3 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT3 of GpioOutXbar 
};

union OUTPUT3MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT3MUXENABLE_BITS  bit;  
};

struct OUTPUT4MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT4 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT4 of GpioOutXbar 
};

union OUTPUT4MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT4MUXENABLE_BITS  bit;  
};

struct OUTPUT5MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT5 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT5 of GpioOutXbar 
};

union OUTPUT5MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT5MUXENABLE_BITS  bit;  
};

struct OUTPUT6MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT6 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT6 of GpioOutXbar 
};

union OUTPUT6MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT6MUXENABLE_BITS  bit;  
};

struct OUTPUT7MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT7 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT7 of GpioOutXbar 
};

union OUTPUT7MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT7MUXENABLE_BITS  bit;  
};

struct OUTPUT8MUXENABLE_BITS {                                   // bit descriptions
    Uint16  Mux0:1;                                                // 0 Selects the output of mux0 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux1:1;                                                // 1 Selects the output of Mux1 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux2:1;                                                // 2 Selects the output of Mux2 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux3:1;                                                // 3 Selects the output of Mux3 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux4:1;                                                // 4 Selects the output of Mux4 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux5:1;                                                // 5 Selects the output of Mux5 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux6:1;                                                // 6 Selects the output of Mux6 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux7:1;                                                // 7 Selects the output of Mux7 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux8:1;                                                // 8 Selects the output of Mux8 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux9:1;                                                // 9 Selects the output of Mux9 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux10:1;                                               // 10 Selects the output of Mux10 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux11:1;                                               // 11 Selects the output of Mux11 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux12:1;                                               // 12 Selects the output of Mux12 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux13:1;                                               // 13 Selects the output of Mux13 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux14:1;                                               // 14 Selects the output of Mux14 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux15:1;                                               // 15 Selects the output of Mux15 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux16:1;                                               // 16 Selects the output of Mux16 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux17:1;                                               // 17 Selects the output of Mux17 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux18:1;                                               // 18 Selects the output of Mux18 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux19:1;                                               // 19 Selects the output of Mux19 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux20:1;                                               // 20 Selects the output of Mux20 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux21:1;                                               // 21 Selects the output of Mux21 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux22:1;                                               // 22 Selects the output of Mux22 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux23:1;                                               // 23 Selects the output of Mux23 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux24:1;                                               // 24 Selects the output of Mux24 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux25:1;                                               // 25 Selects the output of Mux25 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux26:1;                                               // 26 Selects the output of Mux26 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux27:1;                                               // 27 Selects the output of Mux27 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux28:1;                                               // 28 Selects the output of Mux28 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux29:1;                                               // 29 Selects the output of Mux29 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux30:1;                                               // 30 Selects the output of Mux30 to drive OUTPUT8 of GpioOutXbar 
    Uint16  Mux31:1;                                               // 31 Selects the output of Mux31 to drive OUTPUT8 of GpioOutXbar 
};

union OUTPUT8MUXENABLE_REG {
    Uint32  all;  
    struct  OUTPUT8MUXENABLE_BITS  bit;  
};

struct OUTPUTINV_BITS {                                          // bit descriptions
    Uint16  OUT0:1;                                                // 0 Selects polarity for OUTPUT1 of GpioOutXbar 
    Uint16  OUT1:1;                                                // 1 Selects polarity for OUTPUT2 of GpioOutXbar 
    Uint16  OUT2:1;                                                // 2 Selects polarity for OUTPUT3 of GpioOutXbar 
    Uint16  OUT3:1;                                                // 3 Selects polarity for OUTPUT4 of GpioOutXbar 
    Uint16  OUT4:1;                                                // 4 Selects polarity for OUTPUT5 of GpioOutXbar 
    Uint16  OUT5:1;                                                // 5 Selects polarity for OUTPUT6 of GpioOutXbar 
    Uint16  OUT6:1;                                                // 6 Selects polarity for OUTPUT7 of GpioOutXbar 
    Uint16  OUT7:1;                                                // 7 Selects polarity for OUTPUT8 of GpioOutXbar 
    Uint16  rsvd1:8;                                               // 15:8 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union OUTPUTINV_REG {
    Uint32  all;  
    struct  OUTPUTINV_BITS  bit;  
};

struct XTRIPLOCK_BITS {                                            // bit descriptions
    Uint16  LOCK:1;                                                // 0 Locks the configuration for GpioOutXbar 
    Uint16  rsvd1:15;                                              // 15:1 Reserved 
    Uint16  KEY:16;                                                // 31:16 Write Protection KEY 
};

union XTRIPLOCK_REG {
    Uint32  all;  
    struct  XTRIPLOCK_BITS  bit;  
};

struct OUTPUTLATCH_BITS {                                        // bit descriptions
    Uint16  OUT0:1;                                                // 0 Records the OUTPUT1 of GpioOutXbar 
    Uint16  OUT1:1;                                                // 1 Records the OUTPUT2 of GpioOutXbar 
    Uint16  OUT2:1;                                                // 2 Records the OUTPUT3 of GpioOutXbar 
    Uint16  OUT3:1;                                                // 3 Records the OUTPUT4 of GpioOutXbar 
    Uint16  OUT4:1;                                                // 4 Records the OUTPUT5 of GpioOutXbar 
    Uint16  OUT5:1;                                                // 5 Records the OUTPUT6 of GpioOutXbar 
    Uint16  OUT6:1;                                                // 6 Records the OUTPUT7 of GpioOutXbar 
    Uint16  OUT7:1;                                                // 7 Records the OUTPUT8 of GpioOutXbar 
    Uint16  rsvd1:8;                                               // 15:8 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union OUTPUTLATCH_REG {
    Uint32  all;  
    struct  OUTPUTLATCH_BITS  bit;  
};

struct OUTPUTLATCHCLR_BITS {                                     // bit descriptions
    Uint16  OUT0:1;                                                // 0 Clears the Output-Latch for OUTPUT1 of GpioOutXbar 
    Uint16  OUT1:1;                                                // 1 Clears the Output-Latch for OUTPUT2 of GpioOutXbar 
    Uint16  OUT2:1;                                                // 2 Clears the Output-Latch for OUTPUT3 of GpioOutXbar 
    Uint16  OUT3:1;                                                // 3 Clears the Output-Latch for OUTPUT4 of GpioOutXbar 
    Uint16  OUT4:1;                                                // 4 Clears the Output-Latch for OUTPUT5 of GpioOutXbar 
    Uint16  OUT5:1;                                                // 5 Clears the Output-Latch for OUTPUT6 of GpioOutXbar 
    Uint16  OUT6:1;                                                // 6 Clears the Output-Latch for OUTPUT7 of GpioOutXbar 
    Uint16  OUT7:1;                                                // 7 Clears the Output-Latch for OUTPUT8 of GpioOutXbar 
    Uint16  rsvd1:8;                                               // 15:8 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union OUTPUTLATCHCLR_REG {
    Uint32  all;  
    struct  OUTPUTLATCHCLR_BITS  bit;  
};

struct OUTPUTLATCHFRC_BITS {                                     // bit descriptions
    Uint16  OUT0:1;                                                // 0 Sets the Output-Latch for OUTPUT1 of GpioOutXbar 
    Uint16  OUT1:1;                                                // 1 Sets the Output-Latch for OUTPUT2 of GpioOutXbar 
    Uint16  OUT2:1;                                                // 2 Sets the Output-Latch for OUTPUT3 of GpioOutXbar 
    Uint16  OUT3:1;                                                // 3 Sets the Output-Latch for OUTPUT4 of GpioOutXbar 
    Uint16  OUT4:1;                                                // 4 Sets the Output-Latch for OUTPUT5 of GpioOutXbar 
    Uint16  OUT5:1;                                                // 5 Sets the Output-Latch for OUTPUT6 of GpioOutXbar 
    Uint16  OUT6:1;                                                // 6 Sets the Output-Latch for OUTPUT7 of GpioOutXbar 
    Uint16  OUT7:1;                                                // 7 Sets the Output-Latch for OUTPUT8 of GpioOutXbar 
    Uint16  rsvd1:8;                                               // 15:8 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union OUTPUTLATCHFRC_REG {
    Uint32  all;  
    struct  OUTPUTLATCHFRC_BITS  bit;  
};

struct OUTPUTLATCHENABLE_BITS {                                  // bit descriptions
    Uint16  OUT0:1;                                                // 0 Selects the output latch to drive OUTPUT1 for GpioOutXbar 
    Uint16  OUT1:1;                                                // 1 Selects the output latch to drive OUTPUT2 for GpioOutXbar 
    Uint16  OUT2:1;                                                // 2 Selects the output latch to drive OUTPUT3 for GpioOutXbar 
    Uint16  OUT3:1;                                                // 3 Selects the output latch to drive OUTPUT4 for GpioOutXbar 
    Uint16  OUT4:1;                                                // 4 Selects the output latch to drive OUTPUT5 for GpioOutXbar 
    Uint16  OUT5:1;                                                // 5 Selects the output latch to drive OUTPUT6 for GpioOutXbar 
    Uint16  OUT6:1;                                                // 6 Selects the output latch to drive OUTPUT7 for GpioOutXbar 
    Uint16  OUT7:1;                                                // 7 Selects the output latch to drive OUTPUT8 for GpioOutXbar 
    Uint16  rsvd1:8;                                               // 15:8 Reserved 
    Uint16  rsvd2:16;                                              // 31:16 Reserved 
};

union OUTPUTLATCHENABLE_REG {
    Uint32  all;  
    struct  OUTPUTLATCHENABLE_BITS  bit;  
};

struct TRIP4MUX16TO31CFG_BITS {                                    // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP4 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP4 of EPwmXbar 
};

union TRIP4MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP4MUX16TO31CFG_BITS  bit;  
};

struct TRIP5MUX16TO31CFG_BITS {                                    // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP5 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP5 of EPwmXbar 
};

union TRIP5MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP5MUX16TO31CFG_BITS  bit;  
};

struct TRIP7MUX16TO31CFG_BITS {                                    // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP7 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP7 of EPwmXbar 
};

union TRIP7MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP7MUX16TO31CFG_BITS  bit;  
};

struct TRIP8MUX16TO31CFG_BITS {                                    // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP8 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP8 of EPwmXbar 
};

union TRIP8MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP8MUX16TO31CFG_BITS  bit;  
};

struct TRIP9MUX16TO31CFG_BITS {                                    // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP9 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP9 of EPwmXbar 
};

union TRIP9MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP9MUX16TO31CFG_BITS  bit;  
};

struct TRIP10MUX16TO31CFG_BITS {                                   // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP10 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP10 of EPwmXbar 
};

union TRIP10MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP10MUX16TO31CFG_BITS  bit;  
};

struct TRIP11MUX16TO31CFG_BITS {                                   // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP11 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP11 of EPwmXbar 
};

union TRIP11MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP11MUX16TO31CFG_BITS  bit;  
};

struct TRIP12MUX16TO31CFG_BITS {                                   // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for TRIP12 of EPwmXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for TRIP12 of EPwmXbar 
};

union TRIP12MUX16TO31CFG_REG {
    Uint32  all;  
    struct  TRIP12MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG0MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG0 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG0 of ClbXbar 
};

union AUXSIG0MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG0MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG1MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG1 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG1 of ClbXbar 
};

union AUXSIG1MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG1MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG2MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG2 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG2 of ClbXbar 
};

union AUXSIG2MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG2MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG3MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG3 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG3 of ClbXbar 
};

union AUXSIG3MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG3MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG4MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG4 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG4 of ClbXbar 
};

union AUXSIG4MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG4MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG5MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG5 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG5 of ClbXbar 
};

union AUXSIG5MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG5MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG6MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG6 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG6 of ClbXbar 
};

union AUXSIG6MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG6MUX16TO31CFG_BITS  bit;  
};

struct AUXSIG7MUX16TO31CFG_BITS {                                  // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for AUXSIG7 of ClbXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for AUXSIG7 of ClbXbar 
};

union AUXSIG7MUX16TO31CFG_REG {
    Uint32  all;  
    struct  AUXSIG7MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT1MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT1 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT1 of GpioOutXbar 
};

union OUTPUT1MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT1MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT2MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT2 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT2 of GpioOutXbar 
};

union OUTPUT2MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT2MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT3MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT3 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT3 of GpioOutXbar 
};

union OUTPUT3MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT3MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT4MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT4 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT4 of GpioOutXbar 
};

union OUTPUT4MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT4MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT5MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT5 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT5 of GpioOutXbar 
};

union OUTPUT5MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT5MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT6MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT6 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT6 of GpioOutXbar 
};

union OUTPUT6MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT6MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT7MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT7 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT7 of GpioOutXbar 
};

union OUTPUT7MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT7MUX16TO31CFG_BITS  bit;  
};

struct OUTPUT8MUX16TO31CFG_BITS {                                // bit descriptions
    Uint16  Mux16:2;                                               // 1:0 Mux16 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux17:2;                                               // 3:2 Mux17 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux18:2;                                               // 5:4 Mux18 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux19:2;                                               // 7:6 Mux19 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux20:2;                                               // 9:8 Mux20 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux21:2;                                               // 11:10 Mux21 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux22:2;                                               // 13:12 Mux22 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux23:2;                                               // 15:14 Mux23 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux24:2;                                               // 17:16 Mux24 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux25:2;                                               // 19:18 Mux25 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux26:2;                                               // 21:20 Mux26 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux27:2;                                               // 23:22 Mux27 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux28:2;                                               // 25:24 Mux28 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux29:2;                                               // 27:26 Mux29 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux30:2;                                               // 29:28 Mux30 Configuration for OUTPUT8 of GpioOutXbar 
    Uint16  Mux31:2;                                               // 31:30 Mux31 Configuration for OUTPUT8 of GpioOutXbar 
};

union OUTPUT8MUX16TO31CFG_REG {
    Uint32  all;  
    struct  OUTPUT8MUX16TO31CFG_BITS  bit;  
};

#if defined(CPU1)
struct TRIG_REGS {
    Uint16                               INPUT1SELECT;               // INPUT1 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT2SELECT;               // INPUT2 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT3SELECT;               // INPUT3 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT4SELECT;               // INPUT4 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT5SELECT;               // INPUT5 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT6SELECT;               // INPUT7 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT7SELECT;               // INPUT8 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT8SELECT;               // INPUT9 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT9SELECT;               // INPUT10 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT10SELECT;              // INPUT11 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT11SELECT;              // INPUT11 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT12SELECT;              // INPUT12 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT13SELECT;              // INPUT13 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT14SELECT;              // INPUT14 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT15SELECT;              // INPUT15 Input Select Register (GPIO0 to x) 
    Uint16                               INPUT16SELECT;              // INPUT16 Input Select Register (GPIO0 to x) 
    Uint16                               rsvd1[14];                // Reserved 
    union   INPUTLOCK_REG               INPUTLOCK;               // INPUT Input Select Lock Register 
    union   TRIGLATCH1_REG                TRIGLATCH1;                // X-Bar Input Latch Register-1 
    union   TRIGLATCH2_REG                TRIGLATCH2;                // X-Bar Input Latch Register-2 
    union   TRIGLATCH3_REG                TRIGLATCH3;                // X-Bar Input Latch Register-3 
    Uint16                               rsvd2[2];                 // Reserved 
    union   TRIGLATCHCLR1_REG             TRIGLATCHCLR1;             // X-Bar Input Latch Clear Register-1 
    union   TRIGLATCHCLR2_REG             TRIGLATCHCLR2;             // X-Bar Input Latch Clear Register-2 
    union   TRIGLATCHCLR3_REG             TRIGLATCHCLR3;             // X-Bar Input Latch Clear Register-3 
    Uint16                               rsvd3[18];                // Reserved 
    union   SYNCSEL_REG                  SYNCSELECT;                  // Sync Input and Output Select Register 
    union   EXTADCSOCSEL_REG             EXTADCSOCSELECT;             // External ADCSOC Select Register 
    union   SYNCSOCLOCK_REG              SYNCSOCLOCK;              // SYNCSEL and EXTADCSOC Select Lock register 
};
#endif

#if defined(CPU1)
struct EPWM_XBAR_REGS {
    union   TRIP4MUX0TO15CFG_REG         TRIP4MUX0TO15CFG;         // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-0 
    union   TRIP4MUX16TO31CFG_REG        TRIP4MUX16TO31CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-0 (Reserved) 
    union   TRIP5MUX0TO15CFG_REG         TRIP5MUX0TO15CFG;         // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-1 
    union   TRIP5MUX16TO31CFG_REG        TRIP5MUX16TO31CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-1 (Reserved) 
    union   TRIP7MUX0TO15CFG_REG         TRIP7MUX0TO15CFG;         // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-2 
    union   TRIP7MUX16TO31CFG_REG        TRIP7MUX16TO31CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-2 (Reserved) 
    union   TRIP8MUX0TO15CFG_REG         TRIP8MUX0TO15CFG;         // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-3 
    union   TRIP8MUX16TO31CFG_REG        TRIP8MUX16TO31CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-3 (Reserved) 
    union   TRIP9MUX0TO15CFG_REG         TRIP9MUX0TO15CFG;         // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-4 
    union   TRIP9MUX16TO31CFG_REG        TRIP9MUX16TO31CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-4 (Reserved) 
    union   TRIP10MUX0TO15CFG_REG        TRIP10MUX0TO15CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-5 
    union   TRIP10MUX16TO31CFG_REG       TRIP10MUX16TO31CFG;       // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-5 (Reserved) 
    union   TRIP11MUX0TO15CFG_REG        TRIP11MUX0TO15CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-6 
    union   TRIP11MUX16TO31CFG_REG       TRIP11MUX16TO31CFG;       // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-6 (Reserved) 
    union   TRIP12MUX0TO15CFG_REG        TRIP12MUX0TO15CFG;        // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-7 
    union   TRIP12MUX16TO31CFG_REG       TRIP12MUX16TO31CFG;       // EPwmXbar (XTRIP X-Bar) Group Select Register-1 for Output-7 (Reserved) 
    union   TRIP4MUXENABLE_REG           TRIP4MUXENABLE;           // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-0 
    union   TRIP5MUXENABLE_REG           TRIP5MUXENABLE;           // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-1 
    union   TRIP7MUXENABLE_REG           TRIP7MUXENABLE;           // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-2 
    union   TRIP8MUXENABLE_REG           TRIP8MUXENABLE;           // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-3 
    union   TRIP9MUXENABLE_REG           TRIP9MUXENABLE;           // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-4 
    union   TRIP10MUXENABLE_REG          TRIP10MUXENABLE;          // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-5 
    union   TRIP11MUXENABLE_REG          TRIP11MUXENABLE;          // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-6 
    union   TRIP12MUXENABLE_REG          TRIP12MUXENABLE;          // EPwmXbar (XTRIP X-Bar) Mux Enable Register for Output-7 
    Uint16                               rsvd1[8];                 // Reserved 
    union   TRIPOUTINV_REG               TRIPOUTINV;               // EPwmXbar (XTRIP X-Bar) Output Inversion Register 
    Uint16                               rsvd2[4];                 // Reserved 
    union   TRIPLOCK_REG                 TRIPLOCK;                 // EPwmXbar Configuration Lock register 
};
#endif

#if defined(CPU1)
struct OUTPUT_XBAR_REGS {
    union   OUTPUT1MUX0TO15CFG_REG     OUTPUT1MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-0 
    union   OUTPUT1MUX16TO31CFG_REG    OUTPUT1MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-0 (Reserved) 
    union   OUTPUT2MUX0TO15CFG_REG     OUTPUT2MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-1 
    union   OUTPUT2MUX16TO31CFG_REG    OUTPUT2MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-1 (Reserved) 
    union   OUTPUT3MUX0TO15CFG_REG     OUTPUT3MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-2 
    union   OUTPUT3MUX16TO31CFG_REG    OUTPUT3MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-2 (Reserved) 
    union   OUTPUT4MUX0TO15CFG_REG     OUTPUT4MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-3 
    union   OUTPUT4MUX16TO31CFG_REG    OUTPUT4MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-3 (Reserved) 
    union   OUTPUT5MUX0TO15CFG_REG     OUTPUT5MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-4 
    union   OUTPUT5MUX16TO31CFG_REG    OUTPUT5MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-4 (Reserved) 
    union   OUTPUT6MUX0TO15CFG_REG     OUTPUT6MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-5 
    union   OUTPUT6MUX16TO31CFG_REG    OUTPUT6MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-5 (Reserved) 
    union   OUTPUT7MUX0TO15CFG_REG     OUTPUT7MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-6 
    union   OUTPUT7MUX16TO31CFG_REG    OUTPUT7MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-6 (Reserved) 
    union   OUTPUT8MUX0TO15CFG_REG     OUTPUT8MUX0TO15CFG;     // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-7 
    union   OUTPUT8MUX16TO31CFG_REG    OUTPUT8MUX16TO31CFG;    // GpioOutXbar (OUTPUT X-Bar) Group Select Register-1 for Output-7 (Reserved) 
    union   OUTPUT1MUXENABLE_REG       OUTPUT1MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-0 
    union   OUTPUT2MUXENABLE_REG       OUTPUT2MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-1 
    union   OUTPUT3MUXENABLE_REG       OUTPUT3MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-2 
    union   OUTPUT4MUXENABLE_REG       OUTPUT4MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-3 
    union   OUTPUT5MUXENABLE_REG       OUTPUT5MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-4 
    union   OUTPUT6MUXENABLE_REG       OUTPUT6MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-5 
    union   OUTPUT7MUXENABLE_REG       OUTPUT7MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-6 
    union   OUTPUT8MUXENABLE_REG       OUTPUT8MUXENABLE;       // GpioOutXbar (OUTPUT X-Bar) Mux Enable Register for Output-7 
    union   OUTPUTLATCH_REG            OUTPUTLATCH;            // GpioOutXbar (OUTPUT X-Bar) Output Latch Register 
    union   OUTPUTLATCHCLR_REG         OUTPUTLATCHCLR;         // GpioOutXbar (OUTPUT X-Bar) Output Latch Clear register 
    union   OUTPUTLATCHFRC_REG         OUTPUTLATCHFRC;         // GpioOutXbar (OUTPUT X-Bar) Output Latch Clear register 
    union   OUTPUTLATCHENABLE_REG      OUTPUTLATCHENABLE;      // GpioOutXbar (OUTPUT X-Bar) Output Latch Enable Register 
    union   OUTPUTINV_REG              OUTPUTINV;              // GpioOutXbar (OUTPUT X-Bar) Output Inversion Register 
    Uint16                               rsvd1[4];                 // Reserved 
    union   XTRIPLOCK_REG              OUTPUTLOCK;                // GpioOutXbar Configuration Lock register 
};
#endif

//---------------------------------------------------------------------------
// Xbar External References & Function Declarations:
//

#if defined(CPU1)
extern volatile struct TRIG_REGS TrigRegs;
#endif
#if defined(CPU1)
extern volatile struct EPWM_XBAR_REGS EPwmXbarRegs;
#endif
#if defined(CPU1)
extern volatile struct OUTPUT_XBAR_REGS OutputXbarRegs;
#endif

#ifdef __cplusplus
}
#endif                                                             /* extern "C" */


#endif                                                             // end of F2837x_XBAR_H definition
//===========================================================================
// End of file.
//===========================================================================
