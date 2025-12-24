/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <https://www.gnu.org/licenses/old-licenses/gpl-2.0>.
 */

/** @file company_type.h Types related to companies. */

#ifndef COMPANY_TYPE_H
#define COMPANY_TYPE_H

#include "core/base_bitset_type.hpp"
#include "core/pool_type.hpp"

/** Amount of bits used to encode company identifiers inside the game state. */
static constexpr uint8_t COMPANY_ID_BITS = 6;
/** Maximum representable company slot number based on the encoding width. */
static constexpr uint8_t COMPANY_ID_MASK = (1U << COMPANY_ID_BITS) - 1;
/** Number of company slots supported by the pool. */
static constexpr uint8_t COMPANY_POOL_SIZE = 50;
/** Legacy maximum number of companies for backwards compatibility. */
static constexpr uint8_t LEGACY_MAX_COMPANIES = 15;

using CompanyID = PoolID<uint8_t, struct CompanyIDTag, COMPANY_POOL_SIZE, 0xFF>;

/* 'Fake' companies used for networks */
static constexpr CompanyID COMPANY_INACTIVE_CLIENT{253}; ///< The client is joining
static constexpr CompanyID COMPANY_NEW_COMPANY{254}; ///< The client wants a new company
static constexpr CompanyID COMPANY_SPECTATOR{255}; ///< The client is spectating

using Owner = CompanyID;
static constexpr Owner OWNER_BEGIN = Owner::Begin(); ///< First owner
static constexpr Owner OWNER_TOWN{CompanyID::End()}; ///< A town owns the tile, or a town is expanding
static constexpr Owner OWNER_NONE{static_cast<Owner::ValueType>(CompanyID::End() + 1)}; ///< The tile has no ownership
static constexpr Owner OWNER_WATER{static_cast<Owner::ValueType>(CompanyID::End() + 2)}; ///< The tile/execution is done by "water"
static constexpr Owner OWNER_DEITY{static_cast<Owner::ValueType>(CompanyID::End() + 3)}; ///< The object is owned by a superuser / goal script
static constexpr Owner OWNER_END{static_cast<Owner::ValueType>(CompanyID::End() + 4)}; ///< Last + 1 owner
static constexpr Owner INVALID_OWNER = Owner::Invalid(); ///< An invalid owner

static_assert(OWNER_END.base() < COMPANY_INACTIVE_CLIENT.base(), "Owner special values must stay below network pseudo companies");

static constexpr uint8_t MAX_COMPANIES = CompanyID::End().base();
static_assert(MAX_COMPANIES <= COMPANY_ID_MASK, "Company ID mask must cover all company slots.");
static_assert(MAX_COMPANIES <= 64, "CompanyMask storage requires 64 bits or fewer.");
static const uint MAX_LENGTH_PRESIDENT_NAME_CHARS = 32; ///< The maximum length of a president name in characters including '\0'
static const uint MAX_LENGTH_COMPANY_NAME_CHARS   = 32; ///< The maximum length of a company name in characters including '\0'

static const uint MAX_HISTORY_QUARTERS            = 24; ///< The maximum number of quarters kept as performance's history

static const uint MIN_COMPETITORS_INTERVAL = 0;   ///< The minimum interval (in minutes) between competitors.
static const uint MAX_COMPETITORS_INTERVAL = 500; ///< The maximum interval (in minutes) between competitors.

typedef Owner CompanyID;

static constexpr uint64_t COMPANY_MASK_ALL = (uint64_t{1} << MAX_COMPANIES) - 1;

class CompanyMask : public BaseBitSet<CompanyMask, CompanyID, uint64_t, COMPANY_MASK_ALL> {
public:
        constexpr CompanyMask() : BaseBitSet<CompanyMask, CompanyID, uint64_t, COMPANY_MASK_ALL>() {}
	static constexpr size_t DecayValueType(CompanyID value) { return value.base(); }

	constexpr auto operator <=>(const CompanyMask &) const noexcept = default;
};

struct Company;

struct CompanyManagerFace {
	uint style = 0; ///< Company manager face style.
	uint32_t bits = 0; ///< Company manager face bits, meaning is dependent on style.
	std::string style_label; ///< Face style label.
};

/** The reason why the company was removed. */
enum CompanyRemoveReason : uint8_t {
	CRR_MANUAL,    ///< The company is manually removed.
	CRR_AUTOCLEAN, ///< The company is removed due to autoclean.
	CRR_BANKRUPT,  ///< The company went belly-up.

	CRR_END,       ///< Sentinel for end.

	CRR_NONE = CRR_MANUAL, ///< Dummy reason for actions that don't need one.
};

/** The action to do with CMD_COMPANY_CTRL. */
enum CompanyCtrlAction : uint8_t {
	CCA_NEW,    ///< Create a new company.
	CCA_NEW_AI, ///< Create a new AI company.
	CCA_DELETE, ///< Delete a company.

	CCA_END,    ///< Sentinel for end.
};

/** The action to do with CMD_COMPANY_ALLOW_LIST_CTRL. */
enum CompanyAllowListCtrlAction : uint8_t {
	CALCA_ADD, ///< Create a public key.
	CALCA_REMOVE, ///< Remove a public key.

	CALCA_END,    ///< Sentinel for end.
};

#endif /* COMPANY_TYPE_H */
