// main.cpp
// Dylan Armstrong, 2026
// Lachlan Wick, 2026
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <httplib.h>

// Explicitly target the API subdomain to bypass structural redirection rules
constexpr char* BRICKOGNIZE_HOST = "api.brickognize.com";

int main() {
    httplib::Server svr;

    time_t timestamp;
    time(&timestamp);
    struct tm datetime = *localtime(&timestamp);
    std::string image_path = "/home/eng30002/eng30002-legot21/hardware/img/" <<strftime(output, 50, "%m/%d/%y", &datetime);
    int Timeout =1000;
    int result = std::system("rpicam-still --zsl -o " << image_path << ".jpg --timeout " << Timeout);











    // Global CORS Preflight Handler for browsers
    svr.Options(R"((.*))", [](const httplib::Request& /*req*/, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; 
    });

    svr.Get("/api/capture-and-recognize", [](const httplib::Request& /*req*/, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        httplib::SSLClient client(BRICKOGNIZE_HOST);
        client.set_follow_location(true);

        
        std::ifstream file(image_path, std::ios::binary);

        if (!file.is_open()) {
            res.set_content("{\"error\": \"Could not open image file locally\"}", "application/json");
            return;
        }

        std::string image_buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        httplib::UploadFormDataItems items = {
            { "query_image", image_buffer, "brick_photo.jpg", "image/jpeg" }
        };

        httplib::Headers headers = {
            { "accept", "application/json" }
        };

        if (auto api_res = client.Post("/predict/parts/", headers, items)) {
            res.status = api_res->status;
            res.set_content(api_res->body, "application/json");
        } else {
            auto err = api_res.error();
            
            res.status = 502;
            res.set_content("{\"error\": \"Failed to reach Brickognize API from C++ server\"}", "application/json");
        }
    });

    std::cout << "C++ Backend listening on http://0.0.0.0" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
