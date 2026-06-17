## ADDED Requirements

### Requirement: List all playlists
The `PlaylistService` SHALL provide `allPlaylists()` returning `QVector<Playlist>` from `IPlaylistRepository`.

#### Scenario: Get playlists
- **WHEN** `allPlaylists()` is called
- **THEN** all playlists from the repository are returned

### Requirement: Create playlist
The `PlaylistService` SHALL provide `createPlaylist(const QString &name, const QString &description = {})` returning `Playlist`.

#### Scenario: Create new playlist
- **WHEN** `createPlaylist("My Playlist")` is called
- **THEN** a new playlist is created in the repository and returned

### Requirement: Delete playlist
The `PlaylistService` SHALL provide `deletePlaylist(const QString &playlistId)` returning `bool`.

#### Scenario: Delete existing playlist
- **WHEN** `deletePlaylist(id)` is called for an existing playlist
- **THEN** the playlist is removed from the repository and true is returned

### Requirement: Add song to playlist
The `PlaylistService` SHALL provide `addSong(const QString &playlistId, const Song &song)` returning `bool`.

#### Scenario: Add song
- **WHEN** `addSong(playlistId, song)` is called
- **THEN** the song is appended to the playlist

### Requirement: Remove song from playlist
The `PlaylistService` SHALL provide `removeSong(const QString &playlistId, const QString &songId)` returning `bool`.

#### Scenario: Remove song
- **WHEN** `removeSong(playlistId, songId)` is called
- **THEN** the song is removed from the playlist

### Requirement: Get playlist detail
The `PlaylistService` SHALL provide `playlistDetail(const QString &playlistId)` returning `std::optional<Playlist>` with songs loaded.

#### Scenario: Get detail
- **WHEN** `playlistDetail(id)` is called
- **THEN** the playlist with its full song list is returned

### Requirement: Service signals
The `PlaylistService` SHALL emit `playlistsChanged()` when playlists are added/removed, and `playlistContentChanged(const QString &playlistId)` when songs change.

#### Scenario: Signal on create
- **WHEN** `createPlaylist()` is called
- **THEN** `playlistsChanged()` is emitted
