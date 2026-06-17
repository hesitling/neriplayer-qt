## ADDED Requirements

### Requirement: Play a song by domain object
The `PlayerService` SHALL provide `playSong(const Song &song)` as `QCoro::Task<void>`. It SHALL delegate to `PlaybackController::play()`.

#### Scenario: Play a song
- **WHEN** `playSong()` is called with a valid `Song`
- **THEN** the `PlaybackController` starts playback and `isPlaying()` returns true

### Requirement: Playback controls
The `PlayerService` SHALL provide `pause()`, `resume()`, `stop()`, `next()`, `prev()`, `seek(qint64)` that delegate to `PlaybackController` and `PlayQueue`.

#### Scenario: Next song
- **WHEN** `next()` is called and the queue has a next song
- **THEN** the next song starts playing

#### Scenario: Previous song
- **WHEN** `prev()` is called and the queue has a previous song
- **THEN** the previous song starts playing

### Requirement: Current song access
The `PlayerService` SHALL provide `currentSong()` returning `std::optional<Song>` and `isPlaying()` returning `bool`.

#### Scenario: Query current song
- **WHEN** a song is playing
- **THEN** `currentSong()` returns the playing song and `isPlaying()` returns true

### Requirement: Queue management
The `PlayerService` SHALL provide `setQueue(const QVector<Song> &songs, int startIndex)`, `addToQueue(const Song &song)`, `removeFromQueue(int index)`, and `queue()` returning `QVector<Song>`.

#### Scenario: Set queue and play
- **WHEN** `setQueue()` is called with 10 songs and startIndex 3
- **THEN** the queue is set and song at index 3 starts playing

### Requirement: Volume control
The `PlayerService` SHALL provide `setVolume(double)`, `volume()`, `setMuted(bool)`, `isMuted()` delegating to `PlaybackController`.

#### Scenario: Set volume
- **WHEN** `setVolume(0.5)` is called
- **THEN** the playback volume is set to 50%

### Requirement: Service signals
The `PlayerService` SHALL emit `currentSongChanged(const Song &)`, `playbackStateChanged(PlaybackState)`, `positionChanged(qint64)`, `durationChanged(qint64)`, `errorOccurred(const QString &)`.

#### Scenario: State change propagation
- **WHEN** the `PlaybackController` emits `playbackStateChanged(Playing)`
- **THEN** `PlayerService` emits `playbackStateChanged(Playing)`
