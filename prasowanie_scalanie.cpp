#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

/**
 * @struct AISMessage
 * @brief Struktura reprezentuj�ca wiadomo�� AIS.
 */
struct AISMessage {
    int totalFragments;  ///< ��czna liczba fragment�w wiadomo�ci.
    int fragmentNumber;  ///< Numer fragmentu wiadomo�ci.
    std::string messageId;  ///< Identyfikator wiadomo�ci.
    char channel;  ///< Kana�.
    std::string payload;  ///< Dane wiadomo�ci.
    int fillBits;  ///< Liczba bit�w dope�niaj�cych.
    int checksum;  ///< Suma kontrolna.
};

/**
 * @brief Parsuje pojedyncz� wiadomo�� AIS.
 * 
 * @param message Wiadomo�� AIS w formie tekstowej.
 * @return Sparsowana wiadomo�� AIS w postaci struktury AISMessage.
 */
AISMessage parseAISMessage(const std::string& message) {
    AISMessage parsed;
    int messageId;
    int tempChecksum;
    char tempChannel;
    char payload[1024];

    int result = std::sscanf(message.c_str(), "!AIVDM,%d,%d,%d,%c,%[^,],%d*%x",
                &parsed.totalFragments, &parsed.fragmentNumber, &messageId,
                &tempChannel, payload, &parsed.fillBits, &tempChecksum);

    if (result == 7) {
        parsed.messageId = std::to_string(messageId);
        parsed.channel = tempChannel;
        parsed.payload = payload;
        parsed.checksum = tempChecksum;
    } else {
        parsed.totalFragments = -1;
    }

    return parsed;
}

/**
 * @brief Scala fragmenty wiadomo�ci AIS.
 * 
 * @param messages Wektor wiadomo�ci AIS w formie tekstowej.
 * @return Wektor scalonych wiadomo�ci AIS.
 */
std::vector<std::string> mergeAISMessages(const std::vector<std::string>& messages) {
    std::map<std::tuple<int, std::string, char>, std::vector<std::pair<int, std::string>>> groupedMessages;

    for (const auto& message : messages) {
        AISMessage parsed = parseAISMessage(message);
        if (parsed.totalFragments == -1) {
            std::cerr << "Nie uda�o si� sparsowa� wiadomo�ci: " << message << std::endl;
            continue;
        }

        auto key = std::make_tuple(parsed.totalFragments, parsed.messageId, parsed.channel);
        groupedMessages[key].emplace_back(parsed.fragmentNumber, parsed.payload);
    }

    std::vector<std::string> mergedMessages;
    for (auto it = groupedMessages.begin(); it != groupedMessages.end(); ++it) {
        std::vector<std::pair<int, std::string>>& fragments = it->second;
        if (fragments.size() == 1 && fragments[0].first == 1) {
            mergedMessages.push_back(fragments[0].second); // Pojedyncza wiadomo��
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
 * @brief Wczytuje wiadomo�ci AIS z pliku tekstowego.
 * 
 * @param filename Nazwa pliku do wczytania.
 * @return Wektor wiadomo�ci AIS w formie tekstowej.
 */
std::vector<std::string> readMessagesFromFile(const std::string& filename) {
    std::vector<std::string> messages;
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            messages.push_back(line);
        }
        file.close();
    } else {
        std::cerr << "Nie mo�na otworzy� pliku: " << filename << std::endl;
    }

    return messages;
}

/**
 * @brief Funkcja g��wna programu.
 * 
 * @return Kod wyj�cia programu.
 */
int main() {
    std::vector<std::string> aisMessages = readMessagesFromFile("AIS_messages.txt");

    if (aisMessages.empty()) {
        std::cerr << "Brak wiadomo�ci do przetworzenia." << std::endl;
        return 1;
    }

    std::vector<std::string> merged = mergeAISMessages(aisMessages);
    for (const auto& message : merged) {
        std::cout << "Po��czona wiadomo��: " << message << std::endl;
    }

    return 0;
}

