
#include "BinaryDataWriter.h"
#include <stdexcept>


BinaryDataWriter::BinaryDataWriter(std::ostream& out, EndiannessType endianness)
	: _out(out)
	, _endianness(endianness)
{
}

BinaryDataWriter::~BinaryDataWriter()
{
}

void BinaryDataWriter::seekp(long pos)
{
	_out.seekp(pos);
}

void BinaryDataWriter::fill(char fillData, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		writeByte(fillData);
	}
}

void BinaryDataWriter::align(int unit)
{
	long pos = static_cast<long>(_out.tellp());
	int n = static_cast<int>(pos & (unit-1));
	if (n)
	{
		int nPadding = unit - n;
		fill(0, nPadding);
	}
}

void BinaryDataWriter::fixReferences()
{
	long currentPos = static_cast<long>(_out.tellp());

	for (ReferenceMapType::const_iterator i = _referenceMap.begin(), end = _referenceMap.end(); i != end; i++)
	{
		const Reference& ref = i->second;
		if (ref.referencePosList.empty()) continue;	// 参照先なし
		if (ref.dataPos < 0) 
		{
			throw std::logic_error(i->first + "に対応する dataPos が記録されていません");
		}

		// 参照元のポジションを書き換える
		for (Reference::ReferencePosListType::const_iterator posIt = ref.referencePosList.begin(); posIt != ref.referencePosList.end(); posIt++)
		{
			_out.seekp(*posIt);
			writeInt(static_cast<int>(ref.dataPos));
		}
	}
	
	_out.seekp(currentPos);
}

void BinaryDataWriter::writeReference(const std::string &key, bool align4Bytes)
{
	Reference ref;
	if (_referenceMap.find(key) != _referenceMap.end()) ref = _referenceMap.at(key);
	
	if (align4Bytes) align(4);
	long pos = static_cast<long>(_out.tellp());
	ref.referencePosList.push_back(pos);
	_referenceMap[key] = ref;
	writeInt(0);
}

void BinaryDataWriter::setReference(const std::string &key)
{
	Reference ref;
	if (_referenceMap.find(key) != _referenceMap.end()) ref = _referenceMap.at(key);

	align(4);
	ref.dataPos = static_cast<long>(_out.tellp());
	_referenceMap[key] = ref;
}

void BinaryDataWriter::writeByte(char data)
{
	char buf[1];
	buf[0] = data;
	_out.write(buf, sizeof(buf));
}

void BinaryDataWriter::writeShort(short data)
{
	char buf[2];
	if (_endianness == ET_LITTLE_ENDIAN)
	{
		buf[0] = static_cast<char>(data);
		buf[1] = static_cast<char>(data >> 8);
	}
	else
	{
		buf[0] = static_cast<char>(data >> 8);
		buf[1] = static_cast<char>(data);
	}
	_out.write(buf, sizeof(buf));
}

void BinaryDataWriter::writeInt(int data)
{
	char buf[4];
	if (_endianness == ET_LITTLE_ENDIAN)
	{
		buf[0] = static_cast<char>(data);
		buf[1] = static_cast<char>(data >> 8);
		buf[2] = static_cast<char>(data >> 16);
		buf[3] = static_cast<char>(data >> 24);
	}
	else
	{
		buf[0] = static_cast<char>(data >> 24);
		buf[1] = static_cast<char>(data >> 16);
		buf[2] = static_cast<char>(data >> 8);
		buf[3] = static_cast<char>(data);
	}
	_out.write(buf, sizeof(buf));
}

void BinaryDataWriter::writeFloat(float data)
{
	union {
		float	f;
		int		i;
	} c;
	c.f = data;
	writeInt(c.i);
}

void BinaryDataWriter::writeString(const std::string& str)
{
	_out.write(str.c_str(), str.length());
	writeByte(0);
}

