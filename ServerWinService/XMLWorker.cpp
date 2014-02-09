#include "XMLWorker.h"
//#include <fstream>
 

XMLWorker::XMLWorker(const char * fileName): fileName(fileName)
{
	inialize();
	//this->fileName = std::string(fileName);
	if(tinyxml2::XML_NO_ERROR == m_doc.LoadFile(fileName))
		return;
	/* TODO :  handle error*/
}


XMLWorker::XMLWorker(void)
{
	inialize();
	using namespace tinyxml2;
	if(XML_NO_ERROR == m_doc.LoadFile(DefaultFileName))
	{
		// unable to open file
		XMLElement* root = m_doc.FirstChildElement();
		m_pairs = root->FirstChildElement(CStrKeyValPair);
		if(m_pairs) // all ok 
			return;
	}
	// create new document
	fileName  = std::string(DefaultFileName);
	const char* pub = "<?xml version='1.1'encoding=\"UTF-8\" ?><root/>";
	m_doc.Clear();
	m_doc.Parse(pub);
	XMLElement* root = m_doc.FirstChildElement();
	XMLElement* pairs = m_doc.NewElement( CStrKeyValPair );
	root->InsertEndChild( pairs );
	m_pairs = pairs;
}

void XMLWorker::SaveFile(const char * fileName)
{
	if(m_doc.NoChildren())
		return;
	if(fileName == nullptr)
		fileName = DefaultFileName;
	m_doc.SaveFile(fileName);
}

bool XMLWorker::Add(const std::string & tagName, const std::string & tagValue)
{
	using namespace tinyxml2;
	for( const XMLNode* node = m_pairs->FirstChild(); node; node=node->NextSibling() ) 
	{
		auto attrKey = node->ToElement()->FindAttribute(CStrKey);
		if (attrKey == 0)
			continue;
		if(tagName.compare(attrKey->Value()) == 0)
		{
			 return false; // we already have this key, if you want to change it then use set instead
		}
	}
	XMLElement* data = m_doc.NewElement( CStrKeyValPair );
	data->SetAttribute(CStrKey, tagName.c_str());
	data->SetAttribute(CStrVal, tagValue.c_str());
	m_pairs->InsertEndChild(data);
	return true; // pair added
}
bool XMLWorker::Set(const std::string & tagName, const std::string & tagValue)
{
	using namespace tinyxml2;
	for( XMLNode* node = m_pairs->FirstChild(); node; node=node->NextSibling() ) 
	{
		const XMLAttribute * attrKey = static_cast<const XMLElement*>(node->ToElement())->FindAttribute(CStrKey);
		if (attrKey == 0)
			continue;
		if(tagName.compare(attrKey->Value()) == 0)
		{
			 node->ToElement()->SetAttribute(CStrVal, tagValue.c_str());
			 return true; // value was changed
		}
	}
	return false; // we don't have this key, if you want to add it then use add instead
}
bool XMLWorker::Get(const std::string & tagName, std::string & tagValue)
{
	using namespace tinyxml2;
	for(const XMLNode* node = m_pairs->FirstChild(); node; node=node->NextSibling() ) 
	{
		const XMLAttribute * attrKey = node->ToElement()->FindAttribute(CStrKey);
		if (attrKey == 0)
			continue;
		if(tagName.compare(attrKey->Value()) == 0)
		{
			attrKey = node->ToElement()->FindAttribute(CStrVal);
			if(attrKey == 0)
				return false; // Value not found
			tagValue = std::string(attrKey->Value());
			return true; // tagValue holds the value associated with tagName key
		}
	}
	return false; // we don't have this key
}
void XMLWorker::inialize()
{
	CStrKeyValPair = "KeyValuePair";
	CStrKey = "Key";
	CStrVal = "Value";
	DefaultFileName = "C:\\Users\\User01\\Documents\\Visual Studio 2012\\Projects\\ServerWinService\\ServerWinService\\KVdata.xml";
}
XMLWorker::~XMLWorker(void)
{
}
