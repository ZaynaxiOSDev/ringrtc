/*
 * Copyright 2019-2021 Signal Messenger, LLC
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#ifndef RFFI_API_MEDIA_H__
#define RFFI_API_MEDIA_H__

#include "api/media_stream_interface.h"
#include "media/base/video_broadcaster.h"
#include "pc/video_track_source.h"
#include "rffi/api/rffi_defs.h"

typedef struct {
  uint32_t width;
  uint32_t height;
  webrtc::VideoRotation rotation;
} RffiVideoFrameMetadata;

typedef struct {
  // Passes ownership of the buffer
  void (*onVideoFrame)(void* obj_borrowed, RffiVideoFrameMetadata, webrtc::VideoFrameBuffer* frame_buffer_borrowed);
} VideoSinkCallbacks;

namespace webrtc {
namespace rffi {

// A simple implementation of a VideoSinkInterface which be used to attach to a incoming video
// track for rendering by calling Rust_addVideoSink.
class VideoSink : public rtc::VideoSinkInterface<webrtc::VideoFrame>, public rtc::RefCountInterface {
 public:
  VideoSink(void* obj, VideoSinkCallbacks* cbs);
  ~VideoSink() override;

  void OnFrame(const webrtc::VideoFrame& frame) override;

 private:
  void* obj_;
  VideoSinkCallbacks cbs_;
};

// A simple implementation of a VideoTrackSource which can be used for pushing frames into
// an outgoing video track for encoding by calling Rust_pushVideoFrame.
class VideoSource : public VideoTrackSource {
 public:
  VideoSource();
  ~VideoSource() override;

  void PushVideoFrame(const webrtc::VideoFrame& frame);

 protected:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override {
    return &broadcaster_;
  }

 private:
  rtc::VideoBroadcaster broadcaster_;
};

} // namespace rffi
} // namespace webrtc

// Parses track->id()
// Returns 0 upon failure
RUSTEXPORT uint32_t Rust_getTrackIdAsUint32(webrtc::MediaStreamTrackInterface* track_borrowed_rc);

// Same as AudioTrack::set_enabled
RUSTEXPORT void Rust_setAudioTrackEnabled(webrtc::AudioTrackInterface* track_borrowed_rc, bool);

// Same as VideoTrack::set_enabled
RUSTEXPORT void Rust_setVideoTrackEnabled(webrtc::VideoTrackInterface* track_borrowed_rc, bool);

// Same as VideoTrack::set_content_hint with true == kText and false == kNone
RUSTEXPORT void Rust_setVideoTrackContentHint(webrtc::VideoTrackInterface* track_borrowed_rc, bool);

// Gets the first video track from the stream, or nullptr if there is none.
RUSTEXPORT webrtc::VideoTrackInterface* Rust_getFistVideoTrack(
    webrtc::MediaStreamInterface* track_borrowed_rc);

// Creates an VideoSink to the given track and attaches it to the track to
// get frames from C++ to Rust.
// Passed-in "obj" must live at least as long as the VideoSink,
// which likely means as long as the VideoTrack,
// which likely means as long as the PeerConnection.
RUSTEXPORT void Rust_addVideoSink(
    webrtc::VideoTrackInterface* track_borrowed_rc, void* obj_borrowed, VideoSinkCallbacks* cbs_borrowed);

// Same as VideoSource::PushVideoFrame, to get frames from Rust to C++.
RUSTEXPORT void Rust_pushVideoFrame(webrtc::rffi::VideoSource* source_borrowed_rc, webrtc::VideoFrameBuffer* buffer_borrowed_rc);

// RGBA => I420
RUSTEXPORT webrtc::VideoFrameBuffer* Rust_createVideoFrameBufferFromRgba(
  uint32_t width, uint32_t height, uint8_t* rgba_borrowed);

// I420 => RGBA
RUSTEXPORT void Rust_convertVideoFrameBufferToRgba(
  const webrtc::VideoFrameBuffer* buffer, uint8_t* rgba_out);

// RGBA => I420
RUSTEXPORT webrtc::VideoFrameBuffer* Rust_copyAndRotateVideoFrameBuffer(
    const webrtc::VideoFrameBuffer* buffer_borrowed_rc, webrtc::VideoRotation rotation);


#endif /* RFFI_API_MEDIA_H__ */
