#pragma  once

#include "xxstring.h"
#include <stdint.h>
#include <map>

using namespace std;



enum {BASE64_OK = 0, BASE64_INVALID};

#define BASE64_ENCODE_OUT_SIZE(s)	(((s) + 2) / 3 * 4)
#define BASE64_DECODE_OUT_SIZE(s)	(((s)) / 4 * 3)



#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))


/* BASE 64 encode table */
static const char base64en[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/',
};

#define BASE64_PAD	'='


#define BASE64DE_FIRST	'+'
#define BASE64DE_LAST	'z'
/* ASCII order for BASE 64 decode, -1 in unused character */
static const signed char base64de[] = {
	/* '+', ',', '-', '.', '/', '0', '1', '2', */
	62, -1, -1, -1, 63, 52, 53, 54,

	/* '3', '4', '5', '6', '7', '8', '9', ':', */
	55, 56, 57, 58, 59, 60, 61, -1,

	/* ';', '<', '=', '>', '?', '@', 'A', 'B', */
	-1, -1, -1, -1, -1, -1, 0, 1,

	/* 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', */
	2, 3, 4, 5, 6, 7, 8, 9,

	/* 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', */
	10, 11, 12, 13, 14, 15, 16, 17,

	/* 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
	18, 19, 20, 21, 22, 23, 24, 25,

	/* '[', '\', ']', '^', '_', '`', 'a', 'b', */
	-1, -1, -1, -1, -1, -1, 26, 27,

	/* 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', */
	28, 29, 30, 31, 32, 33, 34, 35,

	/* 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', */
	36, 37, 38, 39, 40, 41, 42, 43,

	/* 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', */
	44, 45, 46, 47, 48, 49, 50, 51,
};

static int
base64_encode(const unsigned char *in, unsigned int inlen,  char *out)
{
	unsigned int i, j;

	for (i = j = 0; i < inlen; i++) {
		int s = i % 3; 			/* from 6/gcd(6, 8) */

		switch (s) {
		case 0:
			out[j++] = base64en[(in[i] >> 2) & 0x3F];
			continue;
		case 1:
			out[j++] = base64en[((in[i - 1] & 0x3) << 4) + ((in[i] >> 4) & 0xF)];
			continue;
		case 2:
			out[j++] = base64en[((in[i - 1] & 0xF) << 2) + ((in[i] >> 6) & 0x3)];
			out[j++] = base64en[in[i] & 0x3F];
		}
	}

	/* move back */
	i -= 1;

	/* check the last and add padding */
	if ((i % 3) == 0) {
		out[j++] = base64en[(in[i] & 0x3) << 4];
		out[j++] = BASE64_PAD;
		out[j++] = BASE64_PAD;
	}
	else if ((i % 3) == 1) {
		out[j++] = base64en[(in[i] & 0xF) << 2];
		out[j++] = BASE64_PAD;
	}

	return BASE64_OK;
}

static int
base64_decode(const char *in, unsigned int inlen, unsigned char *out)
{
	unsigned int i, j;

	for (i = j = 0; i < inlen; i++) {
		int c;
		int s = i % 4; 			/* from 8/gcd(6, 8) */

		if (in[i] == '=')
			return BASE64_OK;

		if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST ||
			(c = base64de[in[i] - BASE64DE_FIRST]) == -1)
			return BASE64_INVALID;

		switch (s) {
		case 0:
			out[j] = ((unsigned int)c << 2) & 0xFF;
			continue;
		case 1:
			out[j++] += ((unsigned int)c >> 4) & 0x3;

			/* if not last char with padding */
			if (i < (inlen - 3) || in[inlen - 2] != '=')
				out[j] = ((unsigned int)c & 0xF) << 4;
			continue;
		case 2:
			out[j++] += ((unsigned int)c >> 2) & 0xF;

			/* if not last char with padding */
			if (i < (inlen - 2) || in[inlen - 1] != '=')
				out[j] = ((unsigned int)c & 0x3) << 6;
			continue;
		case 3:
			out[j++] += (unsigned char)c;
		}
	}

	return BASE64_OK;
}




class _encoding
{
	typedef unsigned long size_t;
	typedef _encoding	  type_t;
	string  m_error;
public:


	const string& what(){ return m_error;}
	_encoding(){throw std::exception();};

	_encoding(const char* data) 
	{
		m_int.assign(data);
	}

	_encoding(const wchar_t* data)
	{
		m_int.assign((const char*)data, wcslen(data) * sizeof(wchar_t));
	}

	_encoding(const char* data, size_t sz) 
	{
		m_int.assign((const char*)data, sz);
	}

	_encoding(const wchar_t* data, size_t sz) 
	{
		m_int.assign((const char*)data, sz * sizeof(wchar_t));
	}


	_encoding(const string& str)
	{
		m_int = str;
	}

	_encoding(const wstring& str)
	{
		m_int.assign((const char*)str.c_str(), str.size() * sizeof(wchar_t));
	}





	~_encoding()
	{
		m_int.clear();
		//m_out.clear();
	}

private:
	_encoding& MultiBytes(int cp = CP_UTF8)
	{
		int charactersNeeded = WideCharToMultiByte(cp, 0, (LPCWSTR)m_int.c_str(), m_int.size() / sizeof(wchar_t), NULL, 0, NULL, NULL);

		m_out.resize(charactersNeeded);

		int nRet = WideCharToMultiByte(cp, NULL, (LPCWSTR)m_int.c_str(), m_int.size() / sizeof(wchar_t), (LPSTR)m_out.c_str(), charactersNeeded, NULL, NULL);

		return *this;
	}


	_encoding& WideChar(int cp = CP_UTF8)
	{
		int charactersNeeded = MultiByteToWideChar(cp, 0, (LPCSTR)m_int.c_str(), m_int.size(), NULL, 0 );

		wstring tmp;
		tmp.resize(charactersNeeded * 2);
	

		int nRet = MultiByteToWideChar(cp, NULL, (LPCSTR)m_int.c_str(), m_int.size(), (LPWSTR)tmp.c_str(), charactersNeeded);

		m_out.assign((LPSTR)tmp.c_str(), tmp.size() * 2 + 2);

	

		return *this;
	}

