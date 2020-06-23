// Copyright(c) 2020-present, Roland Munguia.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "Psd/Psd.h"
#include "Psd/PsdPlatform.h"

#include "Psd/PsdMallocAllocator.h"
#include "Psd/PsdNativeFile.h"

#include "Psd/PsdDocument.h"

#include "ase_to_psd/decoder.h"

int main(void) 
{
    Aseprite::Decoder myDecoder;
   auto temp = *myDecoder.parse("./input.ase");
   temp.m_Layers.size();
}

// TODO: Add doctest
// TODO: Document aseprite.h
// TODO: Add PCH Support
// TODO: App Veyor Support
// TODO: Move .h from /src to /include