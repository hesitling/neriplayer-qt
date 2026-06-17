## Why

Phase 1-3 deliver domain models, repositories, platform APIs, and a playback engine — but they are disconnected. There is no business logic layer that coordinates these components. Without services, the UI (Phase 5) cannot consume the backend, and end-to-end workflows (search → play → save) cannot be tested.

## What Changes

- Add `src/service/` with 4 service classes that wrap existing backend components
- Add `src/viewmodel/` with 5 ViewModel classes that expose Qt properties and signals for UI binding
- Register all services and viewmodels in `ServiceLocator`
- Each service coordinates 1-2 backend modules; each ViewModel wraps 1 service

## Non-goals

- No UI code (Phase 5)
- No download/sync services (Phase 6) — `DownloadService` is stubbed only
- No new platform API clients (Bilibili, YouTube are separate changes)

## Capabilities

### New Capabilities
- `player-service`: Wraps `PlaybackController` + `PlayQueue`, exposes play/pause/next/prev/seek, current song, queue manipulation, volume
- `search-service`: Aggregates search across registered `IMusicPlatformPlugin` instances, returns unified `SearchResult`
- `playlist-service`: Wraps `IPlaylistRepository`, provides playlist CRUD, add/remove songs, import/export
- `auth-service`: Manages per-platform login state, cookie/token persistence via `SecureStorage`, login/logout flows
- `player-viewmodel`: Exposes `PlayerService` state as `Q_PROPERTY` (currentSong, isPlaying, position, duration, volume)
- `search-viewmodel`: Exposes `SearchService` with query, results, platform filter, pagination
- `playlist-viewmodel`: Exposes `PlaylistService` with playlist list, detail, editing state

### Modified Capabilities

None — existing specs are not changing requirements.

## Impact

- **Affected layers**: `src/service/`, `src/viewmodel/` (new), `src/app/` (registration)
- **Dependencies**: Uses existing `PlaybackController`, `IPlaylistRepository`, `IMusicPlatformPlugin`, `SecureStorage`
- **Tests**: New test files in `tests/service/`, `tests/viewmodel/`
