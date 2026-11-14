#pragma once

/*
 * DSKY-matic save-state framework.
 *
 * On-disk layout (little-endian, fixed-width; byte-packed so it is
 * portable across host endianness and compiler padding):
 *
 *   magic[8]     = "DSKYMATC"
 *   u32          format version
 *   lp-string    ISO8601 local save time (no timezone)
 *   f64          sim elapsed seconds
 *   u32          section count
 *   repeat per section:
 *     lp-string  section name (e.g. "agc_t", "sat_bridge", "sim_clock")
 *     u64        section byte length
 *     bytes      opaque payload, format defined by that section's writer
 *
 * Length-prefixed strings are a u32 length followed by that many raw
 * bytes (not null-terminated).
 *
 * Primitive encodings:
 *   - Integers:   stored little-endian, two's-complement for signed.
 *   - Booleans:   one byte, 0 or 1.
 *   - Floats:     IEEE 754 binary32 / binary64, stored little-endian
 *                 by reinterpret.  Universal on every platform we
 *                 target (x86/ARM/POWER/SPARC in their modern form).
 *   - Arrays:     element-by-element, no intervening padding.
 *
 * BinaryWriter emits those encodings regardless of host endianness
 * (manual byte packing).  BinaryReader reverses them.  Neither
 * depends on struct layout — callers walk their structs field by
 * field.
 */

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace dskymatic {

class BinaryWriter {
public:
    const std::vector<uint8_t> &bytes() const { return m_bytes; }
    std::vector<uint8_t>       &bytes()       { return m_bytes; }
    size_t                      size() const  { return m_bytes.size(); }

    void write_u8 (uint8_t  v) { m_bytes.push_back(v); }
    void write_u16(uint16_t v);
    void write_u32(uint32_t v);
    void write_u64(uint64_t v);
    void write_i8 (int8_t   v) { write_u8 ((uint8_t) v); }
    void write_i16(int16_t  v) { write_u16((uint16_t)v); }
    void write_i32(int32_t  v) { write_u32((uint32_t)v); }
    void write_i64(int64_t  v) { write_u64((uint64_t)v); }
    void write_bool(bool    v) { write_u8(v ? 1u : 0u); }

    void write_f32(float  v);
    void write_f64(double v);

    void write_bytes (const void *p, size_t n);
    void write_string(const std::string &s);

private:
    std::vector<uint8_t> m_bytes;
};

class BinaryReader {
public:
    BinaryReader(const uint8_t *data, size_t len) : m_p(data), m_n(len), m_pos(0) {}
    BinaryReader(const std::vector<uint8_t> &v) : BinaryReader(v.data(), v.size()) {}

    bool   eof() const { return m_pos >= m_n; }
    size_t remaining() const { return m_pos > m_n ? 0 : (m_n - m_pos); }
    bool   ok() const  { return !m_fail; }
    size_t pos() const { return m_pos; }

    uint8_t  read_u8();
    uint16_t read_u16();
    uint32_t read_u32();
    uint64_t read_u64();
    int8_t   read_i8()  { return (int8_t)  read_u8();  }
    int16_t  read_i16() { return (int16_t) read_u16(); }
    int32_t  read_i32() { return (int32_t) read_u32(); }
    int64_t  read_i64() { return (int64_t) read_u64(); }
    bool     read_bool();

    float    read_f32();
    double   read_f64();

    void        read_bytes(void *p, size_t n);
    std::string read_string();

private:
    const uint8_t *m_p;
    size_t         m_n;
    size_t         m_pos;
    bool           m_fail = false;

    void fail(const char *msg);
};

struct Section {
    std::string          name;
    std::vector<uint8_t> data;
};

struct SaveFile {
    // Bump when the on-disk layout changes incompatibly.  Section writers
    // may version themselves inside their own payload too.
    static constexpr uint32_t kFormatVersion = 1;
    static constexpr char     kMagic[8] = {'D','S','K','Y','M','A','T','C'};

    uint32_t             version        = kFormatVersion;
    std::string          iso_timestamp;      // local, no tz suffix
    double               sim_elapsed_sec = 0.0;
    std::vector<Section> sections;

    bool write(const std::string &path) const;
    static bool read(const std::string &path, SaveFile &out);

    const Section *find(const std::string &name) const;
};

// ISO8601 local time, no timezone suffix, filename-safe
// (e.g. "2026-11-13T18-42-17").  ':' replaced with '-' so the string
// drops into any filesystem.
std::string make_iso_timestamp_local();

// "saved-state-<iso>"  (no file extension; the format is self-describing
// via the magic bytes).
std::string make_save_filename();

} // namespace dskymatic
