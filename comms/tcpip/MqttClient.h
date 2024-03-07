/*
 * MqttClient.h
 *
 *  Created on: Jan 18, 2024
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_MQTTCLIENT_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_MQTTCLIENT_H_

#include <comms/ethernet/w5500/TcpClient.h>
#include <comms/tcpip/IPv4Address.h>
#include <string.h>

class MqttClient : public TcpClient
{
protected:
	static const constexpr uint8_t Connect = 0x1 << 4;
	static const constexpr uint8_t ConnectAck = 0x2 << 4;
	static const constexpr uint8_t Publish = 0x3 << 4;
	static const constexpr uint8_t PublishAck = 0x4 << 4;
	static const constexpr uint8_t PublishReceived = 0x5 << 4;
	static const constexpr uint8_t PublishRelease = 0x6 << 4;
	static const constexpr uint8_t PublishComplete = 0x7 << 4;
	static const constexpr uint8_t Subscribe = 0x8 << 4;
	static const constexpr uint8_t SubscribeAck = 0x9 << 4;
	static const constexpr uint8_t Unsubscribe = 0xa << 4;
	static const constexpr uint8_t UnsubscribeAck = 0xb << 4;
	static const constexpr uint8_t PingRequest = 0xc << 4;
	static const constexpr uint8_t PingResponse = 0xd << 4;
	static const constexpr uint8_t Disconnect = 0xe << 4;
	static const constexpr uint8_t Authenticate = 0xf << 4;

	static const constexpr uint8_t FlagsConnect = 0b0000;
	static const constexpr uint8_t FlagsConnectAck = 0b0000;
	static const constexpr uint8_t FlagsPublish = 0b0000;
	static const constexpr uint8_t FlagsPublishAck = 0b0000;
	static const constexpr uint8_t FlagsPublishReceived = 0b0000;
	static const constexpr uint8_t FlagsPublishRelease = 0b0010;
	static const constexpr uint8_t FlagsPublishComplete = 0b0000;
	static const constexpr uint8_t FlagsSubscribe = 0b0010;
	static const constexpr uint8_t FlagsSubscribeAck = 0b0000;
	static const constexpr uint8_t FlagsUnsubscribe = 0b0010;
	static const constexpr uint8_t FlagsUnsubscribeAck = 0b0000;
	static const constexpr uint8_t FlagsPingRequest = 0b0000;
	static const constexpr uint8_t FlagsPingResponse = 0b0000;
	static const constexpr uint8_t FlagsDisconnect  = 0b0000;
	static const constexpr uint8_t FlagsAuthenticate = 0b0000;

	static const constexpr uint8_t ConnectFlagUsername = 0x80;
	static const constexpr uint8_t ConnectFlagPassword = 0x40;
	static const constexpr uint8_t ConnectFlagWillRetain = 0x20;
	static const constexpr uint8_t ConnectFlagWillQoS = 0x10;
	static const constexpr uint8_t ConnectFlagWillFlag = 0x08 | 0x04;
	static const constexpr uint8_t ConnectFlagCleanStart = 0x02;
	static const constexpr uint8_t ConnectFlagReserved = 0x01;

	static const constexpr uint8_t PropertySessionExpiryInterval = 0x11;
	static const constexpr uint8_t PropertyPayloadFormatIndicator = 0x01;
	static const constexpr uint8_t PropertyMessageExpiryInterval = 0x01;

public:
	MqttClient(Socket* socket, IPv4Address broker, const char* client_id, uint16_t keep_alive=60, uint16_t port=1883) : TcpClient(socket)
	{
		this->broker = broker;
		this->port = port;
		this->client_id = client_id;
		this->keep_alive = keep_alive;
	}


	bool connect(void)
	{
		bool ret = TcpClient::connect(broker, port);
		if (!ret)
			return false;

		const uint8_t properties_length = 5;
		const uint8_t client_id_length = strlen(client_id);
		uint16_t protocol_name_length = 4;
		uint8_t control_header = Connect | FlagsConnect;
		uint8_t packet_length = 1 + 1 + 2 + protocol_name_length + 1 + 1 + 2 + properties_length + 1 + client_id_length;
		const char* protocol_name = "MQTT";
		const uint8_t protocol_version = 5;
		const uint8_t connect_flags = ConnectFlagCleanStart;
		const uint8_t property = PropertySessionExpiryInterval;
		const uint32_t property_value = 0x00000020;

		// Flush read buffer.
		flush_read();

		// Fixed header
		write(control_header);
		write(packet_length);
		write16(protocol_name_length);
		write(protocol_name, protocol_name_length);
		write(protocol_version);

		// Variable header
		write(connect_flags);
		write16(keep_alive);
		write(properties_length);
		write(property);
		write32(property_value);
		write16(client_id_length);
		write(client_id, client_id_length);
		flush_write();

		// Get ACK response.
		osDelay(100);
		control_header = read();
		packet_length = read();
		while (available())
			read();

		connected = control_header == ConnectAck;
		return connected;
	}

	void write16(uint16_t value)
	{
		write(value >> 8);
		write(value & 0xff);
	}

	void write32(uint32_t value)
	{
		write(value >> 24 & 0xff);
		write(value >> 16 & 0xff);
		write(value >> 8 & 0xff);
		write(value & 0xff);
	}

	bool publish(const char* topic, uint8_t* data, uint16_t length)
	{
		bool ret = TcpClient::connect(broker, port);
		if (!ret)
			return false;

		flush_read();
		const uint16_t topic_length = strlen(topic);

		// Fixed header
		write(Publish | FlagsPublish);  // Control header
		write(2 + topic_length + 1 + length);  // Remaining packet length

		// Variable header
		write16(topic_length);  // Length of topic name.
		write(topic, topic_length);  // Topic name.
		write((uint8_t)0); // Properties length.

		// Data
		write(data, length);
		flush_write();
		osDelay(100);

		flush_read();
		return true;
	}

	bool is_connected(void)
	{
		return connected;
	}

private:
	IPv4Address broker;
	uint16_t port;
	const char* client_id;
	uint16_t keep_alive;
	bool connected;
};

#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_MQTTCLIENT_H_ */
