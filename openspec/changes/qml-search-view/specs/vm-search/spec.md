## MODIFIED Requirements

### Requirement: Query property with debounce
`SearchViewModel` SHALL expose `query` (QString, read/write, notify: `queryChanged`). Setting `query` SHALL start a 500ms single-shot `QTimer`. When the timer fires, `search()` SHALL be called automatically.

#### Scenario: Debounce triggers search
- **WHEN** `query` is set to "hello"
- **THEN** a 500ms timer SHALL start. After 500ms, `search()` SHALL be called with the current query

#### Scenario: Rapid typing resets debounce
- **WHEN** `query` is set to "h", then "he", then "hel" within 500ms
- **THEN** only one `search()` call SHALL occur (for "hel"), 500ms after the last set

## ADDED Requirements

### Requirement: selectPlatformByName for QML binding
`SearchViewModel` SHALL expose `Q_INVOKABLE void selectPlatformByName(const QString &name)`. It SHALL iterate `m_plugins`, find the plugin whose `platformName()` matches `name` (case-sensitive), and call `setSelectedPlatform()` with the corresponding `MusicPlatform` enum value. If no plugin matches, the method SHALL do nothing.

#### Scenario: Select platform by name
- **WHEN** `selectPlatformByName("NetEase")` is called and a plugin with `platformName() == "NetEase"` exists
- **THEN** `selectedPlatform` SHALL be set to `MusicPlatform::NetEase`

#### Scenario: Unknown platform name ignored
- **WHEN** `selectPlatformByName("UnknownPlatform")` is called
- **THEN** `selectedPlatform` SHALL NOT change

### Requirement: playSong emits requestPlay
`SearchViewModel` SHALL expose `Q_INVOKABLE void playSong(int index)`. It SHALL call `m_results->songAt(index)` and, if the returned song has a non-empty `id`, emit `requestPlay(song)`.

#### Scenario: Valid index emits signal
- **WHEN** `playSong(3)` is called and `results` contains a song at index 3
- **THEN** `requestPlay` SHALL be emitted with that song

#### Scenario: Out-of-bounds index — no signal
- **WHEN** `playSong(999)` is called and `results` has fewer than 1000 songs
- **THEN** `requestPlay` SHALL NOT be emitted