	unsigned char charof(WORD w)
	{

		if(w == *(WORD*)"00") return 0x00;if(w == *(WORD*)"01") return 0x01;if(w == *(WORD*)"02") return 0x02;if(w == *(WORD*)"03") return 0x03;if(w == *(WORD*)"04") return 0x04;if(w == *(WORD*)"05") return 0x05;if(w == *(WORD*)"06") return 0x06;if(w == *(WORD*)"07") return 0x07;
		if(w == *(WORD*)"08") return 0x08;if(w == *(WORD*)"09") return 0x09;if(w == *(WORD*)"0a") return 0x0a;if(w == *(WORD*)"0b") return 0x0b;if(w == *(WORD*)"0c") return 0x0c;if(w == *(WORD*)"0d") return 0x0d;if(w == *(WORD*)"0e") return 0x0e;if(w == *(WORD*)"0f") return 0x0f;
		if(w == *(WORD*)"10") return 0x10;if(w == *(WORD*)"11") return 0x11;if(w == *(WORD*)"12") return 0x12;if(w == *(WORD*)"13") return 0x13;if(w == *(WORD*)"14") return 0x14;if(w == *(WORD*)"15") return 0x15;if(w == *(WORD*)"16") return 0x16;if(w == *(WORD*)"17") return 0x17;
		if(w == *(WORD*)"18") return 0x18;if(w == *(WORD*)"19") return 0x19;if(w == *(WORD*)"1a") return 0x1a;if(w == *(WORD*)"1b") return 0x1b;if(w == *(WORD*)"1c") return 0x1c;if(w == *(WORD*)"1d") return 0x1d;if(w == *(WORD*)"1e") return 0x1e;if(w == *(WORD*)"1f") return 0x1f;
		if(w == *(WORD*)"20") return 0x20;if(w == *(WORD*)"21") return 0x21;if(w == *(WORD*)"22") return 0x22;if(w == *(WORD*)"23") return 0x23;if(w == *(WORD*)"24") return 0x24;if(w == *(WORD*)"25") return 0x25;if(w == *(WORD*)"26") return 0x26;if(w == *(WORD*)"27") return 0x27;
		if(w == *(WORD*)"28") return 0x28;if(w == *(WORD*)"29") return 0x29;if(w == *(WORD*)"2a") return 0x2a;if(w == *(WORD*)"2b") return 0x2b;if(w == *(WORD*)"2c") return 0x2c;if(w == *(WORD*)"2d") return 0x2d;if(w == *(WORD*)"2e") return 0x2e;if(w == *(WORD*)"2f") return 0x2f;
		if(w == *(WORD*)"30") return 0x30;if(w == *(WORD*)"31") return 0x31;if(w == *(WORD*)"32") return 0x32;if(w == *(WORD*)"33") return 0x33;if(w == *(WORD*)"34") return 0x34;if(w == *(WORD*)"35") return 0x35;if(w == *(WORD*)"36") return 0x36;if(w == *(WORD*)"37") return 0x37;
		if(w == *(WORD*)"38") return 0x38;if(w == *(WORD*)"39") return 0x39;if(w == *(WORD*)"3a") return 0x3a;if(w == *(WORD*)"3b") return 0x3b;if(w == *(WORD*)"3c") return 0x3c;if(w == *(WORD*)"3d") return 0x3d;if(w == *(WORD*)"3e") return 0x3e;if(w == *(WORD*)"3f") return 0x3f;
		if(w == *(WORD*)"40") return 0x40;if(w == *(WORD*)"41") return 0x41;if(w == *(WORD*)"42") return 0x42;if(w == *(WORD*)"43") return 0x43;if(w == *(WORD*)"44") return 0x44;if(w == *(WORD*)"45") return 0x45;if(w == *(WORD*)"46") return 0x46;if(w == *(WORD*)"47") return 0x47;
		if(w == *(WORD*)"48") return 0x48;if(w == *(WORD*)"49") return 0x49;if(w == *(WORD*)"4a") return 0x4a;if(w == *(WORD*)"4b") return 0x4b;if(w == *(WORD*)"4c") return 0x4c;if(w == *(WORD*)"4d") return 0x4d;if(w == *(WORD*)"4e") return 0x4e;if(w == *(WORD*)"4f") return 0x4f;
		if(w == *(WORD*)"50") return 0x50;if(w == *(WORD*)"51") return 0x51;if(w == *(WORD*)"52") return 0x52;if(w == *(WORD*)"53") return 0x53;if(w == *(WORD*)"54") return 0x54;if(w == *(WORD*)"55") return 0x55;if(w == *(WORD*)"56") return 0x56;if(w == *(WORD*)"57") return 0x57;
		if(w == *(WORD*)"58") return 0x58;if(w == *(WORD*)"59") return 0x59;if(w == *(WORD*)"5a") return 0x5a;if(w == *(WORD*)"5b") return 0x5b;if(w == *(WORD*)"5c") return 0x5c;if(w == *(WORD*)"5d") return 0x5d;if(w == *(WORD*)"5e") return 0x5e;if(w == *(WORD*)"5f") return 0x5f;
		if(w == *(WORD*)"60") return 0x60;if(w == *(WORD*)"61") return 0x61;if(w == *(WORD*)"62") return 0x62;if(w == *(WORD*)"63") return 0x63;if(w == *(WORD*)"64") return 0x64;if(w == *(WORD*)"65") return 0x65;if(w == *(WORD*)"66") return 0x66;if(w == *(WORD*)"67") return 0x67;
		if(w == *(WORD*)"68") return 0x68;if(w == *(WORD*)"69") return 0x69;if(w == *(WORD*)"6a") return 0x6a;if(w == *(WORD*)"6b") return 0x6b;if(w == *(WORD*)"6c") return 0x6c;if(w == *(WORD*)"6d") return 0x6d;if(w == *(WORD*)"6e") return 0x6e;if(w == *(WORD*)"6f") return 0x6f;
		if(w == *(WORD*)"70") return 0x70;if(w == *(WORD*)"71") return 0x71;if(w == *(WORD*)"72") return 0x72;if(w == *(WORD*)"73") return 0x73;if(w == *(WORD*)"74") return 0x74;if(w == *(WORD*)"75") return 0x75;if(w == *(WORD*)"76") return 0x76;if(w == *(WORD*)"77") return 0x77;
		if(w == *(WORD*)"78") return 0x78;if(w == *(WORD*)"79") return 0x79;if(w == *(WORD*)"7a") return 0x7a;if(w == *(WORD*)"7b") return 0x7b;if(w == *(WORD*)"7c") return 0x7c;if(w == *(WORD*)"7d") return 0x7d;if(w == *(WORD*)"7e") return 0x7e;if(w == *(WORD*)"7f") return 0x7f;
		if(w == *(WORD*)"80") return 0x80;if(w == *(WORD*)"81") return 0x81;if(w == *(WORD*)"82") return 0x82;if(w == *(WORD*)"83") return 0x83;if(w == *(WORD*)"84") return 0x84;if(w == *(WORD*)"85") return 0x85;if(w == *(WORD*)"86") return 0x86;if(w == *(WORD*)"87") return 0x87;
		if(w == *(WORD*)"88") return 0x88;if(w == *(WORD*)"89") return 0x89;if(w == *(WORD*)"8a") return 0x8a;if(w == *(WORD*)"8b") return 0x8b;if(w == *(WORD*)"8c") return 0x8c;if(w == *(WORD*)"8d") return 0x8d;if(w == *(WORD*)"8e") return 0x8e;if(w == *(WORD*)"8f") return 0x8f;
		if(w == *(WORD*)"90") return 0x90;if(w == *(WORD*)"91") return 0x91;if(w == *(WORD*)"92") return 0x92;if(w == *(WORD*)"93") return 0x93;if(w == *(WORD*)"94") return 0x94;if(w == *(WORD*)"95") return 0x95;if(w == *(WORD*)"96") return 0x96;if(w == *(WORD*)"97") return 0x97;
		if(w == *(WORD*)"98") return 0x98;if(w == *(WORD*)"99") return 0x99;if(w == *(WORD*)"9a") return 0x9a;if(w == *(WORD*)"9b") return 0x9b;if(w == *(WORD*)"9c") return 0x9c;if(w == *(WORD*)"9d") return 0x9d;if(w == *(WORD*)"9e") return 0x9e;if(w == *(WORD*)"9f") return 0x9f;
		if(w == *(WORD*)"a0") return 0xa0;if(w == *(WORD*)"a1") return 0xa1;if(w == *(WORD*)"a2") return 0xa2;if(w == *(WORD*)"a3") return 0xa3;if(w == *(WORD*)"a4") return 0xa4;if(w == *(WORD*)"a5") return 0xa5;if(w == *(WORD*)"a6") return 0xa6;if(w == *(WORD*)"a7") return 0xa7;
		if(w == *(WORD*)"a8") return 0xa8;if(w == *(WORD*)"a9") return 0xa9;if(w == *(WORD*)"aa") return 0xaa;if(w == *(WORD*)"ab") return 0xab;if(w == *(WORD*)"ac") return 0xac;if(w == *(WORD*)"ad") return 0xad;if(w == *(WORD*)"ae") return 0xae;if(w == *(WORD*)"af") return 0xaf;
		if(w == *(WORD*)"b0") return 0xb0;if(w == *(WORD*)"b1") return 0xb1;if(w == *(WORD*)"b2") return 0xb2;if(w == *(WORD*)"b3") return 0xb3;if(w == *(WORD*)"b4") return 0xb4;if(w == *(WORD*)"b5") return 0xb5;if(w == *(WORD*)"b6") return 0xb6;if(w == *(WORD*)"b7") return 0xb7;
		if(w == *(WORD*)"b8") return 0xb8;if(w == *(WORD*)"b9") return 0xb9;if(w == *(WORD*)"ba") return 0xba;if(w == *(WORD*)"bb") return 0xbb;if(w == *(WORD*)"bc") return 0xbc;if(w == *(WORD*)"bd") return 0xbd;if(w == *(WORD*)"be") return 0xbe;if(w == *(WORD*)"bf") return 0xbf;
		if(w == *(WORD*)"c0") return 0xc0;if(w == *(WORD*)"c1") return 0xc1;if(w == *(WORD*)"c2") return 0xc2;if(w == *(WORD*)"c3") return 0xc3;if(w == *(WORD*)"c4") return 0xc4;if(w == *(WORD*)"c5") return 0xc5;if(w == *(WORD*)"c6") return 0xc6;if(w == *(WORD*)"c7") return 0xc7;
		if(w == *(WORD*)"c8") return 0xc8;if(w == *(WORD*)"c9") return 0xc9;if(w == *(WORD*)"ca") return 0xca;if(w == *(WORD*)"cb") return 0xcb;if(w == *(WORD*)"cc") return 0xcc;if(w == *(WORD*)"cd") return 0xcd;if(w == *(WORD*)"ce") return 0xce;if(w == *(WORD*)"cf") return 0xcf;
		if(w == *(WORD*)"d0") return 0xd0;if(w == *(WORD*)"d1") return 0xd1;if(w == *(WORD*)"d2") return 0xd2;if(w == *(WORD*)"d3") return 0xd3;if(w == *(WORD*)"d4") return 0xd4;if(w == *(WORD*)"d5") return 0xd5;if(w == *(WORD*)"d6") return 0xd6;if(w == *(WORD*)"d7") return 0xd7;
		if(w == *(WORD*)"d8") return 0xd8;if(w == *(WORD*)"d9") return 0xd9;if(w == *(WORD*)"da") return 0xda;if(w == *(WORD*)"db") return 0xdb;if(w == *(WORD*)"dc") return 0xdc;if(w == *(WORD*)"dd") return 0xdd;if(w == *(WORD*)"de") return 0xde;if(w == *(WORD*)"df") return 0xdf;
		if(w == *(WORD*)"e0") return 0xe0;if(w == *(WORD*)"e1") return 0xe1;if(w == *(WORD*)"e2") return 0xe2;if(w == *(WORD*)"e3") return 0xe3;if(w == *(WORD*)"e4") return 0xe4;if(w == *(WORD*)"e5") return 0xe5;if(w == *(WORD*)"e6") return 0xe6;if(w == *(WORD*)"e7") return 0xe7;
		if(w == *(WORD*)"e8") return 0xe8;if(w == *(WORD*)"e9") return 0xe9;if(w == *(WORD*)"ea") return 0xea;if(w == *(WORD*)"eb") return 0xeb;if(w == *(WORD*)"ec") return 0xec;if(w == *(WORD*)"ed") return 0xed;if(w == *(WORD*)"ee") return 0xee;if(w == *(WORD*)"ef") return 0xef;
		if(w == *(WORD*)"f0") return 0xf0;if(w == *(WORD*)"f1") return 0xf1;if(w == *(WORD*)"f2") return 0xf2;if(w == *(WORD*)"f3") return 0xf3;if(w == *(WORD*)"f4") return 0xf4;if(w == *(WORD*)"f5") return 0xf5;if(w == *(WORD*)"f6") return 0xf6;if(w == *(WORD*)"f7") return 0xf7;
		if(w == *(WORD*)"f8") return 0xf8;if(w == *(WORD*)"f9") return 0xf9;if(w == *(WORD*)"fa") return 0xfa;if(w == *(WORD*)"fb") return 0xfb;if(w == *(WORD*)"fc") return 0xfc;if(w == *(WORD*)"fd") return 0xfd;if(w == *(WORD*)"fe") return 0xfe;if(w == *(WORD*)"ff") return 0xff;

		if(w== *(WORD*)"00") return 0X00;if(w== *(WORD*)"01") return 0X01;if(w== *(WORD*)"02") return 0X02;if(w== *(WORD*)"03") return 0X03;if(w== *(WORD*)"04") return 0X04;if(w== *(WORD*)"05") return 0X05;if(w== *(WORD*)"06") return 0X06;if(w== *(WORD*)"07") return 0X07;
		if(w== *(WORD*)"08") return 0X08;if(w== *(WORD*)"09") return 0X09;if(w== *(WORD*)"0A") return 0X0A;if(w== *(WORD*)"0B") return 0X0B;if(w== *(WORD*)"0C") return 0X0C;if(w== *(WORD*)"0D") return 0X0D;if(w== *(WORD*)"0E") return 0X0E;if(w== *(WORD*)"0F") return 0X0F;
		if(w== *(WORD*)"10") return 0X10;if(w== *(WORD*)"11") return 0X11;if(w== *(WORD*)"12") return 0X12;if(w== *(WORD*)"13") return 0X13;if(w== *(WORD*)"14") return 0X14;if(w== *(WORD*)"15") return 0X15;if(w== *(WORD*)"16") return 0X16;if(w== *(WORD*)"17") return 0X17;
		if(w== *(WORD*)"18") return 0X18;if(w== *(WORD*)"19") return 0X19;if(w== *(WORD*)"1A") return 0X1A;if(w== *(WORD*)"1B") return 0X1B;if(w== *(WORD*)"1C") return 0X1C;if(w== *(WORD*)"1D") return 0X1D;if(w== *(WORD*)"1E") return 0X1E;if(w== *(WORD*)"1F") return 0X1F;
		if(w== *(WORD*)"20") return 0X20;if(w== *(WORD*)"21") return 0X21;if(w== *(WORD*)"22") return 0X22;if(w== *(WORD*)"23") return 0X23;if(w== *(WORD*)"24") return 0X24;if(w== *(WORD*)"25") return 0X25;if(w== *(WORD*)"26") return 0X26;if(w== *(WORD*)"27") return 0X27;
		if(w== *(WORD*)"28") return 0X28;if(w== *(WORD*)"29") return 0X29;if(w== *(WORD*)"2A") return 0X2A;if(w== *(WORD*)"2B") return 0X2B;if(w== *(WORD*)"2C") return 0X2C;if(w== *(WORD*)"2D") return 0X2D;if(w== *(WORD*)"2E") return 0X2E;if(w== *(WORD*)"2F") return 0X2F;
		if(w== *(WORD*)"30") return 0X30;if(w== *(WORD*)"31") return 0X31;if(w== *(WORD*)"32") return 0X32;if(w== *(WORD*)"33") return 0X33;if(w== *(WORD*)"34") return 0X34;if(w== *(WORD*)"35") return 0X35;if(w== *(WORD*)"36") return 0X36;if(w== *(WORD*)"37") return 0X37;
		if(w== *(WORD*)"38") return 0X38;if(w== *(WORD*)"39") return 0X39;if(w== *(WORD*)"3A") return 0X3A;if(w== *(WORD*)"3B") return 0X3B;if(w== *(WORD*)"3C") return 0X3C;if(w== *(WORD*)"3D") return 0X3D;if(w== *(WORD*)"3E") return 0X3E;if(w== *(WORD*)"3F") return 0X3F;
		if(w== *(WORD*)"40") return 0X40;if(w== *(WORD*)"41") return 0X41;if(w== *(WORD*)"42") return 0X42;if(w== *(WORD*)"43") return 0X43;if(w== *(WORD*)"44") return 0X44;if(w== *(WORD*)"45") return 0X45;if(w== *(WORD*)"46") return 0X46;if(w== *(WORD*)"47") return 0X47;
		if(w== *(WORD*)"48") return 0X48;if(w== *(WORD*)"49") return 0X49;if(w== *(WORD*)"4A") return 0X4A;if(w== *(WORD*)"4B") return 0X4B;if(w== *(WORD*)"4C") return 0X4C;if(w== *(WORD*)"4D") return 0X4D;if(w== *(WORD*)"4E") return 0X4E;if(w== *(WORD*)"4F") return 0X4F;
		if(w== *(WORD*)"50") return 0X50;if(w== *(WORD*)"51") return 0X51;if(w== *(WORD*)"52") return 0X52;if(w== *(WORD*)"53") return 0X53;if(w== *(WORD*)"54") return 0X54;if(w== *(WORD*)"55") return 0X55;if(w== *(WORD*)"56") return 0X56;if(w== *(WORD*)"57") return 0X57;
		if(w== *(WORD*)"58") return 0X58;if(w== *(WORD*)"59") return 0X59;if(w== *(WORD*)"5A") return 0X5A;if(w== *(WORD*)"5B") return 0X5B;if(w== *(WORD*)"5C") return 0X5C;if(w== *(WORD*)"5D") return 0X5D;if(w== *(WORD*)"5E") return 0X5E;if(w== *(WORD*)"5F") return 0X5F;
		if(w== *(WORD*)"60") return 0X60;if(w== *(WORD*)"61") return 0X61;if(w== *(WORD*)"62") return 0X62;if(w== *(WORD*)"63") return 0X63;if(w== *(WORD*)"64") return 0X64;if(w== *(WORD*)"65") return 0X65;if(w== *(WORD*)"66") return 0X66;if(w== *(WORD*)"67") return 0X67;
		if(w== *(WORD*)"68") return 0X68;if(w== *(WORD*)"69") return 0X69;if(w== *(WORD*)"6A") return 0X6A;if(w== *(WORD*)"6B") return 0X6B;if(w== *(WORD*)"6C") return 0X6C;if(w== *(WORD*)"6D") return 0X6D;if(w== *(WORD*)"6E") return 0X6E;if(w== *(WORD*)"6F") return 0X6F;
		if(w== *(WORD*)"70") return 0X70;if(w== *(WORD*)"71") return 0X71;if(w== *(WORD*)"72") return 0X72;if(w== *(WORD*)"73") return 0X73;if(w== *(WORD*)"74") return 0X74;if(w== *(WORD*)"75") return 0X75;if(w== *(WORD*)"76") return 0X76;if(w== *(WORD*)"77") return 0X77;
		if(w== *(WORD*)"78") return 0X78;if(w== *(WORD*)"79") return 0X79;if(w== *(WORD*)"7A") return 0X7A;if(w== *(WORD*)"7B") return 0X7B;if(w== *(WORD*)"7C") return 0X7C;if(w== *(WORD*)"7D") return 0X7D;if(w== *(WORD*)"7E") return 0X7E;if(w== *(WORD*)"7F") return 0X7F;
		if(w== *(WORD*)"80") return 0X80;if(w== *(WORD*)"81") return 0X81;if(w== *(WORD*)"82") return 0X82;if(w== *(WORD*)"83") return 0X83;if(w== *(WORD*)"84") return 0X84;if(w== *(WORD*)"85") return 0X85;if(w== *(WORD*)"86") return 0X86;if(w== *(WORD*)"87") return 0X87;
		if(w== *(WORD*)"88") return 0X88;if(w== *(WORD*)"89") return 0X89;if(w== *(WORD*)"8A") return 0X8A;if(w== *(WORD*)"8B") return 0X8B;if(w== *(WORD*)"8C") return 0X8C;if(w== *(WORD*)"8D") return 0X8D;if(w== *(WORD*)"8E") return 0X8E;if(w== *(WORD*)"8F") return 0X8F;
		if(w== *(WORD*)"90") return 0X90;if(w== *(WORD*)"91") return 0X91;if(w== *(WORD*)"92") return 0X92;if(w== *(WORD*)"93") return 0X93;if(w== *(WORD*)"94") return 0X94;if(w== *(WORD*)"95") return 0X95;if(w== *(WORD*)"96") return 0X96;if(w== *(WORD*)"97") return 0X97;
		if(w== *(WORD*)"98") return 0X98;if(w== *(WORD*)"99") return 0X99;if(w== *(WORD*)"9A") return 0X9A;if(w== *(WORD*)"9B") return 0X9B;if(w== *(WORD*)"9C") return 0X9C;if(w== *(WORD*)"9D") return 0X9D;if(w== *(WORD*)"9E") return 0X9E;if(w== *(WORD*)"9F") return 0X9F;
		if(w== *(WORD*)"A0") return 0XA0;if(w== *(WORD*)"A1") return 0XA1;if(w== *(WORD*)"A2") return 0XA2;if(w== *(WORD*)"A3") return 0XA3;if(w== *(WORD*)"A4") return 0XA4;if(w== *(WORD*)"A5") return 0XA5;if(w== *(WORD*)"A6") return 0XA6;if(w== *(WORD*)"A7") return 0XA7;
		if(w== *(WORD*)"A8") return 0XA8;if(w== *(WORD*)"A9") return 0XA9;if(w== *(WORD*)"AA") return 0XAA;if(w== *(WORD*)"AB") return 0XAB;if(w== *(WORD*)"AC") return 0XAC;if(w== *(WORD*)"AD") return 0XAD;if(w== *(WORD*)"AE") return 0XAE;if(w== *(WORD*)"AF") return 0XAF;
		if(w== *(WORD*)"B0") return 0XB0;if(w== *(WORD*)"B1") return 0XB1;if(w== *(WORD*)"B2") return 0XB2;if(w== *(WORD*)"B3") return 0XB3;if(w== *(WORD*)"B4") return 0XB4;if(w== *(WORD*)"B5") return 0XB5;if(w== *(WORD*)"B6") return 0XB6;if(w== *(WORD*)"B7") return 0XB7;
		if(w== *(WORD*)"B8") return 0XB8;if(w== *(WORD*)"B9") return 0XB9;if(w== *(WORD*)"BA") return 0XBA;if(w== *(WORD*)"BB") return 0XBB;if(w== *(WORD*)"BC") return 0XBC;if(w== *(WORD*)"BD") return 0XBD;if(w== *(WORD*)"BE") return 0XBE;if(w== *(WORD*)"BF") return 0XBF;
		if(w== *(WORD*)"C0") return 0XC0;if(w== *(WORD*)"C1") return 0XC1;if(w== *(WORD*)"C2") return 0XC2;if(w== *(WORD*)"C3") return 0XC3;if(w== *(WORD*)"C4") return 0XC4;if(w== *(WORD*)"C5") return 0XC5;if(w== *(WORD*)"C6") return 0XC6;if(w== *(WORD*)"C7") return 0XC7;
		if(w== *(WORD*)"C8") return 0XC8;if(w== *(WORD*)"C9") return 0XC9;if(w== *(WORD*)"CA") return 0XCA;if(w== *(WORD*)"CB") return 0XCB;if(w== *(WORD*)"CC") return 0XCC;if(w== *(WORD*)"CD") return 0XCD;if(w== *(WORD*)"CE") return 0XCE;if(w== *(WORD*)"CF") return 0XCF;
		if(w== *(WORD*)"D0") return 0XD0;if(w== *(WORD*)"D1") return 0XD1;if(w== *(WORD*)"D2") return 0XD2;if(w== *(WORD*)"D3") return 0XD3;if(w== *(WORD*)"D4") return 0XD4;if(w== *(WORD*)"D5") return 0XD5;if(w== *(WORD*)"D6") return 0XD6;if(w== *(WORD*)"D7") return 0XD7;
		if(w== *(WORD*)"D8") return 0XD8;if(w== *(WORD*)"D9") return 0XD9;if(w== *(WORD*)"DA") return 0XDA;if(w== *(WORD*)"DB") return 0XDB;if(w== *(WORD*)"DC") return 0XDC;if(w== *(WORD*)"DD") return 0XDD;if(w== *(WORD*)"DE") return 0XDE;if(w== *(WORD*)"DF") return 0XDF;
		if(w== *(WORD*)"E0") return 0XE0;if(w== *(WORD*)"E1") return 0XE1;if(w== *(WORD*)"E2") return 0XE2;if(w== *(WORD*)"E3") return 0XE3;if(w== *(WORD*)"E4") return 0XE4;if(w== *(WORD*)"E5") return 0XE5;if(w== *(WORD*)"E6") return 0XE6;if(w== *(WORD*)"E7") return 0XE7;
		if(w== *(WORD*)"E8") return 0XE8;if(w== *(WORD*)"E9") return 0XE9;if(w== *(WORD*)"EA") return 0XEA;if(w== *(WORD*)"EB") return 0XEB;if(w== *(WORD*)"EC") return 0XEC;if(w== *(WORD*)"ED") return 0XED;if(w== *(WORD*)"EE") return 0XEE;if(w== *(WORD*)"EF") return 0XEF;
		if(w== *(WORD*)"F0") return 0XF0;if(w== *(WORD*)"F1") return 0XF1;if(w== *(WORD*)"F2") return 0XF2;if(w== *(WORD*)"F3") return 0XF3;if(w== *(WORD*)"F4") return 0XF4;if(w== *(WORD*)"F5") return 0XF5;if(w== *(WORD*)"F6") return 0XF6;if(w== *(WORD*)"F7") return 0XF7;
		if(w== *(WORD*)"F8") return 0XF8;if(w== *(WORD*)"F9") return 0XF9;if(w== *(WORD*)"FA") return 0XFA;if(w== *(WORD*)"FB") return 0XFB;if(w== *(WORD*)"FC") return 0XFC;if(w== *(WORD*)"FD") return 0XFD;if(w== *(WORD*)"FE") return 0XFE;if(w== *(WORD*)"FF") return 0XFF;

		return 00;
	}

