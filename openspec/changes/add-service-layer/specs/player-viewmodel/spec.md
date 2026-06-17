## ADDED Requirements

### Requirement: Current song property
The `PlayerViewModel` SHALL expose `Q_PROPERTY(Song currentSong READ currentSong NOTIFY currentSongChanged)`.

#### Scenario: Song changes
- **WHEN** `PlayerService` emits `currentSongChanged`
- **THEN** the `currentSong` property updates and `currentSongChanged` signal fires

### Requirement: Playback state properties
The `PlayerViewModel` SHALL expose `Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY playbackStateChanged)`, `Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)`, `Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)`.

#### Scenario: State change
- **WHEN** playback starts
- **THEN** `isPlaying` becomes true and `playbackStateChanged` fires

### Requirement: Volume property
The `PlayerViewModel` SHALL expose `Q_PROPERTY(double volume READ volume WRITE setVolume NOTIFY volumeChanged)` and `Q_PROPERTY(bool isMuted READ isMuted WRITE setMuted NOTIFY mutedChanged)`.

#### Scenario: Volume change via property
- **WHEN** `setVolume(0.8)` is called on the ViewModel
- **THEN** `PlayerService::setVolume(0.8)` is called

### Requirement: Playback actions
The `PlayerViewModel` SHALL expose `Q_INVOKABLE` methods: `play()`, `pause()`, `togglePlayPause()`, `next()`, `prev()`, `seek(qint64)`.

#### Scenario: Toggle play/pause
- **WHEN** `togglePlayPause()` is called while playing
- **THEN** playback pauses
