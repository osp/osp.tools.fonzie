SET(potrace_SRCS
curve.c  
decompose.c  
potracelib.c  
trace.c
)

ADD_LIBRARY(potrace STATIC ${potrace_SRCS})
