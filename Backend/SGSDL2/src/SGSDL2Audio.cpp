//
//  SGSDK2Audio.cpp
//  sgsdl2
//
//  Created by Andrew Cain on 28/11/2013.
//  Copyright (c) 2013 Andrew Cain. All rights reserved.
//

#include "SGSDL2Audio.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "sgBackendUtils.h"

#define SG_MAX_CHANNELS 64

static Mix_Chunk * _sgsdl2_sound_channels[SG_MAX_CHANNELS];
static Mix_Music * _current_music;


void sgsdl2_init_audio()
{
    Mix_Init(~0);
}

void sgsdl2_open_audio()
{
    if ( Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0 )
    {
        set_error_state("Unable to load audio. Mix_OpenAudio failed.");
        return;
    }

    Uint16 format;
    Mix_QuerySpec(&_sgsdk_system_data.audio_specs.audio_rate, &format, &_sgsdk_system_data.audio_specs.audio_channels);
    _sgsdk_system_data.audio_specs.times_opened++;
    _sgsdk_system_data.audio_specs.audio_format = format;
    
    Mix_AllocateChannels(SG_MAX_CHANNELS);
}

void sgsdl2_close_audio()
{
    Mix_CloseAudio();
    _sgsdk_system_data.audio_specs.times_opened--;
    if ( 0 == _sgsdk_system_data.audio_specs.times_opened )
    {
        sg_audiospec empty = { 0 };
        _sgsdk_system_data.audio_specs = empty;
    }
}

int sgsdl2_get_channel(sg_sound_data *sound)
{
    for (int i = 0; i < SG_MAX_CHANNELS; i++)
    {
        if ( _sgsdl2_sound_channels[i] == sound->data && Mix_Playing(i) )
        {
            return i;
        }
    }
    return -1;
}



sg_sound_data sgsdl2_load_sound_data(const char * filename, sg_sound_kind kind)
{
    sg_sound_data result = { SGSD_UNKNOWN, NULL } ;
    
    result.kind = kind;
    
    switch (kind)
    {
        case SGSD_SOUND_EFFECT:
        {
            result.data = Mix_LoadWAV(filename);
            break;
        }
        case SGSD_MUSIC:
        {
            result.data = Mix_LoadMUS(filename);
            break;
        }
        default:
            break;
    }
    
    return result;
}

void sgsdl2_close_sound_data(sg_sound_data * sound )
{
    if ( ! sound ) return;
    
    switch (sound->kind)
    {
        case SGSD_MUSIC:
            Mix_FreeMusic((Mix_Music*)sound->data);
            break;
        
        case SGSD_SOUND_EFFECT:
            Mix_FreeChunk((Mix_Chunk*)sound->data);
            break;
            
        default:
            break;
    }
    
    sound->kind = SGSD_UNKNOWN;
    sound->data = NULL;
}

void sgsdl2_play_sound(sg_sound_data * sound, int loops, float volume)
{
    if ( (!sound) || (!sound->data) ) return;
    
    switch (sound->kind)
    {
        case SGSD_SOUND_EFFECT:
        {
            Mix_Chunk *effect = (Mix_Chunk*) sound->data;
            int channel = Mix_PlayChannel( -1, effect, loops);
            if (channel >= 0 && channel < SG_MAX_CHANNELS)
            {
                Mix_Volume(channel, (int)(volume * MIX_MAX_VOLUME));
                _sgsdl2_sound_channels[channel] = effect;   // record which channel is playing the effect
            }
            break;
        }
        case SGSD_MUSIC:
        {
            Mix_PlayMusic((Mix_Music *)sound->data, loops);
            Mix_VolumeMusic((int)volume * MIX_MAX_VOLUME);
            _current_music = (Mix_Music *)sound->data;
            break;
        }
        default:
            break;
    }
}

float sgsdl2_sound_playing(sg_sound_data * sound)
{
    if ( ! sound ) {
        return 0.0f;
    }
    
    switch (sound->kind)
    {
        case SGSD_SOUND_EFFECT:
        {
            int idx = sgsdl2_get_channel(sound);
            return ( idx >= 0 && idx < SG_MAX_CHANNELS ? 1.0f : 0.0f );
        }
        case SGSD_MUSIC:
        {
            if ( _current_music == sound->data && Mix_PlayingMusic() ) return 1.0f;
        }
            
        default:
            break;
    }
    
    return 0.0f;
}

