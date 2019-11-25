#pragma once
#include <IncludeMe.h>

struct PlayerStat
{
	std::string name;
	uint32_t score;
	PlayerStat():name("Unknown"),score(0){}
	PlayerStat(std::string name, uint32_t score) :name(name), score(score) {	}
};

class MyClient : public PNet::Client
{
private:
	std::vector<PlayerStat> players;
public:
	MyClient();
	~MyClient();
	void SendPlayerStats(PlayerStat player);
	//void GetHighScoreBoard();
	//client
	bool ProcessPacket(std::shared_ptr<PNet::Packet> packet) override;
	void OnConnect() override;
	//void OnConnectFail() override;
	//void OnDisconnect(std::string reason) override;

	std::vector<PlayerStat> getStats();
};