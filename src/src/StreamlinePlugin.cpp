/*
 * Copyright (C) 2019 Space Ape Games
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstring>
#include <dlfcn.h>
#include <android/log.h>

#include "unity/IUnityInterface.h"
#include "unity/IUnityProfilerCallbacks.h"
#include "gator/streamline_annotate.h"
#include "gator/streamline_annotate.c"

static IUnityProfilerCallbacks* s_UnityProfilerCallbacks = NULL;
const UnityProfilerMarkerDesc* s_DefaultMarkerDesc = NULL;

static int GetEventColor(UnityProfilerMarkerFlags flags)
{
    if(flags & kUnityProfilerMarkerFlagScriptUser)
        return ANNOTATE_BLUE;

    if(flags & kUnityProfilerMarkerFlagWarning)
        return ANNOTATE_YELLOW;

    if(flags & kUnityProfilerMarkerFlagVerbosityDebug)
        return ANNOTATE_LTGRAY;

    if(flags & kUnityProfilerMarkerFlagVerbosityInternal)
        ANNOTATE_DKGRAY;

    if(flags & kUnityProfilerMarkerFlagVerbosityAdvanced)
        ANNOTATE_BLACK;

    return ANNOTATE_GREEN;
}

static void BeginSingleEvent(const UnityProfilerMarkerDesc* eventDesc, unsigned short eventDataCount, const UnityProfilerMarkerData* eventData)
{
    const int color = GetEventColor(eventDesc->flags);

    if (eventDataCount > 1 && eventDesc == s_DefaultMarkerDesc)
    {
        // Profiler.Default marker emits UTF16 string as the second metadata parameter.
        // For simplicity we slice UTF16 data to char.
        uint32_t size = eventData[1].size;
        const uint16_t* first = static_cast<const uint16_t*>(eventData[1].ptr);
        const uint16_t* last = reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(eventData[1].ptr) + size);
        const int length = size / 2 + 1;
        char newName[length];

        for (int i = 0; first < last && i < length; ++first)
        {
            newName[i++] = static_cast<char>(*first);
        }
        newName[length - 1] = 0;

        ANNOTATE_MARKER_COLOR_STR(color, newName);
    }
    else
    {
        ANNOTATE_MARKER_COLOR_STR(color, eventDesc->name);
    }
}

static void BeginMarkerEvent(const UnityProfilerMarkerDesc* eventDesc, unsigned short eventDataCount, const UnityProfilerMarkerData* eventData)
{
    const int color = GetEventColor(eventDesc->flags);

    if (eventDataCount > 1 && eventDesc == s_DefaultMarkerDesc)
    {
        // Profiler.Default marker emits UTF16 string as the second metadata parameter.
        // For simplicity we slice UTF16 data to char.
        uint32_t size = eventData[1].size;
        const uint16_t* first = static_cast<const uint16_t*>(eventData[1].ptr);
        const uint16_t* last = reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(eventData[1].ptr) + size);
        const int length = size / 2 + 1;
        char newName[length];

        for (int i = 0; first < last && i < length; ++first)
        {
            newName[i++] = static_cast<char>(*first);
        }
        newName[length - 1] = 0;

        ANNOTATE_COLOR(color, newName);
    }
    else
    {
        ANNOTATE_COLOR(color, eventDesc->name);
    }
}

static void EndMarkerEvent(const UnityProfilerMarkerDesc* eventDesc, unsigned short eventDataCount, const UnityProfilerMarkerData* eventData)
{
    ANNOTATE_END();
}

static void UNITY_INTERFACE_API StreamlineEventCallback(const UnityProfilerMarkerDesc* eventDesc, UnityProfilerMarkerEventType eventType, unsigned short eventDataCount, const UnityProfilerMarkerData* eventData, void* userData)
{
    switch (eventType)
    {
        case kUnityProfilerMarkerEventTypeSingle:
        {
            BeginSingleEvent(eventDesc, eventDataCount, eventData);
            break;
        }

        case kUnityProfilerMarkerEventTypeBegin:
        {
            BeginMarkerEvent(eventDesc, eventDataCount, eventData);
            break;
        }

        case kUnityProfilerMarkerEventTypeEnd:
        {
            EndMarkerEvent(eventDesc, eventDataCount, eventData);
            break;
        }
    }
}

static void UNITY_INTERFACE_API StreamlineCreateEventCallback(const UnityProfilerMarkerDesc* eventDesc, void* userData)
{
    // Special handling for Profiler.Default markers
    if (!s_DefaultMarkerDesc)
    {
        if (!strcmp(eventDesc->name, "Profiler.Default"))
            s_DefaultMarkerDesc = eventDesc;
    }

    s_UnityProfilerCallbacks->RegisterMarkerEventCallback(eventDesc, StreamlineEventCallback, NULL);
}

extern "C" void UNITY_INTERFACE_API MarkStreamlineRegion(const char* name)
{
    ANNOTATE_MARKER_COLOR_STR(ANNOTATE_RED, name);
}

extern "C" void UNITY_INTERFACE_API InitStreamlineUnityPlugin()
{
    __android_log_print(ANDROID_LOG_DEBUG, "StreamlineUnity", "Unity Streamline integration plugin init finished");
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    ANNOTATE_SETUP;

    s_UnityProfilerCallbacks = unityInterfaces->Get<IUnityProfilerCallbacks>();
    s_UnityProfilerCallbacks->RegisterCreateMarkerCallback(StreamlineCreateEventCallback, NULL);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
    s_UnityProfilerCallbacks->UnregisterCreateMarkerCallback(StreamlineCreateEventCallback, NULL);
    s_UnityProfilerCallbacks->UnregisterMarkerEventCallback(NULL, StreamlineEventCallback, NULL);
}
