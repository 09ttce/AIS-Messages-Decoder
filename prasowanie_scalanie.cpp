#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

/**
 * @struct AISMessage
 * @brief Struktura reprezentuj¹ca wiadomoœæ AIS.
 */
struct AISMessage {
    int totalFragments;  ///< £¹czna liczba fragmentów wiadomoœci.
    int fragmentNumber;  ///< Numer fragmentu wiadomoœci.
    std::string messageId;  ///< Identyfikator wiadomoœci.
    char channel;  ///< Kana³.
    std::string payload;  ///< Dane wiadomoœci.
    int fillBits;  ///< Liczba bitów dope³niaj¹cych.
    int checksum;  ///< Suma kontrolna.
};

/**
 * @brief Parsuje pojedyncz¹ wiadomoœæ AIS.
 * 
 * @param message Wiadomoœæ AIS w formie tekstowej.
 * @return Sparsowana wiadomoœæ AIS w postaci struktury AISMessage.
 */
AISMessage parseAISMessage(const std::string& message) {
    AISMessage parsed;
    char tempChannel;
    char payload[1024];
    int messageId;

    // Debugowanie wejœciowego komunikatu
    std::cout << "Parsing message: " << message << std::endl;

    int result = std::sscanf(message.c_str(), "!AIVDM,%d,%d,%d,%c,%[^,],%d*%x",
                &parsed.totalFragments, &parsed.fragmentNumber, &messageId,
                &tempChannel, payload, &parsed.fillBits, &parsed.checksum);

    if (result == 7) {
        parsed.messageId = std::to_string(messageId);
        parsed.channel = tempChannel;
        parsed.payload = payload;
    } else {
        parsed.totalFragments = -1;
    }

    // Debugowanie wyniku parsowania
    std::cout << "Parsed result: totalFragments=" << parsed.totalFragments
              << ", fragmentNumber=" << parsed.fragmentNumber
              << ", messageId=" << parsed.messageId
              << ", channel=" << parsed.channel
              << ", payload=" << parsed.payload
              << ", fillBits=" << parsed.fillBits
              << ", checksum=" << parsed.checksum
              << std::endl;

    return parsed;
}

/**
 * @brief Scala fragmenty wiadomoœci AIS.
 * 
 * @param messages Wektor wiadomoœci AIS w formie tekstowej.
 * @return Wektor scalonych wiadomoœci AIS.
 */
std::vector<std::string> mergeAISMessages(const std::vector<std::string>& messages) {
    std::map<std::tuple<std::string, char>, std::vector<std::pair<int, std::string>>> groupedMessages;

    for (const auto& message : messages) {
        AISMessage parsed = parseAISMessage(message);
        if (parsed.totalFragments == -1) {
            std::cerr << "Nie uda³o siê sparsowaæ wiadomoœci: " << message << std::endl;
            continue;
        }

        auto key = std::make_tuple(parsed.messageId, parsed.channel);
        groupedMessages[key].emplace_back(parsed.fragmentNumber, parsed.payload);
    }

    std::vector<std::string> mergedMessages;
    for (auto it = groupedMessages.begin(); it != groupedMessages.end(); ++it) {
        std::vector<std::pair<int, std::string>>& fragments = it->second;
        if (fragments.size() == 1) {
            mergedMessages.push_back(fragments[0].second); // Pojedyncza wiadomoœæ
        } else {
            std::sort(fragments.begin(), fragments.end());
            std::stringstream mergedPayload;
            for (auto fragIt = fragments.begin(); fragIt != fragments.end(); ++fragIt) {
                mergedPayload << fragIt->second;
            }
            mergedMessages.push_back(mergedPayload.str());
        }
    }

    return mergedMessages;
}

/**
 * @brief Wczytuje wiadomoœci AIS z pliku tekstowego, pomijaj¹c znaczniki czasowe.
 * 
 * @param filename Nazwa pliku do wczytania.
 * @return Wektor wiadomoœci AIS w formie tekstowej.
 */
std::vector<std::string> readMessagesFromFile(const std::string& filename) {
    std::vector<std::string> messages;
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            // Znalezienie pozycji pierwszego wyst¹pienia wiadomoœci AIS
            size_t pos = line.find("!AIVDM");
            if (pos != std::string::npos) {
                messages.push_back(line.substr(pos));
            }
        }
        file.close();
    } else {
        std::cerr << "Nie mo¿na otworzyæ pliku: " << filename << std::endl;
    }

    return messages;
}

/**
 * @brief Funkcja g³ówna programu.
 * 
 * @return Kod wyjœcia programu.
 */
int main() {
    std::vector<std::string> aisMessages = readMessagesFromFile("ais_messages1.txt");

    if (aisMessages.empty()) {
        std::cerr << "Brak wiadomoœci do przetworzenia." << std::endl;
        return 1;
    }

    std::vector<std::string> merged = mergeAISMessages(aisMessages);
    for (const auto& message : merged) {
        std::cout << "Po³¹czona wiadomoœæ: " << message << std::endl;
    }

    return 0;
}

