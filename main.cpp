#include "mbed.h"

DigitalOut myled(LED1);
Serial pc(SERIAL_TX, SERIAL_RX);

SPI spi(D11, D12, D13); // mosi, miso, sclk
DigitalOut cs(D10); //cs

char writeBuffer[256];

void readID()
{
    cs = 0;
    spi.write(0x90); //send command
    spi.write(0x00); //dummy
    spi.write(0x00); //dummy
    spi.write(0x00); //dummy
    char manufacturer = spi.write(0x00);
    char device = spi.write(0x00);
    cs = 1;
    
    pc.printf("Manufacturer ID: 0x%02x \n\r", manufacturer);
    pc.printf("Device ID: 0x%02x \n\r", device);
}

void readPage(char a3, char a2, char a1, int length)
{
    pc.printf("Reading 0x%02x%02x%02x - 0x%02x%02xFF:\n\r", a3, a2, a1, a3, a2);
    cs = 0;
    spi.write(0x03); //send command
    spi.write(a3); //send MSB of address
    spi.write(a2); //send middle of address
    spi.write(a1); //send LSB of address
    for(int addr = a1; addr  < length; addr++)
    {
        char data = spi.write(0x00);
        pc.printf("%02x ", data);
    }
    cs = 1;
    pc.printf("\n\r");
    
}

char getStatus1(bool verbose = true)
{
    cs = 0;
    spi.write(0x05); //send command
    
    char status = spi.write(0x00); //sread status
    
    if(verbose)
    {
        pc.printf("Status 1: 0x%02x \n\r", status);
    }
    
    cs = 1;
    
    return status;
}

void waitBusy()
{
    char status = 1;
    cs = 0;
    spi.write(0x05); //send command
    
    while((status & 0x01) != 0) //keep reading status until the busy bit becomes 0
    {
        status = spi.write(0x00); //read status
        pc.printf("Status 1: 0x%02x (%s) \n\r", status, ((status & 0x01) != 0 ? "BUSY" : "NOT BUSY"));
    }
    cs = 1;
}

void writeEnable()
{
    pc.printf("Enabling Write...");
    cs = 0;
    spi.write(0x06); //send command
    cs = 1;
    pc.printf("%s \n\r", ((getStatus1(false) & 0x02) == 0 ? "FAIL..." : "SUCCESS.."));
}

void erasePage(char a3, char a2, char a1)
{
    cs = 0;
    spi.write(0x20); //send command
    spi.write(a3); //send MSB of address
    spi.write(a2); //send middle of address
    spi.write(a1); //send LSB of address
    cs = 1;
    
    pc.printf("Erasing 0x%02x%02x%02x... \n\r", a3, a2, a1);
}

void reset()
{
    pc.printf("Resetting... \n\r");
    cs = 0;
    spi.write(0x66);
    cs = 1;
    wait(0.2);
    cs = 0;
    spi.write(0x99);
    cs = 1;
    wait(0.2);
}

void writePage(char a3, char a2, char a1)
{
    cs = 0;
    spi.write(0x02); //send command
    spi.write(a3); //send MSB of address
    spi.write(a2); //send middle of address
    spi.write(a1); //send LSB of address
    for(int addr = a1; addr < 256; addr++)
    {
        spi.write(writeBuffer[addr]);
    }
    
    cs = 1;
    
    pc.printf("Writing buffer to 0x%02x%02x%02x\n\r", a3, a2, a1);
}

void bufferPage()
{
    pc.printf("Loading buffer: \n\r");
    int i = 0;
    
    while(i < 256)
    {
        char data = pc.getc();
        writeBuffer[i] = data;     
        pc.printf("%02x ", data);
        i++;
    }
     
    pc.printf("\n\r");
}

void eraseChip()
{
    pc.printf("Erasing Chip...");
    cs = 0;
    spi.write(0xc7); //send command
    cs = 1;
    waitBusy();
}

int main() {
    cs = 1;
    spi.format(8,3);
    spi.frequency(1000000);
    wait(1);
    pc.baud (115200);
    pc.printf("Hello World \n\r");
    wait(0.2);
    bool exit = false;
    char command[4];
    
    while(!exit)
    {
        pc.printf("Command: ");
        for(int i = 0 ; i  < 4; i++)
        {
            char c = pc.getc();
            command[i] = c;
            pc.printf("%02x  ", c);    
        }
        pc.printf("\n\r");
        
        switch(command[0])
        {
            case 'c': reset(); break;
            case 'd': eraseChip(); break;
            case 'i': readID(); break;
            case 'b': bufferPage(); break;
            case 'r': readPage(command[1],command[2], command[3], 256); break;
            case 'R': readPage(command[1],command[2], command[3], 8192); break;
            case 'q': writeEnable(); break;
            case 'w': writePage(command[1], command[2], command[3]); waitBusy(); break;
            case 'e': erasePage(command[1], command[2], command[3]); waitBusy(); break;
            case 'x': exit = true; break;
            default: break;
        }
    }
    
    pc.printf("Goodbye World \n\r");
}
