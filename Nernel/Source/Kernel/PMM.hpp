#pragma once

namespace PMM
{
	void Load();

	UInt AllocatePage();
	void ReleasePage(UInt pAddress);
};
