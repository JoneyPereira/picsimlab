/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#ifndef BOARD_QEMU_H
#define BOARD_QEMU_H

#include "board.h"
#include "qemu.h"

typedef enum { QEMU_SIM_NONE = 0, QEMU_SIM_STM32, QEMU_SIM_ESP32 } QEMUSimType;

class bsim_qemu : virtual public board {
public:
    bsim_qemu(void);
    ~bsim_qemu(void);
    int DebugInit(int dtyppe) override;
    lxString GetDebugName(void) override { return "GDB"; };
    void DebugLoop(void) override;
    int CpuInitialized(void) override;
    void MSetSerial(const char* port) override;
    int MInit(const char* processor, const char* fname, float freq) override;
    void MEnd(void) override;
    int MGetArchitecture(void) override;
    void MDumpMemory(const char* fname) override;
    void MEraseFlash(void) override;
    void MSetFreq(float freq) override;
    float MGetFreq(void) override;
    void MSetVCC(float vcc) override;
    float MGetVCC(void) override;
    float MGetInstClockFreq(void) override;
    void MSetPin(int pin, unsigned char value) override;
    void MSetPinDOV(int pin, unsigned char ovalue) override;
    unsigned char MGetPin(int pin) override;
    const picpin* MGetPinsValues(void) override;
    void MStep(void) override;
    void MStepResume(void) override;
    void MReset(int flags) override;
    void EvThreadRun(CThread& thread) override;
    user_timer_t timer;
    virtual void Run_CPU_ns(uint64_t time) = 0;

protected:
    int MipsStrToIcount(const char* mipstr);
    const char* IcountToMipsStr(int icount);
    const char* IcountToMipsItens(char* buffer);
    unsigned int ns_count;
    void pins_reset(void);
    int icount;
#ifdef _WIN_
    HANDLE serialfd[4];
#else
    int serialfd[4];
#endif
    int procid;
    picpin pins[256];
    unsigned int serialbaud;
    float serialexbaud;
    float freq;
    char fname[2048];
    char fname_[2048];
    char fname_bak[2048];
    unsigned short ADCvalues[16];
    lxMutex* mtx_qinit;
    int qemu_started;
    QEMUSimType SimType;

private:
    int load_qemu_lib(const char* path);
};

#endif /* BOARD_QEMU_H */