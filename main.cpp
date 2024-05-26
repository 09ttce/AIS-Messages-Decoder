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
			std::cerr << "Niepoprawny znak w wiadomoœci AIS: " << c << std::endl;
			return bitString;
		}
	}
	return bitString;
}

int extractBits(const std::string& bitString, int start, int length) {
	return std::bitset<32>(bitString.substr(start, length)).to_ulong();
}

// Funkcja do odczytu wartoœci liczby z okreœlonego zakresu bitów jako liczba ze znakiem
int extractSignedBits(const std::string& bitString, int start, int length) {
	int value = std::bitset<32>(bitString.substr(start, length)).to_ulong();
	if (value & (1 << (length - 1))) { // Jeœli najstarszy bit jest ustawiony (znak)
		value -= (1 << length); // Zastosuj kodowanie uzupe³nienia do dwóch
	}
	return value;
}


int main()
{
	std::ifstream file("data.txt");
	if (!file)
	{
		std::cerr << "Nie mo¿na otworzyæ pliku!" << std::endl;
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

	for (const auto& Dane : dane)
	{
		std::string aisMessage = "54S3wJ01vs;1K8@KV204q@tpT60:222222222216:@?994wU0AQi0CTjp888";
    std::string bitString = decodeAIS(aisMessage);

	if (!bitString.empty()) {
		//std::cout << "Zdekodowany ci¹g bitów: " << bitString << std::endl;

		
		int mmsi = extractBits(bitString, 0, 40);

		

		
		std::cout << "MMSI: " << mmsi << std::endl;
		
	}
	else {
		std::cerr << "B³¹d w dekodowaniu wiadomoœci AIS." << std::endl;
	}
	}
	return 0;
}