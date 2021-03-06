/// \file fs.h
/// Definitions for dealing with UEFI's disk access functions
#pragma once

#include <uefi/types.h>
#include <uefi/boot_services.h>
#include <uefi/protos/file.h>

namespace boot {
namespace fs {

/// A file or directory in a filesystem.
class file
{
public:
	file(file &&o);
	file(file &o);
	~file();

	/// Open another file or directory, relative to this one.
	/// \arg path  Relative path to the target file from this one
	file open(const wchar_t *path);

	/// Load the contents of this file into memory.
	/// \arg out_size  _out:_ The number of bytes loaded
	/// \arg mem_type  The UEFI memory type to use for allocation
	/// \returns       A pointer to the loaded memory. Memory will be
	///                page-aligned.
	void * load(
		size_t *out_size,
		uefi::memory_type mem_type = uefi::memory_type::loader_data);

private:
	friend file get_boot_volume(uefi::handle, uefi::boot_services*);

	file(uefi::protos::file *f, uefi::boot_services *bs);

	uefi::protos::file *m_file;
	uefi::boot_services *m_bs;
};

/// Get the filesystem this loader was loaded from.
/// \returns  A `file` object representing the root directory of the volume
file get_boot_volume(uefi::handle image, uefi::boot_services *bs);

} // namespace fs
} // namespace boot
