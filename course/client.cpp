#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>

const std::map<std::string, std::string> genres = {

    {"Rap", "116"},
    {"Pop", "132"},
    {"Reggae", "144"},
    {"R&B", "165"},
    {"Soul", "169"},
    {"Alterantive", "85"},
};

const std::array<std::string, 2> discogMethods = {

    "tracks",
    // BROKEN 8/30/26 "artists",
    "albums"
};

namespace TopList {
    struct Track {
        
        std::string trackName;
        std::string artistName;
        std::string picture;
    };

/*
 * Broken 8/30/26
    struct Artist {
        
        std::string artistName;
        std::string picture;
    };
*/
    struct Album {
        
        std::string albumName;
        std::string artistName;
        std::string picture;
    };
}
// https://api.deezer.com/chart/116/tracks

using json = nlohmann::json;

std::string retrieveData(const std::string genre, const std::string discogMethod) {

    sf::Http http;
    http.setHost("api.deezer.com");

    // HTTP request
    sf::Http::Request req;

    //std::string uri = "/chart/116/tracks"; 

    try {
        
        // Fill the request...
        req.setMethod(sf::Http::Request::Method::Get);
        req.setUri("chart/" + genre + "/" + discogMethod);
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

void parseAlbumResponse(const std::string &body) {

    try {
        
        json data = json::parse(body);

        std::vector<TopList::Album> albums;

        int count = 0;
        for (const auto &album : data["data"]) {
            if (count == 5) break;

            TopList::Album a;

            a.albumName = album["title"];
            a.artistName = album["artist"]["name"];
            a.picture = album["artist"]["picture"];

            albums.push_back(a);

            count++;
        }

        for (const auto &album : albums) {

            std::cout << "album title: " << album.albumName << "\n";
            std::cout << "artist name: " << album.artistName << "\n";
            std::cout << "picture: " << album.picture << "\n";
            std::cout << "-------------------------------" << "\n";

        }
    }
    catch (json::parse_error &e) {

        std::cerr << "parse error at : " << e.what() << "\n";
    }
}

void parseTrackResponse(const std::string &body) {

    try {
        
        json data = json::parse(body);

        std::vector<TopList::Track> tracks;

        int count = 0;
        for (const auto &track : data["data"]) {
            if (count == 5) break;

            TopList::Track t;

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

/*
 * BROKEN 8/30/26
void parseArtistResponse(const std::string &body) {

    try {
        
        json data = json::parse(body);

        std::cout << data.dump(3) << "\n";

        std::vector<TopList::Artist> artists;

        int count = 0;
        for (const auto &artist : data["data"]) {
            if (count == 5) break;

            TopList::Artist a;

            a.artistName = artist["artist"]["name"];
            a.picture = artist["artist"]["picture"];

            artists.push_back(t);

            count++;
        }

        for (const auto &artist : artist) {

            std::cout << "artist name: " << artist.artistName << "\n";
            std::cout << "picture: " << artist.picture << "\n";
            std::cout << "-------------------------------" << "\n";

        }

   }
    catch (json::parse_error &e) {

        std::cerr << "parse error at : " << e.what() << "\n";
    }
}
*/

int main() {

    std::string res = "";

    for (const std::string &method : discogMethods) {

        for (const auto &[key, value] : genres) {

            std::cout << "\n\n---- Retrieving top " << method << " for " << key << " ----" << "\n\n";

            res = retrieveData(value, method);

            if (method == "albums") {
                parseAlbumResponse(res);
            }
            else if ( method == "tracks") {
                parseTrackResponse(res);
            }
        }
    }
    //parseTrackResponse(res);

    return 0;
}
