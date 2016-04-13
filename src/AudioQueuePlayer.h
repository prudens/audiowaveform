//
//  AudioQueuePlayer.hpp
//  audio_engine
//
//  Created by zhangng on 16/4/5.
//  Copyright © 2016年 snailgame. All rights reserved.
//

#ifndef AudioQueuePlayer_hpp
#define AudioQueuePlayer_hpp
#include <AudioToolbox/AudioToolbox.h>
#include <stdio.h>
#include <functional>
#define NumberOfAudioDataBuffers 5
typedef std::function<void(AudioQueueBufferRef buffer, AudioStreamBasicDescription audioFormat)> AudioBufferPlayerBlock;
typedef std::function<void(AudioQueueBufferRef buffer)> AudioBufferPlayerBlock2;
typedef std::function<void(AudioQueueBufferRef buffer,AudioStreamBasicDescription audioformat)> AudioBufferRecordBlock;
class CAudioQueuePlayer
{
public:
    CAudioQueuePlayer();
    void InitPlayout(int sampleRate, int channels, int bitsPerChannel,int packetsPerBuffer);
    void InitRecord(int sampleRate, int channels, int bitsPerChannel,int packetsPerBuffer);
    void StartPlay();
    void StartRecord();
    void StopPlay();
    void StopRecord();
    
    void setUpPlayAudio();
    void setUpRecordAudio();
    
    void InitPlayQueue();
    void InitRecordQueue();
    void UnInitPlayQueue();
    void UnInitRecordQueue();
    void tearDownPlayAudio();
    void tearDownRecordAudio();
    
    void setUpPlayQueueBuffers();
    void primePlayQueueBuffers();
    void setUpRecordQueueBuffers();
    void primeRecordQueueBuffers();
    
    void setUpAudioSession();
    void tearDownAudioSession();
    
    AudioBufferPlayerBlock playblock;
    AudioBufferRecordBlock recordblock;
    bool playing;
    bool recording;

    AudioStreamBasicDescription playaudioFormat;
    AudioStreamBasicDescription recordaudioFormat;
    AudioQueueRef _playQueue;
    AudioQueueRef _recordQueue;
    // the audio queue buffers for the playback audio queue
    AudioQueueBufferRef _playQueueBuffers[NumberOfAudioDataBuffers];
    AudioQueueBufferRef _recordQueueBuffers[NumberOfAudioDataBuffers];
    // the number of audio data packets to use in each audio queue buffer
    int _playpacketsPerBuffer;
    int _recordpacketsPerBuffer;
    // the number of bytes to use in each audio queue buffer
    int _playbytesPerBuffer;
    int _recordbytesPerBuffer;
    float gain;
};

class CAudioQueuePlayer2
{
public:
    CAudioQueuePlayer2();
    void InitPlayout(int sampleRate, int channels, int bitsPerChannel,int framesize);
    void StartPlay();
    void StopPlay();
    
    AudioBufferPlayerBlock2 _playblock;
    bool          _playing;
    AudioQueueRef _playQueue;
    AudioQueueBufferRef _playQueueBuffers[NumberOfAudioDataBuffers];
};

#endif /* AudioQueuePlayer_hpp */
