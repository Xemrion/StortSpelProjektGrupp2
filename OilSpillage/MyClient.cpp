#include "MyClient.h"
#include <iostream>

MyClient::MyClient()
{
	players.clear();
}

MyClient::~MyClient()
{
	players.clear();
}

void MyClient::SendPlayerStats(PlayerStat player)
{
	std::shared_ptr<PNet::Packet> playerData = std::make_shared<Packet>(PNet::PacketType::PT_PlayerStats);
	*playerData << player.name;
	*playerData << player.score;
	connection.pm_outgoing.Append(playerData);
}

bool MyClient::ProcessPacket(std::shared_ptr<PNet::Packet> packet)
{
	switch (packet->GetPacketType())
	{
	case PNet::PacketType::PT_ChatMessage:
	{
		std::string chatmessage;
		*packet >> chatmessage;
		std::cout << "Chat Message: " << chatmessage << std::endl;
		break;
	}
	case PNet::PacketType::PT_IntegerArray:
	{
		uint32_t arraySize = 0;
		*packet >> arraySize;
		std::cout << "Array Size: " << arraySize << std::endl;
		for (uint32_t i = 0; i < arraySize; i++)
		{
			uint32_t element = 0;
			*packet >> element;
			std::cout << "Element[" << i << "] - " << element << std::endl;
		}
		break;
	}
	case PNet::PacketType::PT_PlayerArray:
	{
		uint32_t playerSize = 0;
		*packet >> playerSize;
		std::cout << "Number of Players : " << playerSize << std::endl;
		for (uint32_t i = 0; i < playerSize; i++)
		{
			std::string name;
			std::uint32_t score = 0;
			*packet >> name;
			*packet >> score;
			this->players.push_back(PlayerStat(name, score));
			std::cout << "Player name: " <<name << "	 Score :" << score  << std::endl;
		}
		this->CloseConnection("Du suger");
		break;
	}
	case PNet::PacketType::PT_PlayerStats:
	{
		/*as a client we should not be in this case */
		std::string name;
		std::uint32_t score;
		*packet >> name;
		*packet >> score;
		this->players.push_back(PlayerStat(name, score));
		std::cout << "Player name: " << name << "\nScore :" << score << std::endl;
		break;
	}
	default:
		std::cout << "Unrecognized packet type: " << packet->GetPacketType() << std::endl;
		return false;
	}

	return true;
}

void MyClient::OnConnect()
{
	std::cout << "Successfully connected to the server!" << std::endl;
	std::shared_ptr<PNet::Packet> helloPacket = std::make_shared<PNet::Packet>(PNet::PacketType::PT_ChatMessage);
	*helloPacket << std::string("Hello I am a new Client");
	connection.pm_outgoing.Append(helloPacket);

}

std::vector<PlayerStat> MyClient::getStats()
{
	return this->players;
}
