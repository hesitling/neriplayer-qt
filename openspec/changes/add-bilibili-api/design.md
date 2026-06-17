## Context

The project has `IMusicPlatformPlugin` interface (in `src/api/common/`) and a working `NeteaseClient` implementation. Bilibili's API differs significantly: it uses WBI signing (MD5-based anti-tamper), cookie-based auth (not token-based), and serves video streams where audio must be extracted.

Reference: Android NeriPlayer's `BiliClient.kt` (1555 lines) and `docs/modules/api/bilibili.md`.

## Goals / Non-Goals

**Goals:**
- Implement `BilibiliClient` implementing `IMusicPlatformPlugin`
- Support QR code login with cookie persistence
- Support video search and audio stream extraction
- Support favorites management
- WBI request signing

**Non-Goals:**
- Video playback (audio only)
- Comments, live streaming, user uploads
- Bilibili-specific UI

## Decisions

### 1. WBI signing in a separate helper class

WBI signing involves mixin key derivation from `/x/web-interface/nav`, a 64-element permutation table, and MD5 hashing. This is isolated in `BilibiliClient` private methods to keep the main API methods clean.

**Why:** WBI is Bilibili-specific and changes occasionally. Isolating it makes updates easier.

### 2. Cookie-based auth stored in SecureStorage

Bilibili uses browser cookies (`SESSDATA`, `bili_jct`, `DedeUserID`) for auth. These are stored in `SecureStorage` as a JSON map, same pattern as Android's `BiliCookieRepository`.

**Why:** `SecureStorage` already handles AES-256-GCM encryption. No new crypto needed.

### 3. Audio extraction from DASH streams

Bilibili's `/x/player/wbi/playurl` returns DASH streams with separate audio tracks. The client extracts the highest quality audio stream URL. Falls back to progressive MP4 if DASH unavailable.

**Why:** The player only needs audio. Extracting from DASH avoids downloading video data.

### 4. BilibiliParser for JSON → struct conversion

A dedicated `BilibiliParser` class handles all JSON parsing, keeping `BilibiliClient` focused on API orchestration.

**Why:** Separation of concerns. Parser can be tested independently with fixture JSON.

## Risks / Trade-offs

- **WBI key rotation** — Bilibili occasionally changes the WBI signing algorithm. Mitigation: Log signing failures, fall back to unsigned requests (may get rate-limited).
- **Cookie expiry** — Bilibili cookies expire after ~30 days. Mitigation: `AuthService` handles re-login flow.
- **Rate limiting** — Bilibili rate-limits aggressively. Mitigation: Respect 429 responses, implement exponential backoff.
