#include "C/Precomp.h"

#include "C/7z.h"
#include "C/7zAlloc.h"
#include "C/7zBuf.h"
#include "C/7zCrc.h"
#include "C/7zFile.h"
#include "C/7zVersion.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#define kInputBufSize ((size_t)1 << 18)//0x4000 = 262144 bytes = 256 kbytes

static SRes DecompressA(const std::string& a_path)
{
    const ISzAlloc g_Alloc = { SzAlloc, SzFree };

    ISzAlloc allocImp = g_Alloc;
    ISzAlloc allocTempImp = g_Alloc;

    CFileInStream archiveStream;
    CLookToRead2 lookStream;
    CSzArEx db;

    Byte* outBuffer = nullptr; // it must be 0 before first call for each new archive.
    size_t outBufferSize = 0;  // it can have any value before first call (if outBuffer = 0)

    if (InFile_Open(&archiveStream.file, a_path.c_str()))
        return SZ_ERROR_DATA;

    FileInStream_CreateVTable(&archiveStream);
    archiveStream.wres = 0;
    LookToRead2_CreateVTable(&lookStream, false);
    lookStream.buf = nullptr;

    {
        lookStream.buf = (Byte*)ISzAlloc_Alloc(&allocImp, kInputBufSize);
        if (!lookStream.buf)
            return SZ_ERROR_MEM;

        lookStream.bufSize = kInputBufSize;
        lookStream.realStream = &archiveStream.vt;
        LookToRead2_Init(&lookStream);
    }

    CrcGenerateTable();

    SRes Result = SZ_OK;

    try
    {
        SzArEx_Init(&db);

        SRes Res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);
        if (Res != SZ_OK)
            throw Res;

        std::filesystem::path PathBase = a_path;
        PathBase.remove_filename();

        // if you need cache, use these 3 variables.
        // if you use external function, you can make these variable as static.

        UInt32 blockIndex = 0xFFFFFFFF; // it can have any value before first call (if outBuffer = 0)

        for (UInt32 i = 0; i < db.NumFiles; ++i)
        {
            const bool isDir = SzArEx_IsDir(&db, i);

            std::vector<UInt16>  TempVector;

            size_t Size = SzArEx_GetFileNameUtf16(&db, i, nullptr);
            TempVector.resize(Size);

            SzArEx_GetFileNameUtf16(&db, i, TempVector.data());

            std::string PathArch;//[!] that can be wrong charsets transformation, but for latin letters it should be enough.
            std::for_each(TempVector.begin(), TempVector.end(), [&PathArch](UInt16 data) {PathArch.push_back(static_cast<char>(data)); });
            std::filesystem::path Path = PathBase;
            Path.append(PathArch);

            size_t offset = 0;
            size_t outSizeProcessed = 0;
            if (SzArEx_Extract(&db, &lookStream.vt, i, &blockIndex, &outBuffer, &outBufferSize, &offset, &outSizeProcessed, &allocImp, &allocTempImp) != SZ_OK)
                break;

            if (isDir)
            {
                std::filesystem::create_directories(Path);
                continue;
            }

            std::fstream FileOut(Path, std::ios::binary | std::ios::out);
            if (!FileOut.good())
                throw SZ_ERROR_FAIL;

            size_t processedSize = outSizeProcessed;
            FileOut.write(reinterpret_cast<char*>(outBuffer + offset), processedSize);
            FileOut.close();

#if defined(_WIN32)//[TBD] It doesn't work properly with Linux that's why it's only for Windows.
            if (SzBitWithVals_Check(&db.MTime, i))
            {
                const CNtfsFileTime* t = &db.MTime.Vals[i];
                time_t TimeRaw = t->Low;
                TimeRaw |= (static_cast<time_t>(t->High) << 32);

                auto TimeRaw2 = TimeRaw / 10000000;//transform into seconds

                std::chrono::seconds TimeSec(TimeRaw2);
                std::chrono::time_point<std::chrono::file_clock> TimePoint(TimeSec);
                std::filesystem::last_write_time(Path, TimePoint);
            }
#endif
            // [*] it doesn't write following file attributes:
            // 1. creation time ("Created")
            // 2. time of last access ("Accessed")
            // 3. other attributes
        }
    }
    catch (SRes res)
    {
        Result = res;
    }

    ISzAlloc_Free(&allocImp, outBuffer);

    SzArEx_Free(&db, &allocImp);
    ISzAlloc_Free(&allocImp, lookStream.buf);

    File_Close(&archiveStream.file);

    return Result;
}

namespace utils
{
namespace arch_7z2201_simple
{

void Decompress(const std::string& path)
{
    std::string ResStr = "Arch error: ";
    auto Res = DecompressA(path);
    switch (Res)
    {
    case SZ_OK: return;
    case SZ_ERROR_DATA:         ResStr += "file not found"; break;
    case SZ_ERROR_MEM:          ResStr += "cannot allocate memory"; break;
    case SZ_ERROR_CRC:          ResStr += "CRC error"; break;
    case SZ_ERROR_READ:         ResStr += "Read Error"; break;
    case SZ_ERROR_FAIL:         ResStr += "failed"; break;
    case SZ_ERROR_UNSUPPORTED:  ResStr += "decoder doesn't support this archive"; break;
    default:                    ResStr += std::to_string(Res); break;
    }

    throw std::runtime_error(ResStr);
}

}
}