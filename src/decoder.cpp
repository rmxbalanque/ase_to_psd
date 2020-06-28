// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "ase_to_psd/decoder.h"
#include <utility>

// TODO: Remove!
#include <iostream>

#define ASEFILE_MAGIC 0xA5E0
#define ASEFRAME_MAGIC 0xF1FA

namespace Aseprite
{
    ///////////////////////////////////////////////////////////////////////////
    // Decoder
    ///////////////////////////////////////////////////////////////////////////

    std::unique_ptr<File> Aseprite::Decoder::parse(const std::string &path)
    {
        m_Path = path;
        m_AseFileStream.open(m_Path, std::ios_base::in | std::ios_base::binary);

        if (m_AseFileStream)
        {
            auto asedoc = std::make_unique<File>();

            // Parse header.
            const auto file_header(read<FileHeader>());

            if (file_header.m_Good)
            {
                // Set decoder pixel format.
                m_PixelFormat = file_header.m_ColorDepth == 32 ? PixelFormat::RGBA : (file_header.m_ColorDepth == 16
                                                                                     ? PixelFormat::Grayscale
                                                                                     : PixelFormat::Indexed);

                for (word_t cf = 0; cf < file_header.m_FrameCount; ++cf)
                {
                    const auto frame_header(read<FrameHeader>());
                    m_AseFileStream.seekg(frame_header.m_StartPos + 16);

                    if (frame_header.m_Good)
                    {
                        // Create new frame.
                        Frame newFrame;
                        newFrame.m_Duration = frame_header.m_Duration;
                        asedoc->m_Frames.push_back(newFrame);

                        // Read chunks in frame.
                        for (word_t cc = 0; cc < frame_header.m_ChunkCount; ++cc)
                        {
                            const auto chunk_header(read<ChunkHeader>());

                            switch (chunk_header.m_Type)
                            {
                                case Chunk::Type::OldPaletteA:
                                    std::cout << "Old Palette A Read!" << "\n";
                                    break;
                                case Chunk::Type::OldPaletteB:
                                    std::cout << "Old Palette B Read!" << "\n";
                                    break;
                                case Chunk::Type::Layer:
                                    std::cout << "Layer Read!" << "\n";
                                    asedoc->m_Layers.push_back(read<Layer>());
                                    break;
                                case Chunk::Type::Cel:
                                    std::cout << "Cel Read!" << "\n";
                                    asedoc->m_Frames[asedoc->m_Frames.size() - 1].m_Cels.push_back(read<Cel>(chunk_header));
                                    if (asedoc->m_Frames[asedoc->m_Frames.size() - 1].m_Cels.back().m_TypeData.index() == 1)
                                    {
                                        decompress_cel_data(std::get<1>(asedoc->m_Frames[asedoc->m_Frames.size() - 1].m_Cels.back().m_TypeData));
                                        exit(0);
                                    }
                                    break;
                                case Chunk::Type::CelExtra:
                                    std::cout << "Cel Extra Read!" << "\n";
                                    break;
                                case Chunk::Type::Mask:
                                    std::cout << "Mask Read!" << "\n";
                                    break;
                                case Chunk::Type::Path:
                                    std::cout << "Path Read!" << "\n";
                                    break;
                                case Chunk::Type::FrameTags:
                                    std::cout << "Frame Read!" << "\n";
                                    break;
                                case Chunk::Type::Palette:
                                    std::cout << "Palette Read!" << "\n";
                                    break;
                                case Chunk::Type::UserData:
                                    std::cout << "User Data Read!" << "\n";
                                    break;
                                case Chunk::Type::Slice:
                                    std::cout << "Slice Read!" << "\n";
                                    break;
                            }

                            m_AseFileStream.seekg(chunk_header.m_StartPos + chunk_header.m_Size);
                        }
                    }
                }

                return asedoc;
            }
        }

        return std::unique_ptr<File>{};
    }

    ///////////////////////////////////////////////////////////////////////////
    // Decompression
    ///////////////////////////////////////////////////////////////////////////

