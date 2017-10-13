#include <Windows.h>
#include "xxstring.h"
#include "utities.h"

#pragma  comment(lib, "pcre.lib")


CMyPcre::CMyPcre(const string& pattern,  bool casesensitive)
{
	pcre_compile(pattern, casesensitive);
}


int CMyPcre::pcre_compile(const string& pattern,  bool casesensitive )
{
	const  char* err;
	int nErrOffset;
	m_result.clear();


	int optflag = 0;
	optflag = (optflag | PCRE_BSR_ANYCRLF);
	if(casesensitive == false)
	{
		optflag = (optflag | PCRE_CASELESS);
	}
	m_re = ::pcre_compile(pattern.c_str(), optflag,  &err,  &nErrOffset, NULL);

	if(!m_re)
	{
		throw std::exception("CMyPcre::pcre_compile failed");
	}

	

	return 0;
}

int CMyPcre::pcre_match_r( const string& subject )
{

	unsigned int offset = 0;
	unsigned int len    = subject.size();
	int cnt;
	while (offset < len && (cnt = pcre_exec(m_re, 0, subject.c_str(), len, offset, 0, m_vec, VEC_COUNT) > 0))
	{
		int fullmatchsz = m_vec[1];
		for(int i = 0; i < cnt; ++i)
		{
			string tmp;
			const char* startpos = subject.c_str() + m_vec[2*i];
			const char* endpos	  = subject.c_str() + m_vec[2*i + 1];

			tmp.assign(startpos, endpos-startpos);
			m_result.push_back(tmp);
		}
		offset = m_vec[1];
	}

	return 0;
}


CMyPcre::CMyPcre()
{
	m_re = NULL;
}

CMyPcre::~CMyPcre()
{
	if(m_re)
	{
		pcre_free(m_re);
	}

}

string CMyPcre::GetSubCapture(int number)
{
	if(number < m_result.size())
	{
		return m_result[number];
	}
	else
	{
		return "";
	}
}




int CMyPcre::pcre_match( const string& subject,  bool bRecursive )
{

	unsigned int offset = 0;
	unsigned int len    = subject.size();

	while(offset<subject.size())
	{

		int cnt = pcre_exec(m_re,  NULL, subject.c_str(), len, offset, (PCRE_BSR_ANYCRLF | PCRE_NOTEMPTY), m_vec, VEC_COUNT);

		if(cnt < 0) 
		{ // Something bad happened..
			switch(cnt) {
			case PCRE_ERROR_NOMATCH      : ("String did not match the pattern\n");        break;
			case PCRE_ERROR_NULL         : throw std::exception("Something was null\n");                      break;
			case PCRE_ERROR_BADOPTION    : throw std::exception("A bad option was passed\n");                 break;
			case PCRE_ERROR_BADMAGIC     : throw std::exception("Magic number bad (compiled re corrupt?)\n"); break;
			case PCRE_ERROR_UNKNOWN_NODE : throw std::exception("Something kooky in the compiled re\n");      break;
			case PCRE_ERROR_NOMEMORY     : throw std::exception("Ran out of memory\n");                       break;
			default                      : throw std::exception("Unknown error\n");                           break;
			} /* end switch */

			break;
		} 

		else 
		{
			if (cnt == 0)
			{
				("But too many substrings were found to fit in subStrVec!\n");

				cnt = VEC_COUNT / 3;
			}
			const char* substring;

			for(int j=0; j<cnt; j++) {
				pcre_get_substring(subject.c_str(), m_vec, cnt, j, &(substring));
				string tmp;
				tmp.assign(substring);
				m_result.push_back(tmp);

				pcre_free_substring(substring);

			}

			m_groupcnt.push_back(cnt);

			offset = m_vec[1];


		}

		if(bRecursive == false)
		{
			break;;
		}


	}

	return 0;
}



int CMyPcre::testpcre()
{
	char *regex = "From:([^@]+)@([^\r\n]+)";
	char str[]  = "From:regular.expressions@example.com\r\n"\
		"From:exddd@43434.com\r\n"\
		"From:7853456@exgem.com\r\n";


	char* reg2 = "referer:[\t ]*http[s]?:\/\/([^:\/\r\n]+)";
	char str2[] = ""\
				"Host: windows10.microdone.cn:5076"\
				"Connection: keep-alive"\
				"Cache-Control: max-age=0"\
				"Accept: */"\
				"User-Agent: Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36"\
				"Referer: http://192.168.1.158:8088/DemoGeeMeePG/"\
				"Accept-Encoding: gzip, deflate, sdch"\
				"Accept-Language: zh-CN,zh;q=0.8"\
				"Cookie: Hm_lvt_c337010bc5a1154d2fb6741a4d77d226=1477902757; __utma=261155345.1563425408.1477902757.1477902757.1477902757.1; __utmz=261155345.1477902757.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none)";

	char* reg3 = "[0-9]{1,3}(?=([0-9]{3})+\\.\/g)";
	char str3[] = "12345678901.34";


	try
	{
		CMyPcre re(reg3,false);
		re.pcre_match(str3);

	}
	catch(std::exception& ep)
	{
		const char* e = ep.what();
	}

	

	return 1;


}