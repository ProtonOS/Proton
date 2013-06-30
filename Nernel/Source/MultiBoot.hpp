#pragma once

namespace MultiBoot {
    struct MultiBootHeader;
	void Load(UInt32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader);
};
