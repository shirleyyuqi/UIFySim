#pragma once
#include "IRecord.h"
#include "avi/AVIEncoder.h"
#include "HiroCamDef.h"
namespace HiroCamSDK {
    class CAVIRecord : public IRecord
    {
    public:
        CAVIRecord(void);
        ~CAVIRecord(void);
        
        virtual HRSDK_CMDRESULT OpenFile(const HR_CHAR* filename);
        virtual HR_S32 WriteVideo(HRSDK_DecFrame* data);
        virtual HR_S32 WriteAudio(HRSDK_DecFrame* data);
        virtual HRSDK_CMDRESULT CloseFile();
    protected:
        HiroCamSDK::Model::CAVIEncoder m_AVIEncoder;
        HR_S64        m_nFileTotalSize;
        HR_BOOL        m_bHasRecordKeyFrameGot;
        HR_S32        m_nVideoWidth;
        HR_S32        m_nVideoHeight;
        HR_S64        m_iPrePts;
        float        m_iTotalTime;
        HR_CHAR        *m_pFristAudio;
        HR_U32        m_iFristAudioLen;
    };
}
