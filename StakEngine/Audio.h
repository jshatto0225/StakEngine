#pragma once

struct sound {
    // Sound Data
};

// Audio Devices
const char **GetAvailableAudioDeviceNames();
void SetActiveAudioDevice(const char *DeviceName);

// Sounds
void PlaySound(sound *Sound);
sound LoadSound(const char *FilePath);
