#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include "UIMarkup.h"

#define _T(a)   a

const char* CharNext(const char* pstr)
{
    return ++pstr;
}

CMarkup::CMarkup(const char* pstrXML)
{
    m_pstrXML = NULL;
    m_pElements = NULL;
    m_nElements = 0;
    m_bPreserveWhitespace = true;
    if( pstrXML != NULL ) Load(pstrXML);
}

CMarkup::~CMarkup()
{
    Release();
}

bool CMarkup::IsValid() const
{
    return m_pElements != NULL;
}

void CMarkup::SetPreserveWhitespace(bool bPreserve)
{
    m_bPreserveWhitespace = bPreserve;
}

bool CMarkup::Load(const char* pstrXML)
{
	Release();
    int cchLen = (int)strlen(pstrXML) + 1;
    m_pstrXML = (char*)(malloc(cchLen * sizeof(char)));
    memcpy(m_pstrXML, pstrXML, cchLen * sizeof(char));
    m_pstrXML[cchLen-1] = 0;
    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool CMarkup::LoadFromFile(JNIEnv* env, jobject assetManager, const char* pstrFilename, int encoding)
{
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if(mgr==NULL) {
        __android_log_print(ANDROID_LOG_DEBUG, "T", "a\n");
        return false;
    }

    Release();
    AAsset* asset = AAssetManager_open(mgr, pstrFilename, AASSET_MODE_UNKNOWN);
    if(asset) {
        __android_log_print(ANDROID_LOG_DEBUG, "T", "1\n");
        off_t dwSize = AAsset_getLength(asset);

        __android_log_print(ANDROID_LOG_DEBUG, "T", "aaa  %d\n", dwSize);
        if( dwSize == 0 ) return _Failed(_T("File is empty"));
        if ( dwSize > 4096*1024 ) return _Failed(_T("File too large"));

        m_pstrXML = (char*)malloc( dwSize + 1);
        int dwRead = AAsset_read(asset, m_pstrXML, dwSize);
        __android_log_print(ANDROID_LOG_DEBUG, "T", "bbb  %d\n", dwSize);
        if( dwRead != dwSize ) {
            Release();
            return _Failed(_T("Could not read file"));
        }
        __android_log_print(ANDROID_LOG_DEBUG, "T", "ccc  %d\n", dwSize);
        AAsset_close(asset);
        __android_log_print(ANDROID_LOG_DEBUG, "T", "ddd  %d\n", dwSize);
        m_pstrXML[dwSize] = 0;
        __android_log_print(ANDROID_LOG_DEBUG, "T", "eee  %d\n", dwSize);
        bool bRes = _Parse();
        __android_log_print(ANDROID_LOG_DEBUG, "T", "fff  %d\n", dwSize);
        if( !bRes ) Release();
        __android_log_print(ANDROID_LOG_DEBUG, "T", "gggg  %d\n", dwSize);
        return bRes;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "T", "errrrrrrrr\n");
    return false;
}

void CMarkup::Release()
{
    if( m_pstrXML != NULL ) free(m_pstrXML);
    if( m_pElements != NULL ) free(m_pElements);
    m_pstrXML = NULL;
    m_pElements = NULL;
}

void CMarkup::GetLastErrorMessage(const char* pstrMessage, int cchMax) const
{
    strncpy((char*)pstrMessage, m_szErrorMsg, cchMax);
}

void CMarkup::GetLastErrorLocation(const char* pstrSource, int cchMax) const
{
    strncpy((char*)pstrSource, m_szErrorXML, cchMax);
}

CMarkupNode* CMarkup::GetRoot()
{
    if( m_nElements == 0 ) return new CMarkupNode();
    return new CMarkupNode(this, 1);
}

bool CMarkup::_Parse()
{
    _ReserveElement(); // Reserve index 0 for errors
    memset(m_szErrorMsg, 0x00, sizeof(m_szErrorMsg));
    memset(m_szErrorXML, 0x00, sizeof(m_szErrorXML));
    char* pstrXML = m_pstrXML;
    return _Parse(pstrXML, 0);
}

bool CMarkup::_Parse(char*& pstrText, unsigned long iParent)
{
    _SkipWhitespace(pstrText);
    unsigned long iPrevious = 0;
    for( ; ; ) 
    {
        if( *pstrText == _T('\0') && iParent <= 1 ) return true;
        _SkipWhitespace(pstrText);
        if( *pstrText != _T('<') ) return _Failed(_T("Expected start tag"), pstrText);
        if( pstrText[1] == _T('/') ) return true;
        *pstrText++ = _T('\0');
        _SkipWhitespace(pstrText);
        // Skip comment or processing directive
        if( *pstrText == _T('!') || *pstrText == _T('?') ) {
            char ch = *pstrText;
            if( *pstrText == _T('!') ) ch = _T('-');
            while( *pstrText != _T('\0') && !(*pstrText == ch && *(pstrText + 1) == _T('>')) ) pstrText = (char*)::CharNext(pstrText);
            if( *pstrText != _T('\0') ) pstrText += 2;
            _SkipWhitespace(pstrText);
            continue;
        }
        _SkipWhitespace(pstrText);
        // Fill out element structure
        XMLELEMENT* pEl = _ReserveElement();
        unsigned long iPos = pEl - m_pElements;
        pEl->iStart = pstrText - m_pstrXML;
        pEl->iParent = iParent;
        pEl->iNext = pEl->iChild = 0;
        if( iPrevious != 0 ) m_pElements[iPrevious].iNext = iPos;
        else if( iParent > 0 ) m_pElements[iParent].iChild = iPos;
        iPrevious = iPos;
        // Parse name
        const char* pstrName = pstrText;
        _SkipIdentifier(pstrText);
        char* pstrNameEnd = pstrText;
        if( *pstrText == _T('\0') ) return _Failed(_T("Error parsing element name"), pstrText);
        // Parse attributes
        if( !_ParseAttributes(pstrText) ) return false;
        _SkipWhitespace(pstrText);
        if( pstrText[0] == _T('/') && pstrText[1] == _T('>') )
        {
            pEl->iData = pstrText - m_pstrXML;
            *pstrText = _T('\0');
            pstrText += 2;
        }
        else
        {
            if( *pstrText != _T('>') ) return _Failed(_T("Expected start-tag closing"), pstrText);
            // Parse node data
            pEl->iData = ++pstrText - m_pstrXML;
            char* pstrDest = pstrText;
            if( !_ParseData(pstrText, pstrDest, _T('<')) ) return false;
            // Determine type of next element
            if( *pstrText == _T('\0') && iParent <= 1 ) return true;
            if( *pstrText != _T('<') ) return _Failed(_T("Expected end-tag start"), pstrText);
            if( pstrText[0] == _T('<') && pstrText[1] != _T('/') ) 
            {
                if( !_Parse(pstrText, iPos) ) return false;
            }
            if( pstrText[0] == _T('<') && pstrText[1] == _T('/') ) 
            {
                *pstrDest = _T('\0');
                *pstrText = _T('\0');
                pstrText += 2;
                _SkipWhitespace(pstrText);
                int cchName = pstrNameEnd - pstrName;
                if( strncmp(pstrText, pstrName, cchName) != 0 ) return _Failed(_T("Unmatched closing tag"), pstrText);
                pstrText += cchName;
                _SkipWhitespace(pstrText);
                if( *pstrText++ != _T('>') ) return _Failed(_T("Unmatched closing tag"), pstrText);
            }
        }
        *pstrNameEnd = _T('\0');
        _SkipWhitespace(pstrText);
    }
}

CMarkup::XMLELEMENT* CMarkup::_ReserveElement()
{
    if( m_nElements == 0 ) m_nReservedElements = 0;
    if( m_nElements >= m_nReservedElements ) {
        m_nReservedElements += (m_nReservedElements / 2) + 500;
        m_pElements = static_cast<XMLELEMENT*>(realloc(m_pElements, m_nReservedElements * sizeof(XMLELEMENT)));
    }
    return &m_pElements[m_nElements++];
}

void CMarkup::_SkipWhitespace(char*& pstr) const
{
    while( *pstr > _T('\0') && *pstr <= _T(' ') ) pstr = (char*)::CharNext(pstr);
}

void CMarkup::_SkipIdentifier(char*& pstr) const
{
    while( *pstr != _T('\0') && (*pstr == _T('_') || *pstr == _T(':') || isalnum(*pstr)) ) pstr = (char*)::CharNext(pstr);
}

bool CMarkup::_ParseData(char*& pstrText, char*& pstrDest, char cEnd)
{
    while( *pstrText != _T('\0') && *pstrText != cEnd ) {
		if( *pstrText == _T('&') ) {
			while( *pstrText == _T('&') ) {
				_ParseMetaChar(++pstrText, pstrDest);
			}
			if (*pstrText == cEnd)
				break;
		}

        if( *pstrText == _T(' ') ) {
            *pstrDest++ = *pstrText++;
            if( !m_bPreserveWhitespace ) _SkipWhitespace(pstrText);
        }
        else {
            char* pstrTemp = (char*)::CharNext(pstrText);
            while( pstrText < pstrTemp) {
                *pstrDest++ = *pstrText++;
            }
        }
    }
    // Make sure that MapAttributes() works correctly when it parses
    // over a value that has been transformed.
    char* pstrFill = pstrDest + 1;
    while( pstrFill < pstrText ) *pstrFill++ = _T(' ');
    return true;
}

void CMarkup::_ParseMetaChar(char*& pstrText, char*& pstrDest)
{
    if( pstrText[0] == _T('a') && pstrText[1] == _T('m') && pstrText[2] == _T('p') && pstrText[3] == _T(';') ) {
        *pstrDest++ = _T('&');
        pstrText += 4;
    }
    else if( pstrText[0] == _T('l') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
        *pstrDest++ = _T('<');
        pstrText += 3;
    }
    else if( pstrText[0] == _T('g') && pstrText[1] == _T('t') && pstrText[2] == _T(';') ) {
        *pstrDest++ = _T('>');
        pstrText += 3;
    }
    else if( pstrText[0] == _T('q') && pstrText[1] == _T('u') && pstrText[2] == _T('o') && pstrText[3] == _T('t') && pstrText[4] == _T(';') ) {
        *pstrDest++ = _T('\"');
        pstrText += 5;
    }
    else if( pstrText[0] == _T('a') && pstrText[1] == _T('p') && pstrText[2] == _T('o') && pstrText[3] == _T('s') && pstrText[4] == _T(';') ) {
        *pstrDest++ = _T('\'');
        pstrText += 5;
    }
    else {
        *pstrDest++ = _T('&');
    }
}

bool CMarkup::_ParseAttributes(char*& pstrText)
{   
    if( *pstrText == _T('>') ) return true;
    *pstrText++ = _T('\0');
    _SkipWhitespace(pstrText);
    while( *pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('/') ) {
        _SkipIdentifier(pstrText);
        char* pstrIdentifierEnd = pstrText;
        _SkipWhitespace(pstrText);
        if( *pstrText != _T('=') ) return _Failed(_T("Error while parsing attributes"), pstrText);
        *pstrText++ = _T(' ');
        *pstrIdentifierEnd = _T('\0');
        _SkipWhitespace(pstrText);
        if( *pstrText++ != _T('\"') ) return _Failed(_T("Expected attribute value"), pstrText);
        char* pstrDest = pstrText;
        if( !_ParseData(pstrText, pstrDest, _T('\"')) ) return false;
        if( *pstrText == _T('\0') ) return _Failed(_T("Error while parsing attribute string"), pstrText);
        *pstrDest = _T('\0');
        if( pstrText != pstrDest ) *pstrText = _T(' ');
        pstrText++;
        _SkipWhitespace(pstrText);
    }
    return true;
}

bool CMarkup::_Failed(const char* pstrError, const char* pstrLocation)
{
    // Register last error
    printf(_T("XML Error: %s"), pstrError);
   // if( pstrLocation != NULL ) printf((char*)pstrLocation);
    //strncpy(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
    //strncpy(m_szErrorXML, pstrLocation != NULL ? pstrLocation : _T(""), lengthof(m_szErrorXML) - 1);
    return false; // Always return 'false'
}
//////
CMarkupNode::CMarkupNode() : m_pOwner(NULL)
{
}

CMarkupNode::CMarkupNode(CMarkup* pOwner, int iPos) : m_pOwner(pOwner), m_iPos(iPos), m_nAttributes(0)
{
}

CMarkupNode* CMarkupNode::GetSibling()
{
    if( m_pOwner == NULL ) return new CMarkupNode();
    unsigned long iPos = m_pOwner->m_pElements[m_iPos].iNext;
    if( iPos == 0 ) return new CMarkupNode();
    return new CMarkupNode(m_pOwner, iPos);
}

bool CMarkupNode::HasSiblings() const
{
    if( m_pOwner == NULL ) return false;
    unsigned long iPos = m_pOwner->m_pElements[m_iPos].iNext;
    return iPos > 0;
}

CMarkupNode* CMarkupNode::GetChild()
{
    if( m_pOwner == NULL ) return new CMarkupNode();
    unsigned long iPos = m_pOwner->m_pElements[m_iPos].iChild;
    if( iPos == 0 ) return new CMarkupNode();
    return new CMarkupNode(m_pOwner, iPos);
}

CMarkupNode* CMarkupNode::GetChild(const char* pstrName)
{
    if( m_pOwner == NULL ) return new CMarkupNode();
    unsigned long iPos = m_pOwner->m_pElements[m_iPos].iChild;
    while( iPos != 0 ) {
        if( strcmp(m_pOwner->m_pstrXML + m_pOwner->m_pElements[iPos].iStart, pstrName) == 0 ) {
            return new CMarkupNode(m_pOwner, iPos);
        }
        iPos = m_pOwner->m_pElements[iPos].iNext;
    }
    return new CMarkupNode();
}

bool CMarkupNode::HasChildren() const
{
    if( m_pOwner == NULL ) return false;
    return m_pOwner->m_pElements[m_iPos].iChild != 0;
}

CMarkupNode* CMarkupNode::GetParent()
{
    if( m_pOwner == NULL ) return new CMarkupNode();
    unsigned int iPos = m_pOwner->m_pElements[m_iPos].iParent;
    if( iPos == 0 ) return new CMarkupNode();
    return new CMarkupNode(m_pOwner, iPos);
}

bool CMarkupNode::IsValid() const
{
    return m_pOwner != NULL;
}

const char* CMarkupNode::GetName() const
{
    if( m_pOwner == NULL ) return NULL;
    return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
}

const char* CMarkupNode::GetValue() const
{
    if( m_pOwner == NULL ) return NULL;
    return m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
}

const char* CMarkupNode::GetAttributeName(int iIndex)
{
    if( m_pOwner == NULL ) return NULL;
    if( m_nAttributes == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= m_nAttributes ) return _T("");
    return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iName;
}

const char* CMarkupNode::GetAttributeValue(int iIndex)
{
    if( m_pOwner == NULL ) return NULL;
    if( m_nAttributes == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= m_nAttributes ) return _T("");
    return m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue;
}

const char* CMarkupNode::GetAttributeValue(const char* pstrName)
{
    if( m_pOwner == NULL ) return NULL;
    if( m_nAttributes == 0 ) _MapAttributes();
    for( int i = 0; i < m_nAttributes; i++ ) {
        if( strcmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return m_pOwner->m_pstrXML + m_aAttributes[i].iValue;
    }
    return _T("");
}

bool CMarkupNode::GetAttributeValue(int iIndex, char* pstrValue, int cchMax)
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    if( iIndex < 0 || iIndex >= m_nAttributes ) return false;
    strncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[iIndex].iValue, cchMax);
    return true;
}

