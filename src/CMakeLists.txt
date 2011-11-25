IF(TESSERACT_FOUND)
	SET(OCR_SRC ocr.cpp)
	SET(OCR_LIB ${TESSERACT_LIBRARIES})
	SET(OCR_INCLUDE ${TESSERACT_INCLUDE_DIR})
ENDIF(TESSERACT_FOUND)

IF(FONTFORGE_FOUND)
	SET(FF_SRC ff_wrapper.c)
ENDIF(FONTFORGE_FOUND)

SET(nancy_SRCS
main.cpp
glyphtrace.cpp
scanimage.cpp
fontupdate.cpp
options.cpp
gridspec.cpp
magicmetrics.cpp
ufotemplate.cpp
composer.cpp
document.cpp
${OCR_SRC}
${FF_SRC}
)

SET(nancy_MOC_HDRS
glyphtrace.h
)

QT4_WRAP_CPP(nancy_MOC_SRCS  ${nancy_MOC_HDRS})


include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_BINARY_DIR}
${QT_INCLUDE_DIR}
${QT_QT_INCLUDE_DIR}
#${POTRACE_INCLUDE_DIR}
#${SVNClient_INCLUDE_DIR}
${SANE_INCLUDE_DIR}
${PROJECT_SOURCE_DIR}/${potrace_dir}
${FREETYPE_INCLUDE_DIRS}
${OCR_INCLUDE}
${FONTFORGE_INCLUDE_DIR}
${PODOFO_INCLUDE_DIR}
)

add_executable(fonzie
${nancy_SRCS}
${nancy_MOC_SRCS}
)

target_link_libraries(fonzie
potrace
${QT_LIBRARIES}
${SANE_LIBRARIES}
#${SVNClient_LIBRARIES}
${FREETYPE_LIBRARIES}
${OCR_LIB}
${FONTFORGE_LIBRARIES}
${PODOFO_LIBRARIES}
)


