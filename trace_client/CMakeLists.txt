add_definitions ("-DTRACE_ENABLED")
add_library (trace_client STATIC trace.cpp)
#set_target_properties(trace_client PROPERTIES PUBLIC_HEADER "trace.h;${CMAKE_CURRENT_BINARY_DIR}/config.h")
set_target_properties (trace_client PROPERTIES PUBLIC_HEADER "trace.h")

install (TARGETS trace_client
  # IMPORTANT: Add the trace_client library to the "export-set"
  EXPORT FloggingLibraryDepends
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/flogging" COMPONENT dev)
