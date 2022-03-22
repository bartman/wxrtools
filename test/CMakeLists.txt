enable_testing()

SET(TEST_EXECUTABLES
        test-parse
        test-session
        test-summary
        test-best
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
