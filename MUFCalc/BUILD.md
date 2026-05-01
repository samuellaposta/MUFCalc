# Building MUFCalc

MUFCalc is built two ways: locally on Windows for development, and on
GitHub Actions for distributable installers. Both paths use the same
source code; only the token-injection mechanism differs.

## Local development build (Windows)

You need this once per Windows machine. After setup, open `MUFCalc.pro`
in Qt Creator and build normally.

### One-time setup

1. **Install Qt 6** (6.5 or newer) with the MinGW or MSVC compiler.
2. **Generate a GitHub Personal Access Token** at
   https://github.com/settings/tokens.
   - Type: classic
   - Scope: `public_repo`
   - Expiry: 90 days (or whatever your security policy allows)
3. **Create your local token file:**
   - Copy `src/gui/local_token.h.example` to `src/gui/local_token.h`
   - Open `local_token.h` and replace the `ghp_xxxx...` placeholder with
     your real token.
   - Save. This file is gitignored — it cannot be pushed.
4. **Build** in Qt Creator: Build → Run qmake → Build All.

### What `local_token.h` does

`FeedbackDialog.cpp` looks for `local_token.h` at compile time with
`__has_include`. If the file exists, the real token is baked into the
local `.exe`. If it does not exist, the placeholder string
`PLACEHOLDER_TOKEN` is used and the Send Feedback dialog will return
"Bad credentials" until you create the file.

## CI build (GitHub Actions)

The workflow at `.github/workflows/build-installer.yml` builds the
installer on Windows runners. It runs on:

- Every push to `main` → produces `MUFCalcSetup_v2.exe` as a workflow
  artifact (downloadable from the Actions tab for 30 days).
- Every tag matching `v*.*.*` → produces the artifact AND attaches it
  to a GitHub Release with auto-generated release notes.
- Manual trigger from the Actions tab → same as a push, on demand.

### One-time secret setup

1. Open the repo on GitHub.
2. Settings → Secrets and variables → Actions → **New repository secret**
3. Name: `MUFCALC_GITHUB_TOKEN`
4. Value: paste the same token you used in `local_token.h` (or a separate
   one, for least privilege).
5. Click **Add secret**.

Without this secret the workflow will fail with a clear error message
on its first run.

### Triggering a release build

```bash
git tag v2.0.0
git push origin v2.0.0
```

The workflow runs, builds, packages, and creates a GitHub Release at
`https://github.com/samuellaposta/MUFCalc/releases/tag/v2.0.0` with the
installer attached. Anyone visiting that release page can download and
install MUFCalc with no further setup.

### Downloading the installer from a non-tag build

For builds triggered by a regular push or manual run:

1. Open https://github.com/samuellaposta/MUFCalc/actions
2. Click the most recent successful run (green checkmark)
3. Scroll to the bottom — under **Artifacts**, click `MUFCalcSetup_v2`
4. The download is a `.zip` containing `MUFCalcSetup_v2.exe`. Unzip
   and run.

## Token rotation

If a token is ever exposed (in a screenshot, log, screenshare, etc.),
revoke and rotate it:

1. Revoke at https://github.com/settings/tokens (click "Delete" on the
   compromised token).
2. Generate a new PAT with the same scope.
3. Update `MUFCALC_GITHUB_TOKEN` in the GitHub Actions secret (Settings
   → Secrets → click the secret → Update).
4. Update your local `src/gui/local_token.h` with the new value.
5. Rebuild — no code changes needed.

## Why this layout

The architectural goal is that **the real token never appears in any file
that gets committed**. It lives in exactly two places: the GitHub Actions
secret (encrypted, accessible only to the workflow) and your local
`local_token.h` (gitignored, on your machine only). This makes accidental
exposure structurally impossible — there is no committed file from which
the token could leak.
