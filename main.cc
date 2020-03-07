#include "./Log/Logging.h"
#include "./Tcp/TcpConnection.h"
#include "./Tcp/TcpServer.h"

void onMessage(TcpConnection* conn, string& in) {
  conn->setSend(in);
}
int main(int argc, char** argv) {
  TcpServer server("80", 2);
  server.setOnMessageCallback (bind(onMessage,std::placeholders::_1, std::placeholders::_2));
  server.start();
}
