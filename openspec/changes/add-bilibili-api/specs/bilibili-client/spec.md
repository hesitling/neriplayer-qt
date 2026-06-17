## ADDED Requirements

### Requirement: QR code login
The `BilibiliClient` SHALL provide `generateQrCode()` returning `ApiResult<QrCodeData>` and `checkQrCodeStatus(key)` returning `ApiResult<LoginResult>`. On confirmation, cookies are stored in `SecureStorage`.

#### Scenario: Generate QR code
- **WHEN** `generateQrCode()` is called
- **THEN** a QR code URL and polling key are returned

#### Scenario: Login confirmed
- **WHEN** `checkQrCodeStatus()` returns `Confirmed`
- **THEN** cookies are stored and `isAuthenticated()` returns true

### Requirement: Cookie persistence
The `BilibiliClient` SHALL persist cookies (`SESSDATA`, `bili_jct`, `DedeUserID`) in `SecureStorage` and restore them on construction.

#### Scenario: Restore cookies on startup
- **WHEN** the client is constructed with saved cookies
- **THEN** `isAuthenticated()` returns true

### Requirement: WBI request signing
The `BilibiliClient` SHALL sign requests to WBI-protected endpoints using MD5 hash of sorted parameters + mixin key derived from `/x/web-interface/nav`.

#### Scenario: Signed request
- **WHEN** a WBI-protected endpoint is called
- **THEN** the request includes `wts` and `w_rid` parameters

### Requirement: Video search
The `BilibiliClient` SHALL provide `searchVideos(keyword, page)` returning `ApiResult<SearchVideoPage>` via `IMusicPlatformPlugin::search()`.

#### Scenario: Search videos
- **WHEN** `searchVideos("music")` is called
- **THEN** a paginated list of video results is returned

### Requirement: Video detail
The `BilibiliClient` SHALL provide `getVideoDetail(bvid)` returning `ApiResult<VideoDetail>`.

#### Scenario: Get video detail
- **WHEN** `getVideoDetail("BV1xx411c7mD")` is called
- **THEN** video metadata including pages list is returned

### Requirement: Audio stream extraction
The `BilibiliClient` SHALL provide `getAudioStream(bvid, cid)` returning `ApiResult<AudioStream>` by extracting the audio track from DASH streams.

#### Scenario: Get audio from DASH
- **WHEN** `getAudioStream()` is called for a video with DASH streams
- **THEN** the highest quality audio stream URL is returned

#### Scenario: Fallback to progressive
- **WHEN** DASH audio is unavailable
- **THEN** the progressive MP4 URL is returned

### Requirement: Favorites management
The `BilibiliClient` SHALL provide `getUserFavorites(mid)`, `getFavoriteDetail(mediaId, page)`, `addVideoToFavorite(mediaId, bvid)`, `removeVideoFromFavorite(mediaId, bvid)`.

#### Scenario: Get favorites
- **WHEN** `getUserFavorites(mid)` is called
- **THEN** a list of favorite folders is returned

#### Scenario: Add to favorite
- **WHEN** `addVideoToFavorite(mediaId, bvid)` is called
- **THEN** the video is added to the specified favorite folder

### Requirement: Hot searches
The `BilibiliClient` SHALL provide `getHotSearches()` returning `ApiResult<QStringList>`.

#### Scenario: Get hot searches
- **WHEN** `getHotSearches()` is called
- **THEN** a list of trending keywords is returned

### Requirement: User profile
The `BilibiliClient` SHALL provide `getUserProfile()` returning `ApiResult<UserProfile>` from `/x/web-interface/nav`.

#### Scenario: Get profile
- **WHEN** `getUserProfile()` is called while authenticated
- **THEN** user profile with name, avatar, level is returned
