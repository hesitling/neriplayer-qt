## Context

Phase 1-3 deliver: `Song`/`Playlist` domain models, `SongRepository`/`PlaylistRepository`/`SettingsRepository`/`PlayerStateRepository`/`PlayHistoryRepository`, `NeteaseClient` (IMusicPlatformPlugin), `PlaybackController`/`PlayQueue`/`QtMultimediaBackend`. These are all registered in `ServiceLocator` via `NeriPlayerApplication`.

Currently there is no coordination layer. The `PlaybackController` can play songs but nothing orchestrates search → select → play. The repositories can CRUD but nothing exposes them in a UI-friendly way.

## Goals / Non-Goals

**Goals:**
- Create `PlayerService` that wraps `PlaybackController` and provides a clean API for UI consumption
- Create `SearchService` that queries all registered `IMusicPlatformPlugin` instances
- Create `PlaylistService` that wraps `IPlaylistRepository` with business logic
- Create `AuthService` that manages per-platform login state
- Create ViewModels that expose service state as `Q_PROPERTY` + signals
- All services and viewmodels registered in `ServiceLocator`

**Non-Goals:**
- No UI widgets (Phase 5)
- No download service (Phase 6)
- No new platform clients (separate change)

## Decisions

### 1. Services are thin wrappers, not fat controllers

Each service delegates to 1-2 existing components. `PlayerService` wraps `PlaybackController`, `SearchService` wraps `IMusicPlatformPlugin` instances, `PlaylistService` wraps `IPlaylistRepository`. Services add coordination logic (e.g., search aggregation) but do not duplicate repository or API logic.

**Why:** Keeps services testable and avoids god-objects. The existing components are already well-designed.

### 2. ViewModels use Q_PROPERTY, not raw signals

ViewModels expose `Q_PROPERTY` with `NOTIFY` signals so Qt bindings (and later QML) work. ViewModels own no business logic — they delegate to services and re-expose state.

**Why:** Standard MVVM pattern. `Q_PROPERTY` enables declarative UI binding in Phase 5.

### 3. AuthService stores credentials via SecureStorage

`AuthService` wraps per-platform login flows. Cookies/tokens are persisted in `SecureStorage` (already implemented). On startup, `AuthService` restores login state from storage.

**Why:** `SecureStorage` already handles AES-256-GCM encryption. No new crypto needed.

### 4. SearchService aggregates with parallel coroutines

`SearchService::search()` calls `IMusicPlatformPlugin::search()` on all registered plugins concurrently using `QCoro::Task` and collects results. Failed platform searches are logged and skipped, not fatal.

**Why:** Users expect results from all platforms. A single platform failure should not block others.

## Risks / Trade-offs

- **ViewModel lifetime** — ViewModels must outlive UI but be destroyed before services. Mitigation: `ServiceLocator` destruction order is controlled by `NeriPlayerApplication::~NeriPlayerApplication()`.
- **SearchService parallel failures** — If all platforms fail, return empty result with error. Mitigation: Log each failure, emit `errorOccurred` signal.
- **AuthService complexity** — Each platform has different auth flows. Mitigation: `AuthService` delegates to `IMusicPlatformPlugin::login()` which each client implements.
