#ifndef _MP4_H_
#define _MP4_H_
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

const uint8_t AVC_PROFILE_INDICATION_BASELINE = 0x42;
const uint8_t AVC_PROFILE_INDICATION_H264_MAIN = 0x4D;
const uint8_t AVC_PROFILE_INDICATION_H264_EXTENDED = 0x58;
const uint8_t AVC_PROFILE_INDICATION_H264_HIGH = 0x64;
const uint8_t AVC_PROFILE_INDICATION_H264_HIGH_10 = 0x6E;
const uint8_t AVC_PROFILE_INDICATION_H264_HIGH_422 = 0x7A;
const uint8_t AVC_PROFILE_INDICATION_H264_HIGH_444 = 0xF4;

struct MP4_INFO
{
    uint64_t recTimeSec;
    double bitrateKbps;
    double fizeSizeMB;
};

struct TKHD_BOX_INFO
{
    uint8_t version;                        // varsion
    uint8_t flags[3];                       // flags
    uint32_t creationTime;                  // UTC Since 1904-01-01 00:00:00
    uint32_t modificationTime;              // UTC Since 1904-01-01 00:00:00
    uint32_t trackId;                       // track id
    uint32_t reserved;                      // reserved
    uint32_t duration;                      // time value that indicates the duration of the track in time scale units
    uint8_t reserved2[8];                   // 8 reserved bytes (set to 0)
    int16_t layer;                          // layer
    int16_t alternateGroup;                 // alternate group
    int16_t volume;                         // volume
    int16_t reserved3;                      // reserved
    int32_t matrix[9];                      // transformation matrix
    uint16_t width;                         // width
    uint16_t widthQuality;                  // width quality
    uint16_t height;                        // height
    uint16_t heightQuality;                 // height quality
};

struct MVHD_BOX_INFO
{
    uint8_t version;                        // varsion
    uint8_t flags[3];                       // flags
    uint32_t creationTime;                  // UTC Since 1904-01-01 00:00:00
    uint32_t modificationTime;              // UTC Since 1904-01-01 00:00:00
    uint32_t timeScale;                     // second
    uint32_t duration;                      // time value that indicates the duration of the movie in time scale units
    int32_t rate;                           // preferred rate
    int16_t volume;                         // preferred volume
    uint8_t reserved[10];                   // 10 reserved bytes (set to 0)
    int32_t matrix[9];                      // transformation matrix
    uint32_t preDefined[6];                 // pre-defined
    uint32_t nextTrackId;                   // next track id
};

struct STSD_BOX_INFO
{
    uint8_t version;                        // varsion
    uint8_t flags[3];                       // flags
    uint32_t entryCount;                    // entry count
};

struct AVC1_BOX_INFO
{
    uint8_t reserved[6];                    // 6 reserved bytes (set to 0)
    uint16_t dataReferenceIndex;            // data reference index
    uint16_t version;                       // version
    uint16_t revisionLevel;                 // revision level
    uint32_t vendor;                        // vendor
    uint32_t temporalQuality;               // temporal quality
    uint32_t spatialQuality;                // spatial quality
    uint16_t width;                         // width
    uint16_t height;                        // height
    uint32_t horizontalResolution;          // horizontal resolution
    uint32_t verticalResolution;            // vertical resolution
    uint32_t dataSize;                      // data size
    uint16_t frameCount;                    // frame count
    uint8_t compressorName[32];             // compressor name
    uint16_t depth;                         // depth
    int16_t colorTableId;                   // color table id
};

struct SEQUENCE_PARAMETER_SET
{
    uint16_t sequenceParameterSetLength;            // nal unit length
    std::vector<uint8_t> nalUnit;                   // nal unit
};

struct PICTURE_PARAMETER_SET
{
    uint16_t pictureParameterSetLength;             // nal unit length
    std::vector<uint8_t> nalUnit;                   // nal unit
};

struct AVCC_BOX_INFO
{
    uint8_t configurationVersion;                               // configuration version
    uint8_t AVCProfileIndication;                               // AVC profile indication(0x42:BaseLine, 0x64:High)
    uint8_t profileCompatibility;                               // profile compatibility
    uint8_t AVCLevelIndication;                                 // AVC level indication
    uint8_t lengthSizeMinusOne;                                 // length size minus one
    uint8_t numOfSequenceParameterSets;                         // number of sequence parameter sets
    std::vector<SEQUENCE_PARAMETER_SET> sequenceParameterSets;  // sequence parameter sets
    uint8_t numOfPictureParameterSets;                          // number of picture parameter sets
    std::vector<PICTURE_PARAMETER_SET> pictureParameterSets;    // picture parameter sets
    uint8_t chromaFormat;                                       // chroma format
    uint8_t bitDepthLumaMinus8;                                 // bit depth luma minus 8
    uint8_t bitDepthChromaMinus8;                               // bit depth chroma minus 8
    uint8_t numOfSequenceParameterSetExt;                       // number of sequence parameter set ext
    std::vector<SEQUENCE_PARAMETER_SET> sequenceParameterSetExts;  // sequence parameter set ext
};

class Mp4
{
public:
    static bool ReadMp4Info(std::string filePath, MP4_INFO &mp4Info, std::string &errMsg);
private:
    static bool byteToUInt16Be(std::ifstream &ifs, uint16_t &value);
    static bool byteToUInt24Be(std::ifstream &ifs, uint32_t &value);
    static bool byteToUInt32Be(std::ifstream &ifs, uint32_t &value);
    static bool byteToUInt64Be(std::ifstream &ifs, uint64_t &value);
    static bool byteToInt16Be(std::ifstream &ifs, int16_t &value);
    static bool byteToInt32Be(std::ifstream &ifs, int32_t &value);
    static bool readBoxHeader(std::ifstream &ifs, uint64_t &readSize, uint64_t &boxSize, std::string &type, std::string &errMsg);
    static bool readMvhdBox(std::ifstream &ifs, uint64_t &readSize, MVHD_BOX_INFO &mvhdBoxInfo);
    static bool readTkhdBox(std::ifstream &ifs, uint64_t &readSize, TKHD_BOX_INFO &tkhdBoxInfo);
    static bool readStsdBox(std::ifstream &ifs, uint64_t &readSize, STSD_BOX_INFO &stsdBoxInfo);
    static bool readAvc1Box(std::ifstream &ifs, uint64_t &readSize, AVC1_BOX_INFO &avc1BoxInfo);
    static bool readAvccBox(std::ifstream &ifs, const uint64_t boxSize, uint64_t &readSize, AVCC_BOX_INFO &avccBoxInfo);
    
};
#endif // _MP4_H_