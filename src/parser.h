// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ASE_TO_PSD_PARSER_H
#define ASE_TO_PSD_PARSER_H

#pragma once

// Reader Interface
template<typename T, std::size_t bytes = sizeof(T)>
struct AseType
{
    static T read(std::ifstream &asefile, size_t n = 1)
    {
        if (asefile.is_open() && asefile)
        {
            T buffer;
            asefile.read(reinterpret_cast<char *>(&buffer), bytes * n);

            if (!asefile) // ERROR: Failed to read all the bytes.
                throw std::exception();

            return buffer;
        }

        throw std::exception();
    }
};

template<>
struct AseType<string_t>
{
    static string_t read(std::ifstream &asefile)
    {
        if (asefile.is_open() && asefile)
        {
            auto strLength = static_cast<size_t>(AseType<word_t>::read(asefile));
            string_t strBuffer;
            strBuffer.reserve(strLength);
            asefile.read(const_cast<char *>(strBuffer.data()), strLength);

            if (!asefile) // ERROR: Failed to read all the bytes.
                throw std::exception();

            return strBuffer;
        }

        throw std::exception();
    }
};


#endif //ASE_TO_PSD_PARSER_H
