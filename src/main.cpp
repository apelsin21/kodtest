#include <iostream>
#include <fstream>
#include <memory>

#include <cstring>

//JsonCpp
#include "json/json.h"

//libcurl
#include "curl/curl.h"

bool loadJsonFile(const std::string& where, std::string& outString) {
	std::ifstream inFile(where, std::ios::in);
	std::string line, src;

	if(inFile.is_open()) {
		while(std::getline(inFile, line) && inFile.good()) {
			src += line;// += "\n";
		}

		inFile.close();
	} else {
		std::cerr << "Failed to open json file " << where << "\n";
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

struct DownloadedData {
	char* data;
	size_t size;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realSize = size * nmemb;
	DownloadedData* downloaded = (DownloadedData*)userp;

	downloaded->data = (char*)realloc(downloaded->data, downloaded->size + realSize +1);
	if(downloaded->data == nullptr) {
		std::cerr << "Out of memory!\n";
		return 0;
	}

	memcpy(&(downloaded->data[downloaded->size]), contents, realSize);
	downloaded->size += realSize;
	downloaded->data[downloaded->size] = 0;

	return realSize;
}

bool downloadFile(CURL* curl, const std::string& where, std::string& result) {
}

class FileDownloader {
	protected:
		CURL* m_CurlHandle;
	public:
		FileDownloader() {
			m_CurlHandle = curl_easy_init();
		}
		~FileDownloader() {
			curl_easy_cleanup(m_CurlHandle);
		}

		bool downloadFile(const std::string& where, std::string& result) {
			if(m_CurlHandle == nullptr) {
				std::cerr << "Tried to download file with uninitialized libcurl.\n";
				return false;
			}

			std::cout << "Downloading file " << where<< "\n";

			char errorBuffer[CURL_ERROR_SIZE];
			CURLcode res;
			DownloadedData downloaded;

			downloaded.data = (char*)malloc(1); 
			downloaded.size = 0;

			curl_easy_setopt(m_CurlHandle, CURLOPT_URL, where.c_str());
			curl_easy_setopt(m_CurlHandle, CURLOPT_ERRORBUFFER, errorBuffer);
			curl_easy_setopt(m_CurlHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
			curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(m_CurlHandle, CURLOPT_WRITEDATA, (void*)&downloaded);
			curl_easy_setopt(m_CurlHandle, CURLOPT_FAILONERROR, true);

			errorBuffer[0] = 0;

			res = curl_easy_perform(m_CurlHandle);

			if(res != CURLE_OK) {
				std::size_t errLen = strlen(errorBuffer);
				fprintf(stderr, "libcurl: (%d) ", res);

				//Sometimes fprintf is just more convenient
				if(errLen) {
					fprintf(stderr, "%s%s", errorBuffer, ((errorBuffer[errLen - 1] != '\n') ? "\n" : ""));
				} else {
					fprintf(stderr, "%s\n", curl_easy_strerror(res));
				}

				return false;
			}

			result = downloaded.data;

			free(downloaded.data);

			return true;
		}
};

int main(void) {
	FileDownloader downloader;

	std::string result;
	if(!downloader.downloadFile("http://api.scb.se/OV0104/v1/doris/sv/ssd/START/ME/ME0104/ME0104D/ME0104T4", result)) {
		return -1;
	}

	std::cout << result << "\n";
	
	return 0;
}
