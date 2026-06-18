## Context

Phase 4 ViewModels are complete — `PlayerViewModel` exposes all playback state via `Q_PROPERTY` and control methods via `Q_INVOKABLE`. The QML infrastructure (PR 1) is in place: `main.qml` has the shell layout with sidebar, StackView, and a placeholder PlayerBar rectangle. `playerVm` is registered as a QML context property.

**Blocker**: `Song` is a plain struct with `Q_DECLARE_METATYPE` but no `Q_GADGET`. QML cannot access `song.name` or `song.artist` — it only sees an opaque `QVariant`. This blocks every QML component that displays song data.

## Goals / Non-Goals

**Goals:**
- Make Song QML-accessible via `Q_GADGET` + `Q_PROPERTY(READ)` — zero C++ API breakage
- Build PlayerBar with Spotify-style layout: transport controls + seek bar (center), now-playing info (left), volume (right)
- Provide toast notifications for player errors
- Add keyboard shortcuts: Space (play/pause), Left/Right (seek ±5s)

**Non-Goals:**
- Queue UI (panel, drag-to-reorder)
- Lyrics display (deferred to PR 6)
- System tray integration (deferred to Phase 7)
- Animations and transitions (deferred to Phase 7)
- Proper QML enum registration (`Q_ENUM`) — use integer comparisons for now

## Decisions

### D1: Song Q_GADGET with READ getters

**Decision**: Add `Q_GADGET` macro and `Q_PROPERTY(Type name READ getName CONSTANT)` for 7 fields to `Song.h`. Getters use `get` prefix (e.g., `getId()`, `getName()`) and are `const`-qualified.

**Why READ + CONSTANT over MEMBER**: MEMBER makes QML properties read-write, which would let QML accidentally mutate a Song value type. READ + CONSTANT makes properties explicitly read-only. Song remains a plain struct with public members — C++ code still uses `song.name` directly. The `get` prefix avoids shadowing the member names.

**Alternative considered — Convenience properties on PlayerViewModel** (`currentSongName`, `currentSongArtist`, etc.): Rejected because it duplicates for every ViewModel that exposes Song (SearchViewModel, PlaylistViewModel, etc.) and doesn't scale.

**Alternative considered — Wrapper Q_GADGET**: Rejected as unnecessary indirection. Song is already a value type.

**Fields to expose**:

| Field | Type | Used by |
|-------|------|---------|
| `id` | QString | Search, Playlists |
| `name` | QString | PlayerBar, Search, Playlists |
| `artist` | QString | PlayerBar, Search, Playlists |
| `album` | QString | Search, Playlists |
| `durationMs` | qint64 | PlayerBar, Search, Playlists |
| `coverUrl` | QUrl | PlayerBar, Search, Playlists |
| `platform` | MusicPlatform | Search, Playlists |

### D2: Spotify-style PlayerBar layout

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ 80px                                                                     │
├────────────┬──────────────────────────────────────┬──────────────────────┤
│ NowPlaying │ TransportControls                    │ VolumeControl        │
│ Info       │                                      │                      │
│            │  ⏮  ▶  ⏭  🔀  🔁                  │  🔊 ────             │
│ [art] Title│       ─────●─────  0:42 / 3:18       │                      │
│      Artist│                                      │                      │
└────────────┴──────────────────────────────────────┴──────────────────────┘
```

**Why Spotify style**: Center section groups related controls (transport + seek). Now-playing info is glanceable on the left. Volume is independent on the right. This is the most common layout in modern music players.

**Alternative — Horizontal all-in-one row**: Puts seek bar alongside transport controls. Rejected because it crams too much into one row at 80px height.

### D3: Seek slider drag-latch

**Problem**: While user drags the slider, `positionMs` updates from the player and snaps the slider back.

**Solution**: A `seeking` boolean flag. When user presses the slider, `seeking = true` and the slider value stops following `positionMs`. On release, `seeking = false` and `playerVm.seek(value)` fires once.

```qml
Slider {
    id: seekSlider
    property bool seeking: false
    value: seeking ? value : playerVm.positionMs
    onMoved: { seeking = false; playerVm.seek(value) }
    onPressedChanged: if (pressed) seeking = true
}
```

### D4: Toast component for errors

**Decision**: Standalone `Toast.qml` component, anchored above PlayerBar in `main.qml`.

**Why not inline in PlayerBar**: Toast is reusable — login errors (Settings), network errors (Search) will need it later. Placing it in `main.qml` as an overlay makes it a shared utility.

**Behavior**: Slides in from bottom, auto-dismisses after 4 seconds, manual dismiss via close button.

### D5: Keyboard shortcuts in main.qml

**Decision**: `Shortcut` elements in `main.qml` (not PlayerBar) so they work regardless of which view is active.

| Key | Action |
|-----|--------|
| Space | Toggle play/pause |
| Left | Seek backward 5s |
| Right | Seek forward 5s |

**Why 5 seconds**: Music player (not video). 5s is a reasonable default. Configurable values deferred to settings.

### D6: Integer comparisons for enums in QML

**Decision**: Compare `repeatMode` and `playbackState` against integer values (0, 1, 2...) in QML.

**Why not Q_ENUM**: Proper enum registration requires a QObject holder class, registration with `qmlRegisterUncreatableType`, and affects all PRs. Deferring to Phase 7 keeps PR 2 focused.

**Risk**: Magic numbers in QML. Mitigated by comments and the enum definitions in `Enums.h`.

### D7: Cover image placeholder

**Decision**: Show a Material icon (`music_note`) when `coverUrl` is empty.

```qml
Image {
    source: coverUrl
    visible: status === Image.Ready
}
Image {
    source: "image://icon/music_note"
    visible: coverUrl.toString() === ""
}
```

## Risks / Trade-offs

**[Risk] Song Q_GADGET may affect moc processing** → Song.h is already in the build. Adding Q_GADGET triggers moc to generate a meta-object. A `Song.cpp` compilation unit with `#include "moc_Song.cpp"` is needed for moc output. Verify that moc picks up the header correctly and the build succeeds with no warnings.

**[Risk] Integer enum comparisons are fragile** → If enum values change in C++, QML comparisons break silently. Mitigation: Add comments referencing `Enums.h`, and Phase 7 will add proper `Q_ENUM` registration.

**[Trade-off] Toast positioning** → Anchoring above PlayerBar means it overlaps content in the StackView. Acceptable for a 4-second auto-dismiss notification. Could move to top-right in Phase 7 if needed.
