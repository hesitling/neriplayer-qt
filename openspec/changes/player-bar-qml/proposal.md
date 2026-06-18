## Why

The PlayerBar is the core playback UI — users need to see what's playing, control transport, seek, and adjust volume. The PlayerViewModel (Phase 4) is already wired up with all necessary properties and signals, but the `Song` domain struct is not QML-accessible (no `Q_GADGET`), which blocks all QML components that display song data.

## What Changes

- **Song Q_GADGET**: Add `Q_GADGET` and `Q_PROPERTY(READ getX, CONSTANT)` macros to `Song` struct so QML can access `song.name`, `song.artist`, `coverUrl`, etc. No C++ API breakage — public members stay as-is.
- **PlayerBar QML components**: Create 4 new QML files — `PlayerBar.qml` (container), `NowPlayingInfo.qml` (album art + song info), `TransportControls.qml` (play/pause, next, prev, shuffle, repeat, seek slider), `VolumeControl.qml` (volume slider + mute toggle).
- **Toast notification**: Reusable `Toast.qml` component for error display (slides in above PlayerBar, auto-dismisses).
- **Keyboard shortcuts**: Space for play/pause, Left/Right arrows for seek (±5 seconds), added to `main.qml`.
- **Integration**: Replace the placeholder PlayerBar in `main.qml` with the real component, wire toast to `playerVm` error signals.

## Non-goals

- Queue UI (drag-to-reorder, queue panel) — deferred to Phase 7
- Lyrics display in PlayerBar — deferred to PR 6 (LyricsDialog)
- System tray integration — deferred to Phase 7
- Animations and transitions — deferred to Phase 7 (Polish)
- Proper enum registration for QML (`Q_ENUM`) — deferred to Phase 7; PR 2 uses integer comparisons

## Capabilities

### New Capabilities
- `ui-player-bar`: Bottom player bar with now-playing info, transport controls, seek slider, volume control, and keyboard shortcuts. Includes toast notification for error display.

### Modified Capabilities
- `core-domain-models`: Song struct gains `Q_GADGET` and `Q_PROPERTY` macros for QML property access (7 fields: `id`, `name`, `artist`, `album`, `durationMs`, `coverUrl`, `platform`).

## Impact

- **domain layer**: `Song.h` — add `Q_GADGET` macro and `Q_PROPERTY` declarations (no member changes)
- **ui layer**: 5 new QML files in `src/qml/`, update `qml.qrc`
- **app layer**: `main.qml` — add keyboard shortcuts, replace placeholder with PlayerBar, wire toast
- **No new dependencies**: Uses existing Qt Quick Controls Material components
- **No API changes**: PlayerViewModel contract unchanged; Song public members unchanged
