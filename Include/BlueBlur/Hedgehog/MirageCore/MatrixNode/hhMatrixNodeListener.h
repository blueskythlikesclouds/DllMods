#pragma once

#include <BlueBlur.h>
#include <Hedgehog/Base/hhObject.h>

namespace Hedgehog::Mirage
{
    class CMatrixNodeListener : public Base::CObject
    {
    public:
        CMatrixNodeListener(const null_ctor&) {}
        CMatrixNodeListener() {}

        virtual ~CMatrixNodeListener() = default;
        virtual bool MatrixNodeUpdatedCallback(const Hedgehog::Math::CMatrix& matrix, size_t flags) = 0;
    };
}