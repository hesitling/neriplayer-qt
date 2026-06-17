## ADDED Requirements

### Requirement: Search query property
The `SearchViewModel` SHALL expose `Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)`.

#### Scenario: Set query
- **WHEN** `setQuery("music")` is called
- **THEN** `queryChanged` fires

### Requirement: Search results property
The `SearchViewModel` SHALL expose `Q_PROPERTY(QVector<Song> results READ results NOTIFY resultsChanged)`.

#### Scenario: Search completes
- **WHEN** a search completes
- **THEN** `results` updates and `resultsChanged` fires

### Requirement: Platform filter
The `SearchViewModel` SHALL expose `Q_PROPERTY(MusicPlatform platformFilter READ platformFilter WRITE setPlatformFilter NOTIFY platformFilterChanged)`.

#### Scenario: Filter by platform
- **WHEN** `setPlatformFilter(MusicPlatform::NetEase)` is called
- **THEN** subsequent searches only query Netease

### Requirement: Execute search
The `SearchViewModel` SHALL expose `Q_INVOKABLE QCoro::Task<void> executeSearch()` that triggers `SearchService::search()`.

#### Scenario: Execute search
- **WHEN** `executeSearch()` is called with query "test"
- **THEN** `SearchService::search("test", platformFilter)` is called and results update

### Requirement: Loading state
The `SearchViewModel` SHALL expose `Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)`.

#### Scenario: Loading indicator
- **WHEN** `executeSearch()` starts
- **THEN** `isLoading` becomes true; when complete, it becomes false
