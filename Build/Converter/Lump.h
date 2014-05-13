//
//  Lump.h
//

#ifndef Lump_h
#define Lump_h

#include <vector>
#include <string>


class LumpSet;


class Lump
{
public:
	enum DataType
	{
		S16,
		S32,
		FLOAT,
		COLOR,
		STRING,
		SET
	};

	const DataType	type;
	union
	{
		int				i;
		float			f;
		std::string*	s;
		LumpSet*		p;
	} data;


	static Lump* s16Data(int value);
	static Lump* s32Data(int value);
	static Lump* floatData(float value);
	static Lump* colorData(int color);
	static Lump* stringData(const std::string& value);

	static Lump* set(const std::string& className, bool isReference = false);

	void add(Lump* lump);
	void addFirst(Lump* lump);
	size_t count() const;

	~Lump();

private:
	explicit Lump(DataType type);
};



class LumpSet
{
public:
	enum ArrayType
	{
		NO_ARRAY,
		ARRAY,
		U16_ARRAY
	};

	typedef std::vector<Lump*> SetType;

	const std::string	className;
	const ArrayType		arrayType;
	const bool			isReference;
	SetType				set;

	LumpSet(const std::string& className, ArrayType arrayType, bool isReference);
	~LumpSet();
};



#endif
