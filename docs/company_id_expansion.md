# Company ID Range Expansion Notes

## Current usage highlights
- `CompanyID` is defined as `PoolID<uint8_t, ...>` with `MAX_COMPANIES` derived from the end marker, tying all owners and masks to an 8-bit range. 【F:src/company_type.h†L16-L40】
- Network configuration currently serialises `network.max_companies` as `uint8_t` in both server updates and client handling. 【F:src/network/network_server.cpp†L810-L819】【F:src/network/network_client.cpp†L1141-L1152】
- Save/load schemas rely on `MAX_COMPANIES`-sized arrays; town ratings and unwanted house caches use conditional array lengths keyed to the company limit. 【F:src/saveload/town_sl.cpp†L267-L270】
- Settings storage constrains `network.max_companies` to `uint8_t`, matching the current limit. 【F:src/settings_type.h†L340-L368】
- Packet documentation still references company identifiers encoded in single bytes with ranges `1..MAX_COMPANIES` or `0..MAX_COMPANIES-1`. 【F:src/network/core/tcp_game.h†L159-L200】
- Network statistics and UI helpers depend on `std::array<..., MAX_COMPANIES>` containers (e.g., `NetworkCompanyStatsArray`) that would need resizing with a wider identifier. 【F:src/network/network_func.h†L47-L60】

## Proposed widening strategy
1. **Type expansion**
   - Promote `CompanyID` to a wider base type (e.g., `uint16_t`) while keeping the `PoolID` helper. Adjust `CompanyMask` backing storage and any `Owner` aliases to match the new width. 【F:src/company_type.h†L16-L40】
   - Update `MAX_COMPANIES` to reflect the new `End()` marker and propagate dependent constants (UI arrays, statistics caches, legend tables) that use fixed `MAX_COMPANIES` sizing.

2. **Settings and configuration**
   - Broaden `network.max_companies` to the new type, update clamps in GUI code, and ensure defaults do not exceed the previous limit unless explicitly configured. 【F:src/network/network_server.cpp†L810-L819】【F:src/network/network_gui.cpp†L982-L1124】
   - Expand console parsing and validation paths that compare against `MAX_COMPANIES` to accept the widened range.

3. **Protocol updates**
   - Introduce a protocol bump where company identifiers and limits are transmitted as the wider integer (e.g., `uint16_t`) in packets such as `PACKET_SERVER_MOVE` and `PACKET_SERVER_CONFIG_UPDATE`. 【F:src/network/network_server.cpp†L798-L819】【F:src/network/core/tcp_game.h†L159-L200】
   - Add compatibility handling: servers should reject pre-bump clients (clear error) or provide a compatibility mode that caps to 8-bit and disallows joining beyond the legacy range.

4. **Save/load compatibility**
   - Extend savegame schema entries that currently store `MAX_COMPANIES`-length arrays (town ratings/unwanted lists, company masks in map ownership, etc.) to the new limit with a new save version. Provide upgrade logic that pads legacy saves with defaults and downgrades by trimming/saturating to the legacy maximum when exporting to older versions. 【F:src/saveload/town_sl.cpp†L267-L270】【F:src/saveload/afterload.cpp†L249-L263】

5. **Runtime containers**
   - Resize `std::array` and fixed-size pools keyed by `MAX_COMPANIES` (statistics caches, small map legends, AI settings) and adjust any static asserts or UI bounds that assume the old count. 【F:src/network/network_func.h†L47-L60】【F:src/settings_type.h†L340-L368】

6. **Testing plan**
   - Add protocol-level tests to verify round-tripping of widened company IDs between server and client, including rejection of legacy clients when limits exceed 255.
   - Extend save/load tests to cover migration from legacy saves and operation with the expanded company count.
   - Simulate gameplay with the higher limit to validate UI lists, AI slots, and statistics screens under load.

## Migration considerations
- **Protocol versioning:** define a new minimal network version for widened IDs; older clients receive an explicit incompatibility error and hint to downgrade or enable legacy-cap mode.
- **Compatibility mode:** optional server flag that constrains `MAX_COMPANIES` to 255 for mixed environments while using the widened internal type, keeping packet payloads in the legacy width when the cap is active.
- **Save downgrade path:** when saving to older formats, enforce the legacy limit by dropping excess companies or remapping ownership to `OWNER_NONE` with clear warnings in logs.
