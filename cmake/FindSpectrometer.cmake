MACRO(FIND_SPECTROMETER)

FIND_PATH (SPECTROMETER_INCLUDE_DIR "libspectrometer.h" PATHS "/usr/local/include"
                                                              "/usr/include")

FIND_LIBRARY(SPECTROMETER_LIBRARY "spectrometer" PATHS "/usr/local/lib"
                                                       "/usr/lib"
                                                       #if you know that your library is installed elsewhere, put this specific path here
            )

SET (SPECTROMETER_FOUND "NO")

IF (SPECTROMETER_LIBRARY AND SPECTROMETER_INCLUDE_DIR)
    SET (SPECTROMETER_FOUND "YES")
ENDIF (SPECTROMETER_LIBRARY AND SPECTROMETER_INCLUDE_DIR)

MESSAGE(STATUS "Spectrometer global dependencies: ${SPECTROMETER_FOUND}: ${SPECTROMETER_INCLUDE_DIR}; ${SPECTROMETER_LIBRARY}")
MARK_AS_ADVANCED(
    SPECTROMETER_INCLUDE_DIR
    SPECTROMETER_LIBRARY
)

ENDMACRO(FIND_SPECTROMETER)
