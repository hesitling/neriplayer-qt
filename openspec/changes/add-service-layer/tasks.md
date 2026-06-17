## 1. PlayerService

- [ ] 1.1 Create `src/service/PlayerService.h` class declaration
- [ ] 1.2 Implement `PlayerService.cpp`: constructor taking `PlaybackController*`, wire signals
- [ ] 1.3 Implement `playSong()`, `pause()`, `resume()`, `stop()`, `next()`, `prev()`, `seek()`
- [ ] 1.4 Implement `currentSong()`, `isPlaying()`, `volume()`, `setVolume()`, `isMuted()`, `setMuted()`
- [ ] 1.5 Implement queue management: `setQueue()`, `addToQueue()`, `removeFromQueue()`, `queue()`
- [ ] 1.6 Register `PlayerService` in `ServiceLocator`

## 2. SearchService

- [ ] 2.1 Create `src/service/SearchService.h` class declaration
- [ ] 2.2 Implement `SearchService.cpp`: constructor taking `QList<IMusicPlatformPlugin*>`
- [ ] 2.3 Implement `search()` with parallel coroutine calls to all plugins
- [ ] 2.4 Implement `hotSearches()` delegating to platform plugin
- [ ] 2.5 Register `SearchService` in `ServiceLocator`

## 3. PlaylistService

- [ ] 3.1 Create `src/service/PlaylistService.h` class declaration
- [ ] 3.2 Implement `PlaylistService.cpp`: constructor taking `IPlaylistRepository*`
- [ ] 3.3 Implement `allPlaylists()`, `createPlaylist()`, `deletePlaylist()`
- [ ] 3.4 Implement `addSong()`, `removeSong()`, `playlistDetail()`
- [ ] 3.5 Register `PlaylistService` in `ServiceLocator`

## 4. AuthService

- [ ] 4.1 Create `src/service/AuthService.h` class declaration
- [ ] 4.2 Implement `AuthService.cpp`: constructor taking `SecureStorage*`, `QList<IMusicPlatformPlugin*>`
- [ ] 4.3 Implement `isAuthenticated()`, `authenticatedPlatforms()`, `login()`, `logout()`
- [ ] 4.4 Implement credential persistence: save/restore from `SecureStorage`
- [ ] 4.5 Register `AuthService` in `ServiceLocator`

## 5. PlayerViewModel

- [ ] 5.1 Create `src/viewmodel/PlayerViewModel.h` with Q_PROPERTY declarations
- [ ] 5.2 Implement `PlayerViewModel.cpp`: wire to `PlayerService` signals
- [ ] 5.3 Implement Q_INVOKABLE actions: `play()`, `pause()`, `togglePlayPause()`, `next()`, `prev()`, `seek()`

## 6. SearchViewModel

- [ ] 6.1 Create `src/viewmodel/SearchViewModel.h` with Q_PROPERTY declarations
- [ ] 6.2 Implement `SearchViewModel.cpp`: wire to `SearchService`
- [ ] 6.3 Implement `executeSearch()`, platform filter, loading state

## 7. PlaylistViewModel

- [ ] 7.1 Create `src/viewmodel/PlaylistViewModel.h` with Q_PROPERTY declarations
- [ ] 7.2 Implement `PlaylistViewModel.cpp`: wire to `PlaylistService`
- [ ] 7.3 Implement CRUD actions, selected playlist, loading state

## 8. Tests

- [ ] 8.1 Create `tests/service/TestPlayerService.cpp`
- [ ] 8.2 Create `tests/service/TestSearchService.cpp`
- [ ] 8.3 Create `tests/service/TestPlaylistService.cpp`
- [ ] 8.4 Create `tests/service/TestAuthService.cpp`

## 9. Integration

- [ ] 9.1 Update `NeriPlayerApplication::initializeCoreServices()` to create and register all services
- [ ] 9.2 Update `CMakeLists.txt` with all new source files
- [ ] 9.3 Update `TODO.md` to mark Phase 4 tasks
