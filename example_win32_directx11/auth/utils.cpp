#include "utils.hpp"

#include <Windows.h>
#include <sstream>
#include <iomanip>

std::string utils::get_hwid() {
    std::stringstream ss;

    // Volume serial number
    DWORD volumeSerial = 0;
    GetVolumeInformationA("C:\\", NULL, 0, &volumeSerial, NULL, NULL, NULL, 0);
    ss << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << volumeSerial;

    // Computer name
    char compName[MAX_COMPUTERNAME_LENGTH + 1] = {};
    DWORD compSize = sizeof(compName);
    GetComputerNameA(compName, &compSize);
    ss << "-" << compName;

    // Number of processors
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    ss << "-" << sysInfo.dwNumberOfProcessors;
    ss << "-" << sysInfo.dwProcessorType;

    std::string hwid = ss.str();
    // Pad to at least 20 chars if somehow short
    while (hwid.size() < 20) hwid += "0";
    return hwid;
}

std::time_t utils::string_to_timet(std::string timestamp) {
	char* end = nullptr;
	auto cv = strtol(timestamp.c_str(), &end, 10);
	if (end == timestamp.c_str())
		return 0;
	return static_cast<time_t>(cv);
}

std::tm utils::timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}
