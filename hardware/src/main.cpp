// main.cpp
// Dylan Armstrong, 2026

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Explicitly target the API subdomain
constexpr char *BRICKOGNIZE_HOST = "api.brickognize.com";

// Helper function to build a single XML <ITEM> snippet
std::string build_bricklink_xml_item(const std::string &item_id, const std::string &description)
{
    std::ostringstream xml;
    xml << "  <ITEM>\n"
        << "    <ITEMTYPE>P</ITEMTYPE>\n"
        << "    <ITEMID>" << item_id << "</ITEMID>\n"
        << "    <COLOR>11</COLOR>\n"   // Default: Black
        << "    <PRICE>0.10</PRICE>\n" // Default: 0.10
        << "    <QTY>1</QTY>\n"        // Default: 1
        << "    <BULK>1</BULK>\n"
        << "    <CATEGORY></CATEGORY>\n"
        << "    <DESCRIPTION>" << description << "</DESCRIPTION>\n"
        << "    <CONDITION>U</CONDITION>\n" // Default: Used
        << "  </ITEM>" << std::endl;
    return xml.str();
}

int main(int argc, const char* argv[])
{
    httplib::Server svr;

    // Global CORS Preflight Handler for browsers
    svr.Options(R"((.*))", [](const httplib::Request & /*req*/, httplib::Response &res)
                {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200; });

    // Modified endpoint to process response, write local file, and serve XML
    svr.Get("/api/capture-and-recognize", [](const httplib::Request & /*req*/, httplib::Response &res)
            {
        res.set_header("Access-Control-Allow-Origin", "*");

        // Force the SSLClient to use HTTPS secure port 443 directly
        httplib::SSLClient client(BRICKOGNIZE_HOST, 443);
        client.set_follow_location(true);

        std::string image_path = "img/dummy_brick.jpeg"; 
        std::ifstream file(image_path, std::ios::binary);

        if (!file.is_open()) {
            res.status = 500;
            res.set_content("<ERROR>Could not open image file locally</ERROR>", "application/xml");
            return;
        }

        std::string image_buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        httplib::UploadFormDataItems items = {
    { "query_image", image_buffer, "brick_photo.jpg", "image/jpeg" }
};

httplib::Headers headers = {
    { "User-Agent", "LEGO-Sorter/1.0" },
    { "Accept", "application/json" }
};

        if (auto api_res = client.Post("/predict/parts/", headers, items)) {
            if (api_res->status == 200) {
                try {
                    auto raw_json = json::parse(api_res->body);
                    
                    std::ostringstream full_xml;
                    full_xml << "<INVENTORY>\n";

                    // Verify the array structure before attempting to access elements
                    if (raw_json.contains("items") && raw_json["items"].is_array() && !raw_json["items"].empty()) {
                        
                        // Target the first object element inside the array index cleanly
                        auto top_match = raw_json["items"][0];
                        
                        if (top_match.is_object()) {
                            std::string bricklink_id = top_match.value("id", "");
std::string description = top_match.value("name", "");
                            // Safely traverse the nested IDs map layer
                            if (top_match.contains("external_ids") && top_match["external_ids"].is_object()) {
                                auto ext_ids = top_match["external_ids"];
                                if (ext_ids.contains("bricklink")) {
                                    auto bl_ids = ext_ids["bricklink"];
                                    if (bl_ids.is_array() && !bl_ids.empty()) {
                                        bricklink_id = bl_ids[0].get<std::string>();
                                    } else if (bl_ids.is_string()) {
                                        bricklink_id = bl_ids.get<std::string>();
                                    }
                                }
                            }

                            full_xml << build_bricklink_xml_item(bricklink_id, description);
                        }
                    }

                    full_xml << "</INVENTORY>";
                    std::string xml_output = full_xml.str();

                    std::ofstream out_file("inventory.xml");
                    if (out_file.is_open()) {
                        out_file << xml_output;
                        out_file.close();
                        std::cout << "[File IO] Successfully exported inventory.xml" << std::endl;
                    } else {
                        std::cerr << "[File IO Error] Failed to write inventory.xml file" << std::endl;
                    }
                    
                    res.status = 200;
res.set_content(raw_json.dump(), "application/json");

                } catch (const json::parse_error& e) {
                    res.status = 500;
                    res.set_content("<ERROR>Failed to parse API JSON payload</ERROR>", "application/xml");
                }
            } else {
                res.status = api_res->status;
                res.set_content("<ERROR>Upstream API error response</ERROR>", "application/xml");
            }
        } else {
            res.status = 502;
            res.set_content("<ERROR>Failed to reach Brickognize API from C++ server</ERROR>", "application/xml");
        } });

    std::cout << "C++ Backend listening on http://0.0.0" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