	const char* _hexof(unsigned char ch )
	{
		if(ch == 0x00) return "00";if(ch == 0x01) return "01";if(ch == 0x02) return "02";if(ch == 0x03) return "03";if(ch == 0x04) return "04";if(ch == 0x05) return "05";if(ch == 0x06) return "06";if(ch == 0x07) return "07";
		if(ch == 0x08) return "08";if(ch == 0x09) return "09";if(ch == 0x0a) return "0a";if(ch == 0x0b) return "0b";if(ch == 0x0c) return "0c";if(ch == 0x0d) return "0d";if(ch == 0x0e) return "0e";if(ch == 0x0f) return "0f";
		if(ch == 0x10) return "10";if(ch == 0x11) return "11";if(ch == 0x12) return "12";if(ch == 0x13) return "13";if(ch == 0x14) return "14";if(ch == 0x15) return "15";if(ch == 0x16) return "16";if(ch == 0x17) return "17";
		if(ch == 0x18) return "18";if(ch == 0x19) return "19";if(ch == 0x1a) return "1a";if(ch == 0x1b) return "1b";if(ch == 0x1c) return "1c";if(ch == 0x1d) return "1d";if(ch == 0x1e) return "1e";if(ch == 0x1f) return "1f";
		if(ch == 0x20) return "20";if(ch == 0x21) return "21";if(ch == 0x22) return "22";if(ch == 0x23) return "23";if(ch == 0x24) return "24";if(ch == 0x25) return "25";if(ch == 0x26) return "26";if(ch == 0x27) return "27";
		if(ch == 0x28) return "28";if(ch == 0x29) return "29";if(ch == 0x2a) return "2a";if(ch == 0x2b) return "2b";if(ch == 0x2c) return "2c";if(ch == 0x2d) return "2d";if(ch == 0x2e) return "2e";if(ch == 0x2f) return "2f";
		if(ch == 0x30) return "30";if(ch == 0x31) return "31";if(ch == 0x32) return "32";if(ch == 0x33) return "33";if(ch == 0x34) return "34";if(ch == 0x35) return "35";if(ch == 0x36) return "36";if(ch == 0x37) return "37";
		if(ch == 0x38) return "38";if(ch == 0x39) return "39";if(ch == 0x3a) return "3a";if(ch == 0x3b) return "3b";if(ch == 0x3c) return "3c";if(ch == 0x3d) return "3d";if(ch == 0x3e) return "3e";if(ch == 0x3f) return "3f";
		if(ch == 0x40) return "40";if(ch == 0x41) return "41";if(ch == 0x42) return "42";if(ch == 0x43) return "43";if(ch == 0x44) return "44";if(ch == 0x45) return "45";if(ch == 0x46) return "46";if(ch == 0x47) return "47";
		if(ch == 0x48) return "48";if(ch == 0x49) return "49";if(ch == 0x4a) return "4a";if(ch == 0x4b) return "4b";if(ch == 0x4c) return "4c";if(ch == 0x4d) return "4d";if(ch == 0x4e) return "4e";if(ch == 0x4f) return "4f";
		if(ch == 0x50) return "50";if(ch == 0x51) return "51";if(ch == 0x52) return "52";if(ch == 0x53) return "53";if(ch == 0x54) return "54";if(ch == 0x55) return "55";if(ch == 0x56) return "56";if(ch == 0x57) return "57";
		if(ch == 0x58) return "58";if(ch == 0x59) return "59";if(ch == 0x5a) return "5a";if(ch == 0x5b) return "5b";if(ch == 0x5c) return "5c";if(ch == 0x5d) return "5d";if(ch == 0x5e) return "5e";if(ch == 0x5f) return "5f";
		if(ch == 0x60) return "60";if(ch == 0x61) return "61";if(ch == 0x62) return "62";if(ch == 0x63) return "63";if(ch == 0x64) return "64";if(ch == 0x65) return "65";if(ch == 0x66) return "66";if(ch == 0x67) return "67";
		if(ch == 0x68) return "68";if(ch == 0x69) return "69";if(ch == 0x6a) return "6a";if(ch == 0x6b) return "6b";if(ch == 0x6c) return "6c";if(ch == 0x6d) return "6d";if(ch == 0x6e) return "6e";if(ch == 0x6f) return "6f";
		if(ch == 0x70) return "70";if(ch == 0x71) return "71";if(ch == 0x72) return "72";if(ch == 0x73) return "73";if(ch == 0x74) return "74";if(ch == 0x75) return "75";if(ch == 0x76) return "76";if(ch == 0x77) return "77";
		if(ch == 0x78) return "78";if(ch == 0x79) return "79";if(ch == 0x7a) return "7a";if(ch == 0x7b) return "7b";if(ch == 0x7c) return "7c";if(ch == 0x7d) return "7d";if(ch == 0x7e) return "7e";if(ch == 0x7f) return "7f";
		if(ch == 0x80) return "80";if(ch == 0x81) return "81";if(ch == 0x82) return "82";if(ch == 0x83) return "83";if(ch == 0x84) return "84";if(ch == 0x85) return "85";if(ch == 0x86) return "86";if(ch == 0x87) return "87";
		if(ch == 0x88) return "88";if(ch == 0x89) return "89";if(ch == 0x8a) return "8a";if(ch == 0x8b) return "8b";if(ch == 0x8c) return "8c";if(ch == 0x8d) return "8d";if(ch == 0x8e) return "8e";if(ch == 0x8f) return "8f";
		if(ch == 0x90) return "90";if(ch == 0x91) return "91";if(ch == 0x92) return "92";if(ch == 0x93) return "93";if(ch == 0x94) return "94";if(ch == 0x95) return "95";if(ch == 0x96) return "96";if(ch == 0x97) return "97";
		if(ch == 0x98) return "98";if(ch == 0x99) return "99";if(ch == 0x9a) return "9a";if(ch == 0x9b) return "9b";if(ch == 0x9c) return "9c";if(ch == 0x9d) return "9d";if(ch == 0x9e) return "9e";if(ch == 0x9f) return "9f";
		if(ch == 0xa0) return "a0";if(ch == 0xa1) return "a1";if(ch == 0xa2) return "a2";if(ch == 0xa3) return "a3";if(ch == 0xa4) return "a4";if(ch == 0xa5) return "a5";if(ch == 0xa6) return "a6";if(ch == 0xa7) return "a7";
		if(ch == 0xa8) return "a8";if(ch == 0xa9) return "a9";if(ch == 0xaa) return "aa";if(ch == 0xab) return "ab";if(ch == 0xac) return "ac";if(ch == 0xad) return "ad";if(ch == 0xae) return "ae";if(ch == 0xaf) return "af";
		if(ch == 0xb0) return "b0";if(ch == 0xb1) return "b1";if(ch == 0xb2) return "b2";if(ch == 0xb3) return "b3";if(ch == 0xb4) return "b4";if(ch == 0xb5) return "b5";if(ch == 0xb6) return "b6";if(ch == 0xb7) return "b7";
		if(ch == 0xb8) return "b8";if(ch == 0xb9) return "b9";if(ch == 0xba) return "ba";if(ch == 0xbb) return "bb";if(ch == 0xbc) return "bc";if(ch == 0xbd) return "bd";if(ch == 0xbe) return "be";if(ch == 0xbf) return "bf";
		if(ch == 0xc0) return "c0";if(ch == 0xc1) return "c1";if(ch == 0xc2) return "c2";if(ch == 0xc3) return "c3";if(ch == 0xc4) return "c4";if(ch == 0xc5) return "c5";if(ch == 0xc6) return "c6";if(ch == 0xc7) return "c7";
		if(ch == 0xc8) return "c8";if(ch == 0xc9) return "c9";if(ch == 0xca) return "ca";if(ch == 0xcb) return "cb";if(ch == 0xcc) return "cc";if(ch == 0xcd) return "cd";if(ch == 0xce) return "ce";if(ch == 0xcf) return "cf";
		if(ch == 0xd0) return "d0";if(ch == 0xd1) return "d1";if(ch == 0xd2) return "d2";if(ch == 0xd3) return "d3";if(ch == 0xd4) return "d4";if(ch == 0xd5) return "d5";if(ch == 0xd6) return "d6";if(ch == 0xd7) return "d7";
		if(ch == 0xd8) return "d8";if(ch == 0xd9) return "d9";if(ch == 0xda) return "da";if(ch == 0xdb) return "db";if(ch == 0xdc) return "dc";if(ch == 0xdd) return "dd";if(ch == 0xde) return "de";if(ch == 0xdf) return "df";
		if(ch == 0xe0) return "e0";if(ch == 0xe1) return "e1";if(ch == 0xe2) return "e2";if(ch == 0xe3) return "e3";if(ch == 0xe4) return "e4";if(ch == 0xe5) return "e5";if(ch == 0xe6) return "e6";if(ch == 0xe7) return "e7";
		if(ch == 0xe8) return "e8";if(ch == 0xe9) return "e9";if(ch == 0xea) return "ea";if(ch == 0xeb) return "eb";if(ch == 0xec) return "ec";if(ch == 0xed) return "ed";if(ch == 0xee) return "ee";if(ch == 0xef) return "ef";
		if(ch == 0xf0) return "f0";if(ch == 0xf1) return "f1";if(ch == 0xf2) return "f2";if(ch == 0xf3) return "f3";if(ch == 0xf4) return "f4";if(ch == 0xf5) return "f5";if(ch == 0xf6) return "f6";if(ch == 0xf7) return "f7";
		if(ch == 0xf8) return "f8";if(ch == 0xf9) return "f9";if(ch == 0xfa) return "fa";if(ch == 0xfb) return "fb";if(ch == 0xfc) return "fc";if(ch == 0xfd) return "fd";if(ch == 0xfe) return "fe";if(ch == 0xff) return "ff";



		


		return "";

	}

public:

