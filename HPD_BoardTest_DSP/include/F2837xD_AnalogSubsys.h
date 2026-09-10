//###########################################################################
//
// FILE:   F2837x_AnalogSubsys.h
//
// TITLE:  F2837x Device AnalogSubsys Register Definitions.
//
// BUILD:  CUSTOMER RELEASE
//
//###########################################################################
// $TI Release: F2837xD Support Library v110 $
// $Release Date: Mon Apr  7 10:35:37 CDT 2014 $
//###########################################################################

#ifndef F2837x_AnalogSubsys_H
#define F2837x_AnalogSubsys_H
#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------
// AnalogSubsys Individual Register Bit Definitions:

struct TSNSCTL_BITS {                                 // bit descriptions
    Uint16  ENABLE:1;                                 // 0 Temperature Sensor Enable 
    Uint16  rsvd1:15;                                 // 15:1 Reserved 
};

union TSNSCTL_REG {
    Uint16  all;  
    struct  TSNSCTL_BITS  bit;  
};

struct LOCK_BITS {                                    // bit descriptions
    Uint16  rsvd1:3;                                  // 2:0 Reserved 
    Uint16  TSNSCTL:1;                                // 3 Temperature Sensor Control Register Lock 
    Uint16  rsvd2:12;                                 // 15:5 Reserved 
	Uint16  rsvd3:7;                                  // 22:16
    Uint16  ANAREFTRIMA:1;                            // 23 Analog Reference A Trim Register Lock 
    Uint16  ANAREFTRIMB:1;                            // 24 Analog Reference B Trim Register Lock 
    Uint16  ANAREFTRIMC:1;                            // 25 Analog Reference C Trim Register Lock 
    Uint16  ANAREFTRIMD:1;                            // 26 Analog Reference D Trim Register Lock 
    Uint16  rsvd4:5;                                  // 31:27 Reserved 
};

union LOCK_REG {
    Uint32  all;  
    struct  LOCK_BITS  bit;  
};


#if defined(CPU1)
struct ANALOG_SUBSYS_REGS {
    Uint16                   rsvd1[38];               // Reserved 
    union   TSNSCTL_REG      TSNSCTL;                 // Temperature Sensor Control Register 
    Uint16                   rsvd2[7];                // Reserved 
    union   LOCK_REG         LOCK;                    // Lock Register 
    Uint16                   rsvd3[6];                // Reserved 
    Uint32                   ANAREFTRIMA;             // Analog Reference Trim A Register 
    Uint32                   ANAREFTRIMB;             // Analog Reference Trim B Register 
    Uint32                   ANAREFTRIMC;             // Analog Reference Trim C Register 
    Uint32                   ANAREFTRIMD;             // Analog Reference Trim D Register 
    Uint16                   rsvd4[10];               // Reserved 
};
#endif

//---------------------------------------------------------------------------
// AnalogSubsys External References & Function Declarations:
//

#if defined(CPU1)
extern volatile struct ANALOG_SUBSYS_REGS AnalogSubsysRegs;
#endif

#ifdef __cplusplus
}
#endif                                                /* extern "C" */


#endif                                                // end of F2837x_ANALOGSUBSYS_H definition
//===========================================================================
// End of file.
//===========================================================================
