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
SpiRAM SpiRam(SPI_CLOCK_DIV8, SS_PIN, CHIP_23LC1024);

void setup()   {                
  Serial.begin(115200);
}

void loop(){
  read_write_int_example(-53);
  delay(1000);
  read_write_long_example(-9332);
  delay(1000);
  read_write_float_example(-3.14);
  delay(1000);
  read_write_int_array_example();
  delay(1000);
  read_write_long_array_example();
  delay(1000);
  read_write_float_array_example();
  delay(1000);
}

void read_write_int_example(int number){
  Serial.println("Writing and reading int");
  SpiRam.write_int(0,number);
  delay(10);
  int readData = SpiRam.read_int(0);
  if (readData != number){
    Serial.println("Data validation issue - write or read failed!\n");
  }
  else {
    Serial.println("Write and Read Successful\n");
  }
}

void read_write_long_example(long number){
  Serial.println("Writing and reading long");
  SpiRam.write_long(0,number);
  delay(10);
  long readData = SpiRam.read_long(0);
  if (readData != number){
    Serial.println("Data validation issue - write or read failed!\n");
  }
  else {
    Serial.println("Write and Read Successful\n");
  }
}

void read_write_float_example(float number){
  Serial.println("Writing and reading float");
  SpiRam.write_float(0,number);
  delay(10);
  float readData = SpiRam.read_float(0);
  if (readData != number){
    Serial.println("Data validation issue - write or read failed!\n");
  }
  else {
    Serial.println("Write and Read Successful\n");
  }
}

void read_write_int_array_example(){
  Serial.println("Writing and reading int array");
  int bigArray[20];
  for (int i=0;i<20;i++){
    bigArray[i] = i;
  }
  SpiRam.write_ints(0,bigArray,20);
  int newArray[20];
  SpiRam.read_ints(0,newArray,20);
  boolean validation = true;
  for (int i=0;i<20;i++){
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

void read_write_long_array_example(){
  Serial.println("Writing and reading long array");
  long bigArray[20];
  for (int i=0;i<20;i++){
    bigArray[i] = i;
  }
  SpiRam.write_longs(0,bigArray,20);
  long newArray[20];
  SpiRam.read_longs(0,newArray,20);
  boolean validation = true;
  for (int i=0;i<20;i++){
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

void read_write_float_array_example(){
  Serial.println("Writing and reading float array");
  float bigArray[20];
  for (int i=0;i<20;i++){
    bigArray[i] = i/2.;
  }
  SpiRam.write_floats(0,bigArray,20);
  float newArray[20];
  SpiRam.read_floats(0,newArray,20);
  boolean validation = true;
  for (int i=0;i<20;i++){
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
