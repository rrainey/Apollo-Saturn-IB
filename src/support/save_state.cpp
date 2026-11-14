#include "save_state.h"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>

namespace dskymatic {

// -----------------------------------------------------------------------
// BinaryWriter
// -----------------------------------------------------------------------

void BinaryWriter::write_u16(uint16_t v) {
    m_bytes.push_back((uint8_t)(v & 0xff));
    m_bytes.push_back((uint8_t)((v >> 8) & 0xff));
}

void BinaryWriter::write_u32(uint32_t v) {
    for (int i = 0; i < 4; ++i)
        m_bytes.push_back((uint8_t)((v >> (8 * i)) & 0xff));
}

void BinaryWriter::write_u64(uint64_t v) {
    for (int i = 0; i < 8; ++i)
        m_bytes.push_back((uint8_t)((v >> (8 * i)) & 0xff));
}

void BinaryWriter::write_f32(float v) {
    uint32_t bits;
    std::memcpy(&bits, &v, 4);
    write_u32(bits);
}

void BinaryWriter::write_f64(double v) {
    uint64_t bits;
    std::memcpy(&bits, &v, 8);
    write_u64(bits);
}

void BinaryWriter::write_bytes(const void *p, size_t n) {
    const uint8_t *b = (const uint8_t *)p;
    m_bytes.insert(m_bytes.end(), b, b + n);
}

void BinaryWriter::write_string(const std::string &s) {
    write_u32((uint32_t)s.size());
    write_bytes(s.data(), s.size());
}

// -----------------------------------------------------------------------
// BinaryReader
// -----------------------------------------------------------------------

void BinaryReader::fail(const char *msg) {
    if (!m_fail) {
        std::fprintf(stderr, "save_state: BinaryReader fail at pos=%zu: %s\n",
                     m_pos, msg);
        m_fail = true;
    }
}

uint8_t BinaryReader::read_u8() {
    if (m_pos + 1 > m_n) { fail("read_u8 past end"); return 0; }
    return m_p[m_pos++];
}

uint16_t BinaryReader::read_u16() {
    if (m_pos + 2 > m_n) { fail("read_u16 past end"); return 0; }
    uint16_t v = 0;
    v |= (uint16_t)m_p[m_pos + 0] << 0;
    v |= (uint16_t)m_p[m_pos + 1] << 8;
    m_pos += 2;
    return v;
}

uint32_t BinaryReader::read_u32() {
    if (m_pos + 4 > m_n) { fail("read_u32 past end"); return 0; }
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i)
        v |= (uint32_t)m_p[m_pos + i] << (8 * i);
    m_pos += 4;
    return v;
}

uint64_t BinaryReader::read_u64() {
    if (m_pos + 8 > m_n) { fail("read_u64 past end"); return 0; }
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i)
        v |= (uint64_t)m_p[m_pos + i] << (8 * i);
    m_pos += 8;
    return v;
}

bool BinaryReader::read_bool() {
    uint8_t b = read_u8();
    return b != 0;
}

float BinaryReader::read_f32() {
    uint32_t bits = read_u32();
    float v;
    std::memcpy(&v, &bits, 4);
    return v;
}

double BinaryReader::read_f64() {
    uint64_t bits = read_u64();
    double v;
    std::memcpy(&v, &bits, 8);
    return v;
}

void BinaryReader::read_bytes(void *p, size_t n) {
    if (m_pos + n > m_n) { fail("read_bytes past end"); return; }
    std::memcpy(p, m_p + m_pos, n);
    m_pos += n;
}

std::string BinaryReader::read_string() {
    uint32_t n = read_u32();
    if (m_fail) return {};
    if (m_pos + n > m_n) { fail("read_string past end"); return {}; }
    std::string s((const char *)(m_p + m_pos), n);
    m_pos += n;
    return s;
}

// -----------------------------------------------------------------------
// SaveFile
// -----------------------------------------------------------------------

bool SaveFile::write(const std::string &path) const {
    BinaryWriter w;
    w.write_bytes(kMagic, sizeof kMagic);
    w.write_u32(version);
    w.write_string(iso_timestamp);
    w.write_f64(sim_elapsed_sec);
    w.write_u32((uint32_t)sections.size());
    for (const auto &s : sections) {
        w.write_string(s.name);
        w.write_u64((uint64_t)s.data.size());
        w.write_bytes(s.data.data(), s.data.size());
    }

    std::ofstream f(path, std::ios::binary);
    if (!f.is_open()) {
        std::fprintf(stderr, "save_state: cannot open %s for write\n", path.c_str());
        return false;
    }
    f.write((const char *)w.bytes().data(), (std::streamsize)w.bytes().size());
    return f.good();
}

bool SaveFile::read(const std::string &path, SaveFile &out) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f.is_open()) {
        std::fprintf(stderr, "save_state: cannot open %s for read\n", path.c_str());
        return false;
    }
    auto len = f.tellg();
    f.seekg(0, std::ios::beg);
    std::vector<uint8_t> buf((size_t)len);
    f.read((char *)buf.data(), len);
    if (!f.good()) {
        std::fprintf(stderr, "save_state: read failed on %s\n", path.c_str());
        return false;
    }

    BinaryReader r(buf);
    uint8_t magic[sizeof kMagic];
    r.read_bytes(magic, sizeof magic);
    if (std::memcmp(magic, kMagic, sizeof kMagic) != 0) {
        std::fprintf(stderr, "save_state: %s: bad magic bytes\n", path.c_str());
        return false;
    }
    out.version         = r.read_u32();
    if (out.version != kFormatVersion) {
        std::fprintf(stderr,
            "save_state: %s: unsupported format version %u (expected %u)\n",
            path.c_str(), out.version, kFormatVersion);
        return false;
    }
    out.iso_timestamp   = r.read_string();
    out.sim_elapsed_sec = r.read_f64();
    uint32_t nsec       = r.read_u32();
    out.sections.clear();
    out.sections.reserve(nsec);
    for (uint32_t i = 0; i < nsec; ++i) {
        Section s;
        s.name = r.read_string();
        uint64_t sz = r.read_u64();
        s.data.resize((size_t)sz);
        r.read_bytes(s.data.data(), s.data.size());
        out.sections.push_back(std::move(s));
    }
    if (!r.ok()) {
        std::fprintf(stderr, "save_state: %s: short or malformed file\n", path.c_str());
        return false;
    }
    return true;
}

const Section *SaveFile::find(const std::string &name) const {
    for (const auto &s : sections)
        if (s.name == name) return &s;
    return nullptr;
}

// -----------------------------------------------------------------------
// Timestamp / filename helpers
// -----------------------------------------------------------------------

std::string make_iso_timestamp_local() {
    using clock = std::chrono::system_clock;
    auto now = clock::now();
    std::time_t t = clock::to_time_t(now);
    std::tm lt{};
#if defined(_WIN32)
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    char buf[32];
    // ISO 8601, with ':' → '-' in the time portion so the string is
    // safe on any filesystem (Windows disallows ':' in filenames).
    std::snprintf(buf, sizeof buf,
                  "%04d-%02d-%02dT%02d-%02d-%02d",
                  lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday,
                  lt.tm_hour, lt.tm_min, lt.tm_sec);
    return buf;
}

std::string make_save_filename() {
    return "saved-state-" + make_iso_timestamp_local();
}

} // namespace dskymatic