	_encoding& hex(bool bLower = true)
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out="";
		}


		char* fmt;
		if(bLower)
		{
			fmt = "%02x";
		}
		else
		{
			fmt = "%02X";
		}
		
		for (unsigned int i = 0; i < m_int.size(); ++i)
		{
			
			m_out.append(_hexof(m_int[i]));

		}

		return *this;
	}

	_encoding& hex_de()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out.clear();
		}

		if(m_int.size() % 2 != 0)
		{
			m_error = "invalid format";
			throw *this;
		}
		

		m_out.resize(m_int.size() / 2);
		for (unsigned int i = 0; i < m_int.size() / 2; i++)
		{
			m_out[i] = charof(*(WORD*)(m_int._Myptr() + i*2));
		}

		return *this;
	}


	_encoding& astr()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out = "";
		}


		return MultiBytes(CP_ACP);
	}


	_encoding& utf8()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out = "";
		}


		return MultiBytes(CP_UTF8);
	}

	_encoding&  a_utf16()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out.clear();
		}

		return WideChar(CP_ACP);
	}

	_encoding&  u8_utf16()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out.clear();
		}


		return WideChar(CP_UTF8);
	}


	_encoding&  en_base64()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out.clear();
		}

		int szNeeded = CEILING_POS((double)m_int.size() / 3);

		m_out._Grow(szNeeded*4);
		m_out.resize(szNeeded*4);

		base64_encode((const unsigned char*)m_int.c_str(), m_int.size(), m_out._Myptr());

		return *this;
	}

	_encoding&  de_base64()
	{
		if (m_out.size())
		{
			m_int = m_out;
			m_out.clear();
		}

		if (m_int.size())
		{
			
			int padding = 0;
			if (m_int.find("==") != -1)
			{
				padding = 2;
			}
			else if (m_int.find("=") != -1)
			{
				padding = 1;
			}

			int szNeeded = m_int.size()  * 3 / 4 - padding;
			if(szNeeded < 1)
			{
				return *this;
			}

			m_out._Grow(szNeeded);
			m_out.resize(szNeeded);

			base64_decode((const  char*)m_int.c_str(), m_int.size(), (unsigned char*)m_out._Myptr());
		}


		
		return *this;
	}





	string get()
	{
		return m_out;
	}

	wstring getutf16()
	{
		wstring tmp;
		tmp.assign((const wchar_t*)m_out.c_str(), m_out.size() / sizeof(wchar_t));
		return tmp;
	}

	

