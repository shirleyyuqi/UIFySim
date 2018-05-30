#ifndef __UIMARKUP_H__
#define __UIMARKUP_H__

enum
{
    XMLFILE_ENCODING_UTF8 = 0,
    XMLFILE_ENCODING_UNICODE = 1,
    XMLFILE_ENCODING_ASNI = 2,
};

class CMarkup;
class CMarkupNode;


class CMarkup
{
    friend class CMarkupNode;
public:
    CMarkup(const char* pstrXML = NULL);
    ~CMarkup();

    bool Load(const char* pstrXML);
    bool LoadFromFile(JNIEnv* env, jobject assetManager, const char* pstrFilename, int encoding = XMLFILE_ENCODING_UTF8);
    void Release();
    bool IsValid() const;

    void SetPreserveWhitespace(bool bPreserve = true);
    void GetLastErrorMessage(const char* pstrMessage, int cchMax) const;
    void GetLastErrorLocation(const char* pstrSource, int cchMax) const;

    CMarkupNode* GetRoot();

private:
    typedef struct tagXMLELEMENT
    {
        unsigned long iStart;
        unsigned long iChild;
        unsigned long iNext;
        unsigned long iParent;
        unsigned long iData;
    } XMLELEMENT;

    char* m_pstrXML;
    XMLELEMENT* m_pElements;
    unsigned long m_nElements;
    unsigned long m_nReservedElements;
    char m_szErrorMsg[100];
    char m_szErrorXML[50];
    bool m_bPreserveWhitespace;

private:
    bool _Parse();
    bool _Parse(char*& pstrText, unsigned long iParent);
    XMLELEMENT* _ReserveElement();
    inline void _SkipWhitespace(char*& pstr) const;
    inline void _SkipWhitespace(const char*& pstr) const;
    inline void _SkipIdentifier(char*& pstr) const;
    inline void _SkipIdentifier(const char*& pstr) const;
    bool _ParseData(char*& pstrText, char*& pstrData, char cEnd);
    void _ParseMetaChar(char*& pstrText, char*& pstrDest);
    bool _ParseAttributes(char*& pstrText);
    bool _Failed(const char* pstrError, const char* pstrLocation = NULL);
};

class CMarkupNode
{
    friend class CMarkup;
private:
    CMarkupNode();
    CMarkupNode(CMarkup* pOwner, int iPos);

public:
    bool IsValid() const;

    CMarkupNode* GetParent();
    CMarkupNode* GetSibling();
    CMarkupNode* GetChild();
    CMarkupNode* GetChild(const char* pstrName);

    bool HasSiblings() const;
    bool HasChildren() const;
    const char* GetName() const;
    const char* GetValue() const;

    bool HasAttributes();
    bool HasAttribute(const char* pstrName);
    int GetAttributeCount();
    const char* GetAttributeName(int iIndex);
    const char* GetAttributeValue(int iIndex);
    const char* GetAttributeValue(const char* pstrName);
    bool GetAttributeValue(int iIndex, char* pstrValue, int cchMax);
    bool GetAttributeValue(const char* pstrName, char* pstrValue, int cchMax);

private:
    void _MapAttributes();

    enum { MAX_XML_ATTRIBUTES = 64 };

    typedef struct
    {
        unsigned long iName;
        unsigned long iValue;
    } XMLATTRIBUTE;

    int m_iPos;
    int m_nAttributes;
    XMLATTRIBUTE m_aAttributes[MAX_XML_ATTRIBUTES];
    CMarkup* m_pOwner;
};

#endif // __UIMARKUP_H__
