#include "modbus.h"
#include <stdint.h>
#include "hexdump.h"
//typedef unsigned short registers;
//typedef unsigned short unsigned16;

template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I)<<1) {
    static const char* digits = "0123456789ABCDEF";
    std::string rc(hex_len,'0');
    for (size_t i=0, j=(hex_len-1)*4 ; i<hex_len; ++i,j-=4)
        rc[i] = digits[(w>>j) & 0x0f];
    return rc;
}
class ModbusClient {
    public:
    ModbusClient();
    std::string ConvertRegistersToString(__uint16_t  *registers, unsigned short offset, unsigned short nrOfRegisters);
    __uint16_t ConvertRegistersToUnsigned16(__uint16_t *registers);
    __uint16_t ConvertRegistersToUnsigned16(__uint16_t *registers, unsigned short offset);
    float ConvertRegistersToFloat32(__uint16_t *registers);
    float ConvertRegistersToFloat32(__uint16_t *registers, unsigned short offset);

    float ConvertRegistersToFloat32_ABCD(__uint16_t *registers, unsigned short offset);
    float ConvertRegistersToFloat32_DCBA(__uint16_t *registers, unsigned short offset);    
    float ConvertRegistersToFloat32_BADC(__uint16_t *registers, unsigned short offset);        
};


ModbusClient::ModbusClient() {

};

std::string ModbusClient::ConvertRegistersToString(__uint16_t  *registers, unsigned short offset, unsigned short nrOfRegisters){
    std::string str = "";
    for(int i=0; i < nrOfRegisters; i++) {
        str.push_back((registers[i + offset] >> 8u));
        str.push_back(registers[i + offset] & 0x00FFu);
    }
    return str;
}

__uint16_t ModbusClient::ConvertRegistersToUnsigned16(__uint16_t *registers){
    return ConvertRegistersToUnsigned16(registers, 0);
}

__uint16_t ModbusClient::ConvertRegistersToUnsigned16(__uint16_t *registers, unsigned short offset){
    return registers[offset];
};

float ModbusClient::ConvertRegistersToFloat32(__uint16_t *registers){
    return ConvertRegistersToFloat32(registers, 0);
}

static inline uint16_t bswap_16(uint16_t x)
{
    return (x >> 8) | (x << 8);
}

static inline uint32_t bswap_32(uint32_t x)
{
    return (bswap_16(x & 0xffff) << 16) | (bswap_16(x >> 16));
}

float ModbusClient::ConvertRegistersToFloat32(__uint16_t *registers, unsigned short offset){
    float result;
    uint32_t i;

    i = ntohl((((uint32_t)registers[offset]) << 16) + registers[offset + 1]);
    memccpy(&result, &i, 1, 4);
    return result;
};

float ModbusClient::ConvertRegistersToFloat32_ABCD(__uint16_t *registers, unsigned short offset){
    float result;
    uint32_t i;

    i = ntohl(((uint32_t)registers[offset] << 16) + registers[offset + 1]);    
    memccpy(&result, &i, 1, 4);
    return result;
};

float ModbusClient::ConvertRegistersToFloat32_DCBA(__uint16_t *registers, unsigned short offset){
    float result;
    uint32_t i;

    i = ntohl(bswap_32((((uint32_t)registers[offset]) << 16) + registers[offset + 1]));
    memccpy(&result, &i, 1, 4);
    return result;
};

float ModbusClient::ConvertRegistersToFloat32_BADC(__uint16_t *registers, unsigned short offset){
    float result;
    uint32_t i;

    i = ntohl((uint32_t)(bswap_16(registers[offset]) << 16) + bswap_16(registers[offset + 1]));
    memccpy(&result, &i, 1, 4);
    return result;
};

#include <iostream>
#include <istream>
#include <streambuf>
#include <string>
struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};
void dump(char *pBuffer, int size){
    membuf sbuf(pBuffer, pBuffer + size);
    std::istream in(&sbuf);
HexDump::dumpStream(std::cout, in);
}

int main() {
  __uint16_t buffer[200] ;

  memset(&buffer, 0, sizeof(buffer));
  float tst = 22;
  memccpy(&buffer, &tst, 1, sizeof(tst));
  dump((char*)&buffer, 32); 

  modbus *mbus = new modbus("192.168.1.9", 502);
  mbus->modbus_set_slave_id(200);
  mbus->modbus_connect();
  memset(&buffer, 0, sizeof(buffer));
  std::cout << "read 1 result " << mbus->modbus_read_holding_registers(100, 79, buffer) <<std::endl;
  dump((char*)&buffer, 79*2);

  ModbusClient mc = ModbusClient();
  std::string  name = mc.ConvertRegistersToString(buffer, 0, 17);
  std::string  manufacturer = mc.ConvertRegistersToString(buffer, 17, 5);
  std::string  firmware = mc.ConvertRegistersToString(buffer, 23, 17);  
  std::cout << "Name                       : " << name << "\n";
  std::cout << "Manufacturer               : " << manufacturer << "\n";
  std::cout << "Firmware version           : " << firmware << "\n";  

  memset(&buffer, 0, sizeof(buffer));
  std::cout << "read 2 result " << mbus->modbus_read_holding_registers(1100, 6, buffer) <<std::endl;
  dump((char*)&buffer, 6*2);  
  std::cout << "Station Active Max Current : " << mc.ConvertRegistersToFloat32(buffer, 0) <<std::endl;
  std::cout << "Temperature                : " << mc.ConvertRegistersToFloat32(buffer, 0) <<std::endl;  
  std::cout << "OCPP state                 : " << mc.ConvertRegistersToUnsigned16(buffer, 4) <<std::endl;
  std::cout << "Nr of sockets              : " << mc.ConvertRegistersToUnsigned16(buffer, 5) <<std::endl;


  memset(&buffer, 0, sizeof(buffer));
  mbus->modbus_set_slave_id(1);
  std::cout << "read 3 result " << mbus->modbus_read_holding_registers(300, 125, buffer) <<std::endl;
  dump((char*)&buffer, 125*2); 

  memset(&buffer, 0, sizeof(buffer));
  mbus->modbus_set_slave_id(1);
  std::cout << "read 4 result " << mbus->modbus_read_holding_registers(1200, 16, buffer) <<std::endl;
  dump((char*)&buffer, 16*2); 



  mbus->modbus_close();
  delete mbus;



}



        // public static string ConvertRegistersToString(ushort[] registers, int offset, int nrOfRegisters)
        // { 
        // byte[] result = new byte[nrOfRegisters*2];
        // byte[] registerResult = new byte[2];
        
        //     for (int i = 0; i < nrOfRegisters; i++)
        //     {
        //         registerResult = BitConverter.GetBytes(registers[offset + i]);
        //         result[i * 2] = registerResult[1];
        //         result[i * 2 + 1] = registerResult[0];
        //     }
        //     return System.Text.Encoding.Default.GetString(result);
        // }
