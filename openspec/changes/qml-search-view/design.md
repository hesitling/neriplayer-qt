## Context

QeriPlayer Qt's Phase 5 builds QML UI. PRs 0–2 are complete:
- **PR 0**: QmlTask integration — VM one-shot methods return `QCoro::QmlTask`
- **PR 1**: QML shell — `main.qml` with Sidebar, StackView, PlayerBar, placeholder pages
- **PR 2**: PlayerBar — transport controls, seek, volume bound to `playerVm`

The `SearchViewModel` is fully implemented with: `query` (debounced), `selectedPlatform`, `results` (`SongListModel`), `isLoading`, `hasMore`, `hasError`, `error`, `availablePlatforms`, `search()`, `loadMore()`. Navigation is wired: sidebar clicks → `mainVm.navigateTo(view)` → StackView replaces page.

PR 3 builds the first content page: SearchView.

## Goals / Non-Goals

**Goals:**
- Functional search page with query input, platform filter, results list
- Reusable `SongDelegate` for search and future playlist views
- Clean QML ↔ VM binding using existing property/signal patterns
- Infinite scroll for pagination
- Proper empty, loading, and error states

**Non-Goals:**
- Context menus on song items (deferred to dialogs PR)
- Search history, suggestions, autocomplete
- Album/artist search types (song-only)
- Keyboard shortcut for instant search (500ms debounce is acceptable)

## Decisions

### D1: Platform filter — add `selectPlatformByName()` to VM

`MusicPlatform` is `Q_DECLARE_METATYPE` but not `Q_ENUM`'d, so QML can't access enum values by name. The `availablePlatforms` Q_PROPERTY returns `QVariantList` of display strings ("NetEase", "Bilibili").

**Decision**: Add `Q_INVOKABLE void selectPlatformByName(const QString &name)` to `SearchViewModel`. Iterates `m_plugins`, finds matching `platformName()`, calls `setSelectedPlatform()`.

**Alternatives considered:**
- JS enum mapping in QML — fragile, duplicates C++ constants
- `Q_INVOKABLE selectPlatformByIndex(int)` — index fragile if plugin order changes
- Create a `PlatformModel` — over-engineered for 3-4 items

### D2: Play signal — add `playSong(int)` to VM

QML can connect to C++ signals but cannot emit them. To trigger `requestPlay(const Song&)` from QML, we need a Q_INVOKABLE method.

**Decision**: Add `Q_INVOKABLE void playSong(int index)` that calls `m_results->songAt(index)` and emits `requestPlay(song)`. The existing `MainViewModel::connectSignals()` wiring handles the rest.

**Flow:**
```
QML double-click → searchVm.playSong(index)
                  → emits requestPlay(song)
                  → MainViewModel connection
                  → playerVm.play(song)
```

### D3: Debounce — 500ms, direct binding

The TextField binds directly to `searchVm.query`. The VM's debounce timer handles rate-limiting.

**Decision**: Change timer interval from 300ms to 500ms. Direct binding, no explicit search button. Enter key accepts 500ms delay (not worth a separate `searchNow()` method).

### D4: SongDelegate — model-role binding, not explicit properties

`SongListModel` exposes roles (`name`, `artist`, `album`, `durationMs`, `coverUrl`, `platform`, `isPlaying`) via `roleNames()`. QML ListView delegates access these as `model.name`, `model.artist`, etc.

**Decision**: `SongDelegate` reads from `model.*` roles directly. No explicit property declarations needed — it works with any `SongListModel`-backed view (search results, playlist detail, queue).

**Playing highlight**: Compare `model.id === playerVm.currentSong.id` in the delegate. No need for `SongListModel::setPlayingIndex()` in search context.

### D5: Infinite scroll — `onAtYEndChanged`

The VM's `loadMore()` returns `QmlTask` and guards against concurrent calls (`if (m_isLoading) return`).

**Decision**: Use `ListView.onAtYEndChanged` to trigger `loadMore()` when scrolled to bottom. Footer shows `BusyIndicator` when `searchVm.hasMore && searchVm.isLoading`.

### D6: Error display — Toast via main.qml wiring

The existing pattern wires `playerVm.errorChanged` → `toast.show()` in `main.qml`.

**Decision**: Add a `Connections` block in `main.qml` for `searchVm.errorChanged` → `toast.show()`. Keeps error display consistent and SearchView free of error UI concerns.

## Risks / Trade-offs

| Risk | Mitigation |
|------|------------|
| `selectPlatformByName` string matching is case-sensitive | Use `Qt::CaseInsensitive` comparison or exact match from `availablePlatforms` list (which comes from the VM) |
| Rapid scroll triggers many `loadMore()` calls | VM already guards with `m_isLoading` check; only one request in flight at a time |
| `model.platform` is raw `uint8_t` in QML (not enum name) | JS helper `platformDisplayName(int)` maps enum values to strings; acceptable for 3-4 values |
| SongDelegate reused in PR 4 — layout may need adjustment | Keep delegate focused on display; no actions (context menu) in this PR |
