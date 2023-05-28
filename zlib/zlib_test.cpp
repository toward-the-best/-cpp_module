#include <cstdint>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <zlib.h>

struct LocalFileHeader {
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t mod_time;
    uint16_t mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t file_name_length;
    uint16_t extra_field_length;
};

bool readFileToMemory(const std::string &filename, std::vector<uint8_t> &data) {
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::cerr << "Error getting file size: " << filename << std::endl;
        close(fd);
        return false;
    }

    void *ptr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mmap-ing file: " << filename << std::endl;
        close(fd);
        return false;
    }

    data = std::vector<uint8_t>(static_cast<uint8_t *>(ptr),
                                static_cast<uint8_t *>(ptr) + sb.st_size);

    if (munmap(ptr, sb.st_size) == -1) {
        std::cerr << "Error unmapping file: " << filename << std::endl;
        close(fd);
        return false;
    }

    close(fd);
    return true;
}

bool decompressMemory(const std::vector<uint8_t> &compressedData,
                      std::vector<uint8_t> &decompressedData) {

    LocalFileHeader header;
    memcpy(&header, compressedData.data(), sizeof(LocalFileHeader));

    std::cout << "signature            : " << std::hex << header.signature
              << std::endl;
    std::cout << "version              : " << header.version << std::endl;
    std::cout << "flags                : " << header.flags << std::endl;
    std::cout << "compression          : " << header.compression << std::endl;
    std::cout << "mod_time             : " << header.mod_time << std::endl;
    std::cout << "mod_date             : " << header.mod_date << std::endl;
    std::cout << "crc32                : " << std::hex << header.crc32
              << std::endl;
    std::cout << "compressed_size      : " << header.compressed_size
              << std::endl;
    std::cout << "uncompressed_size    : " << header.uncompressed_size
              << std::endl;
    std::cout << "file_name_length     : " << header.file_name_length
              << std::endl;
    std::cout << "extra_field_length   : " << header.extra_field_length
              << std::endl;

    if (header.signature != 0x04034b50) {
        std::cerr << "Error: Invalid local file header signature" << std::endl;
        return false;
    }

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = compressedData.size() - sizeof(LocalFileHeader);
    strm.next_in =
        const_cast<uint8_t *>(compressedData.data() + sizeof(LocalFileHeader));

    if (inflateInit2(&strm, MAX_WBITS + 16) != Z_OK) {
        std::cerr << "Error initializing zlib inflate" << std::endl;
        return false;
    }

    std::vector<uint8_t> buffer(1024);
    do {
        strm.avail_out = buffer.size();
        strm.next_out = buffer.data();
        int ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR) {
            std::cerr << "Error inflating data: " << ret << std::endl;
            inflateEnd(&strm);
            return false;
        } else {
            std::cerr << "!! Error inflating data: " << ret << std::endl;
        }
        decompressedData.insert(decompressedData.end(), buffer.data(),
                                buffer.data() + buffer.size() - strm.avail_out);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);
    return true;
}

int main() {
    std::string filename = "slim.zip";
    std::vector<uint8_t> compressedData;

    if (!readFileToMemory(filename, compressedData)) {
        return 1;
    }

    std::cout << compressedData.size() << std::endl;

    std::vector<uint8_t> decompressedData;
    if (!decompressMemory(compressedData, decompressedData)) {
        return 1;
    }

    std::cout << decompressedData.size() << std::endl;

    // 이제 decompressedData 벡터에 압축 해제된 데이터가 들어있습니다.
    // 필요한 작업을 수행하세요.

    return 0;
}