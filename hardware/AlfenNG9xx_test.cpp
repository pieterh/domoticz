#include "AlfenNG9xx.h"

void main() {
	CDomoticzHardwareBase *pHardware = nullptr;
    pHardware = new AlfenNG9xx("192.168.1.9", 502);
    cout << "Starting";
    pHardware->Start();

    cout << "Started";
    std::string line;
    std::getline( std::cin, line );

    cout << "Stopping";
    pHardware->Stop();
    
    delete pHardware;
    cout << "Done";
}
