#pragma  once

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
public:


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
		m_out.clear();
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

			int szNeeded = m_int.size() / 4 * 3 - padding;

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
#include <openssl/pem.h>


#pragma  comment(lib, "libeay32.lib")

class _crypto
{

	typedef unsigned long size_t;
	typedef _encoding	  type_t;

	string m_int;
	string m_key;
	string m_keypub;
	string m_keypriv;
	string m_out;
	string m_error;
	
	RSA*   m_rsa;

	int    m_rsapadding;
	int    m_pem;

	RSA * createRSA(unsigned char * key,int pub)
	{
		RSA *rsa= NULL;
		BIO *keybio ;
		keybio = BIO_new_mem_buf(key, -1);
		if (keybio==NULL)
		{
			printf( "Failed to create key BIO");
			return 0;
		}
		if(pub)
		{
			rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
		}
		else
		{
			rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
		}

		return rsa;
	}

public:
	_crypto(){throw std::exception("invalid constructor");}
	~_crypto(){m_int.clear(), m_out.clear();};

	_crypto(const char* data) :m_rsapadding(RSA_NO_PADDING),m_pem(0),m_rsa(0)
	{
		m_int.assign(data);
	}

	_crypto(const wchar_t* data):m_rsapadding(RSA_NO_PADDING),m_pem(0),m_rsa(0)
	{
		m_int.assign((const char*)data, wcslen(data) * sizeof(wchar_t));
	}

	_crypto(const char* data, size_t sz) :m_rsapadding(RSA_NO_PADDING),m_pem(0),m_rsa(0)
	{
		m_int.assign((const char*)data, sz);
	}

	_crypto(const wchar_t* data, size_t sz) :m_rsapadding(RSA_NO_PADDING),m_pem(0),m_rsa(0)
	{
		m_int.assign((const char*)data, sz * sizeof(wchar_t));
	}


	_crypto( string& str):m_rsapadding(RSA_NO_PADDING),m_pem(0),m_rsa(0)
	{
		m_int = str;
	}

	_crypto( wstring& str):m_rsapadding(RSA_NO_PADDING),m_pem(0),m_rsa(0)
	{
		m_int.assign((const char*)str.c_str(), str.size() * sizeof(wchar_t));
	}


	_crypto& rsa_en()
	{
		if(m_out.size())
		{
			m_int = m_out;
		}


		RSA* rsa;
			
		if(m_pem)
		{
			rsa = createRSA((unsigned char*)m_keypub.c_str(), 1);
		}


		int sz = RSA_size(rsa);
		m_out.resize(sz);

		
		int iret = RSA_public_encrypt(m_int.size(),(const unsigned char*)m_int.c_str(),(unsigned char*)m_out.c_str(),rsa,m_rsapadding);

		if(iret == -1)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);

		}

		
		return *this;
	}
	_crypto& rsa_de()
	{

		if(m_out.size())
		{
			m_int = m_out;
		}

		RSA* rsa;
		if(m_pem)
		{
			rsa = createRSA((unsigned char*)m_keypriv.c_str(), 0);
		}


		int sz = RSA_size(rsa);
		m_out.resize(sz);

		int iret = RSA_private_decrypt(m_int.size(),(const unsigned char*)m_int.c_str(),(unsigned char*)m_out.c_str(),rsa,m_rsapadding);
		
		if(iret == -1)
		{
			ERR_load_RSA_strings();
			m_error = ERR_error_string(ERR_get_error(), NULL);

		}

		return *this;
	}

	_crypto& rsapadding(int padding)
	{
		m_rsapadding = padding;
		return *this;
	}
	
	_crypto& pubkey(const string& key)
	{
		m_keypub = key;

		m_pem = 1;

		return *this;
	}

	_crypto& prikey(const string& key)
	{

		m_keypriv = key;
		m_pem = 1;

		return *this;
	}


	string get()
	{
		return m_out;
	}

	string error()
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





