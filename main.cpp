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


int sixBitAsciiToValue(char c)
{
	if (c >= '0' && c <= 'W')
	{
		return c - '0';
	}
	else if (c >= '`' && c <= 'w')
	{
		return c - 56;
	}
	return -1;
}

std::string decodeAIS(const std::string &ais)
{
	std::string bitString;
	for (char c : ais)
	{
		int value = sixBitAsciiToValue(c);

		bitString += std::bitset<6>(value).to_string();
		

	}
	
	return bitString;
}
/*
std::string reverseEachBitSegment(const std::string &bitString, int length)
{

	std::string wynik = "";
	for (size_t i = 0; i < bitString.length(); i += length)
	{
		for(int j = length -1; j >= 0; --j){
		wynik += bitString[i + j];
       
		}
	}
	//std::cout << wynik << std::endl;
	return wynik;
}
*/
std::string extractBitSubstring(const std::string &bitString, int start, int end)
{
	// Sprawdzenie poprawności wejściowych parametrów
	if (start < 0 || end >= bitString.size() || start > end)
	{
		std::cout << "Nieprawidłowe pozycje start i end" << std::endl;
		return "Error";
	}
	else
		return bitString.substr(start, end - start + 1);
}

std::string convertBitsToDecimal(const std::string &bitString)
{
	unsigned int decimalValue = 0;
	int length = bitString.size();

	for (int i = 0; i < length; ++i)
	{
		if (bitString[length - 1 - i] == '1')
		{
			decimalValue += static_cast<unsigned int>(std::pow(2, i));
		}
	}
	std::string strValue = std::to_string(decimalValue);
	return strValue;
}

std::string binaryToASCII(const std::string &bitString)
{
	//std::cout << bitString << std::endl;
	std::string result;
	bool encounteredAtSign = false;

	for (size_t i = 0; i < bitString.length(); i += 6)
	{
		// Wyciągnij 6-bitowy ciąg
		std::string chunk = bitString.substr(i, 6);
		// Zamień 6-bitowy ciąg na liczbę dziesiętną
		int decimalValue = std::bitset<6>(chunk).to_ulong();
		//std::cout << decimalValue << " ";

		char asciiChar;
		if (decimalValue >= 0 && decimalValue <= 31)
		{
			asciiChar = static_cast<char>(decimalValue + 64);
		}
		else if (decimalValue >= 32 && decimalValue <= 63)
		{
			asciiChar = static_cast<char>(decimalValue + 32);
		}
		else
		{
			throw std::invalid_argument("Wartość binarna jest poza zakresem");
		}

		if (asciiChar == '@' && i > 7)
		{
			encounteredAtSign = true;
			break;
		}

		result += asciiChar;
	}

	// Usuń końcowe spacje
	result.erase(result.find_last_not_of(' ') + 1);

	return result;
}

std::string navStatus(std::string navi)
{
	int num = std::stoi(navi);
	std::string status;
	switch (num)
	{
	case (0):
		status = "Under way using engine";
		break;
	case (1):
		status = "At anchor";
		break;
	case (2):
		status = "Not under command";
		break;
	case (3):
		status = "Restricted manoeuverability";
		break;
	case (4):
		status = "Constrained by her draught";
		break;
	case (5):
		status = "Moored";
		break;
	case (6):
		status = "Aground";
		break;
	case (7):
		status = "Engaged in fishing";
		break;
	case (8):
		status = "Under way sailing";
		break;
	case (9):
		status = "Reserved for future amendment of Navigational Status for HSC";
		break;
	case (10):
		status = "Reserved for future amendment of Navigational Status for WIG";
		break;
	case (11):
		status = "Power-driven vessel towing astern (regional use)";
		break;
	case (12):
		status = "Power-driven vessel pushing ahead or towing alongside (regional use)";
		break;
	case (13):
		status = "Reserved for future use";
		break;
	case (14):
		status = "AIS-SART is active";
		break;
	default:
		status = "Undefined";

		return status;
	}
}