void sgsdl2_fade_in(sg_sound_data *sound, int loops, int ms)
{
    if ( !sound ) return;
    
    switch (sound->kind)
    {
        case SGSD_SOUND_EFFECT:
        {
            int channel;
            channel = Mix_FadeInChannel(-1, (Mix_Chunk *)sound->data, loops, ms);
            if ( channel >= 0 && channel < SG_MAX_CHANNELS )
            {
                _sgsdl2_sound_channels[channel] = (Mix_Chunk *)sound->data;
            }
            break;
        }
            
        case SGSD_MUSIC:
        {
            Mix_FadeInMusic((Mix_Music *)sound->data, loops, ms);
            _current_music = (Mix_Music *)sound->data;
        }
            
        default:
            break;
    }
}

void sgsdl2_fade_out(sg_sound_data *sound, int ms)
{
    if ( !sound ) return;
    
    switch (sound->kind)
    {
        case SGSD_SOUND_EFFECT:
        {
            int channel = sgsdl2_get_channel(sound);
            Mix_FadeOutChannel(channel, ms);
            break;
        }
            
        case SGSD_MUSIC:
        {
            if ( _current_music == sound->data )
                Mix_FadeOutMusic(ms);
        }
            
        default:
            break;
    }
}

void sgsdl2_fade_all_sound_effects_out(int ms)
{
    Mix_FadeOutChannel(-1, ms);
}

void sgsdl2_fade_music_out(int ms)
{
    Mix_FadeOutMusic(ms);
}

void sgsdl2_set_music_vol(float vol)
{
    Mix_VolumeMusic( (int) MIX_MAX_VOLUME * vol );
}

float sgsdl2_music_vol()
{
    return Mix_VolumeMusic(-1) / (float)MIX_MAX_VOLUME;
}

float sgsdl2_sound_volume(sg_sound_data *sound)
{
    if ( ! sound ) return 0.0f;
    
    switch (sound->kind)
    {
        case SGSD_MUSIC:
            if ( _current_music == sound->data ) return sgsdl2_music_vol();
            break;
        case SGSD_SOUND_EFFECT:
            return Mix_VolumeChunk((Mix_Chunk *)sound->data, -1) / (float)MIX_MAX_VOLUME;
        default:
            break;
    }
    
    return 0.0f;
}

void sgsdl2_set_sound_volume(sg_sound_data *sound, float vol)
{
    if ( !sound ) return;
    
    switch (sound->kind)
    {
        case SGSD_MUSIC:
            if ( _current_music == sound->data )
                sgsdl2_set_music_vol(vol);
            break;
            
        case SGSD_SOUND_EFFECT:
            Mix_VolumeChunk((Mix_Chunk *)sound->data, (int)(vol * MIX_MAX_VOLUME));
        
        default:
            break;
    }
}

void sgsdl2_pause_music()
{
    Mix_PauseMusic();
}

void sgsdl2_resume_music()
{
    if ( Mix_PausedMusic() )
    {
        Mix_ResumeMusic();
    }
}

void sgsdl2_stop_music()
{
    Mix_HaltMusic();
}

void sgsdl2_stop_sound(sg_sound_data *sound)
{
    if ( ! sound ) return;
    
    switch (sound->kind)
    {
        case SGSD_MUSIC:
            if ( _current_music == sound->data ) sgsdl2_stop_music();
            break;
        
        case SGSD_SOUND_EFFECT:
        {
            for (int i = 0; i < SG_MAX_CHANNELS; i++)
            {
                if ( _sgsdl2_sound_channels[i] == sound->data )
                {
                    Mix_HaltChannel(i);
                }
            }
            break;
        }
        default:
            break;
    }
}

void sgsdl2_load_audio_fns(sg_interface *functions)
{
    functions->audio.open_audio = & sgsdl2_open_audio;
    functions->audio.close_audio = & sgsdl2_close_audio;
    functions->audio.load_sound_data = & sgsdl2_load_sound_data;
    functions->audio.play_sound = & sgsdl2_play_sound;
    functions->audio.close_sound_data = & sgsdl2_close_sound_data;
    functions->audio.sound_playing = &sgsdl2_sound_playing;
    functions->audio.fade_in = &sgsdl2_fade_in;
    functions->audio.fade_out = &sgsdl2_fade_out;
    functions->audio.fade_music_out = &sgsdl2_fade_music_out;
    functions->audio.fade_all_sound_effects_out = &sgsdl2_fade_all_sound_effects_out;
    functions->audio.set_music_vol = &sgsdl2_set_music_vol;
    functions->audio.music_vol = &sgsdl2_music_vol;
    functions->audio.sound_volume = &sgsdl2_sound_volume;
    functions->audio.set_sound_volume = &sgsdl2_set_sound_volume;
    functions->audio.pause_music =  & sgsdl2_pause_music;
    functions->audio.resume_music = & sgsdl2_resume_music;
    functions->audio.stop_music =   & sgsdl2_stop_music;
    functions->audio.stop_sound =   & sgsdl2_stop_sound;
}

