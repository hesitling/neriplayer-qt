## ADDED Requirements

### Requirement: Playlist list property
The `PlaylistViewModel` SHALL expose `Q_PROPERTY(QVector<Playlist> playlists READ playlists NOTIFY playlistsChanged)`.

#### Scenario: Playlists loaded
- **WHEN** the ViewModel is constructed
- **THEN** `playlists` is populated from `PlaylistService::allPlaylists()`

### Requirement: Selected playlist property
The `PlaylistViewModel` SHALL expose `Q_PROPERTY(Playlist selectedPlaylist READ selectedPlaylist NOTIFY selectedPlaylistChanged)`.

#### Scenario: Select playlist
- **WHEN** `selectPlaylist(id)` is called
- **THEN** `selectedPlaylist` updates with full song list

### Requirement: Playlist CRUD actions
The `PlaylistViewModel` SHALL expose `Q_INVOKABLE` methods: `createPlaylist(name)`, `deletePlaylist(id)`, `addSong(playlistId, song)`, `removeSong(playlistId, songId)`.

#### Scenario: Create playlist
- **WHEN** `createPlaylist("New")` is called
- **THEN** the playlist is created and `playlistsChanged` fires

### Requirement: Loading state
The `PlaylistViewModel` SHALL expose `Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)`.

#### Scenario: Loading during CRUD
- **WHEN** a CRUD operation starts
- **THEN** `isLoading` becomes true until complete
