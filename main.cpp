

#include<mqtt/async_client.h>
 
using namespace std;
 
const string SERVER_ADDRESS	{ "tcp://localhost:1883" };
const string client_ID		{ "paho_cpp_async_consume" };
const string TOPIC 			{ "hello" };
// const string KEY_STORE      { "/etc/zkey/repository/keys/client/client_cert.pem" };
// const string TRUST_STORE    { "/etc/zkey/repository/keys/ca/ca_cert.pem" };
// const std::string LWT_TOPIC				{ "events/disconnect" };
// const std::string LWT_PAYLOAD			{ "Last will and testament." };
 
const int  QOS = 1;
 
class local_mqtt_client
{
	private:
		string server_address;
		string client_id;
		string topic;
		string ca_cert_pem;
		string client_cert_pem;
		string client_key_pem;
		mqtt::async_client *client = nullptr;
 
	public:
		local_mqtt_client(const string& server_address, const string& client_id, const string& topic, 
							string ca_cert_pem = "", string client_cert_pem = "", string client_key_pem = "") :
				server_address(server_address), client_id(client_id), topic(topic), ca_cert_pem(ca_cert_pem), 
				client_cert_pem(client_cert_pem), client_key_pem(client_key_pem){}
		void connect()
		{
			client = new mqtt::async_client(server_address, client_id);
			auto conn_opts = mqtt::connect_options_builder().clean_session(true).finalize();
 
			try
			{
				client->start_consuming(); 
				agent_utils::write_log("Local MQTT client up and listening", DEBUG);
 
 
				// Connect to the server
 
				cout << "Connecting to the MQTT server..." << flush;
				auto tok = client.connect(connOpts);
 
				// Getting the connect response will block waiting for the
				// connection to complete.
				auto rsp = tok->get_connect_response();
 
				// If there is no session present, then we need to subscribe, but if
				// there is a session, then the server remembers us and our
				// subscriptions.
				if (!rsp.is_session_present())
					client.subscribe(TOPIC, QOS)->wait();
 
				cout << "OK" << endl;
 
				// Consume messages
				// This just exits if the client is disconnected.
				// (See some other examples for auto or manual reconnect)
 
				cout << "Waiting for messages on topic: '" << TOPIC << "'" << endl;
 
				while (true) {
					auto msg = client.consume_message();
					if (!msg) break;
					cout << msg->get_topic() << ": " << msg->to_string() << endl;
				}
 
				// If we're here, the client was almost certainly disconnected.
				// But we check, just to make sure.
 
				if (client.is_connected()) {
					cout << "\nShutting down and disconnecting from the MQTT server..." << flush;
					client.unsubscribe(TOPIC)->wait();
					client.stop_consuming();
					client.disconnect()->wait();
					cout << "OK" << endl;
				}
				else {
					cout << "\nclient was disconnected" << endl;
				}
 
			}
			catch(const mqtt::exception& e)
			{
				string error = e.what();
				agent_utils::write_log(error, ERROR);
				// std::cerr << e.what() << '\n';
			}
		}
 
};
 
/////////////////////////////////////////////////////////////////////////////
 
int main(int argc, char* argv[])
{
	mqtt::async_client client(SERVER_ADDRESS, client_ID);
 
    // auto sslopts = mqtt::ssl_options_builder()
    //                 .trust_store(TRUST_STORE)
    //                 .key_store(KEY_STORE)
    //                 .error_handler([](const std::string& msg) {
    //                     std::cerr << "SSL Error: " << msg << std::endl;
    //                 })
    //                 .finalize();
    // auto willmsg = mqtt::message(LWT_TOPIC, LWT_PAYLOAD, QOS, true);
	auto connOpts = mqtt::connect_options_builder()
		.clean_session(false)
        .clean_session(true)
        // .will(std::move(willmsg))
        // .ssl(std::move(sslopts))
		.finalize();
 
	try {
		// Start consumer before connecting to make sure to not miss messages
 
		client.start_consuming();
 
		// Connect to the server
 
		cout << "Connecting to the MQTT server..." << flush;
		auto tok = client.connect(connOpts);
 
		// Getting the connect response will block waiting for the
		// connection to complete.
		auto rsp = tok->get_connect_response();
 
		// If there is no session present, then we need to subscribe, but if
		// there is a session, then the server remembers us and our
		// subscriptions.
		if (!rsp.is_session_present())
			client.subscribe(TOPIC, QOS)->wait();
 
		cout << "OK" << endl;
 
		// Consume messages
		// This just exits if the client is disconnected.
		// (See some other examples for auto or manual reconnect)
 
		cout << "Waiting for messages on topic: '" << TOPIC << "'" << endl;
 
		while (true) {
			auto msg = client.consume_message();
			if (!msg) break;
			cout << msg->get_topic() << ": " << msg->to_string() << endl;
		}
 
		// If we're here, the client was almost certainly disconnected.
		// But we check, just to make sure.
 
		if (client.is_connected()) {
			cout << "\nShutting down and disconnecting from the MQTT server..." << flush;
			client.unsubscribe(TOPIC)->wait();
			client.stop_consuming();
			client.disconnect()->wait();
			cout << "OK" << endl;
		}
		else {
			cout << "\nclient was disconnected" << endl;
		}
	}
	catch (const mqtt::exception& exc) {
		cerr << "\n  " << exc << endl;
		return 1;
	}
 
 	return 0;
}
 
int main(int argc, char)
{
    return 0;
}