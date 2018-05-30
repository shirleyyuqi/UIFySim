#include "BaseDataType.h"
#include "SinVoicePlayer.h"
#include "ESPcmPlayer.h"
#include "MyPcmPlayerImp.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

using namespace HiroCamSDK;

static const char* const CODE_BOOK = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@_~!#$%^&*,:;./\\[]{}<>|`+-=\"";
static int  bFastQuit = 0;
static void voiceStartPlay();
static ESVoid onSinVoicePlayerStart(ESVoid* cbParam);
static ESVoid onSinVoicePlayerStop(ESVoid* cbParam);

static ESPcmPlayer mPcmPlayer;
static SinVoicePlayer* mSinVoicePlayer = NULL;
static SinVoicePlayerCallback gSinVoicePlayerCallback = {onSinVoicePlayerStart, onSinVoicePlayerStop};

static int mRates[100];
static char _voiceAllData[512];
static char _voiceDataBlock[40];
static unsigned long _voiceAllDataSize;;
static unsigned long mVoiceDataNums;     //声波配网的数据分段的数量，最多为3段，如果数据量小，则少于3段
static int curVoiceDataIndex = 0;
static MUTEX sSinVoiceMutex;
static ESVoid onSinVoicePlayerStart(ESVoid* cbParam)
{
    NSLog(@"onSinVoicePlayerStart ");
}

static ESVoid onSinVoicePlayerStop(ESVoid* cbParam)
{
    NSLog(@"onSinVoicePlayerStop ");
    dispatch_async(dispatch_get_main_queue(), ^{
        LOCK_MUTEX(&sSinVoiceMutex);
        if (mSinVoicePlayer) {
            SinVoicePlayer_stop(mSinVoicePlayer);
        }
        UNLOCK_MUTEX(&sSinVoiceMutex);
    });
    
    if (!bFastQuit) {
        sleep(1);
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        LOCK_MUTEX(&sSinVoiceMutex);
        if (mSinVoicePlayer && !bFastQuit) {
            voiceStartPlay();
        }
        UNLOCK_MUTEX(&sSinVoiceMutex);
    });
    
    NSLog(@"onSinVoicePlayerStop 222");
}

extern "C" void InitSinVoice()
{
    INIT_MUTEX(&sSinVoiceMutex);
}

extern "C" void SinVoice_StartConfigWifi(char* ssid, char* wifiPwd, char* devPwd)
{
    LOCK_MUTEX(&sSinVoiceMutex);
    curVoiceDataIndex = 0;
    memset(_voiceAllData, 0, 512);
    
    NSData *data = [[NSString stringWithUTF8String:ssid] dataUsingEncoding:NSUTF8StringEncoding];
    NSString *afterBase64Ssid = [data base64EncodedStringWithOptions:0];
    
    unsigned long ssidSize = strlen([afterBase64Ssid UTF8String]);
    unsigned long keySize = strlen(wifiPwd);
    unsigned long pwdSize = strlen(devPwd);
    
    if (ssidSize < strlen(CODE_BOOK))
    {
        _voiceAllData[0] = CODE_BOOK[ssidSize];
    }
    
    if (keySize < strlen(CODE_BOOK))
    {
        _voiceAllData[1] = CODE_BOOK[keySize];
    }
    
    if (pwdSize < strlen(CODE_BOOK))
    {
        _voiceAllData[2] = CODE_BOOK[pwdSize];
    }
    
    memcpy(_voiceAllData+3, [afterBase64Ssid UTF8String], ssidSize);
    memcpy(_voiceAllData+3+ssidSize,wifiPwd, keySize);
    memcpy(_voiceAllData+3+ssidSize+keySize, devPwd, pwdSize);
    
    _voiceAllDataSize = strlen(_voiceAllData);  //获取总的数据长度
    
    mPcmPlayer.create = MyPcmPlayerImp_create;
    mPcmPlayer.start = MyPcmPlayerImp_start;
    mPcmPlayer.stop = MyPcmPlayerImp_stop;
    mPcmPlayer.setParam = MyPcmPlayerImp_setParam;
    mPcmPlayer.destroy = MyPcmPlayerImp_destroy;
    mSinVoicePlayer = SinVoicePlayer_create2("com.sinvoice.for_hichip", "SinVoiceDemo", &gSinVoicePlayerCallback, 0, &mPcmPlayer);
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
    
    SinVoicePlayer_getMaxEncoderIndex(mSinVoicePlayer);
    bFastQuit = 0;
    voiceStartPlay();
    UNLOCK_MUTEX(&sSinVoiceMutex);
}

