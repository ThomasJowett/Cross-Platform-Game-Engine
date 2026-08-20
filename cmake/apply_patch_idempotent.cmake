# Applies a patch in a way that is safe to run on every configure, whether or
# not it was already applied (e.g. a dev's working tree that already has it,
# vs. a fresh clone/CI checkout that doesn't).
#
# Usage:
#   cmake -DPATCH_FILE=path/to/patch.diff -DWORKING_DIRECTORY=path/to/repo -P apply_patch_idempotent.cmake

message(STATUS "Applying patch '${PATCH_FILE}' in '${WORKING_DIRECTORY}'...")

# If the patch can be applied in reverse, it's already applied.
execute_process(
	COMMAND git apply --ignore-space-change --ignore-whitespace --reverse --check "${PATCH_FILE}"
	WORKING_DIRECTORY "${WORKING_DIRECTORY}"
	RESULT_VARIABLE ALREADY_APPLIED
	OUTPUT_QUIET
	ERROR_QUIET
)

if (ALREADY_APPLIED EQUAL 0)
	message(STATUS "Patch was already applied")
else()
	execute_process(
		COMMAND git apply --ignore-space-change --ignore-whitespace "${PATCH_FILE}"
		WORKING_DIRECTORY "${WORKING_DIRECTORY}"
		RESULT_VARIABLE APPLY_FAILED
	)
	if (NOT APPLY_FAILED EQUAL 0)
		message(FATAL_ERROR "Failed to apply patch '${PATCH_FILE}'")
	endif()
endif()
