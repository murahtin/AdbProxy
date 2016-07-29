//#include "ProxyServer.h"

#include <iostream>
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class RequestHandler : public HTTPRequestHandler
{
    public:

	void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
	{
        cout << "Request: " << request.getMethod() << " " << request.getURI() << endl;

        // prepare session
        URI uri(request.getURI());
        HTTPClientSession session(uri.getHost(), uri.getPort());

        // prepare path
        string path(uri.getPathAndQuery());
        if (path.empty()) path = "/";

        // send request
        ostream& reqstream = session.sendRequest(request);
        StreamCopier::copyStream(request.stream(), reqstream);

        // get response
        istream& respstream = session.receiveResponse(response);
        ostream& out = response.send();
        StreamCopier::copyStream(respstream, out);
	}
};

class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
    public:

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
		return new RequestHandler;
	}
};

// Класс прокси сервера.
class ProxyServerApplication : public ServerApplication
{
    protected:
    // Главная функция прокси сервера.
	int main(const vector<string>& args)
    {
        // Создаем экземпляр класса HTTPServer.
        HTTPServer server(new RequestHandlerFactory, ServerSocket(8080), new HTTPServerParams);
        // Стартуем HTTPServer.
        server.start();
        // Ожидаем CTRL-C или kill
        waitForTerminationRequest();
        // Останавливаем HTTPServer
        server.stop();
        // Возвращаем признак нормального завершения работы.
        return Application::EXIT_OK;
    }
};

// Главная функция программмы, создает экземпляр класса прокси сервера и запускает его.
int main(int argc, char** argv)
{
    ProxyServerApplication ServerApp;
    return ServerApp.run(argc, argv);
}
