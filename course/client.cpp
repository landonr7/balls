#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

/*
// HTTP request
sf::Http::Request req;

// Fill the request...
request.setMethod(sf::Http::Request::Post);
request.setUri("/page.html");
request.setHttpVersion(1, 1);
request.setField("From", "me");
request.setField("Content-Type", "placeholder");
request.setBody("para1=value1&para2=value2");

// HTTP Response
sf::Http::Response res = http.sendRequest(request);

std::cout << "status: " << resgetStatus() << std::endl;
std::cout << "HTTP version: " << res.getMajorHttpVersion() << ", " << res.getMinorHttpsVersion() << std::endl;
std::cout << "Content-Type header: " << res.getField("Content-Type") << std::endl;
std::cout << "body: " << res.getBody() << std::endl;
*/

namespace Retrieving {
    struct Track {
        std::string trackName;
        std::string artistName;
        std::string picture;

    };

    struct Artist {
        std::string artistName;
        std::string picture;
    };
}
// https://api.deezer.com/chart/116/tracks

using json = nlohmann::json;

std::string retrieveData() {

    sf::Http http;
    http.setHost("api.deezer.com");

    // HTTP request
    sf::Http::Request req;

    std::string uri = "/chart/116/tracks"; 

    try {
        
        // Fill the request...
        req.setMethod(sf::Http::Request::Method::Get);
        req.setUri(uri);
        req.setHttpVersion(1, 1);
    }
    catch (const std::exception &e) {

        std::cerr << "exception: " << e.what() << "\n";
    }

    try {
        
        // HTTP Response
        sf::Http::Response res = http.sendRequest(req);

        return res.getBody();

    }
    catch(const std::exception &e) {

        std::cerr << "execption: " << e.what() << "\n";
    }

    return "null";
}

void parseResponse(const std::string &body) {

    try {
        
        json data = json::parse(body);

        std::vector<Retrieving::Track> tracks;

        int count = 0;
        for (const auto &track : data["data"]) {
            if (count == 5) break;

            Retrieving::Track t;

            t.trackName = track["title"];
            t.artistName = track["artist"]["name"];
            t.picture = track["artist"]["picture"];

            tracks.push_back(t);

            count++;
        }

        for (const auto &track : tracks) {

            std::cout << "track title: " << track.trackName << "\n";
            std::cout << "artist name: " << track.artistName << "\n";
            std::cout << "picture: " << track.picture << "\n";
            std::cout << "-------------------------------" << "\n";

        }
   }
    catch (json::parse_error &e) {

        std::cerr << "parse error at : " << e.what() << "\n";
    }
}

int main() {

    const std::string res = retrieveData();

    parseResponse(res);

    return 0;
}
