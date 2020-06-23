// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "ase_to_psd/decoder.h"
#include <utility>

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
        m_AseFilestream.open(m_Path, std::ios_base::in | std::ios_base::binary);

        if (m_AseFilestream)
        {
            auto asedoc = std::make_unique<File>();

            // Parse header.
            const auto fileHeader(read<FileHeader>());

            if (fileHeader.m_Good)
            {
                for (word_t cf = 0; cf < fileHeader.m_FrameCount; ++cf)
                {
                    const size_t frame_start_pos = m_AseFilestream.tellg();
                    const auto frameHeader(read<FrameHeader>());
                    m_AseFilestream.seekg(frame_start_pos + 16);

                    if (frameHeader.m_Good)
                    {
                        for (word_t cc = 0; cc < frameHeader.m_ChunkCount; ++cc)
                        {
                            const size_t chunk_start_pos = m_AseFilestream.tellg();
                            const auto chunkHeader(read<ChunkHeader>());

                            switch (chunkHeader.m_Type)
                            {
                                case Chunk::Type::OldPaletteA:
                                    break;
                                case Chunk::Type::OldPaletteB:
                                    break;
                                case Chunk::Type::Layer:
                                    asedoc->m_Layers.emplace_back(read<Layer>());
                                    break;
                                case Chunk::Type::Cel:
                                    break;
                                case Chunk::Type::CelExtra:
                                    break;
                                case Chunk::Type::Mask:
                                    break;
                                case Chunk::Type::Path:
                                    break;
                                case Chunk::Type::FrameTags:
                                    break;
                                case Chunk::Type::Palette:
                                    break;
                                case Chunk::Type::UserData:
                                    break;
                                case Chunk::Type::Slice:
                                    break;
                            }

                            m_AseFilestream.seekg(chunk_start_pos + chunkHeader.m_Size);
                        }
                    }
                }

                return asedoc;
            }
        }

        return nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// IO Reading
    ///////////////////////////////////////////////////////////////////////////

    template<>
    byte_t Decoder::read<byte_t>()
    {
        int val = m_AseFilestream.get();

        if (!m_AseFilestream.eof())
            return val;

        return 0;
    }

    template<>
    word_t Decoder::read<word_t>()
    {
        auto a = read<byte_t>();
        auto b = read<byte_t>();

        if (!m_AseFilestream.eof())
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

        if (!m_AseFilestream.eof())
            return ((d << 24) | (c << 16) | (b << 8) | a);
        else
            return 0;
    }

    template<>
    string_t Decoder::read<string_t>()
    {
        const auto len = read<word_t>();
        string_t str(len, '\0');
        m_AseFilestream.read(&str[0], len);
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
        FileHeader aseHeader;

        // Initial values.
        size_t start_pos = m_AseFilestream.tellg();
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
        m_AseFilestream.seekg(start_pos + 128);

        // Valid file header read.
        aseHeader.m_Good = m_AseFilestream.good();
        return aseHeader;
    }

    template<>
    Decoder::FrameHeader Decoder::read<Decoder::FrameHeader>()
    {
        FrameHeader frameHeader;

        // Get initial data.
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

        frameHeader.m_Good = m_AseFilestream.good();
        return frameHeader;
    }

    template<>
    Decoder::ChunkHeader Decoder::read<Decoder::ChunkHeader>()
    {
        ChunkHeader chunkHeader;

        // Get initial data.
        chunkHeader.m_Size = read<dword_t>();
        chunkHeader.m_Type = static_cast<Chunk::Type>(read<word_t>());
        return chunkHeader;
    }

    template<>
    Layer Decoder::read<Layer>()
    {
        Layer layer;

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
}