private: 

	string m_int;
	string m_out;
};

#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#pragma  comment(lib, "libeay32.lib")

class _digest
{
	typedef _digest _type;
	string m_algo;

public:
	_digest(const unsigned char* algo) 
	{
		m_algo.assign((const char*)algo);
	}

	_digest(const  char* d) 
	{
		m_algo.assign(d);
	}

	_digest(const  string& d) 
	{
		m_algo = d;
	}



	string digest(const unsigned char* msg, size_t msgsize)
	{
		const EVP_MD *d;
		EVP_MD_CTX ctx;
		unsigned char md[EVP_MAX_MD_SIZE];
		unsigned int mdn;

		d = EVP_get_digestbyname(m_algo.c_str());
		if (!d)
		{
			return "";
		}

		//printf("Testing digest %s\n", EVP_MD_name(d));


		EVP_MD_CTX_init(&ctx);


		if (!EVP_DigestInit_ex(&ctx, d, NULL)) {
			EVP_MD_CTX_cleanup(&ctx);
			return "";
		}
		if (!EVP_DigestUpdate(&ctx, msg, msgsize)) {

			EVP_MD_CTX_cleanup(&ctx);
			return "";
		}
		if (!EVP_DigestFinal_ex(&ctx, md, &mdn)) {


			EVP_MD_CTX_cleanup(&ctx);
			return "";
		}
		EVP_MD_CTX_cleanup(&ctx);



		string ret;
		ret.assign((const char*)md,mdn);

		return ret;
	}

