#pragma once
#include <Windows.h>
#include <atlcomcli.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>

namespace nv {
    class AudioPlayer {
    public:
        AudioPlayer(WORD nChannels_, DWORD nSamplesPerSec_);

        HRESULT Start();

        HRESULT Stop();

        BYTE* GetBuffer(UINT32 wantFrames);

        HRESULT ReleaseBuffer(UINT32 writtenFrames);

        // FLTP ��ʽ���������ֿ������ǰ����Ǻϲ���һ�𣬡������������ҡ�����
        HRESULT WriteFLTP(float* left, float* right, UINT32 sampleCount);

        HRESULT WriteS16(short* data, UINT32 sampleCount);

        // �������Ҳ�������ֻ����������������Ȼ᲻����
        HRESULT PlaySinWave(int nb_samples);

        // ��������
        HRESULT SetVolume(float v);
    private:
        WORD nChannels;
        DWORD nSamplesPerSec;
        int maxSampleCount; // ��������С����������

        WAVEFORMATEX* pwfx;
        CComPtr<IMMDeviceEnumerator> pEnumerator;
        CComPtr<IMMDevice> pDevice;
        CComPtr<IAudioClient> pAudioClient;
        CComPtr<IAudioRenderClient> pRenderClient;
        CComPtr<ISimpleAudioVolume> pSimpleAudioVolume;

        DWORD flags = 0;

        HRESULT Init();

    };
}