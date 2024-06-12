// Projekt POS
// Dekodowanie AIS

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <math.h>
#include <iomanip>

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


std::string extractBitSubstring(const std::string &bitString, int start, int end)
{
	if (start < 0 || end >= bitString.size() || start > end)
	{
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
	std::string result;
	bool encounteredAtSign = false;

	for (size_t i = 0; i < bitString.length(); i += 6)
	{
		std::string chunk = bitString.substr(i, 6);
		int decimalValue = std::bitset<6>(chunk).to_ulong();

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
		status = "Under way using engine									  ";
		break;
	case (1):
		status = "At anchor													  ";
		break;
	case (2):
		status = "Not under command											  ";
		break;
	case (3):
		status = "Restricted manoeuverability								  ";
		break;
	case (4):
		status = "Constrained by her draught								  ";
		break;
	case (5):
		status = "Moored													  ";
		break;
	case (6):
		status = "Aground													  ";
		break;
	case (7):
		status = "Engaged in fishing										  ";
		break;
	case (8):
		status = "Under way sailing											  ";
		break;
	case (9):
		status = "Reserved for future amendment of Navigational Status for HSC";
		break;
	case (10):
		status = "Reserved for future amendment of Navigational Status for WIG";
		break;
	case (11):
		status = "Power-driven vessel towing astern (regional use)			  ";
		break;
	case (12):
		status = "Power-driven vessel pushing ahead or towing alongside (	  ";
		break;
	case (13):
		status = "Reserved for future use									  ";
		break;
	case (14):
		status = "AIS-SART is active										  ";
		break;
	default:
		status = "Undefined													  ";

		return status;
	}
return status;	
}

std::string formatToTwoDigits(const std::string& number) {
    int num = std::stoi(number);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << num;
    return ss.str();
}

std::string decodePayload(const std::string &bitString, int start, int end, std::string info, std::string dataType)
{
	std::string revString = extractBitSubstring(bitString, start, end);

	if (revString == "Error")
	{
		return "Error";
	}
	
	std::string decimal = convertBitsToDecimal(revString);

	if (dataType == "nav")
	{
		decimal = navStatus(decimal);
		
		return decimal;
	}
	else if (dataType == "b")
	{
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
		lt = lt / 1000000;
		std::string ltude = std::to_string(lt);
		return ltude;
	}
	else if (dataType == "u")
	{
		float speed = std::stof(decimal);
		speed = speed / 10;
		std::string sspeed = std::to_string(speed);
		return sspeed;
	}
	else if (dataType == "t")
	{
		std::string text;
		text = binaryToASCII(revString);
		return text;
	}
	else if(dataType == "clock"){
		std::string t;
		t = formatToTwoDigits(decimal);
		if(t == "60"){
			t = "59";
		}
		
		return t;
	}
	else if (dataType == "")
	{
		return decimal;
	}
}

std::string message1to3(std::string revBitstring)
{

	std::string repeatIndicator, mmsiout, navout, rotout, sogout, posaccout, lonout, latout, cogout, thout, tsout, miout, spareout, rflagout, rstatus;
	repeatIndicator = decodePayload(revBitstring, 6, 7, "RepeatIndicator: ", "");
	mmsiout = decodePayload(revBitstring, 8, 37, "MMSI: ", "");
	if(mmsiout.size() != 9){
		return "	Error! - nieprawidlowe MMSI";
	}
	navout = decodePayload(revBitstring, 38, 41, "NavStatus: ", "nav");
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
	outputstream << mmsiout << "	" << navout<< "				" << rotout << "			" << sogout << "		" << posaccout << "			" << lonout << "		" << latout << "		" << cogout << "		" << thout << "			" << tsout << "			" << miout << "			" << spareout << "			" << rflagout << "			" << rstatus;
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
		return "	Error! - nieprawidlowe MMSI";
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
	etah = decodePayload(revBitstring, 283, 287, "ETA hour: ", "clock");
	etamin = decodePayload(revBitstring, 288, 293, "ETA minute: ", "clock");
	draught = decodePayload(revBitstring, 294, 301, "Draught: ", "u");
	destination = decodePayload(revBitstring, 302, 421, "Destination: ", "t");
	dte = decodePayload(revBitstring, 422, 422, "DTE: ", "");
	std::ostringstream outputstream;
	outputstream  << mmsiout << " 		" << aisversion << "			" << imonumber << "				"<< callsign << "				"<< vesselname << "				" << shiptype << "				" << dimtobow << "-"<< dimtostern << "-" << dimtoport << "-" << dimtosb << "			" << pft << "			" << etam << "-" << etad << "		" << etah << ":" << etamin << "			" << draught << "			" << destination << "			" << dte;
	std::string output;
	output = outputstream.str();
	return output;
}

int main()
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

	outFile1 << "Date			" << "Hour		" << " Msg Type " << "		MMSI		" << "Nav Status														" <<  "	Rate of Turn	" << " Speed " << "		 Pos Accuracy " << "		 Longtitude " << " 	Latitude " << "		 CoG " << " 		Heading " << " Time stamp " << "	Maneuver " << " 	Spare " << "	 RAIM " << "		 RADIO STATUS" <<std::endl;
	outFile5 << "Date			" << "Hour		" << " Msg Type " << "		MMSI		" << "AIS version"	 <<  "		IMO	" << " 				Call Sign " << "			Vessel Name " << "		Ship Type " << " 		Dimension" << "		 EPFD " << " 		ETA Date" << " 		ETA Time " << "		Draught " << "		Destination " << "	DTE" << "	Spare" <<std::endl;

	std::vector<InputData> inputData;
	std::string line;
	std::string buffer;

	
	while (std::getline(file, line))
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

                buffer = nextPayload;
                payload += buffer;
            }
			

		inputData.push_back({date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum});
		
		}
		else
		inputData.push_back({date, hour, messageType, fragsNumber, repeatIndicatorStr, seqID, channelCode, payload, bytes, checkSum});
		
	}
		

	file.close();

	for (const auto &InputData : inputData)
	{
		
		std::string date = InputData.date;
		std::string hour = InputData.hour;
		std::string aisMessage = InputData.payload;
		std::string frags = InputData.fragsNumber;
		std::string repeat = InputData.repeatIndicator;
		std::string bitString = decodeAIS(aisMessage);

		if (bitString == "Error")
		{
			outFile1 << "Error" << std::endl;
		}

		std::string out;
		std::string msgType = decodePayload(bitString, 0, 5, "MessageType: ", "");
		if (msgType == "1" || msgType == "2" || msgType == "3")
		{
			out = message1to3(bitString);
			outFile1 << date << "		" << hour << "		"<<  msgType << "			" << out << std::endl;
		}
		else if (msgType == "5")
		{
			out = message5(bitString);
			outFile5 << date << "		" << hour << "		" << msgType <<"			" <<  out << std::endl;
		}
		
	}
	
	outFile1.close();
	outFile5.close();
	return 0;
}