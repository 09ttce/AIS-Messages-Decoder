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

struct OutputData
{
	std::string date;
	std::string hour;
	std::string mmsi;
	std::string navStatus;
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
		
		bitString += std::bitset<6>(value).to_string();
		
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
        std::cout << "Nieprawidłowe pozycje start i end" << std::endl;
		return "Error";
	}
	else
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
std::string navStatus(std::string navi){
	int num = std::stoi(navi);
	std::string status;
	switch(num){
		case(0):
			status = "Under way using engine";
			break;
		case(1):
			status = "At anchor";
			break;
		case(2):
			status = "Not under command";
			break;	
		case(3):
			status = "Restricted manoeuverability";
			break;
		case(4):
			status = "Constrained by her draught";
			break;
		case(5):
			status = "Moored";
			break;
		case(6):
			status = "Aground";
			break;
		case(7):
			status = "Engaged in fishing";
			break;
		case(8):
			status = "Under way sailing";
			break;
		case(9):
			status = "Reserved for future amendment of Navigational Status for HSC";
			break;
		case(10):
			status = "Reserved for future amendment of Navigational Status for WIG";
			break;
		case(11):
			status = "Power-driven vessel towing astern (regional use)";
			break;
		case(12):
			status = "Power-driven vessel pushing ahead or towing alongside (regional use)";
			break;
		case(13):
			status = "Reserved for future use";
			break;
		case(14):
			status = "AIS-SART is active";
			break;
		default: status = "Undefined";
		
		return status;
	}
	
}

std::string decodePayload(const std::string& bitString, int start, int end, std::string info, std::string dataType){
	std::string revString = extractBitSubstring(bitString, start, end);
	if(revString == "Error"){
		return "Error";
	}
	std::string bitStr = reverseEachBitSegment(revString, revString.length());
	std::string decimal = convertBitsToDecimal(bitStr);
	
	if (dataType == "nav"){
		decimal = navStatus(decimal);
		return decimal;
	}
	else if(dataType == "b"){
		if(decimal == "1"){
			return "True";
		}
		else{
		return "False";
		}
	}
	else if(dataType =="I4"){
		float lt = std::stof(decimal);
		lt = lt / 10000000;
		std::string ltude = std::to_string(lt);
		return ltude;
	}
	else if(dataType == "u"){
		float speed = std::stof(decimal);
		speed = speed / 10;
		std::string sspeed = std::to_string(speed);
		return sspeed;
	}
	else if(dataType == ""){
		return decimal;	
	}

}





int main()  // oczywiście wywali się z tego maina większość i zrobi funkcje jak już będzie działać + podzieli sie na pliki bo to w wymaganiach xd
{
	std::ifstream file("data.txt");
	if (!file)
	{
		std::cerr << "Nie mozna otworzyc pliku!" << std::endl;
		return 1;
	}
	std::ofstream outFile("output.txt");
	if(!outFile){
		std::cerr << "Nie można otworzyć pliku" << std::endl;
		return 1;
	}
	outFile << "Date	" << "Hour		" << "MMSI		" << "Navigation Status" << std::endl;
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


	for (const auto& InputData : inputData){
	//std::string aisMessage = "B3n@?S0000EOkfWiUF8@7wSUkP06";   //testowy ciąg payload z pliku
	std::string date = InputData.date;
	std::string hour = InputData.hour;
	std::string aisMessage = InputData.payload;
    std::string bitString = decodeAIS(aisMessage);
	std::string mmsiout, navout, rotout, sogout, posaccout, lonout, latout, cogout, thout, tsout, miout, spareout, rflagout, rstatus;
	
	std::string revBitstring = reverseEachBitSegment(bitString, 6);


	if(bitString == "Error"){
		outFile << "Error" << std::endl;
		break;
	}
	

	std::string msgType = decodePayload(revBitstring, 0, 5, "MessageType: ", "");
	if(msgType == "1" || msgType == "2" || msgType == "3")
	{
	std::string repeatIndicator = decodePayload(revBitstring, 6, 7, "RepeatIndicator: ", "");
	mmsiout = decodePayload(revBitstring, 8, 37, "MMSI: ", "");
	navout = decodePayload(revBitstring, 38, 41, "NavStatus: ", "");
	rotout = decodePayload(revBitstring, 42, 49, "Rate of Turn: ", "");
	sogout = decodePayload(revBitstring, 50, 59, "Speed over ground: ", "u");
	posaccout = decodePayload(revBitstring, 60, 60, "Position accuracy: ", "b");
	lonout = decodePayload(revBitstring, 61, 88, "Longitude: ", "I4");
	latout = decodePayload(revBitstring, 89, 115, "Latitude: ", "I4");
	cogout = decodePayload(revBitstring, 116, 127, "Course over ground: ", "");
	thout = decodePayload(revBitstring, 128, 136, "True heading: ", "");
	tsout = decodePayload(revBitstring, 137, 142, "Time stamp: ", "");
	miout = decodePayload(revBitstring, 143, 144, "Maneuver indicator: ", "");
	spareout = decodePayload(revBitstring, 145, 147, "Spare: ", "");
	rflagout = decodePayload(revBitstring, 148, 148, "RAIM flag: ", "");
	rstatus = decodePayload(revBitstring, 149, 167, "Radio status: ", "");
	
	}
	else if(msgType == "5"){
		std::cout << " 5 - potem " << std::endl;
	}
	else
	std::cout << "Inny typ wiadomości" << std::endl;
	

	outFile << date << "	" << hour << "	" << mmsiout << "		" << navout << "	" << rotout << "	" << sogout << "	" << posaccout << "		" << lonout << "	" << latout << "	" << cogout << "	" << thout << "	" << tsout << "	" << miout << "	" << spareout << "	" << rflagout << "	" << rstatus << std::endl;
	
	}

	

	


	outFile.close();
	return 0;

}