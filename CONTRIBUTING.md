# Contributing to PocketOS

Thanks for helping improve PocketOS.

PocketOS is an experimental embedded project for the Waveshare ESP32-S3 E-Ink platform. The firmware is developed in Schematic.io, and the exact build configuration is still being verified.

## Before changing code

- Check the current project status in [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md).
- Keep changes focused and explain the problem they solve.
- Do not assume board settings, pin mappings or library versions. Confirm them against the actual project export and hardware.
- Avoid unrelated rewrites of the main firmware entry point.
- Protect SD-card user data; never introduce destructive formatting or migration behavior without an explicit, tested reason.

## Branch workflow

- `main` is the protected baseline for this preparation workflow.
- Release work currently belongs in `release/pocketos-1.0`.
- Use short, descriptive branch names for isolated work.
- Do not merge release work into `main` until it has been reviewed and approved.

## Commit messages

Prefer concise, action-oriented messages, for example:

- `docs: clarify SD-card data layout`
- `fix: handle missing RTC module`
- `feat: add application status indicator`
- `build: pin ESP32 toolchain dependencies`

## Pull requests

A useful pull request should include:

- What changed and why.
- Which files or modules are affected.
- Hardware and software configuration used.
- Build result (or a clear note that it has not been built).
- Hardware test result (or a clear note that it has not been tested).
- Risks, limitations and screenshots/logs where useful.

## Testing expectations

Never describe code as verified unless the relevant test was actually performed. Separate these statements clearly:

- **Compiled:** a clean build completed successfully.
- **Hardware-tested:** the change was exercised on the target device.
- **Not tested:** no relevant build or hardware test has been performed.

Thanks for keeping PocketOS reliable, understandable and friendly to its users.
