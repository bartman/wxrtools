enable_testing()

SET(TEST_EXECUTABLES
        test-parse
)

foreach(test ${TEST_EXECUTABLES})

        ADD_EXECUTABLE(${test}
                ${test}.c
        )

        TARGET_LINK_LIBRARIES(${test}
                wxr_a
                -lm
        )

endforeach(test)
