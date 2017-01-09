#include <sstream>
#include <iterator>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "logger.h"

namespace utils{

    std::vector<std::string> SplitString(const std::string& content){
        std::istringstream iss(content);
        std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, 
            std::istream_iterator<std::string>{}};
        return tokens;
    }

    void SetStringStreamPrecision(std::stringstream &ss, int precision){
        ss.setf(std::ios::fixed, std::ios::floatfield); \
            ss.precision(precision);
    }



    std::tuple<char*, size_t, bool> ReadFileData(const std::string &filename){
        bool ok = false;
        char *fontData = nullptr;
        size_t fontDataSize = 0;

        std::ifstream ifile(filename, std::ios::binary);

        ifile.seekg(0, std::ios::end);
        fontDataSize = ifile.tellg();
        LOG(INFO) << "filename: " << filename << " len: " << fontDataSize;

        fontData = new char[fontDataSize];
        ifile.seekg(0, std::ios::beg);
        ifile.read(fontData, fontDataSize);

        ok = true;

        ifile.close();

        return std::make_tuple(fontData, fontDataSize, ok);
    }
}


