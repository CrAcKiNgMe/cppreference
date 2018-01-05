
#include <Windows.h>
#include <assert.h>



#include "xxstring.h"
#include "_re.h"
#include <regex>
using namespace std;
#include "encoding.h"

#include <fstream>

int a(xstring t);



#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/sha.h>



int main()
{

	std::regex e("Referer");
	string subject = "Referer:";
	std::smatch sm;    // same as std::match_results<string::const_iterator> sm;  
	bool bRet = std::_Regex_match(subject.c_str(), subject.c_str() + subject.size(),
		&sm, e, regex_constants::_Match_partial, false) ;  

	int nsize = sm.size();

	


	try
	{
		//char* ptest = new char[0x7fffffff];
	}
	catch (std::exception& bd)
	{
		printf("error happed [%s]\n", bd.what());

	}

#if 0
// 	assert( wxstring(L"123412341234") ==  _encoding(_xstring("123412341234")).a_utf16().getutf16().c_str() );
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).hex().get().c_str());
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).de_base64().get().c_str());
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).hex().get().c_str());
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).hex().get().c_str());

	


	

 
 	string b64 = "ZWpFc1RnenZ6N3BScEw0QlAwZWRzV0M5QktGT2lWUk56SnhhQnc5enI4ZkI3R2ZIL0hVcmFuT1dJTXk2d0liZklsYnl4Ri8yaTRmdlRXSzRWQ3hISEkvUE9xSzE2TjFaNlA1MUtteWZ5OUNTaUNpR09IWnFlWGNQR2ZVbjJqS08xTXkwUklham42L3dzMm9PYWNEd0lNcGQrdXJRbnQ5MGNVd1phTkh5ZXc0PXsiaWQiOjEsInR5cGUiOiJ0ZXN0IiwicGxhdGZvcm0iOjQsIm5vdGJlZm9yZSI6IjIwMTcxMjI3Iiwibm90YWZ0ZXIiOiIyMDE4MDMyNyJ9";
  	

 	
 	string tmp = _encoding (b64).de_base64().get();

	string pubkey_hex = "30818902818100e6d0ca5906c68a6104868f5972b50fae6d49bfe29be620a4d6b6ab6b1a8ef9705e0dec76a3966dbdc2339fec8edf4dee3e265c5898c86ac685dea222c5c0f520566ea83065394fcd11f48c3a720dfa757f25edb6308844ecfeb234f4acecfabec6640108d57b9e08d43280df707d47010ee5c8b238e6cdc524a42e463451b3c50203010001";
	string pubkey_der = _encoding (pubkey_hex).hex_de().get();


	string pempubkey = _encoding(pubkey_der).en_base64().get();


 	RsaPkcs1(rsa1);
 	rsa1.data(tmp).key(pubkey_der,2).public_decrypt();
	


	


	

	

#endif






	getchar();

	return 0;
}