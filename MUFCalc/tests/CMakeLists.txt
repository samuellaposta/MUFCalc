# Unit tests for MUFCalc core engines
find_package(Qt6 REQUIRED COMPONENTS Test)

set(MUFCALC_TESTS
    TestMufCalculator
    TestDiagnosticEngine
    TestSignificantQuantity
)

foreach(test_name IN LISTS MUFCALC_TESTS)
    add_executable(${test_name} unit/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE mufcalc_core Qt6::Test)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