    Cel::raw_image_t Decoder::decompress_cel_data(const Cel::compressed_image_t &image)
    {


        auto & [width, height, srcData] = image;
        std::cout << width << ", " << height << std::endl;

        std::vector<pixel_t> outBuf(width * height);

        std::fstream out("./raw_uncompressed_image_data.txt", std::ios_base::out | std::ios_base::binary);
        if (out)
        {
            for (const auto byte : outBuf)
            {
                out << byte.m_RGBA;
            }
            out.close();
        }
        return std::make_tuple(width, height, outBuf);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// IO Reading
    ///////////////////////////////////////////////////////////////////////////

    template<>
    byte_t Decoder::read<byte_t>()
    {
        int val = m_AseFileStream.get();

        if (!m_AseFileStream.eof())
            return val;

        return 0;
    }

    template<>
    word_t Decoder::read<word_t>()
    {
        auto a = read<byte_t>();
        auto b = read<byte_t>();

        if (!m_AseFileStream.eof())
            return ((b << 8) | a);
        else
            return 0;
    }

    template<>
    dword_t Decoder::read<dword_t>()
    {
        int a = read<byte_t>();
        int b = read<byte_t>();
        int c = read<byte_t>();
        int d = read<byte_t>();

        if (!m_AseFileStream.eof())
            return ((d << 24) | (c << 16) | (b << 8) | a);
        else
            return 0;
    }

    template<>
    string_t Decoder::read<string_t>()
    {
        const auto len = read<word_t>();
        string_t str(len, '\0');
        m_AseFileStream.read(&str[0], len);
        // TODO: Check end of file.
        return str;
    }

    // TODO: Read pixel_t

    // Conversions.
#define DECODER_READ_CONVERSION_SPECIALIZATION(from_, to_)\
    template<>\
    to_ Decoder::read<to_>()\
    {\
        return static_cast<to_>(read<from_>());\
    }

    DECODER_READ_CONVERSION_SPECIALIZATION(word_t, short_t)

    DECODER_READ_CONVERSION_SPECIALIZATION(dword_t, long_t) // For dword_t -> fixed_t.

    // Headers.
    template<>
    Decoder::FileHeader Decoder::read<Decoder::FileHeader>()
    {
        FileHeader aseHeader{};

        // Initial values.
        aseHeader.m_StartPos = m_AseFileStream.tellg();
        aseHeader.m_Size = read<dword_t>();
        aseHeader.m_MagicNumber = read<word_t>();

        // Error in file header.
        if (aseHeader.m_MagicNumber != ASEFILE_MAGIC)
        {
            aseHeader.m_Good = false;
            return aseHeader;
        }

        // Read data.
        aseHeader.m_FrameCount = read<word_t>();
        aseHeader.m_Width = read<word_t>();
        aseHeader.m_Height = read<word_t>();
        aseHeader.m_ColorDepth = read<word_t>();
        aseHeader.m_Flags = read<dword_t>();
        aseHeader.m_Speed = read<word_t>();
        aseHeader.m_IgnoreA[0] = read<dword_t>();
        aseHeader.m_IgnoreA[1] = read<dword_t>();
        aseHeader.m_PaletteEntry = read<byte_t>();
        aseHeader.m_IgnoreB[0] = read<byte_t>();
        aseHeader.m_IgnoreB[1] = read<byte_t>();
        aseHeader.m_IgnoreB[2] = read<byte_t>();
        aseHeader.m_ColorCount = read<word_t>();
        aseHeader.m_PixelWidth = read<byte_t>();
        aseHeader.m_PixelHeight = read<byte_t>();
        aseHeader.m_GridX = read<short_t>();
        aseHeader.m_GridY = read<short_t>();
        aseHeader.m_GridWidth = read<word_t>();
        aseHeader.m_GridHeight = read<word_t>();

        // Support old aseprite color count format.
        if (aseHeader.m_ColorCount == 0) aseHeader.m_ColorCount = 256;

        // Skip 84 bytes and start at the frame data section.
        m_AseFileStream.seekg(aseHeader.m_StartPos + 128);

        // Valid file header read.
        aseHeader.m_Good = m_AseFileStream.good();
        return aseHeader;
    }

    template<>
    Decoder::FrameHeader Decoder::read<Decoder::FrameHeader>()
    {
        FrameHeader frameHeader{};

        // Get initial data.
        frameHeader.m_StartPos = m_AseFileStream.tellg();
        frameHeader.m_Size = read<dword_t>();
        frameHeader.m_MagicNumber = read<word_t>();

        // Error in frame header.
        if (frameHeader.m_MagicNumber != ASEFRAME_MAGIC)
        {
            frameHeader.m_Good = false;
            return frameHeader;
        }

        // Continue reading data.
        frameHeader.m_ChunkCount = read<word_t>();
        frameHeader.m_Duration = read<word_t>();

        // Padding bytes.
        read<byte_t>();
        read<byte_t>();

        // New chunk field.
        const auto new_chunk_count = read<dword_t>();
        if (frameHeader.m_ChunkCount == 0xFFFF && frameHeader.m_ChunkCount < new_chunk_count)
            frameHeader.m_ChunkCount = new_chunk_count;

        frameHeader.m_Good = m_AseFileStream.good();
        return frameHeader;
    }

    template<>
    pixel_t Decoder::read<pixel_t>()
    {
        pixel_t pixel{};

        switch (m_PixelFormat)
        {
            case PixelFormat::Indexed:
                pixel.m_Indexed = read<byte_t>();
                break;
            case PixelFormat::Grayscale:
                pixel.m_Grayscale[0] = read<byte_t>();
                pixel.m_Grayscale[1] = read<byte_t>();
                break;
            case PixelFormat::RGBA:
                pixel.m_RGBA[0] = read<byte_t>();
                pixel.m_RGBA[1] = read<byte_t>();
                pixel.m_RGBA[2] = read<byte_t>();
                pixel.m_RGBA[3] = read<byte_t>();
                break;
        }

        return pixel;
    }

    template<>
    Decoder::ChunkHeader Decoder::read<Decoder::ChunkHeader>()
    {
        ChunkHeader chunkHeader{};

        // Get initial data.
        chunkHeader.m_StartPos = m_AseFileStream.tellg();
        chunkHeader.m_Size = read<dword_t>();
        chunkHeader.m_Type = static_cast<Chunk::Type>(read<word_t>());
        return chunkHeader;
    }

    template<>
    Layer Decoder::read<Layer>()
    {
        Layer layer{};

        // Read data.
        layer.m_Flags = static_cast<Layer::Flags>(read<word_t>());
        layer.m_Type = static_cast<Layer::Type>(read<word_t>());
        layer.m_ChildLevel = read<word_t>();
        read<word_t>();
        read<word_t>();
        layer.m_BlendMode = static_cast<Layer::BlendMode>(read<word_t>());
        layer.m_Opacity = read<byte_t>();

        // Padding.
        read<byte_t>();
        read<byte_t>();
        read<byte_t>();

        // Read name.
        layer.m_Name = read<string_t>();
        return layer;
    }

    template<>
    Cel Decoder::read<Cel>(const ChunkHeader & chunkHeader)
    {
        Cel cel{};

        // Chunk header pos.
        const size_t chunk_data_start_pos = m_AseFileStream.tellg();

        // Read data.
        cel.m_LayerIndex = read<word_t>();
        cel.m_X = read<short_t>();
        cel.m_Y = read<short_t>();
        cel.m_Opacity = read<byte_t>();
        cel.m_Type = static_cast<Cel::Type>(read<word_t>());
        std::cout << "Cel type" << static_cast<int>(cel.m_Type) << std::endl;
        // Padding bytes. (TODO: Use padding function)
        read<byte_t>();
        read<byte_t>();
        read<byte_t>();
        read<byte_t>();
        read<byte_t>();
        read<byte_t>();
        read<byte_t>();

        // Read type data.
        switch (cel.m_Type)
        {
            case Cel::Type::Raw:
            {
                auto&[width, height, bytes] = std::get<Cel::raw_image_t>(cel.m_TypeData);
                width = read<word_t>();
                height = read<word_t>();
                bytes.reserve(width * height);

                // TODO: Use iterators! For now using push_back to make it work.
                for (int i = 0; i < width * height; ++i)
                {
                    bytes.push_back(read<pixel_t>());
                }
            }
                break;

            case Cel::Type::Linked:
            {
                cel.m_TypeData = Cel::linked_cel_t{};
                std::get<Cel::linked_cel_t>(cel.m_TypeData) = read<word_t>();
            }
                break;

            case Cel::Type::CompressedImage:
            {
                cel.m_TypeData = Cel::compressed_image_t{};
                auto&[width, height, bytes] = std::get<Cel::compressed_image_t>(cel.m_TypeData);
                width = read<word_t>();
                height = read<word_t>();
                bytes.reserve(width * height * static_cast<size_t>(m_PixelFormat));

                // TODO: Use iterators! For now using push_back to make it work.
                for (int i = 0; i < chunkHeader.m_Size - (chunkHeader.m_StartPos - chunk_data_start_pos); ++i)
                {
                    bytes.push_back(read<byte_t>());
                }
            }
                break;
        }

        return cel;
    }
}