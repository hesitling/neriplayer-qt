## ADDED Requirements

### Requirement: SongDelegate displays song metadata
The system SHALL define `SongDelegate.qml` as a reusable list item component that displays song cover art, name, artist, album, platform badge, and duration.

#### Scenario: Full song row displayed
- **WHEN** a SongDelegate is rendered with model data for a song
- **THEN** it SHALL display the cover image, song name, artist name, album name, platform badge, and formatted duration

### Requirement: Cover art with placeholder
The delegate SHALL display a 48×48 rounded cover image from `model.coverUrl`. When the image is unavailable or loading, a music note placeholder icon SHALL be shown.

#### Scenario: Cover image loads successfully
- **WHEN** `model.coverUrl` is a valid image URL
- **THEN** the cover image SHALL be displayed in a 48×48 rounded rectangle

#### Scenario: Cover image unavailable
- **WHEN** `model.coverUrl` is empty or the image fails to load
- **THEN** a music note placeholder (🎵) SHALL be displayed centered in the cover area

### Requirement: Song name and artist display
The song name SHALL be displayed in medium font weight with right-side elision. The artist and album SHALL be displayed below in hint text color with right-side elision.

#### Scenario: Long text elided
- **WHEN** the song name exceeds the available width
- **THEN** the text SHALL be elided at the right edge

### Requirement: Platform badge
A small rounded-rectangle badge SHALL display the platform name (e.g., "NetEase", "Bilibili", "YouTube") mapped from `model.platform`. The badge SHALL be positioned in the right column near the duration.

#### Scenario: NetEase song badge
- **WHEN** `model.platform` is 1 (MusicPlatform::NetEase)
- **THEN** a badge labeled "NetEase" SHALL be displayed

#### Scenario: Unknown platform — no badge
- **WHEN** `model.platform` is 0 (MusicPlatform::Unknown)
- **THEN** no badge SHALL be displayed

### Requirement: Duration display
The song duration SHALL be displayed as `m:ss` format (e.g., "3:42") from `model.durationMs`, right-aligned in the delegate.

#### Scenario: Duration formatted correctly
- **WHEN** `model.durationMs` is 222000 (3 minutes 42 seconds)
- **THEN** the duration label SHALL display "3:42"

### Requirement: Playing highlight
When the delegate's song matches the currently playing song (`model.id === playerVm.currentSong.id`), the delegate SHALL apply an accent tint or highlight to indicate the active song.

#### Scenario: Currently playing song highlighted
- **WHEN** the delegate's `model.id` matches `playerVm.currentSong.id`
- **THEN** the delegate background or text SHALL use an accent color

#### Scenario: Non-playing song — default style
- **WHEN** the delegate's `model.id` does not match `playerVm.currentSong.id`
- **THEN** the delegate SHALL use the default style with no highlight
