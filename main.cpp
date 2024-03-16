#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "mp4.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filepath.mp4>" << std::endl;
        return -1;
    }

    std::string filePath = argv[1];
    MP4_INFO mp4Info;
    std::string errMsg;
    bool result = Mp4::ReadMp4Info(filePath, mp4Info, errMsg);
    if (!result)
    {
        std::cerr << "failed to ReadMp4Info filePath:[" << filePath << "] errMsg:[" << errMsg << "]" << std::endl;
        return -1;
    }
    return 0; 
}