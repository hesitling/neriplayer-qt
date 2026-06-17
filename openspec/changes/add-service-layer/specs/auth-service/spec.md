## ADDED Requirements

### Requirement: Login state query
The `AuthService` SHALL provide `isAuthenticated(MusicPlatform platform)` returning `bool` and `authenticatedPlatforms()` returning `QList<MusicPlatform>`.

#### Scenario: Check login state
- **WHEN** `isAuthenticated(MusicPlatform::NetEase)` is called after login
- **THEN** true is returned

### Requirement: Platform login
The `AuthService` SHALL provide `login(MusicPlatform platform, const QVariantMap &credentials)` as `QCoro::Task<ApiResult<void>>`. It SHALL delegate to the platform's `IMusicPlatformPlugin::login()`.

#### Scenario: Successful login
- **WHEN** `login()` is called with valid credentials
- **THEN** the platform's login succeeds and `isAuthenticated()` returns true

### Requirement: Platform logout
The `AuthService` SHALL provide `logout(MusicPlatform platform)` as `QCoro::Task<void>`. It SHALL clear stored credentials and update login state.

#### Scenario: Logout
- **WHEN** `logout(MusicPlatform::NetEase)` is called
- **THEN** credentials are cleared and `isAuthenticated()` returns false

### Requirement: Credential persistence
The `AuthService` SHALL persist login credentials in `SecureStorage` and restore them on construction.

#### Scenario: Restore on startup
- **WHEN** the app starts and credentials were previously saved
- **THEN** `isAuthenticated()` returns true for the saved platform

### Requirement: Service signals
The `AuthService` SHALL emit `loginStateChanged(MusicPlatform platform, bool authenticated)` when login state changes.

#### Scenario: Signal on login
- **WHEN** login succeeds
- **THEN** `loginStateChanged(platform, true)` is emitted
