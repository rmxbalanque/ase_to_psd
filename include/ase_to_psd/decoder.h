// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ASE_TO_PSD_DECODER_H
#define ASE_TO_PSD_DECODER_H

#pragma once

#include "io_types.h"
#include "ase_to_psd/aseprite.h" // Forward declare (File)
#include <fstream>

namespace Aseprite
{
    class Decoder
    {
    public:
        std::unique_ptr<File> parse(const std::string &path);

    //private:

        struct FileHeader
        {
            // File header data.
            dword_t m_Size;
            word_t m_MagicNumber;
            word_t m_FrameCount;
            word_t m_Width;
            word_t m_Height;
            word_t m_ColorDepth;
            dword_t m_Flags;
            word_t m_Speed;
            dword_t m_IgnoreA[2];
            byte_t m_PaletteEntry;
            byte_t m_IgnoreB[3];
            word_t m_ColorCount;
            byte_t m_PixelWidth;
            byte_t m_PixelHeight;
            short_t m_GridX;
            short_t m_GridY;
            word_t m_GridWidth;
            word_t m_GridHeight;

            // Properly parsed/magic number matches.
            bool m_Good;
            size_t m_StartPos;
        };

        struct FrameHeader
        {
            // Frame header data.
            dword_t m_Size;
            word_t m_MagicNumber;
            word_t m_ChunkCount;
            word_t m_Duration;

            // Properly parsed/magic number matches
            bool m_Good;
            size_t m_StartPos;
        };

        struct ChunkHeader
        {
            // Chunk header data.
            dword_t m_Size;
            Chunk::Type m_Type;
            size_t m_StartPos;
        };

        // Data.
        std::string m_Path;
        std::ifstream m_AseFileStream;
        PixelFormat m_PixelFormat;

        // Decompression
        // for cel.
        Cel::raw_image_t decompress_cel_data(const Cel::compressed_image_t & image);

        // TODO: Use move semantics when copying values around.
        // IO Type reading.
        template<typename T>
        T read()
        {
            T buffer;
            m_AseFileStream.read(reinterpret_cast<char *>(&buffer), 1);

            if (m_AseFileStream && !m_AseFileStream.eof())
                return buffer;

            return 0;
        }

        template<typename T>
        T read(const ChunkHeader & chunkHeader);

        template<typename T, size_t n = sizeof(T)>
        T read(char *buf)
        {
            m_AseFileStream.read(buf, n);
        }

        template<>
        byte_t read<byte_t>();

        template<>
        word_t read<word_t>();

        template<>
        dword_t read<dword_t>();

        template<>
        string_t read<string_t>();

        template<>
        short_t read<short_t>();

        template<>
        long_t read<long_t>();

        template<>
        pixel_t read<pixel_t>();

        template<>
        FileHeader read<FileHeader>();

        template<>
        FrameHeader read<FrameHeader>();

        template<>
        ChunkHeader read<ChunkHeader>();

        // Chunk types

        template<>
        Layer read<Layer>();

        template<>
        Cel read<Cel>(const ChunkHeader & chunkHeader);
    };
}
#endif //ASE_TO_PSD_DECODER_H
