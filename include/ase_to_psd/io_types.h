// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ASE_TO_PSD_IO_TYPES_H
#define ASE_TO_PSD_IO_TYPES_H

#pragma once
#include <cstdint>
#include <string>

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
}

#endif //ASE_TO_PSD_IO_TYPES_H
