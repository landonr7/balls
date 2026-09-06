#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
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
    {"Alternative", "85"},
};

const std::array<std::string, 2> discogMethods = {

    "tracks",
    // Broken 8/30/26 "artists",
    "albums"
};

namespace TopList {
    struct Track {
        
        std::string trackName;
        long trackId;
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
        long albumId;
        std::string artistName;
        std::string cover;
    };
}

using json = nlohmann::json;

std::string retrieveImage(const std::string imageUrl) {

    sf::Http http;
    http.setHost("api.deezer.com");

    // HTTP request
    sf::Http::Request req;

    try {
        
        // Fill the request...
        req.setMethod(sf::Http::Request::Method::Get);
        req.setUri(imageUrl);
        req.setHttpVersion(1, 1);
    }
    catch (const std::exception &e) {

        std::cerr << "Exception: " << e.what() << "\n" << "Request for image URL failed.\n\n";
    }
    
    // HTTP Response
    sf::Http::Response res = http.sendRequest(req);

    int status = static_cast<int>(res.getStatus());
    std::string loc = "";
    std::string host = "";
    int count = 0;
    size_t pos = std::string::npos;
    std::string newUri = "";
    sf::Http::Response newRes;
    
    // Request redirected
    if (status > 300 && status < 400 ) {

        // Url of redirected webpage
        loc = res.getField("Location");
        if (loc.empty()) std::cerr << "Redirected, but no location header.\n\n";
        
        // Look for third "/" in url
        while (count < 3 && (pos = loc.find("/", pos + 1)) != std::string::npos) {
            count++;
        }

        if (count == 3) {

            // Extract the uri
            newUri = loc.substr(pos);

            // Extract the host (this is a bit clunky)
            host = loc.substr(7, pos - 7);

            try {
                
                // Forming new response
                http.setHost(host);
                req.setUri(newUri);

                // HTTP Response
                newRes = http.sendRequest(req);
            }
            catch (const std::exception &e) {
                
                std::cerr << "Exception: " << e.what() << "\n" << 
                "Redirect response failed:\n" <<
                "uri: " << newUri << "\n" <<
                "host: " << host << "\n\n";
            }

            return newRes.getBody();
        }
    }
    else { std::cerr << "Server responded with " << status << "not properly redirected.\n\n"; }

    return "Image not retrieved";
}

std::string retrieveData(const std::string &genre, const std::string &discogMethod) {

    sf::Http http;
    http.setHost("api.deezer.com");

    // HTTP request
    sf::Http::Request req;

    try {
        
        // Fill the request...
        req.setMethod(sf::Http::Request::Method::Get);
        req.setUri("chart/" + genre + "/" + discogMethod);
        req.setHttpVersion(1, 1);
    }
    catch (const std::exception &e) {

        std::cerr << "Exception: " << e.what() << "\n" <<
        "retrieveData failed.\n\n";
    }
    
    // HTTP Response
    sf::Http::Response res = http.sendRequest(req);

    if (res.getStatus() == sf::Http::Response::Status::Ok)
        return res.getBody();
    else 
        return "Failed to retrieve data.";
}

std::vector<TopList::Album> parseAlbumResponse(const std::string &body) {

    std::vector<TopList::Album> albums;
    
    try {
        
        json data = json::parse(body);

        int record = 0;
        for (const auto &album : data["data"]) {
            
            int count = 0;
            size_t pos = std::string::npos;
            
            if (record == 5) break;

            TopList::Album a;

            a.albumName = album["title"];
            a.albumId = album["id"];
            a.artistName = album["artist"]["name"];
            std::string cover = album["cover"];

            // Look for third "/" in url
            while (count < 3 && (pos = cover.find("/", pos + 1)) != std::string::npos) {
                count++;
            }

            if (count == 3) {

                // Extract the uri
                a.cover = cover.substr(pos);

            //a.cover = album["cover"];
            
            }
            
            albums.push_back(a);

            record++;
        }

        for (const auto &album : albums) {

            std::cout << "album title: " << album.albumName << "\n";
            std::cout << "album id: " << album.albumId << "\n";
            std::cout << "artist name: " << album.artistName << "\n";
            std::cout << "cover: " << album.cover << "\n";
            std::cout << "-------------------------------" << "\n";

        }
    }
    catch (json::parse_error &e) {

        std::cerr << "Parse error at: " << e.what() << "\n" << 
        "in parseAlbumResponse\n\n";
    }

    return albums;
}

