#include <Windows.h>
#include "xxstring.h"
#include "utities.h"

#pragma  comment(lib, "pcre2.lib")


_re::_re(const string& pattern,  bool casesensitive)
{
	m_bCaseSensitive = casesensitive;
	compile(pattern);
	
}


int _re::compile(const string& pattern )
{
	const  char* err;
	int nErrOffset;
	m_result.clear();

	int errnumber;
	size_t erroffset;



	int optflag = 0;
	optflag = (optflag | PCRE2_BSR_ANYCRLF);
	if(!m_bCaseSensitive)
	{
		optflag = (optflag | PCRE2_CASELESS);
	}
	m_re = ::pcre2_compile(
		(unsigned char*)pattern.c_str(),               /* the pattern */
		PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
		optflag,                     /* default options */
		&errnumber,          /* for error number */
		&erroffset,          /* for error offset */
		NULL);                 /* use default compile context */

	if(!m_re)
	{
		throw std::exception("_re::pcre2_compile failed");
	}

	

	return 0;
}




_re::_re()
{
	m_replacecnt = 0;
	m_re = NULL;
}

_re::~_re()
{

}

string _re::GetSubCapture(int number)
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




int _re::match( const string& subject,  bool bRecursive )
{

	unsigned int offset = 0;
	unsigned int len    = subject.size();
	PCRE2_SIZE*         ovector;
	int rc = 0;
	while(offset<subject.size())
	{

		pcre2_match_data*  match_data = pcre2_match_data_create_from_pattern(m_re, NULL);;
		rc = pcre2_match(m_re,   (unsigned char*)subject.c_str(), len, offset, (PCRE2_BSR_ANYCRLF | PCRE2_NOTEMPTY), match_data, NULL);

		if( rc > 0 ) 
		{
			ovector = pcre2_get_ovector_pointer(match_data);
			for(int i = 0; i < rc; i++) 
			{
				size_t begin = ovector[2*i];
				size_t len   = ovector[2*i+1] - ovector[2*i];
				string tmp;
				tmp.assign(subject.c_str() + begin, len);
				m_result.push_back(tmp);

			}

			m_groupcnt.push_back(rc);

			if(!bRecursive)
			{
				pcre2_match_data_free(match_data);
				break;
			}

			offset = ovector[1];
			pcre2_match_data_free(match_data);
		}
		else
		{
			pcre2_match_data_free(match_data);
			break;
		}

		

	


	}


	return 0;
}

_re& _re::replace(const string& subject, const string& tar, bool bRecursive /*= true*/)
{
	unsigned int offset = 0;
	unsigned int len    = subject.size();
	size_t          buffersize = 0;

	unsigned char *outbuffer = NULL;

	

	
	pcre2_match_data*  match_data = pcre2_match_data_create_from_pattern(m_re, NULL);;

	size_t rc = pcre2_substitute(m_re, (unsigned char*)subject.c_str(), len, offset, (PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_OVERFLOW_LENGTH |PCRE2_NOTEMPTY), match_data,
		NULL, (unsigned char*)tar.c_str(), tar.size(), outbuffer, &buffersize);

	if(rc != PCRE2_ERROR_NOMEMORY)
	{
		xstring tmp;
		tmp.format("_re::replace errorcode: %d",rc);
		throw std::exception(tmp);;
	}
	 
	m_replaceresult.resize(buffersize);

	rc = pcre2_substitute(m_re, (unsigned char*)subject.c_str(), len, offset, (PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_OVERFLOW_LENGTH |PCRE2_NOTEMPTY), match_data,
		NULL, (unsigned char*)tar.c_str(), tar.size(), (unsigned char*)m_replaceresult.c_str(), &buffersize);

	
	if(rc < 0)
	{
		xstring tmp;
		tmp.format("_re::replace errorcode: %d",rc);
		throw std::exception(tmp);;
	}

	m_replacecnt = rc;

	return *this;;
}

int _re::replacecnt()
{
	return m_replacecnt;;
}

const string& _re::replaceresult()
{
	return m_replaceresult;;
}


int _re::testpcre()
{
	char *reg = "From:([^@]+)@([^\r\n]+)";
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

	char* reg3 = "[0-9]{1,3}(?=[0-9]{3}+\\.)";
	char str3[] = "12345678901.34";


	try
	{
		_re re1(reg,false);
		re1.replace(str,"********",false);


		_re re2(reg2,false);
		re2.match(str2);



		_re re(reg3,false);
		re.match(str3);

	}
	catch(std::exception& ep)
	{
		const char* e = ep.what();
	}

	

	return 1;


}