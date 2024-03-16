#ifndef _MP4_H_
#define _MP4_H_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

struct MP4_INFO
{
    uint64_t recTimeSec;
    double bitrateKbps;
    double fizeSizeMB;
};

// mvhd ボックスの情報
struct MVHD_BOX_INFO
{
    uint8_t version;            // varsion
    uint8_t flags[3];           // reverved for future
    uint32_t creationTime;      // UTC Since 1904-01-01 00:00:00
    uint32_t modificationTime;  // UTC Since 1904-01-01 00:00:00
    uint32_t timeScale;         // second
    uint32_t duration;          // time value that indicates the duration of the movie in time scale units
    int32_t rate;               // preferred rate
    int16_t volume;             // preferred volume
    uint8_t reserved[10];       // 10 reserved bytes (set to 0)
    int32_t matrix[9];          // transformation matrix
    uint32_t preDefined[6];     // pre-defined
    uint32_t nextTrackId;       // next track id
};

class Mp4
{
public:
    static bool ReadMp4Info(std::string filePath, MP4_INFO &mp4Info, std::string &errMsg);
private:
    static bool byteToUInt16Be(std::ifstream &ifs, uint16_t &value);
    static bool byteToUInt32Be(std::ifstream &ifs, uint32_t &value);
    static bool byteToUInt64Be(std::ifstream &ifs, uint64_t &value);
    static bool byteToInt16Be(std::ifstream &ifs, int16_t &value);
    static bool byteToInt32Be(std::ifstream &ifs, int32_t &value);
    static bool readBoxHeader(std::ifstream &ifs, uint64_t &readSize, uint64_t &boxSize, std::string &type, std::string &errMsg);
    static bool readMvhdBox(std::ifstream &ifs, uint64_t &readSize, MVHD_BOX_INFO &mvhdBoxInfo);

};
#endif // _MP4_H_