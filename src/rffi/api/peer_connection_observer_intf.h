/*
 * Copyright 2019-2021 Signal Messenger, LLC
 * SPDX-License-Identifier: AGPL-3.0-only
 */

#ifndef RFFI_API_PEER_CONNECTION_OBSERVER_INTF_H__
#define RFFI_API_PEER_CONNECTION_OBSERVER_INTF_H__

#include "api/peer_connection_interface.h"
#include "rffi/api/rffi_defs.h"
#include "rffi/api/network.h"
#include "rtc_base/network_constants.h"

/**
 * Rust friendly wrapper around a custom class that implements the
 * webrtc::PeerConnectionObserver interface.
 *
 */

namespace webrtc {
namespace rffi {
  class PeerConnectionObserverRffi;

  /* NetworkRoute structure passed between Rust and C++ */
  typedef struct {
     rtc::AdapterType local_adapter_type;
  } NetworkRoute;
} // namespace rffi
} // namespace webrtc

/* Peer Connection Observer callback function pointers */
typedef struct {
  // ICE events
  void (*onIceCandidate)(void* observer_borrowed, const RustIceCandidate* candidate_borrowed);
  void (*onIceCandidatesRemoved)(void* observer_borrowed, const webrtc::rffi::IpPort* addresses_borrowed, size_t);
  void (*onIceConnectionChange)(void* observer_borrowed, webrtc::PeerConnectionInterface::IceConnectionState);
  void (*onIceNetworkRouteChange)(void* observer_borrowed, webrtc::rffi::NetworkRoute);

  // Media events
  void (*onAddStream)(void* observer_borrowed, webrtc::MediaStreamInterface* stream_owned_rc);
  void (*onAddAudioRtpReceiver)(void* observer_borrowed, webrtc::MediaStreamTrackInterface* track_owned_rc);
  void (*onAddVideoRtpReceiver)(void* observer_borrowed, webrtc::MediaStreamTrackInterface* track_owned_rc);

  // Data Channel events
  void (*onSignalingDataChannel)(void* observer_borrowed, webrtc::DataChannelInterface* data_channel_owned_rc);
  void (*onSignalingDataChannelMessage)(void* observer_borrowed, const uint8_t* data_borrowed, size_t);
  void (*onRtpReceived)(void* observer_borrowed, uint8_t, uint16_t, uint32_t, uint32_t, const uint8_t* payload_borrowed, size_t);

  // Frame encryption
  size_t (*getMediaCiphertextBufferSize)(void* observer_borrowed, bool, size_t);
  bool (*encryptMedia)(void* observer_borrowed, bool, const uint8_t* plaintext_borrowed, size_t, uint8_t* ciphertext_out, size_t, size_t* ciphertext_size_out);
  size_t (*getMediaPlaintextBufferSize)(void* observer_borrowed, uint32_t, bool, size_t);
  bool (*decryptMedia)(void* observer_borrowed, uint32_t, bool, const uint8_t* ciphertext_borrowed, size_t, uint8_t* plaintext_out, size_t, size_t* plaintext_size_out);
} PeerConnectionObserverCallbacks;

// Passed-in observer must live at least as long as the PeerConnectionObserverRffi,
// which is at least as long as the PeerConnection.
RUSTEXPORT webrtc::rffi::PeerConnectionObserverRffi*
Rust_createPeerConnectionObserver(void*                                  observer_borrowed,
                                  const PeerConnectionObserverCallbacks* callbacks_borrowed,
                                  bool                                   enable_frame_encryption);

RUSTEXPORT void
Rust_deletePeerConnectionObserver(webrtc::rffi::PeerConnectionObserverRffi* observer_owned);
#endif /* RFFI_API_PEER_CONNECTION_OBSERVER_INTF_H__ */
