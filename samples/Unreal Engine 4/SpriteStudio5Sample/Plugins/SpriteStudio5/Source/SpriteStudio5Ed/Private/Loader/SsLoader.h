#pragma once

#include "SsLoader.h"


class FSsLoader
{
public:
	static class USsProject* LoadSsProject(UObject* InParent, FName InName, EObjectFlags Flags, const uint8*& Buffer, size_t Size);
	static bool FSsLoader::LoadSsAnimePack(struct FSsAnimePack* AnimePack, const uint8*& Buffer, size_t Size);
	static bool FSsLoader::LoadSsCellMap(struct FSsCellMap* CellMap, const uint8*& Buffer, size_t Size);
};

