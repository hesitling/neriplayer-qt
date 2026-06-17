## Why

NetEase is the only implemented platform API. Bilibili is a major music/video source in China and the second platform listed in the roadmap. Implementing `BilibiliClient` validates the `IMusicPlatformPlugin` multi-platform architecture and provides video-to-audio extraction capability unique to Bilibili.

## What Changes

- Add `src/api/bilibili/` with `BilibiliClient`, `BilibiliParser`, `BilibiliTypes`
- Implement `IMusicPlatformPlugin` interface for Bilibili
- Add WBI request signing (Bilibili's anti-tamper mechanism)
- Add QR code login with cookie persistence
- Add video search, detail, stream extraction (DASH + progressive)
- Add favorites management (list, detail, add/remove)
- Add test fixtures and unit tests

## Non-goals

- No UI for Bilibili login (will use `AuthService` in Phase 4)
- No video playback (audio extraction only)
- No Bilibili comments or live streaming

## Capabilities

### New Capabilities
- `bilibili-client`: Bilibili API client implementing `IMusicPlatformPlugin` with auth, search, playback, favorites

### Modified Capabilities
- `api-common`: Add `QrCodeData`, `LoginResult` types if not already present (check existing)

## Impact

- **Affected layers**: `src/api/bilibili/` (new), `src/app/` (registration)
- **Dependencies**: Uses `HttpClient`, `SecureStorage`, `IMusicPlatformPlugin`
- **Tests**: `tests/api/TestBilibiliParser.cpp`, `tests/fixtures/bilibili/`
