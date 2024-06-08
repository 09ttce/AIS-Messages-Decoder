#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>

/**
 * @struct AISMessage
 * @brief Struktura reprezentująca wiadomość AIS.
 */
struct AISMessage {
    int totalFragments;  ///< Łączna liczba fragmentów wiadomości.
    int fragmentNumber;  ///< Numer fragmentu wiadomości.
    std::string messageId;  ///< Identyfikator wiadomości.
    char channel;  ///< Kanał.
    std::string payload;  ///< Dane wiadomości.
    int fillBits;  ///< Liczba bitów dopełniających.
    int checksum;  ///< Suma kontrolna.
};

/**
 * @brief Parsuje pojedynczą wiadomość AIS.
 * 
 * @param message Wiadomość AIS w formie tekstowej.
 * @return Sparsowana wiadomość AIS w postaci struktury AISMessage.
 */
AISMessage parseAISMessage(const std::string& message) {
    AISMessage parsed;
    char tempChannel;
    char payload[1024];
    int messageId;

    // Debugowanie wejściowego komunikatu
    //std::cout << "Parsing message: " << message << std::endl;

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
     
     // std::cout << "Parsed result: totalFragments=" << parsed.totalFragments
          //    << ", fragmentNumber=" << parsed.fragmentNumber
          //    << ", messageId=" << parsed.messageId
           //   << ", channel=" << parsed.channel
           //   << ", payload=" << parsed.payload
           //   << ", fillBits=" << parsed.fillBits
           //   << ", checksum=" << parsed.checksum
           //   << std::endl;

    return parsed;
}

/**
 * @brief Scala fragmenty wiadomości AIS.
 * 
 * @param messages Wektor wiadomości AIS w formie tekstowej.
 * @return Wektor scalonych wiadomości AIS.
 */
std::vector<std::string> mergeAISMessages(const std::vector<std::string>& messages) {
    std::map<std::tuple<std::string, char>, std::vector<std::pair<int, std::string>>> groupedMessages;

    for (const auto& message : messages) {
        AISMessage parsed = parseAISMessage(message);
        if (parsed.totalFragments == -1) {
           // std::cerr << "Nie udało się sparsować wiadomości: " << message << std::endl;
            continue;
        }

        auto key = std::make_tuple(parsed.messageId, parsed.channel);
        groupedMessages[key].emplace_back(parsed.fragmentNumber, parsed.payload);
    }

    std::vector<std::string> mergedMessages;
    for (auto it = groupedMessages.begin(); it != groupedMessages.end(); ++it) {
        std::vector<std::pair<int, std::string>>& fragments = it->second;
        if (fragments.size() == 1) {
            mergedMessages.push_back(fragments[0].second); // Pojedyncza wiadomość
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
 * @brief Wczytuje wiadomości AIS z pliku tekstowego, pomijając znaczniki czasowe.
 * 
 * @param filename Nazwa pliku do wczytania.
 * @return Wektor wiadomości AIS w formie tekstowej.
 */
std::vector<std::string> readMessagesFromFile(const std::string& filename) {
    std::vector<std::string> messages;
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            // Znalezienie pozycji pierwszego wystąpienia wiadomości AIS
            size_t pos = line.find("!AIVDM");
            if (pos != std::string::npos) {
                messages.push_back(line.substr(pos));
            }
        }
        file.close();
    } else {
        std::cerr << "Nie można otworzyć pliku: " << filename << std::endl;
    }

    return messages;
}

/**
 * @brief Funkcja główna programu.
 * 
 * @return Kod wyjścia programu.
 */
int main() {
    std::vector<std::string> aisMessages = readMessagesFromFile("data.txt");

    if (aisMessages.empty()) {
        std::cerr << "Brak wiadomości do przetworzenia." << std::endl;
        return 1;
    }

    std::vector<std::string> merged = mergeAISMessages(aisMessages);
    

   
  // zapisanie do wektora stringów sprasowanych wiadomości
  std::vector<std::string> result(merged.begin(),merged.end());
  // wyświetlenie scalonych wiadomości
  for(const auto& msg : result)
    {
      
      std::cout << msg << std::endl << std::endl;
    }
   return 0;
}