std::string decodePayload(const std::string &bitString, int start, int end, std::string info, std::string dataType)
{
	std::string revString = extractBitSubstring(bitString, start, end);

	if (revString == "Error")
	{
		return "Error";
	}
	//std::string bitStr = reverseEachBitSegment(revString, revString.length());
	std::string decimal = convertBitsToDecimal(revString);

	if (dataType == "nav")
	{
		decimal = navStatus(decimal);
		//std::cout << info << decimal << std::endl;
		return decimal;
	}
	else if (dataType == "b")
	{
		//std::cout << info << decimal << std::endl;
		if (decimal == "1")
		{
			return "True";
		}
		else
		{
			return "False";
		}
	}
	else if (dataType == "I4")
	{
		float lt = std::stof(decimal);
		lt = lt / 10000000;
		std::string ltude = std::to_string(lt);
		//std::cout << info << ltude << std::endl;
		return ltude;
	}
	else if (dataType == "u")
	{
		float speed = std::stof(decimal);
		speed = speed / 10;
		std::string sspeed = std::to_string(speed);
		//std::cout << info << speed << std::endl;
		return sspeed;
	}
	else if (dataType == "t")
	{
		std::string text;
		text = binaryToASCII(revString);
		//std::cout << info << text << std::endl;
		return text;
	}
	else if (dataType == "")
	{
		//std::cout << info << decimal << std::endl;
		return decimal;
	}
}

std::string message1to3(std::string revBitstring)
{

	std::string repeatIndicator, mmsiout, navout, rotout, sogout, posaccout, lonout, latout, cogout, thout, tsout, miout, spareout, rflagout, rstatus;
	repeatIndicator = decodePayload(revBitstring, 6, 7, "RepeatIndicator: ", "");
	mmsiout = decodePayload(revBitstring, 8, 37, "MMSI: ", "");
	if(mmsiout.size() != 9){
		return "Error!";
	}
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

	std::ostringstream outputstream;
	outputstream << "		" << mmsiout << "	" << navout<< "	" << rotout << "	" << sogout << "	" << posaccout << "	" << lonout << "	" << latout << "	" << cogout << "	" << thout << "	" << tsout << "	" << miout << "	" << spareout << "	" << rflagout << "	" << rstatus;
	std::string output;
	output = outputstream.str();

	return output;
}

std::string message5(std::string revBitstring)
{
	std::string repeatIndicator, mmsiout, aisversion, imonumber, callsign, vesselname, shiptype, dimtobow, dimtostern, dimtoport, dimtosb, pft, etam, etad, etah, etamin, draught, destination, dte;
	repeatIndicator = decodePayload(revBitstring, 6, 7, "RepeatIndicator: ", "");
	mmsiout = decodePayload(revBitstring, 8, 37, "MMSI: ", "");
	if(mmsiout.size() != 9){
		return "Error!";
	}
	aisversion = decodePayload(revBitstring, 38, 39, "AisVersion: ", "");
	imonumber = decodePayload(revBitstring, 40, 69, "IMO numb: ", "");
	callsign = decodePayload(revBitstring, 70, 111, "CallSign: ", "t");
	vesselname = decodePayload(revBitstring, 112, 231, "Vessel name: ", "t");
	shiptype = decodePayload(revBitstring, 232, 239, "Ship type: ", "");
	dimtobow = decodePayload(revBitstring, 240, 248, "Dimension to bow: ", "");
	dimtostern = decodePayload(revBitstring, 249, 257, "Dimension to stern: ", "");
	dimtoport = decodePayload(revBitstring, 258, 263, "Dimension to port: ", "");
	dimtosb = decodePayload(revBitstring, 264, 269, "DIm to sb: ", "");
	pft = decodePayload(revBitstring, 270, 273, "Pos fix type: ", "");
	etam = decodePayload(revBitstring, 274, 277, "ETA month: ", "");
	etad = decodePayload(revBitstring, 278, 282, "ETA day: ", "");
	etah = decodePayload(revBitstring, 283, 287, "ETA hour: ", "");
	etamin = decodePayload(revBitstring, 288, 293, "ETA minute: ", "");
	draught = decodePayload(revBitstring, 294, 301, "Draught: ", "u");
	destination = decodePayload(revBitstring, 302, 421, "Destination: ", "t");
	dte = decodePayload(revBitstring, 422, 422, "DTE: ", "");
	std::ostringstream outputstream;
	outputstream << "		" << mmsiout << " 	" << aisversion << "	" << imonumber << "	"<< callsign << "	"<< vesselname << "	" << shiptype << "	" << dimtobow << "	"<< dimtostern << "	" << dimtoport << "	" << dimtosb << "	" << pft << "	" << etam << "	" << etad << "	" << etah << "	" << etamin << "	" << draught << "	" << destination << "	" << dte;
	std::string output;
	output = outputstream.str();
	return output;
}

