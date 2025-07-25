SET(LIBWXR_SRCS
        wxr_lib_ctx.c
        wxr_lib_date.c
        wxr_lib_index.c
        wxr_lib_session.c
        wxr_lib_lift.c
        wxr_lib_entry.c
        wxr_lib_1rm.c
        wxr_lib_utils.c
)

# build a libwxr.so and a libwxr.a

ADD_LIBRARY(wxr_a STATIC ${LIBWXR_SRCS})
ADD_LIBRARY(wxr_so SHARED ${LIBWXR_SRCS})

# each is configured exactly the same way

foreach(wxr wxr_a wxr_so)

        SET_TARGET_PROPERTIES(${wxr} PROPERTIES OUTPUT_NAME wxr)

        SET_PROPERTY(TARGET ${wxr} PROPERTY POSITION_INDEPENDENT_CODE ON)

        #TARGET_COMPILE_DEFINITIONS(${wxr}
        #        PUBLIC var="val"
        #)

        TARGET_INCLUDE_DIRECTORIES(${wxr} INTERFACE
                ${CMAKE_SOURCE_DIR}/wxr/include
        )

        TARGET_INCLUDE_DIRECTORIES(${wxr} PUBLIC
                ${GLIB2_INCLUDE_DIRS}
                ${BLOT_INCLUDE_DIR}
        )

        TARGET_INCLUDE_DIRECTORIES(${wxr} PRIVATE
                ${CMAKE_SOURCE_DIR}/wxr/include
                ${CMAKE_CURRENT_SOURCE_DIR}/include
                ${GIO_INCLUDE_DIRS}
                ${GIOUNIX_INCLUDE_DIRS}
        )

        TARGET_LINK_LIBRARIES(${wxr}
                ${GLIB2_LIBRARIES}
                ${GIO_LIBRARIES}
                ${GIOUNIX_LIBRARIES}
        )

        TARGET_LINK_DIRECTORIES(${wxr} PRIVATE
                ${GLIB2_LIBRARY_DIRS}
                ${GIO_LIBRARY_DIRS}
                ${GIOUNIX_LIBRARY_DIRS}
        )

endforeach(wxr)
