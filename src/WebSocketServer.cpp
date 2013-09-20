#include "WebSocketServer.h"

WebSocketServer::WebSocketServer() : server(NULL) {

}

WebSocketServer::~WebSocketServer() {
	stop();

	if (server != NULL) delete server; server = NULL;
}

void WebSocketServer::listen(int port) {
	server = new websocketpp::server<websocketpp::config::asio>();

	try {
        /*server->set_access_channels(websocketpp::log::alevel::all);
        server->clear_access_channels(websocketpp::log::alevel::frame_payload);*/
        server->clear_access_channels(websocketpp::log::alevel::all);
        server->init_asio();

        server->set_open_handler(websocketpp::lib::bind(&WebSocketServer::onOpen, this, websocketpp::lib::placeholders::_1));
        server->set_close_handler(websocketpp::lib::bind(&WebSocketServer::onClose, this, websocketpp::lib::placeholders::_1));
        server->set_message_handler(websocketpp::lib::bind(&WebSocketServer::onMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
		
		// TODO How to add host?
        server->listen(port);
        server->start_accept();
        server->run();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    } catch (websocketpp::lib::error_code e) {
        std::cout << e.message() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

void WebSocketServer::stop() {
	if (server == NULL) {
		return;
	}

	server->stop();
}

void WebSocketServer::addListener(Listener* listener) {
	listeners.push_back(listener);
}

void WebSocketServer::broadcast(std::string message) {
	for (ConnectionsIt it = connections.begin(); it != connections.end(); it++) {
		server->send(*it, message, websocketpp::frame::opcode::TEXT);
	}
}

void WebSocketServer::onOpen(websocketpp::connection_hdl connection) {
	std::cout << "CONNECTION OPENED" << std::endl;

	connections.insert(connection);

	for (ListenersIt it = listeners.begin(); it != listeners.end(); it++) {
		(*it)->onSocketOpen(connection);
	}
}

void WebSocketServer::onClose(websocketpp::connection_hdl connection) {
	std::cout << "CONNECTION CLOSED" << std::endl;

	connections.erase(connection);

	for (ListenersIt it = listeners.begin(); it != listeners.end(); it++) {
		(*it)->onSocketClose(connection);
	}
}

void WebSocketServer::onMessage(websocketpp::connection_hdl connection, websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
	std::cout << "MESSAGE RECEIVED: " << msg->get_payload() << std::endl;

	for (ListenersIt it = listeners.begin(); it != listeners.end(); it++) {
		(*it)->onSocketMessage(msg->get_payload(), connection, msg);
	}
}