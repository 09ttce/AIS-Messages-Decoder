#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <bitset>

struct Dane
{
	std::string date;
	std::string hour;
	std::string messageType;
	std::string fragsNumber;
	std::string repeatIndicator;
	std::string seqID;
	std::string channelCode;
	std::string mmsi;
	std::string bytes;
	std::string checkSum;
};

int sixBitAsciiToValue(char c) {
	if (c >= '0' && c <= 'W') {
		return c - '0';
	}
	else if (c >= '`' && c <= 'w') {
		return c - '`' + 40;
	}
	return -1;
}


std::string decodeAIS(const std::string& ais) {
	std::string bitString;
	for (char c : ais) {
		int value = sixBitAsciiToValue(c);
		if (value != -1) {
			bitString += std::bitset<6>(value).to_string();
		}
		else {
			std::cerr << "Niepoprawny znak w wiadomo�ci AIS: " << c << std::endl;
			return bitString;
		}
	}
	return bitString;
}
std::string extractBitSubstring(const std::string& bitString, int start, int end) {
    // Sprawdzenie poprawności wejściowych parametrów
    if (start < 0 || end >= bitString.size() || start > end) {
        throw std::out_of_range("Nieprawidłowe pozycje start i end");
    }
    // Wyodrębnienie podciągu bitów
    return bitString.substr(start, end - start + 1);
}
   




int main()
{
	std::ifstream file("data.txt");
	if (!file)
	{
		std::cerr << "Nie mozna otworzyc pliku!" << std::endl;
		return 1;
	}
	std::vector<Dane> dane;
	std::string line;

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, mmsi, bytes, checkSum;
		int repeatIndicator;
		std::getline(ss, date, '	');
		std::getline(ss, hour, '	');
		std::getline(ss, messageType, ',');
		std::getline(ss, fragsNumber, ',');
		std::getline(ss, repeatIndicatorStr, ',');
		repeatIndicator = std::stoi(repeatIndicatorStr);
		std::getline(ss, seqID, ',');
		std::getline(ss, channelCode, ',');
		std::getline(ss, mmsi, ',');
		std::getline(ss, bytes, '*');
		std::getline(ss, checkSum, '*');

		dane.push_back({ date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, mmsi, bytes, checkSum });
	}

	file.close();

	std::cout << "Odczytane dane z pliku: " << std::endl;

	//for (const auto& Dane : dane)
	//{
	std::string aisMessage = "54S3wJ01vs;1K8@KV204q@tpT60:222222222216:@?994wU0AQi0CTjp888";   //testowy ciąg mmsi z pliku
	//std::string aisMessage = Dane.mmsi;
    std::string bitString = decodeAIS(aisMessage);

	std::string messType = extractBitSubstring(bitString, 0, 5);
    std::cout << "Typ wiadomosci: " << messType << std::endl;
	std::string repIndicator = extractBitSubstring(bitString, 6, 7);
    std::cout << "Powtórzenia: " << repIndicator << std::endl;
	std::string mmsiBits = extractBitSubstring(bitString, 8, 37);
    std::cout << "MMSI: " << mmsiBits << std::endl;

	

	//}
	return 0;
}