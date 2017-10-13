extern "C"
{
#define PCRE_STATIC
	#include <pcre.h>
};

#include <vector>
using namespace std;

class CMyPcre
{

public:

	CMyPcre(const string& pattern,  bool casesensitive );

	int pcre_compile(const string& pattern, bool casesensitive = true);
	int pcre_match(const string& subject, bool bRecursive = false);
	int CMyPcre::pcre_match_r( const string& subject );
	CMyPcre();
	~CMyPcre();
	static int CMyPcre::testpcre();

	string CMyPcre::GetSubCapture(int number);
protected:
private:

	pcre* m_re;
#define VEC_COUNT 30
	int  m_vec[30];

	std::vector<std::string> m_result;
	std::vector<int>	   m_groupcnt;

	bool m_bCaseSensitive;


};