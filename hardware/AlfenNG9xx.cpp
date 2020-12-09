#include "AlfenNG9xx.h"
#include "../main/Logger.h"
#include "modbus.h"

#define ALFEN_POLL_INTERVAL 10


CAlfenNG9xx::CAlfenNG9xx(const int ID, const std::string &IPAddress, const unsigned short usIPPort) :
	m_szIPAddress(IPAddress), m_usIPPort(usIPPort)
{
	m_HwdID = ID;
    	Init();
}

void CAlfenNG9xx::Init()
{
}

bool CAlfenNG9xx::StartHardware()
{
	RequestStart();
	Init();
	//Start worker thread
	m_thread = std::make_shared<std::thread>(&CAlfenNG9xx::Do_Work, this);
	SetThreadNameInt(m_thread->native_handle());
	m_bIsStarted = true;
	sOnConnected(this);
	return (m_thread != nullptr);
}

bool CAlfenNG9xx::StopHardware()
{
	if (m_thread)
	{
		RequestStop();
		m_thread->join();
		m_thread.reset();
	}
	m_bIsStarted = false;
	return true;
}

bool CAlfenNG9xx::WriteToHardware(const char* pdata, const unsigned char /*length*/)
{
	return false;
}

void CAlfenNG9xx::Do_Work()
{
	bool bFirstTime = true;
	Log(LOG_STATUS, "Worker started...");
	int sec_counter = ALFEN_POLL_INTERVAL - 5;
	while (!IsStopRequested(1000))
	{
		sec_counter++;
		if (sec_counter % 12 == 0)
		{
			m_LastHeartbeat = mytime(nullptr);
		}

		if ((sec_counter % ALFEN_POLL_INTERVAL == 0) || (bFirstTime))
		{
			sec_counter = 0;
			if (bFirstTime)
			{
				bFirstTime = false;
			}
			GetInfo();
		}

	}
	Log(LOG_STATUS, "Worker stopped...");
}

void CAlfenNG9xx::GetInfo()
{
    Log(LOG_STATUS, "Reading...");
}
