## MODIFIED Requirements

### Requirement: Song model
The system SHALL provide a `Song` struct containing:
- Core identity: `id` (QString), `name` (QString), `artist` (QString), `album` (QString), `albumId` (QString), `durationMs` (qint64, milliseconds), `coverUrl` (QUrl), `mediaUri` (QUrl), `platform` (MusicPlatform enum)
- Lyric matching: `matchedLyric` (QString), `matchedTranslatedLyric` (QString), `matchedLyricSource` (MusicPlatform), `matchedSongId` (QString), `userLyricOffsetMs` (qint64)
- User customizations: `customCoverUrl` (QString), `customName` (QString), `customArtist` (QString)
- Original values: `originalName` (QString), `originalArtist` (QString), `originalCoverUrl` (QString), `originalLyric` (QString), `originalTranslatedLyric` (QString)
- Local file support: `localFileName` (QString), `localFilePath` (QString)
- Platform identifiers: `channelId` (QString), `audioId` (QString), `subAudioId` (QString), `playlistContextId` (QString), `streamUrl` (QString)
- `extra` (QVariantMap for platform-specific fields)

The struct SHALL include the `Q_GADGET` macro and `Q_PROPERTY` declarations with `MEMBER` binding for the following fields to enable QML property access: `id`, `name`, `artist`, `album`, `durationMs`, `coverUrl`, `platform`. All other fields SHALL remain as public members without Q_PROPERTY.

Field names SHALL match Android QeriPlayer's `SongItem` model. All fields SHALL be default-constructible and copyable.

#### Scenario: Construct a Song with all fields
- **WHEN** a Song is constructed with id="1", name="Test", artist="Artist", album="Album", durationMs=180000
- **THEN** the Song SHALL hold all provided values and `platform` SHALL default to `MusicPlatform::Unknown`

#### Scenario: Copy a Song
- **WHEN** a Song is copy-assigned to another Song
- **THEN** all fields in the destination SHALL equal the source

#### Scenario: QML property access
- **WHEN** a Song with name="Test Song", artist="Test Artist", durationMs=180000 is accessed from QML via QVariant
- **THEN** QML SHALL be able to read `.name` as "Test Song", `.artist` as "Test Artist", and `.durationMs` as 180000

#### Scenario: QML property access for coverUrl
- **WHEN** a Song with coverUrl=QUrl("http://example.com/cover.jpg") is accessed from QML via QVariant
- **THEN** QML SHALL be able to read `.coverUrl` as the URL string

#### Scenario: QML property access for platform
- **WHEN** a Song with platform=MusicPlatform::NetEase is accessed from QML via QVariant
- **THEN** QML SHALL be able to read `.platform` as the integer value 1
