# Options are generated in the consumer's CXX context, including transitive requirements.
if(WJR_OPTIONS MATCHES "-march=|/arch:")
    message(FATAL_ERROR "ISA options leaked into the portable module consumer: ${WJR_OPTIONS}")
endif()
