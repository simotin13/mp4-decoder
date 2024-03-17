#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include "mp4.h"

#define SIZE_BOX_HEADER  (8)
#define SIZE_MB_BYTE     (1048576)

bool Mp4::ReadMp4Info(std::string filePath, MP4_INFO &mp4Info, std::string &errMsg)
{
    struct stat st;
    int ret = stat(filePath.c_str(), &st);
    if (ret != 0)
    {
        errMsg = "failed to stat filePath:[" + filePath + "]";
        return false;
    }

    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
    if (!ifs)
    {
        errMsg = "failed to create ifstream filePath:[" + filePath + "]";
        return false;
    }

    TKHD_BOX_INFO tkhdBoxInfo;
    MVHD_BOX_INFO mvhdBoxInfo;
    STSD_BOX_INFO stsdBoxInfo;
    AVC1_BOX_INFO avc1BoxInfo;
    AVCC_BOX_INFO avccBoxInfo;
    size_t totalSize = st.st_size;
    size_t leftSize  = totalSize;
    uint64_t boxSize  = 0;
    uint64_t readSize = 0;
    std::string type;
    uint64_t mdatSize = 0;

    // ファイルサイズ(MB)
    mp4Info.fizeSizeMB = (double)totalSize / (double)SIZE_MB_BYTE;

    bool result = false;
    while(0 < leftSize)
    {
        result = readBoxHeader(ifs, readSize, boxSize, type, errMsg);
        if (!result)
        {
            result = false;
            break;
        }
        std::cout << "TODO: type:[" << type << "] boxSize:[" << boxSize << "]" << std::endl;
        if (type == "moov")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "trak")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "stbl")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "mdia")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "minf")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "dinf")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "stco")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "stsc")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "stsz")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "stts")
        {
            // have child box
            leftSize -= readSize;
        }
        else if (type == "tkhd")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readTkhdBox(ifs, readSize, tkhdBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            seekSize -= readSize;
            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else if (type == "mvhd")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readMvhdBox(ifs, readSize, mvhdBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            seekSize -= readSize;
            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else if (type == "stsd")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readStsdBox(ifs, readSize, stsdBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            leftSize -= readSize;
        }
        else if (type == "mdat")
        {
            uint64_t seekSize = boxSize-readSize;

            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else if (type == "tkhd")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readTkhdBox(ifs, readSize, tkhdBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            seekSize -= readSize;
            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else if (type == "mvhd")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readMvhdBox(ifs, readSize, mvhdBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            seekSize -= readSize;
            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else if (type == "stsd")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readStsdBox(ifs, readSize, stsdBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            seekSize -= readSize;
            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else if (type == "avc1")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readAvc1Box(ifs, readSize, avc1BoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            leftSize -= readSize;
        }
        else if (type == "avcC")
        {
            uint64_t seekSize = boxSize - readSize;
            result = readAvccBox(ifs, boxSize, readSize, avccBoxInfo);
            if (!result)
            {
                result = false;
                break;
            }
            leftSize -= readSize;
        }
        else if (type == "mdat")
        {
            uint64_t seekSize = boxSize-readSize;

            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
        else
        {
            // TODO: skip box
            uint64_t seekSize = boxSize-readSize;
            ifs.seekg(seekSize, std::ios_base::cur);
            leftSize -= boxSize;
        }
    }
    ifs.close();
    return result;
}

bool Mp4::byteToUInt16Be(std::ifstream &ifs, uint16_t &value)
{
    std::vector<unsigned char> buf(2);
    ifs.read((char *)buf.data(), 2);

    uint16_t pos = 0;
    value = 0;
    value += (uint16_t)buf[pos++] << 8;
    value += (uint16_t)buf[pos++];

    return true;
}

bool Mp4::byteToUInt24Be(std::ifstream &ifs, uint32_t &value)
{
    std::vector<unsigned char> buf(3);
    ifs.read((char *)buf.data(), 3);

    uint32_t pos = 0;
    value = 0;
    value += (uint32_t)buf[pos++]   << 16;
    value += (uint32_t)buf[pos++]   << 8;
    value += (uint32_t)buf[pos++];

    return true;
}

bool Mp4::byteToUInt32Be(std::ifstream &ifs, uint32_t &value)
{
    std::vector<unsigned char> buf(4);
    ifs.read((char *)buf.data(), 4);

    uint32_t pos = 0;
    value = 0;
    value += (uint32_t)buf[pos++]   << 24;
    value += (uint32_t)buf[pos++]   << 16;
    value += (uint32_t)buf[pos++]   << 8;
    value += (uint32_t)buf[pos++];

    return true;
}

bool Mp4::byteToUInt64Be(std::ifstream &ifs, uint64_t &value)
{
    std::vector<unsigned char> buf(8);
    ifs.read((char *)buf.data(), 8);

    uint32_t pos = 0;
    value = 0;
    value += (uint64_t)buf[pos++] << 56;
    value += (uint64_t)buf[pos++] << 48;
    value += (uint64_t)buf[pos++] << 40;
    value += (uint64_t)buf[pos++] << 32;
    value += (uint64_t)buf[pos++] << 24;
    value += (uint64_t)buf[pos++] << 16;
    value += (uint64_t)buf[pos++] << 8;
    value += (uint64_t)buf[pos++];
    return true;
}

bool Mp4::byteToInt16Be(std::ifstream &ifs, int16_t &value)
{
    std::vector<unsigned char> buf(2);
    ifs.read((char *)buf.data(), 2);

    uint32_t pos = 0;
    value = 0;
    value += (int32_t)buf[pos++] << 8;
    value += (int32_t)buf[pos++];

    return true;
}

bool Mp4::byteToInt32Be(std::ifstream &ifs, int32_t &value)
{
    std::vector<unsigned char> buf(4);
    ifs.read((char *)buf.data(), 4);

    uint32_t pos = 0;
    value = 0;
    value += (int32_t)buf[pos++]   << 24;
    value += (int32_t)buf[pos++]   << 16;
    value += (int32_t)buf[pos++]   << 8;
    value += (int32_t)buf[pos++];

    return true;
}

bool Mp4::readBoxHeader(std::ifstream &ifs, uint64_t &readSize, uint64_t &boxSize, std::string &type, std::string &errMsg)
{
    readSize = 0;

    uint32_t tmp;
    bool ret = byteToUInt32Be(ifs, tmp);
    if (!ret)
    {
        return false;
    }
    boxSize = tmp;
    readSize += 4;

    // typeの読出
    char buf[4];
    ifs.read(buf, 4);

    type = std::string(buf, 4);
    readSize += 4;

    // Boxサイズが1の場合は8byteの拡張サイズ領域にサイズが含まれている
    if (boxSize == 1)
    {
        ret = byteToUInt64Be(ifs, boxSize);
        if (!ret)
        {
            return false;
        }
        readSize += 8;
    }

    if (boxSize < 1)
    {
        // ファイルが破損している場合の対応
        // ヘッダが適切に作成されていない場合 boxSizeが0になることがある
        // 0の場合は異常とみなす
        return false;
    }

    return true;
}

bool Mp4::readTkhdBox(std::ifstream &ifs, uint64_t &readSize, TKHD_BOX_INFO &tkhdBoxInfo)
{
    std::vector<unsigned char> buf;
    readSize = 0;

    // version
    ifs.read((char*)&(tkhdBoxInfo.version), 1);
    readSize++;

    // Flags
    ifs.read((char *)(tkhdBoxInfo.flags), 3);
    readSize += 3;

    // creation time
    bool result = byteToUInt32Be(ifs, tkhdBoxInfo.creationTime);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // modification time
    result = byteToUInt32Be(ifs, tkhdBoxInfo.modificationTime);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // track id
    result = byteToUInt32Be(ifs, tkhdBoxInfo.trackId);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // reserved
    result = byteToUInt32Be(ifs, tkhdBoxInfo.reserved);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // duration
    result = byteToUInt32Be(ifs, tkhdBoxInfo.duration);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // reserved2
    ifs.read((char*)&(tkhdBoxInfo.reserved2), 8);
    readSize += 8;

    // layer
    result = byteToInt16Be(ifs, tkhdBoxInfo.layer);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // alternate group
    result = byteToInt16Be(ifs, tkhdBoxInfo.alternateGroup);
    if (!result)
    {
        return false;
    }
    readSize += 2;
    
    // volume
    result = byteToInt16Be(ifs, tkhdBoxInfo.volume);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // reserved3
    result = byteToInt16Be(ifs, tkhdBoxInfo.reserved3);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // matrix
    for (int i = 0; i < 9; i++)
    {
        result = byteToInt32Be(ifs, tkhdBoxInfo.matrix[i]);
        if (!result)
        {
            return false;
        }
        readSize += 4;
    }

    // width
    result = byteToUInt16Be(ifs, tkhdBoxInfo.width);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // width quality
    result = byteToUInt16Be(ifs, tkhdBoxInfo.widthQuality);
    if (!result)
    {
        return false;
    }
    readSize += 2;


    // height
    result = byteToUInt16Be(ifs, tkhdBoxInfo.height);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // height quality
    result = byteToUInt16Be(ifs, tkhdBoxInfo.heightQuality);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    return true;
}

bool Mp4::readStsdBox(std::ifstream &ifs, uint64_t &readSize, STSD_BOX_INFO &stsdBoxInfo)
{
    std::vector<unsigned char> buf;
    readSize = 0;

    // version
    ifs.read((char*)&(stsdBoxInfo.version), 1);
    readSize++;

    // Flags
    ifs.read((char *)(stsdBoxInfo.flags), 3);
    readSize += 3;

    // entry count
    bool result = byteToUInt32Be(ifs, stsdBoxInfo.entryCount);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    return true;
}

bool Mp4::readMvhdBox(std::ifstream& ifs, uint64_t &readSize, MVHD_BOX_INFO &mvhdBoxInfo)
{
    std::vector<unsigned char> buf;
    readSize = 0;

    // version
    ifs.read((char*)&(mvhdBoxInfo.version), 1);
    readSize++;

    // Flags
    ifs.read((char *)(mvhdBoxInfo.flags), 3);
    readSize += 3;

    // creation time
    bool result = byteToUInt32Be(ifs, mvhdBoxInfo.creationTime);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // modification time
    result = byteToUInt32Be(ifs, mvhdBoxInfo.modificationTime);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // time scale
    result = byteToUInt32Be(ifs, mvhdBoxInfo.timeScale);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // duration
    result = byteToUInt32Be(ifs, mvhdBoxInfo.duration);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    result = byteToInt32Be(ifs, mvhdBoxInfo.rate);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    result = byteToInt16Be(ifs, mvhdBoxInfo.volume);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    ifs.read((char*)&(mvhdBoxInfo.reserved), 10);
    readSize += 10;

    for (int i = 0; i < 9; i++)
    {
        result = byteToInt32Be(ifs, mvhdBoxInfo.matrix[i]);
        if (!result)
        {
            return false;
        }
        readSize += 4;
    }
    for(int i = 0; i < 6; i++)
    {
        result = byteToUInt32Be(ifs, mvhdBoxInfo.preDefined[i]);
        if (!result)
        {
            return false;
        }
        readSize += 4;
    }

    result = byteToUInt32Be(ifs, mvhdBoxInfo.nextTrackId);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    return true;
}

bool Mp4::readAvc1Box(std::ifstream &ifs, uint64_t &readSize, AVC1_BOX_INFO &avc1BoxInfo)
{
    std::vector<unsigned char> buf;
    readSize = 0;

    ifs.read((char *)(avc1BoxInfo.reserved), 6);
    readSize += 6;

    bool result = byteToUInt16Be(ifs, avc1BoxInfo.dataReferenceIndex);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // version
    result = byteToUInt16Be(ifs, avc1BoxInfo.version);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // revision level
    result = byteToUInt16Be(ifs, avc1BoxInfo.revisionLevel);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // vendor
    result = byteToUInt32Be(ifs, avc1BoxInfo.vendor);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // temporal quality
    result = byteToUInt32Be(ifs, avc1BoxInfo.temporalQuality);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // spatial quality
    result = byteToUInt32Be(ifs, avc1BoxInfo.spatialQuality);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // width
    result = byteToUInt16Be(ifs, avc1BoxInfo.width);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // height
    result = byteToUInt16Be(ifs, avc1BoxInfo.height);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // horizontal resolution
    result = byteToUInt32Be(ifs, avc1BoxInfo.horizontalResolution);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // vertical resolution
    result = byteToUInt32Be(ifs, avc1BoxInfo.verticalResolution);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // data size
    result = byteToUInt32Be(ifs, avc1BoxInfo.dataSize);
    if (!result)
    {
        return false;
    }
    readSize += 4;

    // frame count
    result = byteToUInt16Be(ifs, avc1BoxInfo.frameCount);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // compressor name
    ifs.read((char *)(avc1BoxInfo.compressorName), 32);
    readSize += 32;

    // depth
    result = byteToUInt16Be(ifs, avc1BoxInfo.depth);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    // color table id
    result = byteToInt16Be(ifs, avc1BoxInfo.colorTableId);
    if (!result)
    {
        return false;
    }
    readSize += 2;

    return true;
}

bool Mp4::readAvccBox(std::ifstream &ifs, const uint64_t boxSize, uint64_t &readSize, AVCC_BOX_INFO &avccBoxInfo)
{
    std::vector<unsigned char> buf;
    readSize = 0;

    // configuration version
    ifs.read((char*)&(avccBoxInfo.configurationVersion), 1);
    readSize++;

    // AVCProfileIndication
    ifs.read((char*)&(avccBoxInfo.AVCProfileIndication), 1);
    readSize++;

    // profile compatibility
    ifs.read((char*)&(avccBoxInfo.profileCompatibility), 1);
    readSize++;

    // AVCLevelIndication
    ifs.read((char*)&(avccBoxInfo.AVCLevelIndication), 1);
    readSize++;

    // lengthSizeMinusOne
    ifs.read((char*)&(avccBoxInfo.lengthSizeMinusOne), 1);
    avccBoxInfo.lengthSizeMinusOne = 0x03 & avccBoxInfo.lengthSizeMinusOne;
    readSize++;

    // numOfSequenceParameterSets
    ifs.read((char*)&(avccBoxInfo.numOfSequenceParameterSets), 1);
    avccBoxInfo.numOfSequenceParameterSets = 0x1F & avccBoxInfo.numOfSequenceParameterSets;
    readSize++;

    avccBoxInfo.sequenceParameterSets.clear();
    for (int i = 0; i < avccBoxInfo.numOfSequenceParameterSets; i++)
    {
        // sequenceParameterSetLength
        SEQUENCE_PARAMETER_SET sps;
        bool result = byteToUInt16Be(ifs, sps.sequenceParameterSetLength);
        if (!result)
        {
            return false;
        }
        readSize += 2;

        sps.nalUnit.clear();
        for (int j = 0; j < sps.sequenceParameterSetLength; j++)
        {
            sps.nalUnit.push_back(ifs.get());
            readSize++;
        }
        avccBoxInfo.sequenceParameterSets.push_back(sps);
    }

    // numOfPictureParameterSets
    ifs.read((char*)&(avccBoxInfo.numOfPictureParameterSets), 1);
    readSize++;

    avccBoxInfo.pictureParameterSets.clear();
    for (int i = 0; i < avccBoxInfo.numOfPictureParameterSets; i++)
    {
        // pictureParameterSetLength
        PICTURE_PARAMETER_SET pps;
        bool result = byteToUInt16Be(ifs, pps.pictureParameterSetLength);
        if (!result)
        {
            return false;
        }
        readSize += 2;

        pps.nalUnit.clear();
        for (int j = 0; j < pps.pictureParameterSetLength; j++)
        {
            pps.nalUnit.push_back(ifs.get());
            readSize++;
        }
        avccBoxInfo.pictureParameterSets.push_back(pps);
    }

    avccBoxInfo.chromaFormat = -1;
    avccBoxInfo.bitDepthLumaMinus8 = -1;
    avccBoxInfo.bitDepthChromaMinus8 = -1;
    if (0 < (boxSize - readSize))
    {
        if ((avccBoxInfo.AVCProfileIndication == AVC_PROFILE_INDICATION_H264_HIGH) ||
            (avccBoxInfo.AVCProfileIndication == AVC_PROFILE_INDICATION_H264_HIGH_10) ||
            (avccBoxInfo.AVCProfileIndication == AVC_PROFILE_INDICATION_H264_HIGH_422) ||
            (avccBoxInfo.AVCProfileIndication == AVC_PROFILE_INDICATION_H264_HIGH_444))
        {
            // chromaFormat
            avccBoxInfo.chromaFormat = ifs.get();
            avccBoxInfo.chromaFormat = 0x03 & avccBoxInfo.chromaFormat;
            readSize++;

            // bitDepthLumaMinus8
            avccBoxInfo.bitDepthLumaMinus8 = ifs.get();
            avccBoxInfo.bitDepthLumaMinus8 = 0x07 & avccBoxInfo.bitDepthLumaMinus8;
            readSize++;

            // bitDepthChromaMinus8
            avccBoxInfo.bitDepthChromaMinus8 = ifs.get();
            avccBoxInfo.bitDepthChromaMinus8 = 0x07 & avccBoxInfo.bitDepthChromaMinus8;
            readSize++;

            // numOfSequenceParameterSetExt
            avccBoxInfo.numOfSequenceParameterSetExt = ifs.get();
            readSize++;

            avccBoxInfo.sequenceParameterSetExts.clear();
            for (int i = 0; i < avccBoxInfo.numOfSequenceParameterSetExt; i++)
            {
                // sequenceParameterSetExtLength
                SEQUENCE_PARAMETER_SET sps;
                bool result = byteToUInt16Be(ifs, sps.sequenceParameterSetLength);
                if (!result)
                {
                    return false;
                }
                readSize += 2;

                sps.nalUnit.clear();
                for (int j = 0; j < sps.sequenceParameterSetLength; j++)
                {
                    sps.nalUnit.push_back(ifs.get());
                    readSize++;
                }
                avccBoxInfo.sequenceParameterSetExts.push_back(sps);
            }
        }
    }

    return true;
}
