
#include <iostream>
#include <mqtt/async_client.h>
 
const std::string SERVER_ADDRESS{ "tcp://localhost:1883" };
const std::string CLIENT_ID{ "cpp_mqtt_Client" };
 
class mqtt_callback :public virtual mqtt::callback 
{
	void connected(const std::string& cause)override 
	{
		std::cout << "Connected to MQTT server" << std::endl;
	}
	void connection_lost(const std::string& cause)override
	{
		std::cout << "Connection lost:" << cause << std::endl;
	}
	void message_arrived(mqtt::const_message_ptr message) override 
	{
		std::cout << "Message received: "<< "topic: " << message->get_topic()<< ", payload: " << message->to_string() << std::endl;
	}
 
	void delivery_complete(mqtt::delivery_token_ptr token)override
	{
		std::cout << "Delivery complete" << std::endl;
	}
 
};
int main() {
	// Create an MQTT client
	mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
	try {
		// Connect to the MQTT server
		mqtt::connect_options connOpts;
		client.connect(connOpts)->wait();
		std::cout << "Connected to MQTT server" << std::endl;
		// Do other tasks here...
		// Disconnect from the MQTT server
		client.disconnect()->wait();
		std::cout << "Disconnected from MQTT server" << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << "Error: " << exc.what() << std::endl;
		return 1;
	}
	return 0;
}