/*
 * SpiRam_Extended.h - Library for driving a 23k256 SPI attached SRAM chip,
 * Extended to add easier transfer of integers
 *
 * David Mason, 27/05/2015 - modified to add integer transfer functions
 * Phil Stewart, 18/10/2009 - Original Author of SpiRAM
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef SpiRam_Extended_h
#define SpiRam_Extended_h

#define SPIRam_Extended_LIB_VERSION  "0.1.1"   //by David Mason 27/05/2015

#include <Arduino.h>
#include <SPI.h>

// SRAM opcodes
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

// SRAM Hold line disabled when HOLD == 1
#define HOLD 1

// SRAM modes
#define BYTE_MODE (0x00 | HOLD)
#define PAGE_MODE (0x80 | HOLD)
#define STREAM_MODE (0x40 | HOLD)
#define RESERVED_MODE (0xC0 | HOLD)
#define UNDEFINED_MODE (0xFF)

// ClockDiv values SPI_CLOCK_DIV(XX) eg. SPI_CLOCK_DIV4

// amount of kiloBIT of the chip
#define CHIP_23K256 256    //default
#define CHIP_23LC512 512
#define CHIP_23LCV512 512
#define CHIP_23LC1024 1024
#define CHIP_23LCV1024 1024

class SpiRAM
{
  public:
    SpiRAM(byte clockDiv, byte ssPin, int chiptype=CHIP_23K256);
    void enable();
    void disable();
    char read_byte(long address);
    char write_byte(long address, char data_byte);
    void read_page(long address, char *buffer);
    void write_page(long address, char *buffer);
    void read_stream(long address, char *buffer, long length);
    void write_stream(long address, char *buffer, long length);
    void run_stream(long address, long length);
    void stream_run(char data_byte,long index); //if used, MUST be part of main program
    int read_int(long address);
    int write_int(long address, int data_int);
    void read_ints(long address,int *data_int, long length);
    void write_ints(long address,int *data_int, long length);

  private:
    char _current_mode;
    byte _ssPin;
    int  _chiptype;
    void _prepare(char mode, char action, long address);
    void _set_mode(char mode);
};


#endif
