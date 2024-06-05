#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <math.h>

struct Dane
{
	std::string date;
	std::string hour;
	std::string messageType;
	std::string fragsNumber;
	std::string repeatIndicator;
	std::string seqID;
	std::string channelCode;
	std::string payload;
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


std::string reverseEachBitSegment(const std::string& bitString, int length) {
    

    std::string result;
    for (size_t i = 0; i < bitString.size(); i += length) {
        std::string segment = bitString.substr(i, length);
        std::reverse(segment.begin(), segment.end());
        result += segment;
    }

    return result;
}
std::string extractBitSubstring(const std::string& bitString, int start, int end) {
    // Sprawdzenie poprawności wejściowych parametrów
    if (start < 0 || end >= bitString.size() || start > end) {
        throw std::out_of_range("Nieprawidłowe pozycje start i end");
    }
    // Wyodrębnienie podciągu bitów
    return bitString.substr(start, end - start + 1);
}

unsigned long long convertBitsToDecimal(const std::string& bitString) {
    unsigned long long decimalValue = 0;
    int length = bitString.size();
    
    for (int i = 0; i < length; ++i) {
        if (bitString[length - 1 - i] == '1') {
            decimalValue += static_cast<unsigned long long>(std::pow(2, i));
        }
    }

    return decimalValue;
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
		std::string date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum;
		int repeatIndicator;
		std::getline(ss, date, '	');
		std::getline(ss, hour, '	');
		std::getline(ss, messageType, ',');
		std::getline(ss, fragsNumber, ',');
		std::getline(ss, repeatIndicatorStr, ',');
		repeatIndicator = std::stoi(repeatIndicatorStr);
		std::getline(ss, seqID, ',');
		std::getline(ss, channelCode, ',');
		std::getline(ss, payload, ',');
		std::getline(ss, bytes, '*');
		std::getline(ss, checkSum, '*');

		dane.push_back({ date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum });
	}

	file.close();

	std::cout << "Odczytane dane z pliku: " << std::endl;

	//for (const auto& Dane : dane)
	//{
	std::string aisMessage = "B3n@?S0000EOkfWiUF8@7wSUkP06";   //testowy ciąg mmsi z pliku
	//std::string aisMessage = Dane.payload;
    std::string bitString = decodeAIS(aisMessage);
	//std::cout << "ciąg: " << bitString <<std::endl;

	std::string revBitstring = reverseEachBitSegment(bitString, 6);
    //std::cout << "Odwrócony ciąg bitów: " << revBitstring << std::endl;


	std::string revMessType = extractBitSubstring(revBitstring, 0, 5);
	std::string bitMessType = reverseEachBitSegment(revMessType, revMessType.length());
	unsigned long long messType = convertBitsToDecimal(bitMessType);
    std::cout << "Typ wiadomosci: " << revMessType << std::endl;
	std::cout << "Typ wiadomosci: " << bitMessType << std::endl;
	std::cout << "Typ wiadomosci: " << messType << std::endl;
	std::string revRepIndicator = extractBitSubstring(revBitstring, 6, 7);
	std::string bitRepIndicator = reverseEachBitSegment(revRepIndicator, revRepIndicator.length());
	unsigned long long repIndicator = convertBitsToDecimal(bitRepIndicator);
    std::cout << "Powtórzenia: " << revRepIndicator << std::endl;
	std::cout << "Powtórzenia: " << bitRepIndicator << std::endl;
	std::cout << "Powtórzenia: " << repIndicator << std::endl;
	std::string revMmsiBits = extractBitSubstring(revBitstring, 8, 37);
	std::string bitMmsiBits = reverseEachBitSegment(revMmsiBits, revMmsiBits.length());
	unsigned long long mmsiBits = convertBitsToDecimal(bitMmsiBits);
    std::cout << "MMSI: " << revMmsiBits << std::endl;
	std::cout << "MMSI: " << bitMmsiBits << std::endl;
	std::cout << "MMSI: " << mmsiBits << std::endl;




	return 0;
}