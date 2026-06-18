## ADDED Requirements

### Requirement: SearchView page layout
The system SHALL define `SearchView.qml` as a QML Page containing a search input area, a platform filter, and a results list.

#### Scenario: Page structure
- **WHEN** SearchView is loaded
- **THEN** it SHALL display a TextField for query input, a ComboBox for platform selection, and a ListView for results

### Requirement: Query input bound to SearchViewModel
The TextField SHALL bind bidirectionally to `searchVm.query`. Text changes SHALL update the VM property, which triggers the VM's internal 500ms debounce and auto-search.

#### Scenario: Typing triggers debounced search
- **WHEN** the user types "hello" in the TextField
- **THEN** `searchVm.query` SHALL be updated to "hello" and the VM SHALL dispatch a search after 500ms of inactivity

#### Scenario: Empty query clears results
- **WHEN** the user clears the TextField
- **THEN** `searchVm.query` SHALL become "" and `searchVm.clearResults()` SHALL be triggered by the VM

### Requirement: Platform filter ComboBox
A ComboBox SHALL display `searchVm.availablePlatforms` as its model. Selecting a platform SHALL call `searchVm.selectPlatformByName(selectedText)`.

#### Scenario: Select NetEase platform
- **WHEN** the user selects "NetEase" from the ComboBox
- **THEN** `searchVm.selectPlatformByName("NetEase")` SHALL be called

#### Scenario: Default platform selection
- **WHEN** SearchView is first loaded
- **THEN** the ComboBox SHALL display the first available platform

### Requirement: Results ListView with SongDelegate
The ListView SHALL use `searchVm.results` as its model and `SongDelegate` as its delegate.

#### Scenario: Results displayed
- **WHEN** a search returns 20 songs
- **THEN** the ListView SHALL display 20 SongDelegate items

#### Scenario: Double-click plays song
- **WHEN** the user double-clicks a result at index 5
- **THEN** `searchVm.playSong(5)` SHALL be called

### Requirement: Infinite scroll pagination
The ListView SHALL detect when scrolled to the bottom and call `searchVm.loadMore()` if `searchVm.hasMore` is true and `searchVm.isLoading` is false.

#### Scenario: Scroll to bottom loads more
- **WHEN** the user scrolls to the bottom of the results and `hasMore` is true
- **THEN** `searchVm.loadMore()` SHALL be called

#### Scenario: No duplicate loadMore calls
- **WHEN** `searchVm.isLoading` is true
- **THEN** `loadMore()` SHALL NOT be called regardless of scroll position

### Requirement: Loading state
A BusyIndicator SHALL be visible in the ListView footer when `searchVm.isLoading` is true and `searchVm.hasMore` is true.

#### Scenario: Footer loading indicator
- **WHEN** a search or loadMore is in progress and more results are available
- **THEN** a BusyIndicator SHALL be visible at the bottom of the ListView

### Requirement: Empty states
The view SHALL display contextual messages when no results are shown.

#### Scenario: Initial state — no query
- **WHEN** `searchVm.query` is empty and no search has been performed
- **THEN** a hint message "Search for music across platforms" SHALL be displayed

#### Scenario: No results found
- **WHEN** a search completes with `results.count === 0` and `isLoading` is false
- **THEN** a message "No results found" SHALL be displayed

### Requirement: Error display via Toast
Search errors SHALL be displayed via the existing Toast component in main.qml, wired through `searchVm.errorChanged`.

#### Scenario: API error shown in Toast
- **WHEN** `searchVm.hasError` becomes true
- **THEN** `toast.show(searchVm.error.message)` SHALL be called
