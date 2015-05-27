// Basic Arduino SRAM library function demonstrator
// by  David Mason, 2014 inspired by Ido Gendel (2012) and Martien Remijn (2014)
// Share and enjoy!

#include <SPI.h>
#include <SpiRam_Extended.h>

// SCK (pin13)->chip6  (ISPpin3)
// MISO(pin12)->chip2  (ISPpin1)
// MOSI(pin11)->chip5  (ISPpin4)
// SS  (pin10)->chip1
// The HOLD pin of the chip must be connected directly to Vcc
// Remember Vcc must be at most 4.5V and levelshifters are essential for 23K256
// Leonardo does not have SPI on pins 11-13 but on ISPheader
#define SS_PIN 10
//if SS is not pin 10 then still pin10 must be OUTPUT to prevent SPI slave mode

// information on chip models can be found on e.g. microchip.com
// Optional constructor parameter must be the amount of kiloBIT of the chip
// for example 23K256 -> 256 (default)
// for example 23LC512 -> 512
// for example 23LCV1024 -> 1024

//byte clock = 0-6 to set SPI speed by SPI_CLOCK_DIVxx;
// 0=DIV4; 1=DIV16; 2=DIV64; 3=DIV128; 4=DIV2; 5=DIV8; 6=DIV32; 7=broken
// div2 is the fastest
SpiRAM SpiRam(SPI_CLOCK_DIV2, SS_PIN, CHIP_23LC1024);

void setup()   {                
  Serial.begin(115200);
}

void loop(){
  read_write_int_example();
  delay(1000);
  read_write_int_array_example();
  delay(1000);
}

void read_write_int_example(){
  Serial.println("Writing and reading int");
  SpiRam.write_int(0,923);
  delay(10);
  int readData = SpiRam.read_int(0);
  if (readData != 923){
    Serial.println("Data validation issue - write or read failed!\n");
  }
  else {
    Serial.println("Write and Read Successful\n");
  }
}

void read_write_int_array_example(){
  Serial.println("Writing and reading int array");
  int bigArray[512];
  for (int i=0;i<512;i++){
    bigArray[i] = i;
  }
  SpiRam.write_ints(0,bigArray,512);
  int newArray[512];
  SpiRam.read_ints(0,newArray,512);
  boolean validation = true;
  for (int i=0;i<512;i++){
    if (newArray[i] != bigArray[i]){
      validation = false;
      break;
    }
  }
  if (validation){
    Serial.println("Write and Read Successful\n");
  }
  else{
    Serial.println("Data validation issue - write or read failed!\n");
  }
  
}
