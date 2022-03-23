enable_testing()

SET(TEST_EXECUTABLES
        test-parse
        test-session
        test-summary
        test-best
        test-table
        test-plot
)

foreach(test ${TEST_EXECUTABLES})

        ADD_EXECUTABLE(${test}
                ${test}.c
        )

        TARGET_LINK_LIBRARIES(${test}
                blot_a
                wxr_a
                -lm
        )

endforeach(test)