int main() // oczywiście wywali się z tego maina większość i zrobi funkcje jak już będzie działać + podzieli sie na pliki bo to w wymaganiach xd
{
	std::ifstream file("data.txt");
	if (!file)
	{
		std::cerr << "Nie mozna otworzyc pliku!" << std::endl;
		return 1;
	}
	std::ofstream outFile1("output1.txt");
	if (!outFile1)
	{
		std::cerr << "Nie można otworzyć pliku" << std::endl;
		return 1;
	}
	std::ofstream outFile5("output5.txt");
	if (!outFile5)
	{
		std::cerr << "Nie można otworzyć pliku" << std::endl;
		return 1;
	}

	outFile1 << "Date	" << "Hour		" << "MMSI		" << "Navigation Status" << std::endl;
	std::vector<InputData> inputData;
	std::string line;
	std::string buffer;

	
	while (std::getline(file, line)) // poprawny struct, dobrze czyta dane wejściowe ale na razie tylko linia po linii bez błędów/powtórzeń
	{
		std::stringstream ss(line);
		std::string date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum;
		int repeatIndicator, frgsNumber;
		std::getline(ss, date, '	');
		std::getline(ss, hour, '	');
		std::getline(ss, messageType, ',');
		std::getline(ss, fragsNumber, ',');
		frgsNumber = std::stoi(fragsNumber);
		std::getline(ss, repeatIndicatorStr, ',');
		repeatIndicator = std::stoi(repeatIndicatorStr);
		std::getline(ss, seqID, ',');
		std::getline(ss, channelCode, ',');
		std::getline(ss, payload, ',');
		std::getline(ss, bytes, '*');
		std::getline(ss, checkSum, '*');
		
		if (frgsNumber >= 2) {
			
            std::string nextLine;
            if (std::getline(file, nextLine)) {
                std::stringstream nextSs(nextLine);
                std::string nextDate, nextHour, nextMessageType, nextFragsNumber, nextRepeatIndicatorStr, nextSeqID, nextChannelCode, nextPayload, nextBytes, nextCheckSum;

                std::getline(nextSs, nextDate, '	');
                std::getline(nextSs, nextHour, '	');
                std::getline(nextSs, nextMessageType, ',');
                std::getline(nextSs, nextFragsNumber, ',');
                std::getline(nextSs, nextRepeatIndicatorStr, ',');
                std::getline(nextSs, nextSeqID, ',');
                std::getline(nextSs, nextChannelCode, ',');
                std::getline(nextSs, nextPayload, ',');
                std::getline(nextSs, nextBytes, '*');
                std::getline(nextSs, nextCheckSum, '*');

                buffer = nextPayload;  // Zakładamy, że mmsi jest w polu payload następnej linii
                payload += buffer;  // Dopisujemy mmsi do aktualnego payload
            }
			

		inputData.push_back({date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum});
		
		}
		else
		inputData.push_back({date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum});
	}

	file.close();

	for (const auto &InputData : inputData)
	{
		//std::string aisMessage = "54S3wJ01vs;1K8@KV204q@tpT60:222222222216:@?994wU0AQi0CTjp88888888888880";
		
		std::string date = InputData.date;
		std::string hour = InputData.hour;

		std::string aisMessage = InputData.payload;

		std::string frags = InputData.fragsNumber;
		std::string repeat = InputData.repeatIndicator;
		
		std::string bitString = decodeAIS(aisMessage);

		//std::string revBitstring = reverseEachBitSegment(bitString, 6);

		if (bitString == "Error")
		{
			outFile1 << "Error" << std::endl;
			// break;
		}

		std::string out;
		std::string msgType = decodePayload(bitString, 0, 5, "MessageType: ", "");
		if (msgType == "1" || msgType == "2" || msgType == "3")
		{
			out = message1to3(bitString);
			outFile1 << date << "	" << "	" << hour << "		"<<  msgType << "			" << out << std::endl;
		}
		else if (msgType == "5")
		{
			out = message5(bitString);
			outFile5 << date << hour << msgType << out << std::endl;
		}
		
			//std::cout << "Inny typ wiadomości" << std::endl;
		std::cout << out << std::endl;
		
		// outFile << date << "	" << hour << "	" << msgType << "	 "<< mmsiout << "		" << navout << "	" << rotout << "	" << sogout << "	" << posaccout << "		" << lonout << "	" << latout << "	" << cogout << "	" << thout << "	" << tsout << "	" << miout << "	" << spareout << "	" << rflagout << "	" << rstatus << std::endl;
	}
	

	outFile1.close();
	outFile5.close();
	return 0;
}