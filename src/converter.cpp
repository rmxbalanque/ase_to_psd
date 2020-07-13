// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "ase_to_psd/converter.h"
#include "ase_to_psd/decoder.h"

#include "Psd/Psd.h"
#include "Psd/PsdPlatform.h"

#include "Psd/PsdMallocAllocator.h"
#include "Psd/PsdNativeFile.h"

#include "Psd/PsdDocument.h"
#include "Psd/PsdColorMode.h"
#include "Psd/PsdLayer.h"
#include "Psd/PsdChannel.h"
#include "Psd/PsdChannelType.h"
#include "Psd/PsdLayerMask.h"
#include "Psd/PsdVectorMask.h"
#include "Psd/PsdLayerMaskSection.h"
#include "Psd/PsdImageDataSection.h"
#include "Psd/PsdImageResourcesSection.h"
#include "Psd/PsdParseDocument.h"
#include "Psd/PsdParseLayerMaskSection.h"
#include "Psd/PsdParseImageDataSection.h"
#include "Psd/PsdParseImageResourcesSection.h"
#include "Psd/PsdLayerCanvasCopy.h"
#include "Psd/PsdInterleave.h"
#include "Psd/PsdPlanarImage.h"
#include "Psd/PsdExport.h"
#include "Psd/PsdExportDocument.h"
#include "debugapi.h"
#include "../psd_sdk/src/Samples/PsdTgaExporter.h"
#include "../psd_sdk/src/Samples/PsdTgaExporter.cpp"

PSD_PUSH_WARNING_LEVEL(0)
// disable annoying warning caused by xlocale(337): warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable:4530)
#include <string>
#include <sstream>
PSD_POP_WARNING_LEVEL

PSD_USING_NAMESPACE;

namespace Aseprite
{
    int Convert(const std::string &in_path, const std::string &out_path, bool ase_to_psd)
    {
        // Get wide char paths.
        std::wstring win_path{in_path.begin(), in_path.end()};
        std::wstring wout_path{out_path.begin(), out_path.end()};

        // Decode aseprite file
        Aseprite::Decoder decoder;
        auto aseprite_file = *decoder.parse(in_path);

        // Encode to photoshop file
        psd::MallocAllocator allocator;
        psd::NativeFile file(&allocator);

        // Attempt to open file, if not bail out.
        if (!file.OpenWrite(wout_path.c_str()))
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

        return 0;
    }
}