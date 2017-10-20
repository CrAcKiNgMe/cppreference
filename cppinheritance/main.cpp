#include <Windows.h>
#include <assert.h>
#include "xxstring.h"

#include "_re.h"
#include <fstream>

int a(xstring t);



int main()
{


	try
	{
		//char* ptest = new char[0x7fffffff];
	}
	catch (std::exception& bd)
	{
		printf("error happed [%s]\n", bd.what());

	}

	
// 	assert( wxstring(L"123412341234") ==  _encoding(_xstring("123412341234")).a_utf16().getutf16().c_str() );
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).hex().get().c_str());
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).de_base64().get().c_str());
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).hex().get().c_str());
// 	assert( xstring("313233343132333431323334") ==   _encoding(_xstring("123412341234")).hex().get().c_str());

 	xstring b;
 	b.format("%s", "1234567890123456789012345678901234567890123456789012345678901234567890123456789");

	wxstring wb;
	wb.format(L"%s", L"1234567890123456789012345678901234567890123456789012345678901234567890123456789");

	string a = "12345678901234567890123456789012345678901234567890"
		"12345678901234567890123456789012345678901234567890"
		"12345678901234567890123456789012345678901234567890"
		"12345678901234567890123456789012345678901234567890"
		"12345678901234567890123456789012345678901234567890"
		"123456";
	ifstream  keyfile;
	keyfile.open("public.pem");
	char pubkey[0x1000];
	keyfile.read(pubkey, 0x1000);

	ifstream  keyfile2;
	keyfile2.open("private.pem");
	char prikey[0x1000];
	keyfile2.read(prikey, 0x1000);

	try
	{
		string cipher = RsaNone.data(a).key(pubkey).encrypt().get();//nopadding
		string  plain = RsaNone.data(cipher).key(prikey).decrypt().get();;//nopadding
	}
	catch (_crypto& e)
	{
		MessageBoxA(NULL, e.what().c_str(), "∑¢…˙“Ï≥£", MB_OK);
	}
	
	_datetime::test();


	_re::testpcre();





	getchar();

	return 0;
}