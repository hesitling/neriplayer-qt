## PR 1: PlayerBar (QML)

## 1. Song Q_GADGET

- [x] 1.1 Add `Q_GADGET` macro and `Q_PROPERTY(MEMBER)` declarations to `Song.h` for fields: `id`, `name`, `artist`, `album`, `durationMs`, `coverUrl`, `platform`
- [x] 1.2 Verify build succeeds and `moc` processes `Song.h` correctly (check for moc warnings)

## 2. NowPlayingInfo

- [x] 2.1 Create `src/qml/NowPlayingInfo.qml` with album art `Image` (56x56) and `Column` for song name + artist labels
- [x] 2.2 Add placeholder state: music icon + "No song playing" when `currentSong` is empty or `coverUrl` is empty

## 3. TransportControls

- [x] 3.1 Create `src/qml/TransportControls.qml` with row of `ToolButton`s: shuffle, prev, play/pause (larger), next, repeat
- [x] 3.2 Wire shuffle button to `playerVm.toggleShuffle()` with highlight state on `playerVm.isShuffleEnabled`
- [x] 3.3 Wire repeat button to `playerVm.cycleRepeatMode()` with three-state icon logic (Off=dimmed, One=repeat_one, All=repeat)
- [x] 3.4 Wire play/pause button to `playerVm.pause()` / `playerVm.resume()` based on `playerVm.isPlaying`
- [x] 3.5 Wire prev/next buttons to `playerVm.prev()` / `playerVm.next()`

## 4. SeekSlider

- [x] 4.1 Add seek slider row below transport controls: position `Label`, `Slider`, duration `Label`
- [x] 4.2 Implement drag-latch (`seeking` boolean) to prevent slider snap during user drag
- [x] 4.3 Add `formatTime(ms)` helper function returning "M:SS" or "H:MM:SS" format
- [x] 4.4 Disable slider when `playerVm.durationMs` is 0

## 5. VolumeControl

- [x] 5.1 Create `src/qml/VolumeControl.qml` with mute `ToolButton` and volume `Slider` (0.0–1.0)
- [x] 5.2 Wire mute button to `playerVm.toggleMute()` with icon logic (volume_up/down/mute/off based on level and muted state)
- [x] 5.3 Wire volume slider to `playerVm.volume` (read) and `playerVm.setVolume()` (write on move)

## 6. PlayerBar Container

- [x] 6.1 Create `src/qml/PlayerBar.qml` with `RowLayout`: NowPlayingInfo (left, ~250px), center column (TransportControls + seek bar), VolumeControl (right, ~150px)
- [x] 6.2 Set fixed height 80px, use `Material.dialogColor` or similar dark background
- [x] 6.3 Disable all controls when no song is loaded

## 7. Toast Notification

- [x] 7.1 Create `src/qml/Toast.qml` with slide-in animation, warning icon, message label, close button
- [x] 7.2 Add auto-dismiss timer (4 seconds) and manual dismiss via close button

## 8. Keyboard Shortcuts

- [x] 8.1 Add `Shortcut` for Space → toggle play/pause
- [x] 8.2 Add `Shortcut` for Left arrow → seek backward 5 seconds (clamped to 0)
- [x] 8.3 Add `Shortcut` for Right arrow → seek forward 5 seconds (clamped to durationMs)

## 9. Integration

- [x] 9.1 Replace placeholder PlayerBar `Rectangle` in `main.qml` with `PlayerBar` component
- [x] 9.2 Add `Toast` component to `main.qml` as overlay anchored above PlayerBar
- [x] 9.3 Wire `Connections` on `playerVm.hasErrorChanged` to trigger toast with `playerVm.error.message`
- [x] 9.4 Add keyboard `Shortcut` elements to `main.qml`
- [x] 9.5 Update `src/qml/qml.qrc` to include all new .qml files (PlayerBar, NowPlayingInfo, TransportControls, VolumeControl, Toast)
- [x] 9.6 Verify full build succeeds with no warnings
