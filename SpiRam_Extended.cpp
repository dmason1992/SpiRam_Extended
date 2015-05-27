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

/*
 * SpiRam_Extended version "0.1.1"
 * Added integer read and write functions
 *
 * SPIRAM_LIB_VERSION  "0.3.00"
 * Updated to Arduino 1.0.5 and newer version of SPI "2010 by Cristian Maglie"
 * by Fred Jan Kraan, fjkraan@xs4all.nl, 2014-02-28
 *
 * SPIRAM_LIB_VERSION  "0.3.10"
 * by stah - fix for Mega2560, stah@strictwise.com, 2014-04-20
 *
 * SPIRAM_LIB_VERSION  "0.3.20"
 * Extended for use with 23K256, 23LC(V)512 and 23LC(V)1024
 * as well as adding "run_stream" concept as explained now:
 * The RAM is there because on-chip memory is tiny, so streaming
 * from the chip into a buffer is useless if the intended stream is big.
 * The run_stream instead calls back into the program to let you process
 * each byte without storing it in a buffer (although you could still do that)
 * Also made some mayor cleanup and optimizations (thanks to tips in the forum)
 * Brought to you by Martien Remijn <M10>, m10@xs4all.nl, 2014-09-21
 */

#include <SPI.h>
#include <SpiRam_Extended.h>

//////////////
// Constructor

SpiRAM::SpiRAM(byte clockDiv, byte ssPin, int chiptype)
{
  SPI.begin();
  pinMode(ssPin,OUTPUT);

  // Ensure the RAM chip is disabled in the first instance
  disable();

  // Set the spi mode using the requested clock speed
  SPI.setClockDivider(clockDiv);
  _ssPin = ssPin;
  _chiptype = chiptype;

  // Set the RAM operarion mode flag as undefined
  _current_mode = UNDEFINED_MODE;
}


//////////////////////////////////////
// Enable and disable helper functions
void SpiRAM::enable()
{
  digitalWrite(_ssPin, LOW);
}

void SpiRAM::disable()
{
  digitalWrite(_ssPin, HIGH);
}


//////////////////////////////
// preparation helper function
void SpiRAM::_prepare(char mode, char action, long address)
{
  _set_mode(mode);
  // Write address
  enable();
  SPI.transfer(action);
  if (_chiptype>512) SPI.transfer((char)(address >> 16));
  SPI.transfer((char)(address >> 8));
  SPI.transfer((char)address);
  // don't forget to disable() after the action is done
}


///////////////////////////
// Byte transfer functions
char SpiRAM::read_byte(long address)
{
  char read_byte;

  _prepare(BYTE_MODE,READ,address);
  read_byte = SPI.transfer(0xFF);
  disable();

  return read_byte;
}

char SpiRAM::write_byte(long address, char data_byte)
{
  _prepare(BYTE_MODE,WRITE,address);
  SPI.transfer(data_byte);
  disable();

  return data_byte;
}

///////////////////////////
// Int transfer functions
int SpiRAM::write_int(long address, int data_int)
{
  signed char outBytes[2];
  outBytes[0] = (data_int >> 8);
  outBytes[1] = (data_int);
  _prepare(STREAM_MODE,WRITE,address);
  SPI.transfer(outBytes[0]);
  SPI.transfer(outBytes[1]);
  disable();

  return data_int;
}

int SpiRAM::read_int(long address)
{
  int read_int;
  signed char inBytes[2];
  _prepare(STREAM_MODE,READ,address);
  inBytes[0] = SPI.transfer(0xFF);
  inBytes[1] = SPI.transfer(0xFF);
  disable();
  read_int = (inBytes[0] << 8)|(inBytes[1]);

  return read_int;
}

void SpiRAM::write_ints(long address,int *data_int,long length)
{
  long i;
  signed char outBytes[2];
  _prepare(STREAM_MODE,WRITE,address);
  for (i=0;i<length;i++){
    outBytes[0] = (data_int[i] >> 8);
    outBytes[1] = (data_int[i]);
    SPI.transfer(outBytes[0]);
    SPI.transfer(outBytes[1]);
  }
  disable();
}

void SpiRAM::read_ints(long address,int *data_int, long length)
{
  long i;
  signed char inBytes[2];
  _prepare(STREAM_MODE,READ,address);
  for (i=0;i<length;i++){
    inBytes[0] = SPI.transfer(0xFF);
    inBytes[1] = SPI.transfer(0xFF);
    data_int[i] = (inBytes[0] << 8)|(inBytes[1]);
  }
  disable();

}

//////////////////////////////////////////////////
// Page transfer functions. Bound to current page.
// Passing the boundary will wrap to the beginning
void SpiRAM::read_page(long address, char *buffer)
{
  int i;

  _prepare(PAGE_MODE,READ,address);
  for (i = 0; i < 32; i++) {
    buffer[i] = SPI.transfer(0xFF);
  }
  disable();
}

void SpiRAM::write_page(long address, char *buffer)
{
  int i;

  _prepare(PAGE_MODE,WRITE,address);
  for (i = 0; i < 32; i++) {
    SPI.transfer(buffer[i]);
  }
  disable();
}

//////////////////////////////////////////////////////
// Stream transfer functions. Ignores page boundaries.
void SpiRAM::read_stream(long address, char *buffer, long length)
{
  long i;

  _prepare(STREAM_MODE,READ,address);
  for (i = 0; i < length; i++) {
    buffer[i] = SPI.transfer(0xFF);
  }
  disable();
}

void SpiRAM::run_stream(long address, long length)
{
  long i;

  _prepare(STREAM_MODE,READ,address);
  for (i = 0; i < length; i++) {
    stream_run(SPI.transfer(0xFF),i);
  }
  disable();
}

void SpiRAM::write_stream(long address, char *buffer, long length)
{
  long i;

  _prepare(STREAM_MODE,WRITE,address);
  for (i = 0; i < length; i++) {
    SPI.transfer(buffer[i]);
  }
  disable();
}

/////////////////
// Mode handling
void SpiRAM::_set_mode(char mode)
{
  if (mode != _current_mode)
  { /*       //debug statements
    enable();
    SPI.transfer(RDSR);
    _current_mode=SPI.transfer(0xFF);
    disable();
    Serial.print("Mode was ");
    Serial.println(_current_mode,HEX); /**/
    enable();
    SPI.transfer(WRSR);
    SPI.transfer(mode);
    disable();
    enable();
    SPI.transfer(RDSR);
    _current_mode=SPI.transfer(0xFF);
    disable();  /*       //debug statements
    Serial.print("Mode changed to ");
    Serial.println(_current_mode,HEX);/**/
  }
}
