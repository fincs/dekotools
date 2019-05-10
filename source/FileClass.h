#pragma once
#include <stdio.h>
#include "types.h"

class FileClass
{
	FILE* f;
	bool LittleEndian, own;

	size_t _RawRead(void* buffer, size_t size)
	{
		return fread(buffer, 1, size, f);
	}

	size_t _RawWrite(const void* buffer, size_t size)
	{
		return fwrite(buffer, 1, size, f);
	}

public:
	FileClass(const char* file, const char* mode) : LittleEndian(true), own(true)
	{
		f = fopen(file, mode);
	}
	FileClass(FILE* inf) : f(inf), LittleEndian(true), own(false) { }
	~FileClass()
	{
		if (f && own) fclose(f);
	}

	void SetLittleEndian() { LittleEndian = true; }
	void SetBigEndian() { LittleEndian = false; }

	FILE* get_ptr() { return f; }
	FILE* detach() { FILE* ret = f; f = nullptr; return ret; }
	bool openerror() { return f == nullptr; }

	bool Buffer(int mode, size_t size, char* buffer = nullptr)
	{
		return setvbuf(f, buffer, mode, size) == 0;
	}

	dword_t ReadDword()
	{
		dword_t value;
		_RawRead(&value, sizeof(dword_t));
		return LittleEndian ? le_dword(value) : be_dword(value);
	}

	void WriteDword(dword_t value)
	{
		value = LittleEndian ? le_dword(value) : be_dword(value);
		_RawWrite(&value, sizeof(dword_t));
	}

	word_t ReadWord()
	{
		word_t value;
		_RawRead(&value, sizeof(word_t));
		return LittleEndian ? le_word(value) : be_word(value);
	}

	void WriteWord(word_t value)
	{
		value = LittleEndian ? le_word(value) : be_word(value);
		_RawWrite(&value, sizeof(word_t));
	}

	hword_t ReadHword()
	{
		hword_t value;
		_RawRead(&value, sizeof(hword_t));
		return LittleEndian ? le_hword(value) : be_hword(value);
	}

	void WriteHword(hword_t value)
	{
		value = LittleEndian ? le_hword(value) : be_hword(value);
		_RawWrite(&value, sizeof(hword_t));
	}

	byte_t ReadByte()
	{
		byte_t value;
		_RawRead(&value, sizeof(byte_t));
		return value;
	}

	void WriteByte(byte_t value)
	{
		_RawWrite(&value, sizeof(byte_t));
	}

	word_t ReadVL()
	{
		word_t value = 0;
		byte_t data = 0;
		do
			data = ReadByte(), value = (value << 7) | ((word_t)data & 0x7F);
		while(data & 0x80);
		return value;
	}

	void WriteVL(word_t value)
	{
		int size = 1;
		for(word_t temp = value; temp > 0x7F; temp >>= 7, size ++);
		int shift = 7*(size - 1);
		for(int i = 0; i < (size-1); i ++, shift -= 7)
			WriteByte(0x80 | ((value >> shift) & 0x7F));
		WriteByte((value >> shift) & 0x7F);
	}

	float ReadFloat()
	{
		union { word_t w; float f; } t;
		t.w = ReadWord();
		return t.f;
	}

	void WriteFloat(float value)
	{
		union { word_t w; float f; } t;
		t.f = value;
		WriteWord(t.w);
	}

	void Print(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		vfprintf(f, fmt, args);
		va_end(args);
	}

	bool ReadRaw(void* buffer, size_t size) { return _RawRead(buffer, size) == size; }
	bool WriteRaw(const void* buffer, size_t size) { return _RawWrite(buffer, size) == size; }

	void Seek(long pos, int mode) { fseek(f, pos, mode); }
	long Tell() { return ftell(f); }
	void Rewind() { rewind(f); }
	void Flush() { fflush(f); }
};

static inline char* StringFromFile(const char* filename)
{
	FileClass f(filename, "rb");
	if (f.openerror())
		return nullptr;
	f.Seek(0, SEEK_END);
	long size = f.Tell();
	f.Rewind();
	char* buf = (char*)malloc(size+1);
	if (!buf)
		return nullptr;
	f.ReadRaw(buf, size);
	buf[size] = 0;
	return buf;
}
