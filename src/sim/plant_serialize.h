#pragma once

/*
 * Serializer for the Obliq-generated saturn_ib_stack plant.
 *
 * Only the persistent partitions are written to disk:
 *   - states          (continuous integrator / discrete state)
 *   - data_stores     (memory blocks)
 *   - enable_states   (subsystem enable + trigger-prev flags)
 *   - time / dt / sample_tick / use_rk4 scalars
 *
 * signals and outputs are rebuilt from (states, data_stores, inputs)
 * by `saturn_ib_stack_evaluate_algebraic` at the top of each step,
 * so they are not persisted.  inputs are refilled by the stack
 * adapter from SaturnIBStack on every tick; also not persisted here.
 *
 * The same partitioning applies recursively to the LVDA_LVDC
 * subsystem (segregated inside saturn_ib_stack_t); we serialize its
 * states / data_stores / enable_states too.
 *
 * On-disk format: no struct dumps, every scalar written through
 * dskymatic::BinaryWriter (little-endian canonical).  Field order
 * is fixed by this translation unit; if the generated plant ever
 * adds / removes / renames a field, bump kPlantFormatVersion.
 */

#include "save_state.h"

#include <cstdint>

struct SaturnIBPlantObliq_tag;
typedef struct SaturnIBPlantObliq_tag SaturnIBPlantObliq;

namespace dskymatic {

constexpr uint32_t kPlantFormatVersion = 2;

// Serialize the plant's persistent state.
void plant_serialize  (BinaryWriter &w, const SaturnIBPlantObliq *plant);

// Restore plant persistent state in place.  Caller is responsible
// for having constructed the plant (SaturnIBPlantObliq_create) and
// run no steps on it yet.  Returns false on version mismatch or
// short payload.
bool plant_deserialize(BinaryReader &r, SaturnIBPlantObliq *plant);

} // namespace dskymatic
