// Projekt POS
// Dekodowanie AIS
// komentarze do dokumentacji na koniec bo to ostatni problem najmniej istotny


#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <math.h>

struct InputData
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

std::string convertBitsToDecimal(const std::string& bitString) {
    unsigned int decimalValue = 0;
    int length = bitString.size();
    
    for (int i = 0; i < length; ++i) {
        if (bitString[length - 1 - i] == '1') {
            decimalValue += static_cast<unsigned int>(std::pow(2, i));
        }
    }
	std::string strValue = std::to_string(decimalValue);
    return strValue;
}

std::string decodePayload(const std::string& bitString, int start, int end, std::string info){
	std::string revString = extractBitSubstring(bitString, start, end);
	std::string bitStr = reverseEachBitSegment(revString, revString.length());
	std::string decimal = convertBitsToDecimal(bitStr);
	std::cout << info << decimal << std::endl;
	return decimal;
}



int main()  // oczywiście wywali się z tego maina większość i zrobi funkcje jak już będzie działać + podzieli sie na pliki bo to w wymaganiach xd
{
	std::ifstream file("data.txt");
	if (!file)
	{
		std::cerr << "Nie mozna otworzyc pliku!" << std::endl;
		return 1;
	}
	std::vector<InputData> inputData;
	std::string line;

	while (std::getline(file, line))  // poprawny struct, dobrze czyta dane wejściowe ale na razie tylko linia po linii bez błędów/powtórzeń
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

		inputData.push_back({ date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum });
	}

	file.close();


	//for (const auto& InputData : inputData)
	//{
	std::string aisMessage = "B3n@?S0000EOkfWiUF8@7wSUkP06";   //testowy ciąg payload z pliku
	//std::string aisMessage = InputData.payload;
    std::string bitString = decodeAIS(aisMessage);


	std::string revBitstring = reverseEachBitSegment(bitString, 6);



	std::string msgType = decodePayload(revBitstring, 0, 5, "MessageType: ");
	if(msgType == "1" || msgType == "2" || msgType == "3")
	{
	decodePayload(revBitstring, 6, 7, "RepeatIndicator: ");
	decodePayload(revBitstring, 8, 37, "MMSI: ");
	decodePayload(revBitstring, 38, 41, "NavStatus: ");
	decodePayload(revBitstring, 42, 49, "Rate of Turn: ");
	//potem dokoncze, dodam te inne typy danych
	}
	else if(msgType == "5"){
		// potem ogarne to
	}
	else
	std::cout << "Inny typ wiadomości" << std::endl;

	return 0;
}