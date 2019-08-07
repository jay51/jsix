#pragma once
/// \file process.h
/// The processes and related definitions

#include <stdint.h>
#include "kutil/enum_bitfields.h"
#include "kutil/linked_list.h"
#include "page_manager.h"

typedef int32_t pid_t;
struct cpu_state;


enum class process_flags : uint32_t
{
	running   = 0x00000001,
	ready     = 0x00000002,
	loading   = 0x00000004,

	const_pri = 0x80000000,

	none      = 0x00000000
};
IS_BITFIELD(process_flags);

enum class process_wait : uint8_t
{
	none,
	signal,
	child,
	time,
	send,
	receive
};

/// A process.
/// 
struct process
{
	static const size_t initial_stack_size = 0x1000;

	// Fields used by assembly routines go first.  If you change any of these,
	// be sure to change the assembly definitions in 'tasking.inc'
	uintptr_t rsp;
	uintptr_t rsp0;
	uintptr_t rsp3;
	page_table *pml4;
	// End of assembly fields

	pid_t pid;
	pid_t ppid;

	process_flags flags;

	uint16_t quanta;

	uint8_t priority;

	process_wait waiting;
	uint64_t waiting_info;

	uint32_t return_code;

	uint32_t reserved1;

	uintptr_t kernel_stack;
	size_t kernel_stack_size;

	/// Terminate this process.
	/// \arg code   The return code to exit with.
	void exit(unsigned code);

	/// Copy this process.
	/// \returns    Returns the child's pid to the parent, and
	///             0 to the child.
	pid_t fork();

	/// Unready this process until after the given time
	/// \arg time  The time after which to wake
	/// \returns  Whether the process should be rescheduled
	bool wait_on_time(uint64_t time);

	/// If this process is waiting on a time, check it
	/// \argument now  The current time
	/// \returns  True if this wake was handled
	bool wake_on_time(uint64_t now);

private:
	friend class scheduler;

	/// Set up a new empty kernel stack for this process. Sets rsp0 on this
	/// process object, but also returns it.
	/// \returns   The new rsp0 as a pointer
	void * setup_kernel_stack();

	/// Initialize this process' kenrel stack with a fake return segment for
	/// returning out of task_switch.
	/// \arg rip  The rip to return to
	void add_fake_task_return(uintptr_t rip);
};

using process_list = kutil::linked_list<process>;
using process_node = process_list::item_type;