	string digest(const  string& msg)
	{
		const EVP_MD *d;
		EVP_MD_CTX ctx;
		unsigned char md[EVP_MAX_MD_SIZE];
		unsigned int mdn;

		d = EVP_get_digestbyname(m_algo.c_str());
		if (!d)
		{
			return "";
		}

		//printf("Testing digest %s\n", EVP_MD_name(d));


		EVP_MD_CTX_init(&ctx);


		if (!EVP_DigestInit_ex(&ctx, d, NULL)) {
			EVP_MD_CTX_cleanup(&ctx);
			return "";
		}
		if (!EVP_DigestUpdate(&ctx, msg.c_str(), msg.size())) {

			EVP_MD_CTX_cleanup(&ctx);
			return "";
		}
		if (!EVP_DigestFinal_ex(&ctx, md, &mdn)) {


			EVP_MD_CTX_cleanup(&ctx);
			return "";
		}
		EVP_MD_CTX_cleanup(&ctx);



		string ret;
		ret.assign((const char*)md,mdn);

		return ret;

	}


	

};


class _crypto
{
public:

	enum _algo
	{
		algo_rsa = 1,
		algo_aes = 2,
		algo_trides = 3,
		algo_des   = 4
	};

	enum _keyformat
	{
		kf_pem = 1,
		kf_der = 2,
		kf_hexder = 3,
		kf_SubjectPublicKeyInfo = 4
	};

