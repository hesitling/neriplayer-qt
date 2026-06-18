## PR 1: PlayerBar (QML)

## 1. Song Q_GADGET

- [ ] 1.1 Add `Q_GADGET` macro and `Q_PROPERTY(MEMBER)` declarations to `Song.h` for fields: `id`, `name`, `artist`, `album`, `durationMs`, `coverUrl`, `platform`
- [ ] 1.2 Verify build succeeds and `moc` processes `Song.h` correctly (check for moc warnings)

## 2. NowPlayingInfo

- [ ] 2.1 Create `src/qml/NowPlayingInfo.qml` with album art `Image` (56x56) and `Column` for song name + artist labels
- [ ] 2.2 Add placeholder state: music icon + "No song playing" when `currentSong` is empty or `coverUrl` is empty

## 3. TransportControls

- [ ] 3.1 Create `src/qml/TransportControls.qml` with row of `ToolButton`s: shuffle, prev, play/pause (larger), next, repeat
- [ ] 3.2 Wire shuffle button to `playerVm.toggleShuffle()` with highlight state on `playerVm.isShuffleEnabled`
- [ ] 3.3 Wire repeat button to `playerVm.cycleRepeatMode()` with three-state icon logic (Off=dimmed, One=repeat_one, All=repeat)
- [ ] 3.4 Wire play/pause button to `playerVm.pause()` / `playerVm.resume()` based on `playerVm.isPlaying`
- [ ] 3.5 Wire prev/next buttons to `playerVm.prev()` / `playerVm.next()`

## 4. SeekSlider

- [ ] 4.1 Add seek slider row below transport controls: position `Label`, `Slider`, duration `Label`
- [ ] 4.2 Implement drag-latch (`seeking` boolean) to prevent slider snap during user drag
- [ ] 4.3 Add `formatTime(ms)` helper function returning "M:SS" or "H:MM:SS" format
- [ ] 4.4 Disable slider when `playerVm.durationMs` is 0

## 5. VolumeControl

- [ ] 5.1 Create `src/qml/VolumeControl.qml` with mute `ToolButton` and volume `Slider` (0.0–1.0)
- [ ] 5.2 Wire mute button to `playerVm.toggleMute()` with icon logic (volume_up/down/mute/off based on level and muted state)
- [ ] 5.3 Wire volume slider to `playerVm.volume` (read) and `playerVm.setVolume()` (write on move)

## 6. PlayerBar Container

- [ ] 6.1 Create `src/qml/PlayerBar.qml` with `RowLayout`: NowPlayingInfo (left, ~250px), center column (TransportControls + seek bar), VolumeControl (right, ~150px)
- [ ] 6.2 Set fixed height 80px, use `Material.dialogColor` or similar dark background
- [ ] 6.3 Disable all controls when no song is loaded

## 7. Toast Notification

- [ ] 7.1 Create `src/qml/Toast.qml` with slide-in animation, warning icon, message label, close button
- [ ] 7.2 Add auto-dismiss timer (4 seconds) and manual dismiss via close button

## 8. Keyboard Shortcuts

- [ ] 8.1 Add `Shortcut` for Space → toggle play/pause
- [ ] 8.2 Add `Shortcut` for Left arrow → seek backward 5 seconds (clamped to 0)
- [ ] 8.3 Add `Shortcut` for Right arrow → seek forward 5 seconds (clamped to durationMs)

## 9. Integration

- [ ] 9.1 Replace placeholder PlayerBar `Rectangle` in `main.qml` with `PlayerBar` component
- [ ] 9.2 Add `Toast` component to `main.qml` as overlay anchored above PlayerBar
- [ ] 9.3 Wire `Connections` on `playerVm.hasErrorChanged` to trigger toast with `playerVm.error.message`
- [ ] 9.4 Add keyboard `Shortcut` elements to `main.qml`
- [ ] 9.5 Update `src/qml/qml.qrc` to include all new .qml files (PlayerBar, NowPlayingInfo, TransportControls, VolumeControl, Toast)
- [ ] 9.6 Verify full build succeeds with no warnings
