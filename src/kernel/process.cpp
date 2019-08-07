#include "cpu.h"
#include "debug.h"
#include "log.h"
#include "kernel_heap.h"
#include "process.h"
#include "scheduler.h"

extern "C" void task_fork_return_thunk();

void
process::exit(uint32_t code)
{
	return_code = code;
	flags -= process_flags::running;
	page_manager::get()->delete_process_map(pml4);
}

pid_t
process::fork()
{
	auto &sched = scheduler::get();
	auto *child = sched.create_process();
	kassert(child, "process::fork() got null child");

	child->ppid = pid;
	child->flags =
		process_flags::running |
		process_flags::ready;

	sched.m_runlists[child->priority].push_back(child);

	child->pml4 = page_manager::get()->copy_table(pml4);
	kassert(child->pml4, "process::fork() got null pml4");

	child->rsp3 = bsp_cpu_data.rsp3;
	child->setup_kernel_stack();

	log::debug(logs::task, "Copied process %d to %d",
			pid, child->pid);

	log::debug(logs::task, "    PML4 %016lx", child->pml4);
	log::debug(logs::task, "    RSP3 %016lx", child->rsp3);
	log::debug(logs::task, "    RSP0 %016lx", child->rsp0);

	// Initialize a new empty stack with a fake saved state
	// for returning out of syscall_handler_prelude
	size_t ret_seg_size = sizeof(uintptr_t) * 8;
	child->rsp -= ret_seg_size;

	void *this_ret_seg =
		reinterpret_cast<void*>(rsp0 - ret_seg_size);
	void *child_ret_seg =
		reinterpret_cast<void*>(child->rsp);
	kutil::memcpy(child_ret_seg, this_ret_seg, ret_seg_size);

	child->add_fake_task_return(
		reinterpret_cast<uintptr_t>(task_fork_return_thunk));

	log::debug(logs::task, "    RSP  %016lx", child->rsp);

	return child->pid;
}

void *
process::setup_kernel_stack()
{
	constexpr unsigned null_frame_entries = 2;
	constexpr size_t null_frame_size = null_frame_entries * sizeof(uint64_t);

	void *stack_bottom = g_kernel_heap.allocate(initial_stack_size);
	kutil::memset(stack_bottom, 0, initial_stack_size);

	log::debug(logs::memory, "Created kernel stack at %016lx size 0x%lx",
			stack_bottom, initial_stack_size);

	void *stack_top =
		kutil::offset_pointer(stack_bottom,
				initial_stack_size - null_frame_size);

	uint64_t *null_frame = reinterpret_cast<uint64_t*>(stack_top);
	for (unsigned i = 0; i < null_frame_entries; ++i)
		null_frame[i] = 0;

	kernel_stack_size = initial_stack_size;
	kernel_stack = reinterpret_cast<uintptr_t>(stack_bottom);
	rsp0 = reinterpret_cast<uintptr_t>(stack_top);
	rsp = rsp0;

	return stack_top;
}

void
process::add_fake_task_return(uintptr_t rip)
{
	rsp -= sizeof(uintptr_t) * 7;
	uintptr_t *stack = reinterpret_cast<uintptr_t*>(rsp);

	stack[6] = rip;        // return rip
	stack[5] = rsp0;       // rbp
	stack[4] = 0xbbbbbbbb; // rbx
	stack[3] = 0x12121212; // r12
	stack[2] = 0x13131313; // r13
	stack[1] = 0x14141414; // r14
	stack[0] = 0x15151515; // r15
}

bool
process::wait_on_time(uint64_t time)
{
	waiting = process_wait::time;
	waiting_info = time;
	flags -= process_flags::ready;
	return true;
}

bool
process::wake_on_time(uint64_t now)
{
	if (waiting != process_wait::time ||
		waiting_info > now)
		return false;

	waiting = process_wait::none;
	flags += process_flags::ready;
	return true;
}
