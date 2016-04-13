//
//  AudioUnitDevice.hpp
//  audioqueuedemo
//
//  Created by 陳偉榮 on 16/4/7.
//  Copyright © 2016年 snailgame. All rights reserved.
//

#ifndef AudioUnitDevice_hpp
#define AudioUnitDevice_hpp

#include <stdio.h>
#include <stdint.h>
#include <AudioUnit/AudioUnit.h>

#include <memory>
#include <deque>
#include <mutex>
#include <atomic>

typedef std::function<int(char* data,int max_len)> AudioUnitBufferCallback;
class AudioDeviceIOS
{
public:
    AudioDeviceIOS();
    ~AudioDeviceIOS();
    
    /*sample_rate：采样率
     channel 频道数
     ByteSizePerPacket 每个包的数据大小frame*2*16/8
     */
    void Init(int nSampleRate, int nChannel, int ByteSizePerPacket,int ByteSizePerOutputPacket);
    void Terminate();

    void InitPlayout();
    void InitRecording();
    
    
    void StartPlayout();
    void StopPlayout();
    
    void StartRecording();
    void StopRecording();
    
    AudioUnitBufferCallback playcallback;
    AudioUnitBufferCallback recordcallback;
private:
    // Activates our audio session, creates and initializes the voice-processing
    // audio unit and verifies that we got the preferred native audio parameters.
    bool InitPlayOrRecord();
    
    // Closes and deletes the voice-processing I/O unit.
    void ShutdownPlayOrRecord();
    
    // Helper method for destroying the existing audio unit.
    void DisposeAudioUnit();
    
    // Callback function called on a real-time priority I/O thread from the audio
    // unit. This method is used to signal that recorded audio is available.
    static OSStatus RecordedDataIsAvailable(
                                            void* in_ref_con,
                                            AudioUnitRenderActionFlags* io_action_flags,
                                            const AudioTimeStamp* time_stamp,
                                            UInt32 in_bus_number,
                                            UInt32 in_number_frames,
                                            AudioBufferList* io_data);
    OSStatus OnRecordedDataIsAvailable(
                                       AudioUnitRenderActionFlags* io_action_flags,
                                       const AudioTimeStamp* time_stamp,
                                       UInt32 in_bus_number,
                                       UInt32 in_number_frames);
    
    // Callback function called on a real-time priority I/O thread from the audio
    // unit. This method is used to provide audio samples to the audio unit.
    static OSStatus GetPlayoutData(void* in_ref_con,
                                   AudioUnitRenderActionFlags* io_action_flags,
                                   const AudioTimeStamp* time_stamp,
                                   UInt32 in_bus_number,
                                   UInt32 in_number_frames,
                                   AudioBufferList* io_data);
    OSStatus OnGetPlayoutData(AudioUnitRenderActionFlags* io_action_flags,
                              UInt32 in_number_frames,
                              AudioBufferList* io_data);
    void SetupAudioBuffersForActiveAudioSession();
    bool SetupAndInitializeVoiceProcessingAudioUnit();
    
private:
    AudioUnit vpio_unit_;
    
    
    
    // Extra audio buffer to be used by the playout side for rendering audio.
    // The buffer size is given by FineAudioBuffer::RequiredBufferSizeBytes().
    
    // Provides a mechanism for encapsulating one or more buffers of audio data.
    // Only used on the recording side.
    AudioBufferList audio_record_buffer_list_;
    
    // Temporary storage for recorded data. AudioUnitRender() renders into this
    // array as soon as a frame of the desired buffer size has been recorded.
    
    // Set to 1 when recording is active and 0 otherwise.
    volatile int recording_;
    
    // Set to 1 when playout is active and 0 otherwise.
    volatile int playing_;
    
    // Set to true after successful call to Init(), false otherwise.
    bool initialized_;
    
    // Set to true after successful call to InitRecording(), false otherwise.
    bool rec_is_initialized_;
    
    // Set to true after successful call to InitPlayout(), false otherwise.
    bool play_is_initialized_;
    
    
    // Contains the audio data format specification for a stream of audio.
    AudioStreamBasicDescription application_format_;
    
    int sample_rate_;
    int channels_;
    int ByteSizePerInputPacket_;
    int ByteSizePerOutputPacket_;

    char m_inputbuf[4096*2];
    int inputpos_;
    char m_outputbuf[4096*2];
    int outputpos_;
    std::atomic<int> m_data_len;
};
#endif /* AudioUnitDevice_hpp */
