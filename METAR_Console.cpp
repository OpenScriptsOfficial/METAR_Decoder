#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class MetarDecoder {
private:
    std::string metar;
    std::vector<std::string> tokenize(const std::string& str) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

    std::string decodeWindDirection(const std::string& dir) {
        try {
            int degrees = std::stoi(dir);
            if (degrees >= 0 && degrees <= 360) {
                if (degrees == 0 || degrees == 360) return "North";
                if (degrees == 90) return "East";
                if (degrees == 180) return "South";
                if (degrees == 270) return "West";
                if (degrees > 0 && degrees < 90) return "Northeast";
                if (degrees > 90 && degrees < 180) return "Southeast";
                if (degrees > 180 && degrees < 270) return "Southwest";
                if (degrees > 270 && degrees < 360) return "Northwest";
            }
        }
        catch (...) {
            return "Invalid";
        }
        return "Unknown";
    }

    std::string decodeWindSpeed(const std::string& wind) {
        if (wind.length() >= 5 && wind.substr(3, 2) == "KT") {
            return wind.substr(0, 2) + " knots";
        }
        return "Unknown";
    }

    std::string decodeVisibility(const std::string& vis) {
        if (vis == "9999") return "10 km or more";
        try {
            int visibility = std::stoi(vis);
            return std::to_string(visibility) + " meters";
        }
        catch (...) {
            return "Unknown";
        }
    }

    std::string decodeClouds(const std::string& cloud) {
        if (cloud.substr(0, 3) == "FEW") return "Few clouds at " + cloud.substr(3, 3) + "00 feet";
        if (cloud.substr(0, 3) == "SCT") return "Scattered clouds at " + cloud.substr(3, 3) + "00 feet";
        if (cloud.substr(0, 3) == "BKN") return "Broken clouds at " + cloud.substr(3, 3) + "00 feet";
        if (cloud.substr(0, 3) == "OVC") return "Overcast at " + cloud.substr(3, 3) + "00 feet";
        return "Unknown cloud layer";
    }

    std::string decodeTempDew(const std::string& tempdew) {
        if (tempdew.length() == 5 && tempdew[2] == '/') {
            std::string temp = tempdew.substr(0, 2);
            std::string dew = tempdew.substr(3, 2);
            return "Temperature " + temp + "C, Dew point " + dew + "C";
        }
        return "Unknown";
    }

    std::string decodePressure(const std::string& press) {
        if (press.length() == 5 && press[0] == 'Q') {
            return "Pressure " + press.substr(1, 4) + " hPa";
        }
        return "Unknown";
    }

public:
    MetarDecoder(const std::string& metar_code) : metar(metar_code) {}

    void decode() {
        std::vector<std::string> tokens = tokenize(metar);

        if (tokens.empty() || tokens[0] != "METAR") {
            std::cout << "Invalid METAR format" << "\n";
            return;
        }

        for (size_t i = 1; i < tokens.size(); ++i) {
            const std::string& token = tokens[i];

            if (i == 1 && token.length() == 4 && std::isalpha(token[0])) {
                std::cout << "Aerodrome: " << token << "\n";
            }

            else if (i == 2 && token.length() == 7 && token[6] == 'Z') {
                std::cout << "Time: " << token.substr(0, 2) << "th day, "
                    << token.substr(2, 2) << ":" << token.substr(4, 2) << " UTC" << "\n";
            }

            else if (token.length() >= 5 && token.substr(token.length() - 2) == "KT") {
                std::cout << "Wind: " << decodeWindDirection(token.substr(0, 3)) << " at "
                    << decodeWindSpeed(token.substr(3, 2)) << "\n";
            }

            else if (token.length() == 4 && std::isdigit(token[0])) {
                std::cout << "Visibility: " << decodeVisibility(token) << "\n";
            }

            else if (token.length() == 6 && (token.substr(0, 3) == "FEW" || token.substr(0, 3) == "SCT" ||
                token.substr(0, 3) == "BKN" || token.substr(0, 3) == "OVC")) {
                std::cout << "Clouds: " << decodeClouds(token) << "\n";
            }

            else if (token.length() == 5 && token[2] == '/') {
                std::cout << decodeTempDew(token) << "\n";
            }

            else if (token.length() == 5 && token[0] == 'Q') {
                std::cout << decodePressure(token) << "\n";
            }

            else if (token == "NOSIG") {
                std::cout  << "Remarks: " << "No significant changes expected" << "\n";
            }
            else {
                std::cout << "Unknown token: " << token << "\n";
            }
        }
    }
};

int main() {
    std::string metar_input;

    while (true) {
        std::cout << "Enter METAR: ";
        std::getline(std::cin, metar_input);

        if (metar_input == "exit") {
            std::cout << "Exiting program.\n";
            break;
        }

        if (metar_input.empty()) {
            std::cout << "Please enter a valid METAR or 'exit'\n";
            std::cout << "\n";
            continue;
        }

        std::cout << "\nDecoding METAR: " << "\n";
        std::cout << "----------------\n";
        MetarDecoder decoder(metar_input);
        decoder.decode();
        std::cout << "----------------\n";
        std::cout << "\n";
    }

    return 0;
}