## ADDED Requirements

### Requirement: Search across platforms
The `SearchService` SHALL provide `search(const QString &query, MusicPlatform platform = MusicPlatform::All)` as `QCoro::Task<SearchResult>`. When platform is `All`, it SHALL query all registered `IMusicPlatformPlugin` instances concurrently and merge results.

#### Scenario: Search all platforms
- **WHEN** `search("query", MusicPlatform::All)` is called with 2 registered plugins
- **THEN** both plugins are queried concurrently and results are merged

#### Scenario: Search specific platform
- **WHEN** `search("query", MusicPlatform::NetEase)` is called
- **THEN** only the Netease plugin is queried

#### Scenario: Platform search failure
- **WHEN** one platform's search fails
- **THEN** results from successful platforms are returned, the failure is logged

### Requirement: Search result pagination
The `SearchService` SHALL support pagination via `search(query, platform, page, pageSize)`. It SHALL return `SearchResult` with `hasMore` and `totalCount` populated.

#### Scenario: Paginated search
- **WHEN** `search("query", MusicPlatform::All, 2, 20)` is called
- **THEN** page 2 of results with 20 items per platform is returned

### Requirement: Hot searches
The `SearchService` SHALL provide `hotSearches(MusicPlatform platform)` as `QCoro::Task<QStringList>` returning trending keywords from the specified platform.

#### Scenario: Get hot searches
- **WHEN** `hotSearches(MusicPlatform::NetEase)` is called
- **THEN** a list of trending keywords is returned
