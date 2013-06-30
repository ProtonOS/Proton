#pragma once

#include "Core/Types.hpp"
#include "Core/Ptr.hpp"

namespace MultiBoot {
    struct MultiBootHeader;
	void Load(UInt32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader);
};
