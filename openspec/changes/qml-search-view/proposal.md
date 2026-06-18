## Why

Phase 5 builds QML UI for QeriPlayer. PRs 0–2 are complete (QmlTask integration, QML shell, PlayerBar). The search view is the first user-facing content page — it lets users query across platforms and play results. Without it, the sidebar navigation leads to a placeholder.

## What Changes

- **New `SearchView.qml`** — search page with query input, platform filter ComboBox, results ListView with infinite scroll, loading/empty/error states
- **New `SongDelegate.qml`** — reusable song list item (cover art, title, artist, album, platform badge, duration, playing highlight). Reused later in playlist detail views (PR 4)
- **VM additions** — `SearchViewModel` gains `selectPlatformByName(QString)` for QML ComboBox binding, `playSong(int)` to emit `requestPlay` signal from QML, and debounce interval changed from 300ms to 500ms
- **Shell wiring** — `main.qml` replaces search placeholder with SearchView, wires search errors to Toast

## Non-goals

- Context menus on song items (deferred to dialogs PR)
- Search history or recent searches
- Search suggestions / autocomplete
- Album or artist search types (song-only for now)

## Capabilities

### New Capabilities
- `qml-search-view`: QML search page with query input, platform filter, results list, infinite scroll, and state handling
- `qml-song-delegate`: Reusable song row component for ListView delegates across search and playlist views

### Modified Capabilities
- `vm-search`: Add `selectPlatformByName()` and `playSong()` Q_INVOKABLE methods, change debounce to 500ms

## Impact

| Layer | Files |
|-------|-------|
| viewmodel | `SearchViewModel.h/cpp` — add 2 methods, change timer interval |
| ui (qml) | `SearchView.qml`, `SongDelegate.qml` (new), `main.qml`, `qml.qrc` (modified) |
| tests | `tests/viewmodel/TestSearchViewModel.cpp` — tests for new methods |