	typedef basic_string<unsigned char, char_traits<unsigned char>, allocator<unsigned char> >
		u8string;
private:
	typedef unsigned long size_t;
	typedef _crypto	  type_t;

	u8string m_int;
	u8string m_key;

	int    m_algo;
	int    m_scheme;
	string  m_out;
	string m_error;
	
	RSA*   m_rsa;

	int    m_rsapadding;
	int    m_keyformat;

	RSA * createRSA(unsigned char * key,int pub)
	{
		RSA *rsa= NULL;
		BIO *keybio ;
		keybio = BIO_new_mem_buf(key, -1);
		if (keybio==NULL)
		{
			m_error = "Failed to create key BIO";
			throw *this;
			
		}

		
		if(pub)
		{
			rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
		}
		else
		{
			rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
		}


		if(!rsa)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);
			throw *this;
		}


		return rsa;
	}

public:

	~_crypto(){m_int.clear(), m_out.clear();};


	_crypto(int algo = 1, int scheme = RSA_PKCS1_PADDING):m_algo(algo), m_scheme(scheme){ERR_load_RSA_strings();
	;}

	_crypto& data(const unsigned char* d) 
	{
		m_int.assign(d);
		return *this;
	}


	_crypto& data(const unsigned char* d, size_t sz)
	{
		m_int.assign(d, sz);
		return *this;
	}

	_crypto& data(const  char* d) 
	{
		m_int.assign((const unsigned char*)d);
		return *this;
	}

	_crypto& data(const  string& d) 
	{
		m_int.assign((const unsigned char*)d.c_str(),d.size());
		return *this;
	}

	_crypto& data(const  char* d, size_t sz)
	{
		m_int.assign(( const unsigned char*)d, sz);
		return *this;
	}

	_crypto& encrypt()
	{
		if(m_algo == algo_rsa)
		{
			rsa_en();
		}

		return *this;
	}

	_crypto& private_encrypt()/*用私钥加密*/
	{


		if(m_algo == algo_rsa)
		{
			rsa_priv_en();
		}

		return *this;
	}

	_crypto& public_decrypt()/*用公钥解密*/
	{


		if(m_algo == algo_rsa)
		{
			rsa_pub_de();
		}

		return *this;
	}

	_crypto& decrypt()
	{

		if(m_algo == algo_rsa)
		{
			rsa_de();
		}
	
		return *this;
	}




	_crypto& rsa_en()
	{
		

		RSA* rsa;
			
		if(m_keyformat == kf_pem)
		{
			rsa = createRSA((unsigned char*)m_key.c_str(), 1);
		}
		else if(m_keyformat == kf_der)
		{
			const unsigned char * pPublic = m_key._Myptr();
			rsa = d2i_RSA_PUBKEY(&rsa,  &pPublic, m_key.size() );
		}
		else
		{
			throw *this;
		}


		int sz = RSA_size(rsa);
		m_out.resize(sz);

		
		int iret = RSA_public_encrypt(m_int.size(),m_int.c_str(),(unsigned char*)m_out.c_str(),rsa,m_scheme);

		if(iret == -1)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);
			throw *this;

		}

		
		return *this;
	}
	_crypto& rsa_de()
	{

		extern unsigned char g_sz_rsa_private[2348];

		RSA* rsa;
		if(m_keyformat == kf_pem)
		{
			rsa = createRSA((unsigned char*)m_key.c_str(), 0);
		}
		else if(m_keyformat == kf_der)
		{
			const unsigned char * pPrivate = m_key._Myptr();
			 rsa = d2i_RSAPrivateKey(NULL, &pPrivate, m_key.size());
		}
		else if(m_keyformat == kf_hexder)
		{

			throw *this;
		}


		int sz = RSA_size(rsa);
		m_out.resize(sz);

		

		int iret = RSA_private_decrypt(m_int.size(),m_int.c_str(),(unsigned char*)m_out.c_str(),rsa,m_scheme);
		
		if(iret == -1)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);

		}

		return *this;
	}

	_crypto& rsa_priv_en()
	{


		RSA* rsa;

		if(m_keyformat == kf_pem)
		{
			rsa = createRSA((unsigned char*)m_key.c_str(), 0);
		}
		else if(m_keyformat == kf_der)
		{
			const unsigned char * pPrivate = (const unsigned char *)m_key._Myptr();
			rsa = d2i_RSAPrivateKey(NULL, &pPrivate, m_key.size());

		}
		else
		{
			throw *this;
		}


		int sz = RSA_size(rsa);
		m_out.resize(sz);


		int iret = RSA_private_encrypt(m_int.size(),m_int.c_str(),(unsigned char*)m_out.c_str(),rsa,m_scheme);

		if(iret == -1)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);
			throw *this;

		}


		return *this;
	}
	_crypto& rsa_pub_de()
	{

		extern unsigned char g_sz_rsa_private[2348];

		RSA* rsa;
		if(m_keyformat == kf_pem)//pkcs#1 format
		{
			rsa = createRSA((unsigned char*)m_key.c_str(), 1);
		}
		else if(m_keyformat == kf_der)//pkcs#1 format
		{
			const unsigned char * pPublic = (const unsigned char *)m_key._Myptr();
			rsa = d2i_RSAPublicKey(NULL,  &pPublic, m_key.size() );
		}
		else if(m_keyformat == kf_SubjectPublicKeyInfo  )
		{
			const unsigned char * pPublic = (const unsigned char *)m_key._Myptr();
			rsa = d2i_RSA_PUBKEY(&rsa,  &pPublic, m_key.size() );
		}
		else if(m_keyformat == kf_hexder)
		{

			throw *this;
		}

		if(!rsa)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);
			throw *this;
		}


		int sz = RSA_size(rsa);

		m_out.resize(sz);

		int iret = RSA_public_decrypt(m_int.size(),m_int.c_str(), (unsigned char*)m_out.c_str(),rsa,m_scheme);

		if(iret == -1)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);

		}

		return *this;
	}

	_crypto& key(const unsigned char* key, int keyformat = 1)
	{
		m_key = key;

		m_keyformat = keyformat;

		return *this;
	}


	_crypto& key(const string& key, int keyformat = 1)
	{
		m_key.assign((const unsigned char*)key.c_str(), key.size());

		m_keyformat = keyformat;

		return *this;
	}



	string get()
	{
		return m_out;
	}

	string what()
	{
		return m_error;
	}

	static void test()
	{
		
// 		 		string a = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
// 		 			"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
// 		 			"12345678901234567890123456789012345678901234567890123456";
// 		 		ifstream  keyfile;
// 		 		keyfile.open("public.pem");
// 		 		char pubkey[0x1000];
// 		 		keyfile.read(pubkey, 0x1000);
// 		 
// 		 		ifstream  keyfile2;
// 		 		keyfile2.open("private.pem");
// 		 		char prikey[0x1000];
// 		 		keyfile2.read(prikey, 0x1000);
// 		 
// 				//a nopadding 输入明文长度等于key长度
// 		 		string cipher = _crypto(a).pubkey(pubkey).rsa().get();
// 		 		string  plain = _crypto(cipher).prikey(prikey).rsa_de().get();
// 				//b pkcs1padding nopadding 输入明文长度小于等于key长度 - PKCS1_PADDING_SIZE(11)
// 				string cipher = _crypto(a).pubkey(pubkey).rsa().get();
// 				string  plain = _crypto(cipher).prikey(prikey).rsa_de().get();



	}


};


