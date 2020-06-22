// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ASE_TO_PSD_ASEPRITE_H
#define ASE_TO_PSD_ASEPRITE_H
#pragma once

#include <cstdint>
#include <vector>
#include <fstream>

namespace Aseprite
{
    // .ase file data types
    using byte_t = std::uint8_t;
    using word_t = std::uint16_t;
    using short_t = std::int16_t;
    using dword_t = std::uint32_t;
    using long_t = std::int32_t;
    using fixed_t = std::int32_t;
    using string_t = std::string;

    union pixel_t
    {
        byte_t m_Indexed;
        byte_t m_Grayscale[2];
        byte_t m_RGBA[4];
    };

    enum class Modes
    {
        Indexed = 1,
        Grayscale = 2,
        RGBA = 4,
    };

    enum class Chunks
    {
        OldPaletteA = 0x0004,
        OldPaletteB = 0x0011,
        Layer = 0x2004,
        Cel = 0x2005,
        CelExtra = 0x2006,
        Mask = 0x2016,
        Path = 0x2017,
        FrameTags = 0x2018,
        Palette = 0x2019,
        UserData = 0x2020,
        Slice = 0x2022
    };

    struct Chunk
    {
        // User data.
    };

    struct Layer
    {
        enum class Flags : word_t
        {
            Visible = 1,
            Editable = 2,
            LockMovement = 4,
            Background = 8,
            PreferLinkedCels = 16,
            DisplayLayerGroupCollapsed = 32,
            ReferenceLayer = 64,
        };

        enum class Type : word_t
        {
            Normal = 0,
            Group = 1,
        };

        enum class BlendMode : word_t
        {
            Normal = 0,
            Multiply,
            Screen,
            Overlay,
            Darken,
            Lighten,
            ColorDodge,
            ColorBurn,
            HardLight,
            SoftLight,
            Difference,
            Exclusion,
            Hue,
            Saturation,
            Color,
            Luminosity,
            Addition,
            Subtract,
            Divide,
        };

        Flags m_Flags;
        Type m_Type;
        word_t m_ChildLevel;
        BlendMode m_BlendMode;
        byte_t m_Opacity;
        string_t m_Name;
    };

    struct Cel
    {
        enum class Type : word_t
        {
            Raw = 0,
            Linked,
            CompressedImage,
        };

        word_t m_LayerIndex;
        short_t m_X;
        short_t m_Y;
        byte_t m_Opacity;
        Type m_Type;

        // TODO: Use union or std::variant
        // Raw / Compressed Image
        word_t m_Width;
        word_t m_Height;

        // Raw = Pixels / Compressed Image = Byte
        std::vector<byte_t> m_Data;

        // Linked cel
        word_t m_FramePos;
    };

    struct Frame
    {
        word_t m_Duration;
        std::vector<Cel> m_Cells;
    };

    struct Tag
    {
        enum class LoopAnimationDir : byte_t
        {
            Forward = 0,
            Reverse = 0,
            PingPong = 0,
        };

        word_t m_StartPos;
        word_t m_EndPos;
        LoopAnimationDir m_LoopAnimationDirection;
        byte_t m_RGB[3];
        string_t m_Name;
    };

    struct Slice
    {
        enum class Flags : dword_t
        {
            Slice9 = 1,
            Pivot
        };

        struct Key
        {
            dword_t m_FrameNumber;
            long_t m_X;
            long_t m_Y;
            dword_t m_Width;
            dword_t m_Height;

            // TODO: Use union or std::variant.

            // 9 Slice
            long_t m_CenterX;
            long_t m_CenterY;
            dword_t m_CenterWidth;
            dword_t m_CenterHeight;

            // Pivot.
            long_t m_PivotX;
            long_t m_PivotY;
        };

        string_t m_Name;
    };

    // TODO: Old Palette A, B, and New Palette Chunk

    /*!
     * \brief Aseprite document object.
     */
    class File
    {
        enum class Flags : dword_t
        {
            LayerOpacityIsValid = 1,
        };

        dword_t m_Size;         //!< File size
        word_t m_FrameCount;    //!< Number of frames
        word_t m_Width;         //!< Width of sprite
        word_t m_Height;        //!< Height of sprite
        word_t m_ColorDepth;    //!< Bits per pixel (32 bpp = RGBA, 16 bpp = Grayscale, 8 bpp = Indexed)
        Flags m_Flags;          //!< Aseprite file flag
        byte_t m_PaletteEntry;  //!< Index which represent transparent color in all non-background layers. (Indexed sprites only)
        word_t m_ColorCount;    //!< Number of colors (0 means 256 for old sprites)
        byte_t m_PixelWidth;    //!< Pixel width. If this is or pixel height is zero, pixel ratio is 1:1
        byte_t m_PixelHeight;   //!< Pixel height
        short_t m_GridX;        //!< X position of the grid
        short_t m_GridY;        //!< Y position of the grid
        word_t m_GridWidth;     //!< Grid width (Zero if there is no grid, grid size is 16x16 on Aseprite by default)
        word_t m_GridHeight;    //!< Grid height (Zero if there is no grid)

        std::vector<Frame> m_Frames;    //!< Frame container
        std::vector<Layer> m_Layers;    //!< Layer container
        std::vector<Slice> m_Slices;    //!< Slices container
        std::vector<Tag> m_Tags;        //!< Tags container
    };
}

#endif  //ASE_TO_PSD_ASEPRITE_H