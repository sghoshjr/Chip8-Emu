#ifndef UTILS_CONFIGREADER
#define UTILS_CONFIGREADER

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include "simpleIni.h"

class configReader {
   private:
    std::string configFile = "chip8emu.ini";
    std::array<std::string, 10> recentROM;
    CSimpleIniA ini;

    bool createDefaultConfigFile(const char *file);

   public:
    configReader(std::string filepath);

    std::string getRecentROM(int idx);

    constexpr size_t getRecentROMSize() const noexcept {
        return recentROM.size();
    }
};

#endif // UTILS_CONFIGREADER