#pragma once

#include "Core/Types.hpp"
#include "Core/Ptr.hpp"

namespace MultiBoot {
    struct MultiBootHeader;
	void Load(uint32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader);
};
