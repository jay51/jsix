#pragma once

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

namespace kernel {
namespace args {

constexpr uint32_t magic = 0x600dda7a;
constexpr uint16_t version = 1;

enum class mod_flags : uint32_t
{
	debug = 0x00000001
};

enum class mod_type : uint32_t {
	unknown,

	kernel,
	initrd,

	memory_map,
	page_tables,
	framebuffer,

	max
};

enum class mode : uint8_t {
	normal,
	debug
};

struct module {
	void *location;
	size_t size;
	mod_type type;
	mod_flags flags;
}
__attribute__((packed));


enum class mem_type : uint32_t {
	free,
	args,
	kernel,
	module,
	table,
	acpi,
	uefi_runtime,
	mmio,
	persistent
};

/// Structure to hold an entry in the memory map.
struct mem_entry
{
	uintptr_t start;
	size_t pages;
	mem_type type;
	uint32_t attr;
}
__attribute__((packed));


struct header {
	uint32_t magic;
	uint16_t version;

	mode mode;

	uint8_t _reserved0;

	void *pml4;
	void *page_table_cache;
	uint32_t num_free_tables;

	uint32_t num_modules;
	module *modules;

	mem_entry *mem_map;
	size_t num_map_entries;

	void *runtime_services;
	void *acpi_table;
}
__attribute__((aligned(alignof(max_align_t))));
#pragma pack(pop)

} // namespace args

using entrypoint = __attribute__((sysv_abi)) void (*)(args::header *);

} // namespace kernel
