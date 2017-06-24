macro(add_netstack_test NAME)
  add_executable(test_${NAME} test_${NAME}.cpp)
  target_link_libraries(test_${NAME} stack utils)
  add_test(test_${NAME} test_${NAME})
endmacro()

add_netstack_test(mpool)
add_netstack_test(pktbuf)