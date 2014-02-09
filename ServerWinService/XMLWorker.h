#pragma once
#include <tinyxml2.h>
#include <string>
class XMLWorker
{
	const char* CStrKeyValPair;
	const char* CStrKey;
	const char* CStrVal;
	const char * DefaultFileName;
	std::string fileName;
	tinyxml2::XMLDocument m_doc;
	tinyxml2::XMLElement* m_pairs;
	void inialize();
public:
	bool Add(const std::string & tagName, const std::string & tagValue);
	bool Set(const std::string & tagName, const std::string & tagValue);
	bool Get(const std::string & tagName, std::string & tagValue);
	void SaveFile(const char *);
	static XMLWorker& GetInstance()
	{
		static XMLWorker singleton;
		return singleton;
	}
private:
	XMLWorker(const char * );
	XMLWorker(void);
	~XMLWorker(void);
	//XMLWorker(const XMLWorker&);
	//XMLWorker operator=(const XMLWorker&);
};

