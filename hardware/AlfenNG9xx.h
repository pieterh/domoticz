#include "DomoticzHardware.h"
#include "hardwaretypes.h"

class CAlfenNG9xx : public CDomoticzHardwareBase
{
    public:
	CAlfenNG9xx(int ID, const std::string &IPAddress, unsigned short usIPPort);
	~CAlfenNG9xx() override = default;


    bool WriteToHardware(const char *pdata, unsigned char length) override;



    private:
    void Init();
	bool StartHardware() override;
	bool StopHardware() override;
	void Do_Work();
    void GetInfo();

    std::string m_szIPAddress;
    unsigned short m_usIPPort;
    std::shared_ptr<std::thread> m_thread;        
};
