// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)


#include <iostream>
#include "ase_to_psd/converter.h"
#include "cxxopts.hpp"

int main(int argc, char * argv[])
{
    // CLI ////////////////////////////////////////////////////////////////////

    // Program
    cxxopts::Options cli_options{"ATP_Coverter", "Aseprite to Photoshop file converter"};

    // Options
    cli_options.allow_unrecognised_options().add_options()
    ("help", "Print help")
    ("i,input", "Input file path", cxxopts::value<std::string>())
    ("o,output", "Output file path", cxxopts::value<std::string>())
    ("f,frame", "Aseprite frame to convert to Photoshop file", cxxopts::value<unsigned int>())
    ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("true"))
    ("c,construct", "Constructs Aseprite file from Photoshop file", cxxopts::value<bool>()->default_value("false"));

    // Parse positional arguments
    cli_options.parse_positional({"input", "output"});
    auto result = cli_options.parse(argc, argv);

    // No arguments provided
    if (result.arguments().empty())
    {
        std::cout << cli_options.help() << std::endl;
        exit(0);
    }

    // Interpret options/arguments
    if (result.count("help"))
    {
        std::cout << cli_options.help() << std::endl;
        exit(0);
    }

    if (result.count("input") && result.count("output"))
    {
        return Aseprite::Convert(result["input"].as<std::string>(), result["output"].as<std::string>(), result["construct"].as<bool>());
    }
    else
    {
        if (!result.count("input")) std::cout << "ERROR: Missing input filepath" << std::endl;
        if (!result.count("output")) std::cout << "ERROR: Missing output filepath" << std::endl;
        return 0;
    }

    // TODO: Wrap in try catch
    ///////////////////////////////////////////////////////////////////////////
    // Decode aseprite file
    /*Aseprite::Decoder decoder;
    auto aseprite_file = *decoder.parse("./input.ase");

    // Encode to photoshop file
    psd::MallocAllocator allocator;
    psd::NativeFile file(&allocator);

    // Attempt to open file, if not bail out.
    if (!file.OpenWrite(L"./sample_psd.psd"))
    {
        return 1;
    }

    // Create document
    psd::ExportDocument* psd_document = psd::CreateExportDocument(&allocator, aseprite_file.m_Width, aseprite_file.m_Height, 8u, psd::exportColorMode::RGB);

    // Planar color data.
    auto * planar_col_r = new std::vector<Aseprite::byte_t>;
    auto * planar_col_g = new std::vector<Aseprite::byte_t>;
    auto * planar_col_b = new std::vector<Aseprite::byte_t>;
    auto * planar_col_a = new std::vector<Aseprite::byte_t>;

    // Add layers to document
    for (const auto & layer : aseprite_file.m_Layers)
    {
        auto index = psd::AddLayer(psd_document, &allocator, layer.m_Name.c_str());
    }

    // Add cel image data.
    for (const auto & cel : aseprite_file.m_Frames.begin()->m_Cels)
    {
        switch (cel.m_Type)
        {
            case Aseprite::Cel::Type::Raw:
            {
                auto &[height, width, data] = std::get<Aseprite::Cel::raw_image_t>(cel.m_TypeData);
                break;
            }
            case Aseprite::Cel::Type::Linked:
            {
                auto &frame_pos = std::get<Aseprite::Cel::linked_cel_t>(cel.m_TypeData);
                break;
            }
            case Aseprite::Cel::Type::CompressedImage:
            {
                auto [width, height, data] = decoder.decompress_cel_data(std::get<Aseprite::Cel::compressed_image_t>(cel.m_TypeData));

                planar_col_r->reserve(width * height);
                planar_col_g->reserve(width * height);
                planar_col_b->reserve(width * height);
                planar_col_a->reserve(width * height);

                psd::imageUtil::DeinterleaveRGBA(static_cast<const Aseprite::byte_t *>(data.data()->m_RGBA), planar_col_r->data(), planar_col_g->data(), planar_col_b->data(), planar_col_a->data(), width, height);

                psd::UpdateLayer(psd_document, &allocator, cel.m_LayerIndex, psd::exportChannel::RED, cel.m_X,  cel.m_Y,  cel.m_X + width, cel.m_Y + height, planar_col_r->data(), psd::compressionType::RAW);
                psd::UpdateLayer(psd_document, &allocator, cel.m_LayerIndex, psd::exportChannel::GREEN, cel.m_X, cel.m_Y, cel.m_X + width, cel.m_Y + height, planar_col_g->data(), psd::compressionType::RAW);
                psd::UpdateLayer(psd_document, &allocator, cel.m_LayerIndex, psd::exportChannel::BLUE, cel.m_X, cel.m_Y,  cel.m_X + width, cel.m_Y + height, planar_col_b->data(), psd::compressionType::RAW);
                psd::UpdateLayer(psd_document, &allocator, cel.m_LayerIndex, psd::exportChannel::ALPHA, cel.m_X, cel.m_Y, cel.m_X + width, cel.m_Y + height, planar_col_a->data(), psd::compressionType::RAW);
                break;
            }
        }
    }

    // Free planar color data vectors.
    delete planar_col_r;
    delete planar_col_g;
    delete planar_col_b;
    delete planar_col_a;

    // Write psd document to file
    psd::WriteDocument(psd_document, &allocator, &file);

    psd::DestroyExportDocument(psd_document, &allocator);
    file.Close();
    return 0;*/
    return 0;
}

// TODO: Add doctest
// TODO: Document aseprite.h
// TODO: Add PCH Support
// TODO: App Veyor Support
// TODO: Move .h from /src to /include