bool CMarkupNode::GetAttributeValue(const char* pstrName, char* pstrValue, int cchMax)
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    for( int i = 0; i < m_nAttributes; i++ ) {
        if( strcmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) {
            strncpy(pstrValue, m_pOwner->m_pstrXML + m_aAttributes[i].iValue, cchMax);
            return true;
        }
    }
    return false;
}

int CMarkupNode::GetAttributeCount()
{
    if( m_pOwner == NULL ) return 0;
    if( m_nAttributes == 0 ) _MapAttributes();
    return m_nAttributes;
}

bool CMarkupNode::HasAttributes()
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    return m_nAttributes > 0;
}

bool CMarkupNode::HasAttribute(const char* pstrName)
{
    if( m_pOwner == NULL ) return false;
    if( m_nAttributes == 0 ) _MapAttributes();
    for( int i = 0; i < m_nAttributes; i++ ) {
        if( strcmp(m_pOwner->m_pstrXML + m_aAttributes[i].iName, pstrName) == 0 ) return true;
    }
    return false;
}

void CMarkupNode::_MapAttributes()
{
    m_nAttributes = 0;
    char* pstr = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iStart;
    char* pstrEnd = m_pOwner->m_pstrXML + m_pOwner->m_pElements[m_iPos].iData;
    pstr += strlen(pstr) + 1;
    while( pstr < pstrEnd ) {
        m_pOwner->_SkipWhitespace(pstr);
        m_aAttributes[m_nAttributes].iName = pstr - m_pOwner->m_pstrXML;
        pstr += strlen(pstr) + 1;
        m_pOwner->_SkipWhitespace(pstr);
        if( *pstr++ != _T('\"') ) return; // if( *pstr != _T('\"') ) { pstr = ::CharNext(pstr); return; }
        
        m_aAttributes[m_nAttributes++].iValue = pstr - m_pOwner->m_pstrXML;
        if( m_nAttributes >= MAX_XML_ATTRIBUTES ) return;
        pstr += strlen(pstr) + 1;
    }
}