#ifndef RsaNone
#define RsaNone(VAR) _crypto (VAR)(1,RSA_NO_PADDING);
#endif


#ifndef RsaPkcs1
#define RsaPkcs1(VAR) _crypto (VAR)(1,RSA_PKCS1_PADDING);
#endif


#ifndef RsaOeap
#define RsaOeap(VAR) _crypto (VAR)(1,RSA_PKCS1_OAEP_PADDING);
#endif


#include <time.h>


enum
{
	__start_of_day = 1,
	__start_of_week ,
	__start_of_month,
	__start_of_year

};

#define tm_cast(x) ((struct tm*)x)
class _datetime
{
public:
	typedef unsigned long size_t;
	typedef _datetime	  type_t;
	time_t  m_time;
	struct tm  m_tm;
	string  m_result;
private:
	bool isDate(const string& str)
	{
		return true;
	}
	bool isDateTime(const string& str)
	{
		return true;
	}
	bool isTime(const string& str)
	{
		return true;
	}

public:

	_datetime(string& when)
	{
		string str = _re("[:-\/]").replace(when, "").replaceresult();

		if(stricmp(str.c_str(), "now") == 0)
		{
			m_time  = time(NULL); //unix stamp
			m_tm	= *gmtime(&m_time);
		}
	



	}

	_datetime()
	{	
			m_time  = time(NULL); //unix stamp
			m_tm	= *gmtime(&m_time);
	}


	_datetime& local()
	{
		
		m_tm = *localtime(&m_time);
		return *this;
	};

	operator struct tm*()
	{
		return &m_tm;
	};


	_datetime& year(int& tmyear)
	{
		tmyear =  m_tm.tm_year + 1900;
		return *this;
	}

	_datetime& month(int& tmmonth)
	{
		tmmonth =  m_tm.tm_mon;
		return *this;
	}

	_datetime& day(int& tmday)
	{
		tmday = m_tm.tm_mday;
		return *this;
	}

	_datetime& hour(int& tmhour)
	{
		tmhour =  m_tm.tm_hour;
		return *this;
	}

	_datetime& minute(int& tmmin)
	{
		tmmin =  m_tm.tm_min;
		return *this;
	}

	_datetime& second(int& tmsecond)
	{
		tmsecond =  m_tm.tm_sec;
		return *this;
	}



	_datetime& weekday(int& tmweekday)
	{
		tmweekday =  m_tm.tm_wday;
		return *this;
	}


	_datetime& yearday(int& tmyearday)
	{
		tmyearday =  m_tm.tm_yday;
		return *this;
	}



	_datetime& strtime()
	{
		m_result.resize(26);

		m_tm.tm_mon =11;
		asctime_s(m_result._Myptr(), 26, &m_tm);

		return *this;
	};


	const char* cstr()
	{
		return m_result.c_str();
	}




	static void test()
	{
		_datetime a;

		const char* res = a.local().strtime().cstr();
		int y = tm_cast(a)->tm_year;
	}

};




