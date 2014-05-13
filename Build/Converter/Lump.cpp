//
//  Lump.cpp
//

#include "Lump.h"
#include <assert.h>


Lump::Lump(DataType type) : type(type) {}

Lump::~Lump()
{
	switch (type)
	{
		case S16:
		case S32:
		case FLOAT:
		case COLOR:
			break;
		case STRING:
			delete data.s;
			break;
		case SET:
			delete data.p;
			break;
	}
}

Lump* Lump::s16Data(int value)
{
	Lump* v = new Lump(S16);
	v->data.i = value;
	return v;
}

Lump* Lump::s32Data(int value)
{
	Lump* v = new Lump(S32);
	v->data.i = value;
	return v;
}

Lump* Lump::floatData(float value)
{
	Lump* v = new Lump(FLOAT);
	v->data.f = value;
	return v;
}

Lump* Lump::colorData(int color)
{
	Lump* v = new Lump(COLOR);
	v->data.i = color;
	return v;
}

Lump* Lump::stringData(const std::string& value)
{
	Lump* v = new Lump(STRING);
	v->data.s = new std::string(value);
	return v;
}

Lump* Lump::set(const std::string& className, bool isReference)
{
	size_t len = className.length();
	bool isArray = className.at(len-2) == '[' && className.at(len-1) == ']';
	bool isPointerArray = className.at(len-3) == '*';
	LumpSet::ArrayType arrayType = isArray ? LumpSet::ARRAY : LumpSet::NO_ARRAY;

	std::string name = className;
	if (isArray)
	{
		// ポインタの配列のときは強制的に型を変える
		if (isPointerArray)
		{
			name = "ss::ss_offset";
		}
		else
		{
			name = name.substr(0, len-2);

			// ss_u16型の配列のときは専用配列とする
			if (name == "ss::ss_u16")
			{
				arrayType = LumpSet::U16_ARRAY;
			}
		}
	}

	Lump* v = new Lump(SET);
	v->data.p = new LumpSet(name, arrayType, isReference);
	return v;
}

void Lump::add(Lump* lump)
{
	assert(type == SET);
	data.p->set.push_back(lump);
}

void Lump::addFirst(Lump* lump)
{
	assert(type == SET);
	data.p->set.insert(data.p->set.begin(), lump);
}

size_t Lump::count() const
{
	assert(type == SET);
	return static_cast<int>(data.p->set.size());
}




LumpSet::LumpSet(const std::string& className, ArrayType arrayType, bool isReference)
	: className(className)
	, arrayType(arrayType)
	, isReference(isReference)
{}

LumpSet::~LumpSet()
{
	for (SetType::reverse_iterator it = set.rbegin(); it != set.rend(); it++)
	{
		Lump* lump = *it;
		delete lump;
	}
}



