#pragma once

#include "DomoticzHardware.h"
#include "hardwaretypes.h"

namespace Json
{
	class Value;
};

class EnphaseAPI : public CDomoticzHardwareBase
{
      public:
	EnphaseAPI(int ID, const std::string &IPAddress, unsigned short usIPPort);
	~EnphaseAPI() override = default;
	bool WriteToHardware(const char *pdata, unsigned char length) override;

      private:
	bool StartHardware() override;
	bool StopHardware() override;
	void Do_Work();

	bool getProductionDetails(Json::Value &result);

	void parseProduction(const Json::Value &root);
	void parseConsumption(const Json::Value &root);
	void parseNetConsumption(const Json::Value &root);

	int getSunRiseSunSetMinutes(bool bGetSunRise);

      private:
	bool getInverterDetails(Json::Value& result);
	void parseInverters(const Json::Value& root);
	bool getFromInventory(const Json::Value& root, const std::string &InventoryType, Json::Value& result);
	bool getEnvoyInfo();

	int getSunRiseSunSetMinutes(const bool bGetSunRise);
      private:
	std::string m_szIPAddress;
	std::string m_szSerialNumber;
	std::string m_szEnvoyUsername;
	std::string m_szEnvoyPassword;

	P1Power m_p1power;
	P1Power m_c1power;
	P1Power m_c2power;
	std::shared_ptr<std::thread> m_thread;
};