std::vector<TopList::Track> parseTrackResponse(const std::string &body) {

    std::vector<TopList::Track> tracks;
    
    try {
        
        json data = json::parse(body);

        int record = 0;
        for (const auto &track : data["data"]) {
            
            int count = 0;
            size_t pos = std::string::npos;
            
            if (record == 5) break;

            TopList::Track t;

            t.trackName = track["title"];
            t.trackId = track["id"];
            t.artistName = track["artist"]["name"];
            std::string picture = track["artist"]["picture"];
            // Look for third "/" in url
            while (count < 3 && (pos = picture.find("/", pos + 1)) != std::string::npos) {
                count++;
            }

            if (count == 3) {

                // Extract the uri
                t.picture = picture.substr(pos);

            }
            
            //t.picture = track["artist"]["picture"];

            tracks.push_back(t);

            record++;
        }

        for (const auto &track : tracks) {

            std::cout << "track title: " << track.trackName << "\n";
            std::cout << "track id: " << track.trackId << "\n";
            std::cout << "artist name: " << track.artistName << "\n";
            std::cout << "picture: " << track.picture << "\n";
            std::cout << "-------------------------------" << "\n";

        }
   }
    catch (json::parse_error &e) {

        std::cerr << "Parse error at: " << e.what() << "\n" <<
        "in parseTrackResponse\n\n";
    }

    return tracks;
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
    std::vector<TopList::Track> tracks;
    std::vector<TopList::Album> albums;
    std::vector<std::string> images;
/*
    for (const std::string &method : discogMethods) {

        for (const auto &[key, value] : genres) {

            std::cout << "\n\n---- Retrieving top " << method << " for " << key << " ----" << "\n\n";

            // Fetch info from Deezer  
            res = retrieveData(value, method);

            if (method == "albums") {
                
                albums = parseAlbumResponse(res);

                for (const TopList::Album &album : albums) {
                
                    images.push_back(retrieveImage(album.cover));
                }
            }
            else if ( method == "tracks") {
                
                tracks = parseTrackResponse(res);
                
                for (const TopList::Track &track : tracks) {

                    images.push_back(retrieveImage(track.picture));
                }
            }

        }
    }
*/
   std::string method = "tracks";

    for (const auto &[key, value] : genres) {

        std::cout << "\n\n---- Retrieving top " << method << " for " << key << " ----" << "\n\n";

        // Fetch info from Deezer  
        res = retrieveData(value, method);

        if ( method == "tracks") {
            
            tracks = parseTrackResponse(res);

            for (const TopList::Track &track : tracks) {

                images.push_back(retrieveImage(track.picture));
            }
        }

    }

    // Create main window
	sf::RenderWindow window(sf::VideoMode({600, 720}), "image");
	window.setFramerateLimit(60);

    sf::Texture texture;

    // Main loop
	while (window.isOpen()) {

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
            // If escape pressed, exit window
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->code == sf::Keyboard::Key::Escape) {
					window.close();
				}
			}	
		}

        int idx = 0;
        for (int row = 0; row < 5; row++) {

            for (int col = 0; col < 6; col++) {

                if (!texture.loadFromMemory(images[idx].data(), images[idx].size())) {
                    std::cerr << "Can't load image" << "\n";
                }
             
                sf::Sprite sprite(texture);
                sprite.setPosition({row * 120.f, col * 120.f});
                window.draw(sprite);

                idx++;
            }
        }

        window.display();
    }
    
    return 0;
}
