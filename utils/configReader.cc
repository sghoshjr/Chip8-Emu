#include "configReader.h"

bool configReader::createDefaultConfigFile(const char *file) {
    CSimpleIniA defaultConfig(true, false, false);

    for (int i = 0; i < recentROM.size(); ++i) {
        std::string romNum = "rom";
        romNum += std::to_string(i);
        defaultConfig.SetValue("Recent", romNum.c_str(), "");
    }

    if (defaultConfig.SaveFile(file) >= 0) {
        std::cerr << "Created " << configFile << std::endl;
        return true;
    } else {
        std::cerr << "Failed to create " << configFile << std::endl;
        return false;
    }
}

configReader::configReader(std::string filepath) : ini(true, false, false) {
    std::cerr << "Config File Path : " << (filepath + configFile) << std::endl;
    
    auto file = (filepath + configFile).c_str();
    SI_Error rc = ini.LoadFile(file);

    if (rc < 0) {
        std::cerr << "Could not open iniFile" << std::endl;
        //Create default config file if possible and continue
        createDefaultConfigFile(file);
    }

    //Load 10 Recent ROMs
    for (int i = 0; i < recentROM.size(); ++i) {
        std::string romNum = "rom";
        romNum += std::to_string(i);
        std::string a = ini.GetValue("Recent", romNum.c_str(), "");
        std::cerr << romNum << " : " << a << std::endl;
        recentROM[i] = a;
    }
}

std::string configReader::getRecentROM(int idx) {
    if (idx > recentROM.size()) {
        return "";
    }
    return recentROM[idx];
}