extern "C" void SinVoice_StopConfigWifi()
{
    
    NSLog(@"SinVoice_StopConfigWifi ");
    LOCK_MUTEX(&sSinVoiceMutex);
    if (mSinVoicePlayer) {
        bFastQuit = 1;
        SinVoicePlayer_stop(mSinVoicePlayer);
        NSLog(@"SinVoice_StopConfigWifi 1");

        SinVoicePlayer_destroy(mSinVoicePlayer);
        NSLog(@"SinVoice_StopConfigWifi 2");

        mSinVoicePlayer = NULL;
    }
    UNLOCK_MUTEX(&sSinVoiceMutex);
}



static void voiceStartPlay()
{
    //先计算要切成多少块数据进行配网，每块数据最大为36
    int index = 0;
    if (_voiceAllDataSize <= 34)
    {
        mVoiceDataNums = 1;
    }else{
        mVoiceDataNums = (_voiceAllDataSize - 34) / 35 + 1;
        if (((_voiceAllDataSize - 34) % 35) != 0)
        {
            mVoiceDataNums += 1;
        }
    }
    
    //清空单块数据的内存
    memset(_voiceDataBlock, 0, 40);
    
    //计算出现在的数据库index
    int voiceDataIndexTemp = curVoiceDataIndex % mVoiceDataNums;
    //计算当前index应该截取的数据的长度
    unsigned long needReadVoiceDataLength = 0;
    if (0 == voiceDataIndexTemp)
    {
        if ((voiceDataIndexTemp+1) == mVoiceDataNums)   //是最后一块数据
        {
            needReadVoiceDataLength = _voiceAllDataSize;
        }else                                           //不是最后一块数据
        {
            needReadVoiceDataLength = 34;
        }
    }else
    {
        if ((voiceDataIndexTemp+1) == mVoiceDataNums)   //是最后一块数据
        {
            needReadVoiceDataLength = (_voiceAllDataSize-34-(voiceDataIndexTemp-1)*35);
        }else                                           //不是最后一块数据
        {
            needReadVoiceDataLength = 35;
        }
    }
    
    //是否为第一块数据，第一块数据比较特殊
    if (0 == voiceDataIndexTemp)    //第一块数据，格式为[index][nums][data]
    {
        _voiceDataBlock[0] =  CODE_BOOK[voiceDataIndexTemp];
        _voiceDataBlock[1] = CODE_BOOK[mVoiceDataNums];
        memcpy(_voiceDataBlock+2, _voiceAllData, needReadVoiceDataLength);
    }else
    {
        //第一个数据块之后的数据块，格式为[index][data]
        _voiceDataBlock[0] = CODE_BOOK[voiceDataIndexTemp];
        memcpy(_voiceDataBlock+1, _voiceAllData+34+(voiceDataIndexTemp-1)*35, needReadVoiceDataLength);
    }
    curVoiceDataIndex++;
    
    const char* str = _voiceDataBlock;
    int mPlayCount = (int)strlen(str);
    
    int lenCodeBook = (int)strlen(CODE_BOOK);
    int isOK = 1;
    while ( index < mPlayCount) {
        int i = 0;
        for ( i = 0; i < lenCodeBook; ++i ) {
            if ( str[index] == CODE_BOOK[i] ) {
                mRates[index] = i;
                break;
            }
        }
        if ( i >= lenCodeBook ) {
            isOK = 0;
            break;
        }
        ++index;
    }
    if ( isOK ) {
        if (mSinVoicePlayer)
        {
            SinVoicePlayer_play(mSinVoicePlayer, mRates, mPlayCount);
        }
    }
}
