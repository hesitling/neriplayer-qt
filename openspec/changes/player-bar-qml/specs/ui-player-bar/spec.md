## Purpose

Bottom player bar providing now-playing info, transport controls, seek, volume, keyboard shortcuts, and error toast notifications.

## ADDED Requirements

### Requirement: NowPlayingInfo displays current song
The system SHALL display the current song's album art, name, and artist in the left section of the PlayerBar. When no song is loaded, it SHALL display a placeholder music icon and "No song playing" text.

#### Scenario: Song is playing
- **WHEN** `playerVm.currentSong` has a valid song with name="Test Song", artist="Test Artist", and a valid `coverUrl`
- **THEN** the NowPlayingInfo SHALL show the album art image, "Test Song" as the title, and "Test Artist" as the artist

#### Scenario: No song loaded
- **WHEN** `playerVm.currentSong` is empty (default Song)
- **THEN** the NowPlayingInfo SHALL show a placeholder music icon and "No song playing" text

#### Scenario: Song without cover art
- **WHEN** `playerVm.currentSong` has a valid song but `coverUrl` is empty
- **THEN** the NowPlayingInfo SHALL show a placeholder music icon instead of the album art

### Requirement: TransportControls provide playback actions
The system SHALL display transport control buttons in the center section: previous, play/pause, next, shuffle toggle, and repeat cycle. The play/pause button SHALL be visually prominent (larger than other buttons).

#### Scenario: Play a paused song
- **WHEN** `playerVm.isPlaying` is false and the user clicks the play/pause button
- **THEN** the system SHALL call `playerVm.resume()`

#### Scenario: Pause a playing song
- **WHEN** `playerVm.isPlaying` is true and the user clicks the play/pause button
- **THEN** the system SHALL call `playerVm.pause()`

#### Scenario: Skip to next song
- **WHEN** the user clicks the next button
- **THEN** the system SHALL call `playerVm.next()`

#### Scenario: Skip to previous song
- **WHEN** the user clicks the previous button
- **THEN** the system SHALL call `playerVm.prev()`

#### Scenario: Toggle shuffle
- **WHEN** the user clicks the shuffle button
- **THEN** the system SHALL call `playerVm.toggleShuffle()` and the button SHALL be highlighted when `playerVm.isShuffleEnabled` is true

#### Scenario: Cycle repeat mode
- **WHEN** the user clicks the repeat button
- **THEN** the system SHALL call `playerVm.cycleRepeatMode()` and the button SHALL display the current mode (Off=dimmed, One=repeat_one icon, All=repeat icon highlighted)

### Requirement: SeekSlider allows position seeking
The system SHALL display a seek slider below the transport controls, showing current position and duration labels. The slider SHALL not snap back during user drag.

#### Scenario: Slider follows playback position
- **WHEN** the player is playing and the user is not dragging the slider
- **THEN** the slider value SHALL match `playerVm.positionMs` and the position label SHALL show the formatted current time

#### Scenario: User seeks to a position
- **WHEN** the user drags the slider to a new position and releases
- **THEN** the system SHALL call `playerVm.seek(value)` with the selected position

#### Scenario: Slider does not snap during drag
- **WHEN** the user is dragging the slider and `playerVm.positionMs` updates
- **THEN** the slider value SHALL remain at the drag position, not the playback position

#### Scenario: Duration display
- **WHEN** a song with durationMs=180000 is loaded
- **THEN** the duration label SHALL show "3:00"

### Requirement: VolumeControl allows volume adjustment
The system SHALL display a mute toggle button and a volume slider in the right section of the PlayerBar.

#### Scenario: Adjust volume
- **WHEN** the user drags the volume slider to 0.5
- **THEN** the system SHALL set `playerVm.volume` to 0.5

#### Scenario: Toggle mute
- **WHEN** the user clicks the mute button
- **THEN** the system SHALL call `playerVm.toggleMute()`

#### Scenario: Volume icon reflects level
- **WHEN** `playerVm.volume` is 0.8 and `playerVm.isMuted` is false
- **THEN** the mute button SHALL display the "volume_up" icon

#### Scenario: Muted state
- **WHEN** `playerVm.isMuted` is true
- **THEN** the mute button SHALL display the "volume_off" icon

### Requirement: Toast displays error notifications
The system SHALL display a toast notification above the PlayerBar when a player error occurs. The toast SHALL auto-dismiss after 4 seconds and support manual dismissal.

#### Scenario: Player error triggers toast
- **WHEN** `playerVm.hasError` becomes true with error message "Network timeout"
- **THEN** a toast SHALL slide in displaying "Network timeout" with a warning icon

#### Scenario: Auto-dismiss
- **WHEN** a toast has been visible for 4 seconds
- **THEN** the toast SHALL slide out and become hidden

#### Scenario: Manual dismiss
- **WHEN** the user clicks the close button on the toast
- **THEN** the toast SHALL immediately slide out and become hidden

### Requirement: Keyboard shortcuts for playback
The system SHALL provide global keyboard shortcuts for playback control that work regardless of the active view.

#### Scenario: Space toggles play/pause
- **WHEN** the user presses Space and `playerVm.isPlaying` is true
- **THEN** the system SHALL call `playerVm.pause()`

#### Scenario: Space resumes playback
- **WHEN** the user presses Space and `playerVm.isPlaying` is false
- **THEN** the system SHALL call `playerVm.resume()`

#### Scenario: Left arrow seeks backward
- **WHEN** the user presses Left and `playerVm.positionMs` is 10000
- **THEN** the system SHALL call `playerVm.seek(5000)` (clamped to 0 minimum)

#### Scenario: Right arrow seeks forward
- **WHEN** the user presses Right and `playerVm.positionMs` is 10000 and `playerVm.durationMs` is 180000
- **THEN** the system SHALL call `playerVm.seek(15000)` (clamped to durationMs maximum)

### Requirement: PlayerBar layout
The system SHALL display the PlayerBar at the bottom of the application window with a fixed height of 80 pixels. It SHALL use a three-section horizontal layout: NowPlayingInfo (left, ~250px), TransportControls+SeekBar (center, flexible), VolumeControl (right, ~150px).

#### Scenario: PlayerBar is visible
- **WHEN** the application is running
- **THEN** the PlayerBar SHALL be visible at the bottom of the window below the sidebar and content area

#### Scenario: PlayerBar responds to window resize
- **WHEN** the application window is resized
- **THEN** the PlayerBar SHALL maintain its 80px height and the center section SHALL expand/contract to fill available space
