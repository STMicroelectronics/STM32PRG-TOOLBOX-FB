# PRG-TOOLBOX-FB (STM32PRG-TOOLBOX-FASTBOOT) — Copilot Instructions (C++)

These instructions apply to the **PRG-TOOLBOX-FB** codebase in this repository.

PRG-TOOLBOX-FB is a small console wrapper around the upstream **fastboot** tool, tailored for STM32MP devices. It primarily shells out to a **bundled fastboot binary** and parses/streams its output.

## Project Context

- Language: **C++11** (see `CONFIG += ... c++11` in `PRG-TOOLBOX-FB.pro`).
- No Qt modules are used (`CONFIG -= qt`): this is a plain console application.
- Cross-platform: Windows / Linux / macOS are supported via `#ifdef _WIN32`, `__linux__`, `__APPLE__`.
- Filesystem: uses `std::experimental::filesystem` (linked with `-lstdc++fs`).

## Repository Layout

- `Inc/` — headers.
- `Src/` — implementation.
- `fastboot/` — **runtime dependency**: OS-specific fastboot binaries.
  - `fastboot/Windows/fastboot.exe`
  - `fastboot/Linux/fastboot`
  - `fastboot/MacOS/fastboot`
- `fastboot_usb_driver/` — Windows USB driver material.
- `rules/` — Linux udev rules.

## Build Systems

This repo supports two build entry points:

- **qmake**: `PRG-TOOLBOX-FB.pro`
  - Produces a console executable.
  - Uses static link flags (`-static -static-libgcc -static-libstdc++`).
- **GNU Make**: `Makefile`
  - Uses `g++ -std=c++11` and also links `-static ... -lstdc++fs`.

Avoid introducing new build systems or restructuring outputs unless explicitly requested.

## Runtime / Packaging Constraints (Important)

- The executable derives `toolboxRootPath` from `argv[0]` and expects **the `fastboot/` folder to be adjacent to the executable’s directory**.
- `Fastboot::getFastbootProgramPath()` constructs the fastboot path relative to that root.
- Do **not** hard-code absolute paths or change the on-disk layout (`fastboot/<OS>/...`) without updating both code and distribution content.
- If you change how fastboot is invoked (quoting, redirection, `-s` serial handling), ensure it remains correct across Windows and POSIX shells.

## General Expectations

- Prefer **small, focused diffs**; avoid drive-by refactors.
- Keep the existing style: mostly simple C++ with minimal abstractions.
- Do not add new third-party dependencies unless explicitly requested.
- When changing externally observed behavior (CLI flags, output wording, return codes), call it out clearly and keep backward compatibility when feasible.

## CLI / UX Conventions

- Output is handled via `DisplayManager::print(...)` using wide strings (`wchar_t*`) and colored output.
- Keep message types consistent (`MSG_NORMAL`, `MSG_GREEN`, `MSG_WARNING`, `MSG_ERROR`).
- The CLI supports a small fixed command set (help, version, list devices, download TSV, serial selection). Do not add new commands/options unless asked.

## Error Handling & Return Codes

- Use `ToolboxError` values from `Inc/Error.h` for failure paths.
- Be actionable: include the operation and the relevant path/partition when safe.
- Avoid dumping huge raw fastboot output by default; keep logs readable.

## Safety Constraints (Device Flashing)

This tool can erase/flash partitions. Be conservative with behavior changes:

- Do not make destructive operations the default.
- Validate inputs before executing (file existence, `.tsv` extension where applicable, parameter counts).
- Avoid “helpful” auto-detection that could target the wrong device; keep the explicit `-sn/--serial` behavior.

## What Not To Do

- Don’t update/replace bundled `fastboot` binaries unless explicitly requested (and if so, keep `Notice.txt`/notices in sync).
- Don’t remove static-link flags or the `-lstdc++fs` requirement without confirming toolchain compatibility.
- Don’t reformat unrelated code or rewrite modules for style.