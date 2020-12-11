#include "modbus.h"
#include <stdint.h>

typedef unsigned short registers;
typedef unsigned short unsigned16;

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
    std::string ConvertRegistersToString(registers  *registers, unsigned short offset, unsigned short nrOfRegisters);
    unsigned16 ConvertRegistersToUnsigned16(registers *registers);
    unsigned16 ConvertRegistersToUnsigned16(registers *registers, unsigned short offset);
};


ModbusClient::ModbusClient() {

};

std::string ModbusClient::ConvertRegistersToString(registers  *registers, unsigned short offset, unsigned short nrOfRegisters){
    std::string str = "";
    for(int i=0; i < nrOfRegisters; i++) {
        str.push_back((registers[i + offset] >> 8u));
        str.push_back(registers[i + offset] & 0x00FFu);
    }
    return str;
}

unsigned16 ModbusClient::ConvertRegistersToUnsigned16(registers *registers){
    return ConvertRegistersToUnsigned16(registers, 0);
}

unsigned16 ModbusClient::ConvertRegistersToUnsigned16(registers *registers, unsigned short offset){
    return registers[offset];
};

int main() {
  __uint16_t buffer[200] ;

  modbus *mbus = new modbus("192.168.1.9", 502);
  mbus->modbus_set_slave_id(200);
  mbus->modbus_connect();
  std::cout << "read result " << mbus->modbus_read_holding_registers(100, 79, buffer) << "\n";


  ModbusClient mc = ModbusClient();
  std::string  name = mc.ConvertRegistersToString(buffer, 0, 17);
  std::string  manufacturer = mc.ConvertRegistersToString(buffer, 17, 5);
  std::string  firmware = mc.ConvertRegistersToString(buffer, 23, 17);  
  std::cout << "Name             : " << name << "\n";
  std::cout << "Manufacturer     : " << manufacturer << "\n";
  std::cout << "Firmware version : " << firmware << "\n";  

  mbus->modbus_connect();
  std::cout << "read result " << mbus->modbus_read_holding_registers(1100, 7, buffer) << "\n";
  std::cout << "OCPP state       : " << mc.ConvertRegistersToUnsigned16(buffer, 4) << "\n";
  std::cout << "Nr of sockets    : " << mc.ConvertRegistersToUnsigned16(buffer, 5) << "\n";

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