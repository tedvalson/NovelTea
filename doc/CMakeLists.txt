find_package(Doxygen REQUIRED)

# set the input and output documentation paths
set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR})
set(DOXYGEN_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/doc/github.io/docs)

# see if we can generate the CHM documentation
if(NOVELTEA_OS_WINDOWS)
    # if HHC is found, we can generate the CHM (compressed HTML) output
    find_program(DOXYGEN_HHC_PROGRAM
                 NAMES hhc.exe
                 PATHS "c:/Program Files/HTML Help Workshop"
                 DOC "HTML Help Compiler program")
    if(DOXYGEN_HHC_PROGRAM)
        set(DOXYGEN_GENERATE_HTMLHELP YES)
    else()
        set(DOXYGEN_GENERATE_HTMLHELP NO)
    endif()
else()
    set(DOXYGEN_HHC_PROGRAM)
    set(DOXYGEN_GENERATE_HTMLHELP NO)
endif()

# configure the source Doxyfile by copying it and replacing all @variables@
set(DOXYGEN_CONFIGURED_INPUT ${PROJECT_SOURCE_DIR}/doc/doxyfile)
configure_file(${PROJECT_SOURCE_DIR}/doc/doxyfile.in ${DOXYGEN_CONFIGURED_INPUT} @ONLY)

# copy the files needed by the documentation

# target setup
add_custom_target(doc ALL
                  COMMAND ${CMAKE_COMMAND} -E echo_append "Building NovelTea Documentation..."
                  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIGURED_INPUT}
                  COMMAND ${CMAKE_COMMAND} -E echo "Done."
                  WORKING_DIRECTORY ${DOXYGEN_INPUT_DIR})

# setup install rules
install(DIRECTORY ${DOXYGEN_OUTPUT_DIR}
        DESTINATION ./doc
        COMPONENT doc)
if(DOXYGEN_HHC_PROGRAM)
    install(FILES ${DOXYGEN_OUTPUT_DIR}/NovelTea.chm
            DESTINATION ${INSTALL_MISC_DIR}/doc
            COMPONENT doc)
endif()
