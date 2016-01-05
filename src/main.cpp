#include <iostream>
#include <fstream>

//JsonCpp
#include "json/json.h"

bool loadJsonFile(const std::string& where, std::string& outString) {
	std::ifstream inFile(where, std::ios::in);
	std::string line, src;

	if(inFile.is_open()) {
		while(std::getline(inFile, line) && inFile.good()) {
			src += line;// += "\n";
		}

		inFile.close();
	} else {
		std::cout << "Failed to open json file " << where << "\n";
		return false;
	}

	outString = src;

	return true;
}

class DataPresenter {
	protected:
		std::string data;
	public:
		DataPresenter(std::string toParse) : data(toParse) {
		}
		~DataPresenter() {
		}

		std::string getNicelyFormattedData() {
			std::string result = data;
			return result;
		}
};

int main(void) {
	std::string data;

	if(!loadJsonFile("data/ME0104T4", data)) {
		return -1;
	}

	DataPresenter presenter(data);

	std::cout << presenter.getNicelyFormattedData() << "\n";

	return 0;
}
