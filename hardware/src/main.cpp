#include <iostream>
#include <httplib.h>

int main(int argc, const char* argv[]) {
    httplib::Server svr;

    svr.Get("/hi", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    // Start the server
    svr.listen("0.0.0.0", 8080);
}
