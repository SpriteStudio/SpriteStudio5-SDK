#ifndef __BinaryDataWriter__
#define __BinaryDataWriter__

#include <iostream>
#include <string>
#include <map>
#include <vector>

class BinaryDataWriter
{
public:
	struct Reference
	{
		typedef std::vector<long> ReferencePosListType;
	
		long					dataPos;
		ReferencePosListType	referencePosList;
		
		Reference() : dataPos(-1L) {}
	};

	typedef std::map<std::string, Reference> ReferenceMapType;

	enum EndiannessType
	{
		ET_LITTLE_ENDIAN,
		ET_BIG_ENDIAN
	};

	BinaryDataWriter(std::ostream& out, EndiannessType endianness = ET_LITTLE_ENDIAN);
	virtual ~BinaryDataWriter();

	void seekp(long pos);
	void fill(char fillData, size_t size);
	void align(int n);

	void fixReferences();
	void writeReference(const std::string& key, bool align4Bytes = true);
	void setReference(const std::string& key);

	void writeByte(char data);
	void writeShort(short data);
	void writeInt(int data);
	void writeFloat(float data);
	void writeString(const std::string& str);

private:
	std::ostream&		_out;
	EndiannessType		_endianness;
	ReferenceMapType	_referenceMap;
};

#endif /* defined(__BinaryDataWriter__) */
