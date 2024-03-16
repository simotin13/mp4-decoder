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

    MVHD_BOX_INFO mvhdBoxInfo;
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
            std::cout << "moov boxSize:[" << boxSize << "]" << std::endl;
            leftSize -= readSize;
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